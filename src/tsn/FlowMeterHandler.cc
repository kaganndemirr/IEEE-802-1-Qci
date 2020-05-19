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

#include "FlowMeterHandler.h"
#include "TSNPacket_m.h"
#include "../application/EthernetFrame_m.h"

namespace ieee_802_1_qci {

Define_Module(FlowMeterHandler);

void FlowMeterHandler::initialize()
{
    mFlowMeterTable = check_and_cast<FlowMeterTable*> (getParentModule()->getParentModule()->getSubmodule("flowmeters"));
}

void FlowMeterHandler::handleMessage(cMessage *msg)
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

    int meterCount = pkt->getFlowMeterIdsArraySize();
    if (meterCount > 0) {
        FlowMeter* meter;

        for (int i=0; i<meterCount; i++) {
            meter = mFlowMeterTable->getFlowMeter(pkt->getFlowMeterIds(i));
            if (!meter) {
                throw cRuntimeError("FlowMeter not found!");
            }

            bubbleText.str("");
            bubbleText << "FlowMeter[" << meter->instanceId << "]";

            if (meter->markAllFramesRed) {
                EV_WARN << "Packet dropped because it couldn't pass the meter (MarkAllFramesRed"
                        << ", ID: " << meter->instanceId << ")";

                bubbleText << " DROP[MarkAllFramesRed]";
                bubble(bubbleText.str().c_str());

                delete msg;
                return;
            }

            unsigned int packetSize = frame->getPayloadSize();

            if (mFlowMeterTable->tryGreenBucket(meter->instanceId, packetSize) &&
                    (pkt->getColor() == 0
                    || pkt->getColor() == 1
                    || !meter->colorMode)) { // color-blind
                pkt->setColor(1);
            }
            else if (mFlowMeterTable->tryYellowBucket(meter->instanceId, packetSize) && !meter->dropOnYellow) {
                pkt->setColor(2);
                bubbleText << " YELLOW";
            }
            else {
                std::string reason = (meter->dropOnYellow ? "DropOnYellow" : "PacketSize");

                EV_WARN << "Packet dropped because it couldn't pass the meter (" << reason
                        << ", ID: " << meter->instanceId << ")";

                bubbleText << " DROP[" << reason << "]";
                bubble(bubbleText.str().c_str());

                if (!meter->dropOnYellow && meter->markAllFramesRedEnable) {
                    meter->markAllFramesRed = true;


                    EV_WARN << "markAllFramesRed = true";
                }

                delete msg;
                return;
            }
        }
    }
    else {
        bubbleText << "NO FLOWMETER";
    }

    bubbleText << " PASS";
    bubble(bubbleText.str().c_str());

    send(msg, "out");
}

} //namespace
