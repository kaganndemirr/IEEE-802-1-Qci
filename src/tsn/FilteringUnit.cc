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

        StreamFilter* sf = mStreamFilterTable->getStreamFilter(streamHandle, priority);
        if (sf) {
            // Drop packets > maxSDUSize
            if (sf->maxSDUSize.isActive) {
                unsigned int pktSize = strlen(pkt->getPayload());
                if (pktSize > sf->maxSDUSize.value) {
                    EV_WARN << "Packet dropped due to exceed of MaxSDUSize (" << sf->maxSDUSize.value << "): "
                            << pkt->getName() << " (len: " << pktSize <<  ")";
                    delete msg;
                    return;
                }
            }

            StreamGate* gate = mStreamGateTable->getStreamGate(sf->streamGateId);
            if (gate) {
                // Drop if gate is closed
                if (!gate->state) {
                    EV_WARN << "Packet dropped due to arrival at a closed gate (" << gate->instanceId << "): "
                            << pkt->getName();
                    delete msg;
                    return;
                }

                // TODO set ipv
                // TODO subtract interval octets etc.
            } else {
                // TODO action to take when the gate doesn't exist
            }
        }
    }

    send(msg, "out", msg->getArrivalGate()->getIndex());
}

} //namespace
