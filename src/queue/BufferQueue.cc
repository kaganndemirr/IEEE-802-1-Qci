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

#include "BufferQueue.h"

namespace ieee_802_1_qci {

Define_Module(BufferQueue);

void BufferQueue::initialize()
{
    mQueue = new cPacketQueue("buffer");
    mClock = check_and_cast<Clock*> (getParentModule()->getSubmodule("clk"));
    mInterval = par("interval");
}

void BufferQueue::handleMessage(cMessage *msg)
{
    cPacket* pkt = check_and_cast<cPacket *>(msg);

    if (!pkt) {
        throw cRuntimeError("Received cMessage doesn't contain a cPacket");
    }

    if (mQueue->isEmpty()) {
        mQueue->insert(pkt);
        mClock->scheduleCall(this, mInterval, 0);
    }
    else {
        mQueue->insert(pkt);
    }
}

void BufferQueue::handleParameterChange(const char *parname)
{
    if (!parname) {
        return;
    }

    if (strcmp(parname, "interval") == 0) {
        mInterval = par("interval");
    }
}

simtime_t BufferQueue::tick(int unused)
{
    Enter_Method("tick()");

    if (!mQueue->isEmpty()) {
        cPacket* pkt = mQueue->pop();
        if (!pkt) {
            throw cRuntimeError("Invalid cPacket in queue");
        }

        send(check_and_cast<cMessage *>(pkt), "out", pkt->getArrivalGate()->getIndex());
    }

    if (mQueue->isEmpty()){
        return 0;
    }

    return mInterval;
}

} //namespace
