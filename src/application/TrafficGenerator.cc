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

#include "TrafficGenerator.h"
#include "EthernetFrame_m.h"
#include <iomanip>

namespace ieee_802_1_qci {

Define_Module(TrafficGenerator);

void TrafficGenerator::initialize()
{
    mClock = check_and_cast<Clock*> (getParentModule()->getSubmodule("clk"));
    mDelay = par("startDelay");

    handleParameterChange(nullptr);

    if (mDelay.isZero())
        mClock->scheduleCall(this, mInterval, 0);
    else
        mClock->scheduleCall(this, mDelay, 1);
}

void TrafficGenerator::handleMessage(cMessage *msg)
{
    EthernetFrame* pkt = check_and_cast<EthernetFrame *>(msg);
    double delay = (simTime().dbl() - pkt->getCreationTime()) * 1000;
    EV_INFO << "Packet received src=" << pkt->getSrc()
            << " length=" << pkt->getPayloadSize()
            << " delay=" << delay << "ms";
    delete msg;
}

void TrafficGenerator::handleParameterChange(const char *parname)
{
    if (!parname || strcmp(parname, "target") == 0) {
        mTarget = par("target").stringValue();
    }

    if (!parname || strcmp(parname, "streamId") == 0) {
        mStreamId = par("streamId").intValue();
    }

    if (!parname || strcmp(parname, "priority") == 0) {
        mPriority = par("priority").intValue();
    }

    if (!parname || strcmp(parname, "sendInterval") == 0) {
        mInterval = par("sendInterval");
    }

    if (!parname || strcmp(parname, "msgIcon") == 0) {
        mIcon = par("msgIcon").stringValue();
    }

    if (!parname || strcmp(parname, "msgColor") == 0) {
        mColor = par("msgColor").stringValue();
    }
}

simtime_t TrafficGenerator::tick(int param)
{
    Enter_Method("tick()");

    if (param == 1)
    {
        mClock->scheduleCall(this, mInterval, 0);
        return 0;
    }

    EthernetFrame* msg = new EthernetFrame();
    msg->setStreamId(mStreamId);
    msg->setDst(mTarget);
    msg->setPayloadSize(par("payloadSize").doubleValueInUnit("byte"));
    msg->setPriority(mPriority);
    msg->setCreationTime(simTime().dbl());

    if (mIcon[0] || mColor[0]) {
        cDisplayString dispStr = cDisplayString("");

        if (mIcon[0]) {
            dispStr.setTagArg("i", 0, mIcon);
        }
        if (mColor[0]) {
            dispStr.setTagArg("i", 1, mColor);
            dispStr.setTagArg("i", 2, "50");
        }

        msg->setDisplayString(dispStr.str());
    }

    send(msg, "out");

    return mInterval;
}

} //namespace
