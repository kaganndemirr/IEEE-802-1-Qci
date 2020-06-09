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

#include "Bucket.h"
#include "../TSNPacket_m.h"
#include "../../application/EthernetFrame_m.h"

namespace ieee_802_1_qci {

Define_Module(Bucket);

void Bucket::initialize()
{
    const char* color = par("color").stringValue();
    getDisplayString().setTagArg("i", 1, color);

    if (!strcmp(color, "green")) {
        isGreen = true;
    }
    else if (!strcmp(color, "yellow")) {
        isYellow = true;
    }

    mLastUpdate = simTime();

    addBubbleModule("^");
    addBubbleModule("^.^");
    addBubbleModule("^.^.^");
}

void Bucket::handleMessage(cMessage *msg)
{
    TSNPacket* pkt = check_and_cast<TSNPacket *>(msg);
    EthernetFrame* frame = check_and_cast<EthernetFrame *>(pkt->getEncapsulatedPacket());

    if (isGreen) {
        frame->setColor(1);
    }
    else if (isYellow) {
        frame->setColor(2);
        bubble("Yellow Packet");
    }

    int meterCount = pkt->getFlowMeterIdsArraySize();
    int meterIdx = pkt->getMeterIdx();

    // This was the last meter
    if (meterIdx >= meterCount) {
        send(msg, "out");
        return;
    }

    cModule* queryModule = getModuleByPath("^.query");
    cGate* directIn = queryModule->gate("directIn");

    // Forward back to query for remaining meters
    sendDirect(msg, directIn);
}

} //namespace
