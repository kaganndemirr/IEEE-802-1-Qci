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

#include "FilteringUnit.h"
#include "../application/EthernetFrame_m.h"

namespace ieee_802_1_qci {

Define_Module(FilteringUnit);

void FilteringUnit::initialize()
{
    mStreamFilterTable = check_and_cast<StreamFilterTable*> (getParentModule()->getParentModule()->getSubmodule("streamfilters"));
    mStreamGateTable = check_and_cast<StreamGateTable*> (getParentModule()->getParentModule()->getSubmodule("streamgates"));
    mFlowMeterTable = check_and_cast<FlowMeterTable*> (getParentModule()->getParentModule()->getSubmodule("flowmeters"));
}

void FilteringUnit::handleMessage(cMessage *msg)
{
    EthernetFrame* pkt = check_and_cast<EthernetFrame *>(msg);
    if (pkt) {
        int streamHandle = pkt->getStreamHandle();
        int priority = pkt->getPriority();

        std::ostringstream bubbleText;
        std::string bubbleTextStr;

        // TODO Separate StreamGate handling & FlowMeter handling from FilteringUnit to their own nodes

        StreamFilter* sf = mStreamFilterTable->getStreamFilter(streamHandle, priority);
        if (sf) {
            bubbleText << "StreamFilter: " << sf->instanceId << ", Priority: ";
            if (sf->priority.isWildcard)
                bubbleText << "*";
            else
                bubbleText << sf->priority.value;
            bubbleTextStr = bubbleText.str();
            bubble(bubbleTextStr.c_str());
            bubbleText.str("");

            StreamGate* gate = mStreamGateTable->getStreamGate(sf->streamGateId);
            if (gate) {
                if (gate->closedDueToInvalidRx) {
                    EV_WARN << "Packet dropped due to arrival at a closed gate (InvalidRx) (" << gate->instanceId << "): "
                            << pkt->getName();

                    bubbleText << "Failed StreamGate (InvalidRx): " << gate->instanceId
                            << ", GCL: " << gate->opIndex
                            << ", " << bubbleTextStr;
                    bubble(bubbleText.str().c_str());

                    delete msg;

                    return;
                }

                if (gate->closedDueToOctetsExceeded) {
                    EV_WARN << "Packet dropped due to arrival at a closed gate (OctetsExceeded) (" << gate->instanceId << "): "
                            << pkt->getName();

                    bubbleText << "Failed StreamGate (OctetsExceeded): " << gate->instanceId
                            << ", GCL: " << gate->opIndex
                            << ", " << bubbleTextStr;
                    bubble(bubbleText.str().c_str());

                    delete msg;

                    return;
                }

                // Drop if gate is closed
                if (!gate->state) {
                    EV_WARN << "Packet dropped due to arrival at a closed gate (" << gate->instanceId << "): "
                            << pkt->getName();

                    bubbleText << "Failed StreamGate: " << gate->instanceId
                            << ", GCL: " << gate->opIndex
                            << ", " << bubbleTextStr;
                    bubble(bubbleText.str().c_str());

                    delete msg;

                    if (gate->closedDueToInvalidRxEnable) {
                        gate->closedDueToInvalidRx = true;
                    }

                    return;
                }

                if (!gate->intervalOctetLeft.isNull) {
                    if (pkt->getPayloadSize() > gate->intervalOctetLeft.value) {
                        EV_WARN << "Packet dropped due to arrival at a gate has not enough octet left (" << gate->instanceId << "): "
                                << pkt->getName();

                        bubbleText << "Failed StreamGate (Octet): " << gate->instanceId
                                << ", GCL: " << gate->opIndex
                                << ", " << bubbleTextStr;
                        bubble(bubbleText.str().c_str());

                        delete msg;

                        if (gate->closedDueToOctetsExceededEnable) {
                            gate->closedDueToOctetsExceeded = true;
                        }

                        return;
                    }

                    gate->intervalOctetLeft.value -= pkt->getPayloadSize();
                }

                // TODO set ipv
                // TODO subtract interval octets etc.

                bubbleText << "Passed StreamGate: " << gate->instanceId;
                if (!gate->ipv.isNull)
                    bubbleText << ", IPV: " << gate->ipv.value;
                bubbleText << ", " << bubbleTextStr;
                bubble(bubbleText.str().c_str());
            } else {
                throw cRuntimeError("StreamGate not found!");
            }

            // Drop packets > maxSDUSize
            if (sf->maxSDUSize.isActive) {
                unsigned int pktSize = pkt->getPayloadSize();
                if (pktSize > sf->maxSDUSize.value) {
                    EV_WARN << "Packet dropped due to exceed of MaxSDUSize (" << sf->maxSDUSize.value << "): "
                            << pkt->getName() << " (Len: " << pktSize <<  ")";

                    bubbleText << "Failed MaxSDUSize: " << sf->maxSDUSize.value
                            << ", PktSize: " << pktSize
                            << ", " << bubbleTextStr;
                    bubble(bubbleText.str().c_str());

                    delete msg;
                    return;
                }
            }

            if (sf->flowMeters.size() > 0) {
                FlowMeter* meter;
                for (auto meterId : sf->flowMeters) {
                    meter = mFlowMeterTable->getFlowMeter(meterId);

                    if (meter != NULL) {
                        if (meter->markAllFramesRed) {
                            EV_WARN << "Packet dropped due to arrival at a meter that marks all frames red (" << meterId << "): "
                                    << pkt->getName();

                            bubbleText << "Failed FlowMeter (Red Mark): " << meterId
                                    << ", " << bubbleTextStr;
                            bubble(bubbleText.str().c_str());

                            delete msg;
                            return;
                        }

                        unsigned int packetSize = pkt->getPayloadSize();

                        if (mFlowMeterTable->tryGreenBucket(meterId, packetSize) &&
                                (pkt->getColor() == 0
                                || pkt->getColor() == 1
                                || !meter->colorMode)) { // color-blind
                            pkt->setColor(1);
                        }
                        else if (mFlowMeterTable->tryYellowBucket(meterId, packetSize)) {
                            pkt->setColor(2);
                            bubble("Excess Packet");

                            if (meter->dropOnYellow) {
                                EV_WARN << "Packet dropped due to arrival at a meter that drops yellow marked frames (" << meterId << "): "
                                        << pkt->getName();

                                bubbleText << "Failed FlowMeter (Drop Yellow Mark): " << meterId
                                        << ", " << bubbleTextStr;
                                bubble(bubbleText.str().c_str());

                                delete msg;
                                return;
                            }
                        }
                        else {
                            EV_WARN << "Packet dropped due to arrival at a meter that has not enough tokens (" << meterId << "): "
                                    << pkt->getName();

                            bubbleText << "Failed FlowMeter (Packet Size): " << meterId
                                    << ", " << bubbleTextStr;
                            bubble(bubbleText.str().c_str());

                            if (meter->markAllFramesRedEnable) {
                                meter->markAllFramesRed = true;
                            }

                            delete msg;
                            return;
                        }
                    } else {
                        throw cRuntimeError("FlowMeter not found!");
                    }
                }
            }
        }
    }

    send(msg, "out", msg->getArrivalGate()->getIndex());
}

} //namespace
