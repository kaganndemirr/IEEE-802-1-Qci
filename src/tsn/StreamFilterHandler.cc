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

#include "StreamFilterHandler.h"
#include "TSNPacket_m.h"
#include "../application/EthernetFrame_m.h"

namespace ieee_802_1_qci {

Define_Module(StreamFilterHandler);

void StreamFilterHandler::initialize()
{
    mStreamFilterTable = check_and_cast<StreamFilterTable*> (getParentModule()->getParentModule()->getSubmodule("streamfilters"));
}

void StreamFilterHandler::handleMessage(cMessage *msg)
{
    TSNPacket* pkt = check_and_cast<TSNPacket *>(msg);

    if (!pkt) {
        throw cRuntimeError("Received message isn't a TSNPacket");
    }

    EthernetFrame* frame = check_and_cast<EthernetFrame *>(pkt->getEncapsulatedPacket());

    if (!frame) {
        throw cRuntimeError("Received ControlPacket doesn't contain EthernetFrame");
    }

    int streamHandle = pkt->getStreamHandle();
    int priority = frame->getPriority();

    std::ostringstream bubbleText;

    StreamFilter* sf = mStreamFilterTable->getStreamFilter(streamHandle, priority);

    // Pass if no filter matches
    if (!sf) {
        bubbleText << "PASS[No StreamFilter]";
        bubble(bubbleText.str().c_str());

        send(msg, "out");
        return;
    }

    bubbleText << "StreamFilter[" << sf->instanceId << "]";

    // Drop packets when stream blocked
    if (sf->streamBlockedDueToOversizeFrame) {
        EV_WARN << "Packet dropped because the stream is blocked (OversizeFrame"
                << ", ID: " << sf->instanceId << ")";

        bubbleText << " DROP[streamBlockedDueToOversizeFrame]";
        bubble(bubbleText.str().c_str());

        delete msg;
        return;
    }

    // Drop packets > maxSDUSize
    if (sf->maxSDUSize.isActive) {
        unsigned int pktSize = frame->getPayloadSize();
        if (pktSize > sf->maxSDUSize.value) {
            EV_WARN << "Packet dropped because it exceeds MaxSDUSize (ID: " << sf->instanceId
                    << ", " << pktSize << " > " << sf->maxSDUSize.value << ")";

            bubbleText << " DROP[MaxSDUSize]";
            bubble(bubbleText.str().c_str());

            if (sf->streamBlockedDueToOversizeFrameEnable) {
                sf->streamBlockedDueToOversizeFrame = true;

                EV_WARN << "streamBlockedDueToOversizeFrame = true";
            }

            delete msg;
            return;
        }
    }

    pkt->setStreamGateId(sf->streamGateId);
    pkt->setFlowMeterIdsArraySize(sf->flowMeters.size());

    int idx = 0;
    for (auto meterId : sf->flowMeters) {
        pkt->setFlowMeterIds(idx++, meterId);
    }

    bubbleText << " PASS";
    bubble(bubbleText.str().c_str());

    send(msg, "next");
}

} //namespace
