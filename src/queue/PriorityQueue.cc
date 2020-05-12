//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "PriorityQueue.h"
#include "../application/EthernetFrame_m.h"

namespace ieee_802_1_qci {

Define_Module(PriorityQueue);

void PriorityQueue::initialize()
{
    mPriorityCount = par("priorityCount");
    mInterval = par("interval");

    std::ostringstream queueName;
    for (int i=0; i<mPriorityCount; i++) {
        queueName.str("");
        queueName << "Priority-" << i;
        mQueue.push_back(new cPacketQueue(queueName.str().c_str()));
    }
    WATCH_VECTOR(mQueue);

    mClock = check_and_cast<Clock*> (getParentModule()->getParentModule()->getSubmodule("clk"));
}

void PriorityQueue::handleMessage(cMessage *msg)
{
    cPacket* pkt = check_and_cast<cPacket *>(msg);

    if (!pkt) {
        throw cRuntimeError("Received cMessage doesn't contain a cPacket");
    }

    EthernetFrame* frame = check_and_cast<EthernetFrame *>(pkt);

    if (!frame) {
        throw cRuntimeError("Received cPacket doesn't contain an EthernetFrame");
    }

    int priority = frame->getPriority();
    if (priority < 0 || priority >= mPriorityCount) {
        EV_WARN << "Invalid priority packet: " << frame->getDisplayString();
        delete msg;
        return;
    }

    cPacketQueue* queue = mQueue[priority];

    if (queue->isEmpty()) {
        queue->insert(pkt);
        mClock->scheduleCall(this, mInterval, priority);
    }
    else {
        queue->insert(pkt);
    }
}

void PriorityQueue::handleParameterChange(const char *parname)
{
    if (!parname) {
        return;
    }

    if (strcmp(parname, "interval") == 0) {
        mInterval = par("interval");
    }
}

simtime_t PriorityQueue::tick(int priority)
{
    Enter_Method("tick()");

    if (priority < 0 || priority >= mPriorityCount) {
        throw cRuntimeError("Invalid priority!");
    }

    cPacketQueue* queue = mQueue[priority];

    if (!queue->isEmpty()) {
        cPacket* pkt = queue->pop();
        if (!pkt) {
            throw cRuntimeError("Invalid cPacket in queue");
        }

        send(check_and_cast<cMessage *>(pkt), "out");
    }

    if (queue->isEmpty()){
        return 0;
    }

    return mInterval;
}


} //namespace
