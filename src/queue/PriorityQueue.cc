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
#include "../tsn/TSNPacket_m.h"

namespace ieee_802_1_qci {

Define_Module(PriorityQueue);

void PriorityQueue::initialize()
{
    mPriorityCount = par("priorityCount");
    mInterval = par("interval");

    mTicking = false;

    std::ostringstream queueName;
    for (int i=0; i<mPriorityCount; i++) {
        queueName.str("");
        queueName << "Priority-" << i;
        mQueue.push_back(new cPacketQueue(queueName.str().c_str()));
    }
    WATCH_VECTOR(mQueue);

    mClock = check_and_cast<Clock*> (getParentModule()->getSubmodule("clk"));
}

void PriorityQueue::finish() {
    for (cPacketQueue* queue : mQueue) {
        while (!queue->isEmpty()) {
            cPacket* pkt = queue->pop();
            cancelAndDelete(pkt);
        }

        delete queue;
    }
}

void PriorityQueue::handleMessage(cMessage *msg)
{
    TSNPacket* pkt = check_and_cast<TSNPacket *>(msg);
    EthernetFrame* frame = check_and_cast<EthernetFrame *>(pkt->getEncapsulatedPacket());

    int ipv = pkt->getIpv();
    int priority = frame->getPriority();
    if (priority < 0 || priority >= mPriorityCount) {
        EV_WARN << "Invalid priority packet: " << frame->getDisplayString();
        delete msg;
        return;
    }

    // Use internal priority value
    if (ipv >= 0 && ipv <= mPriorityCount) {
        priority = ipv;
        bubble("Using IPV instead of packet priority!");
    }

    cPacketQueue* queue = mQueue[priority];

    if (mTicking) {
        queue->insert(pkt);
    }
    else {
        mTicking = true;
        queue->insert(pkt);
        mClock->scheduleCall(this, mInterval, 0);
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

simtime_t PriorityQueue::tick(int unused)
{
    Enter_Method("tick()");

    cPacketQueue* queue;
    bool sent = false;

    // Loop in priority order
    for (int i=0; i<mPriorityCount; i++) {
        queue = mQueue[i];

        // Skip empty queues
        if (queue->isEmpty()) {
            continue;
        }

        // Prevent sending more than one packet per tick
        if (!sent) {
            cPacket* pkt = queue->pop();
            if (!pkt) {
                throw cRuntimeError("Invalid cPacket in queue");
            }

            send(pkt, "out");
            sent = true;
        }

        // Continue ticking if there is still a non-empty queue
        if (!queue->isEmpty()) {
            mTicking = true;
            return mInterval;
        }
    }

    // All queues are empty, no need to continue ticking
    mTicking = false;
    return 0;
}


} //namespace
