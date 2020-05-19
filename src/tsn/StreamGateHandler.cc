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

#include "StreamGateHandler.h"
#include "TSNPacket_m.h"
#include "../application/EthernetFrame_m.h"

namespace ieee_802_1_qci {

Define_Module(StreamGateHandler);

void StreamGateHandler::initialize()
{
    mStreamGateTable = check_and_cast<StreamGateTable*> (getParentModule()->getParentModule()->getSubmodule("streamgates"));
}

void StreamGateHandler::handleMessage(cMessage *msg)
{
    TSNPacket* pkt = check_and_cast<TSNPacket *>(msg);

    if (!pkt) {
        throw cRuntimeError("Received message isn't a TSNPacket");
    }

    EthernetFrame* frame = check_and_cast<EthernetFrame *>(pkt->getEncapsulatedPacket());

    if (!frame) {
        throw cRuntimeError("Received TSNPacket doesn't contain EthernetFrame");
    }

    std::ostringstream bubbleText;

    StreamGate* gate = mStreamGateTable->getStreamGate(pkt->getStreamGateId());
    if (!gate) {
        throw cRuntimeError("StreamGate not found!");
    }

    bubbleText << "StreamGate[" << gate->instanceId << "]";

    // Drop packets when gate is closed
    if (gate->closedDueToInvalidRx || gate->closedDueToOctetsExceeded || !gate->state) {
        std::string reason = (
            gate->closedDueToInvalidRx ? "InvalidRx"
            : (gate->closedDueToOctetsExceeded ? "OctetsExceeded" : "ClosedState")
        );

        EV_WARN << "Packet dropped due to arrival at a closed gate (" << reason
                << ", ID: " << gate->instanceId << ", GCL OP: " << gate->opIndex << ")";

        bubbleText << " DROP[" << reason << "]";
        bubble(bubbleText.str().c_str());

        if (!gate->state && gate->closedDueToInvalidRxEnable && !gate->closedDueToInvalidRx) {
            gate->closedDueToInvalidRx = true;

            EV_WARN << "closedDueToInvalidRx = true";
        }

        delete msg;
        return;
    }

    if (!gate->intervalOctetLeft.isNull) {
        unsigned int pktSize = frame->getPayloadSize();

        // Drop packets when its bigger than allowed octets
        if (pktSize > gate->intervalOctetLeft.value) {
            EV_WARN << "Packet dropped due to arrival at a gate that has not enough octet left ("
                    << "ID: " << gate->instanceId << ", GCL OP: " << gate->opIndex << ")";

            bubbleText << " DROP[OctetsExceed]";
            bubble(bubbleText.str().c_str());

            if (gate->closedDueToOctetsExceededEnable) {
                gate->closedDueToOctetsExceeded = true;

                EV_WARN << "closedDueToOctetsExceeded = true";
            }

            delete msg;
            return;
        }

        gate->intervalOctetLeft.value -= pktSize;
    }

    if (!gate->ipv.isNull) {
        pkt->setIpv(gate->ipv.value);
        bubbleText << " IPV[" << gate->ipv.value << "]";
    }

    bubbleText << " PASS";
    bubble(bubbleText.str().c_str());

    send(msg, "next");
}

} //namespace
