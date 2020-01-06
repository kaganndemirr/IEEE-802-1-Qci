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

#include "StreamGateTable.h"

namespace ieee_802_1_qci {

Define_Module(StreamGateTable);

void StreamGateTable::initialize()
{
    mClock = check_and_cast<Clock*> (getParentModule()->getSubmodule("clk"));

    StreamGate gate;
    IPVSpec ipvS;
    std::vector<StreamGateControlOperation> gclOps;

    cXMLElement* table = par("table");
    cXMLElement* gcl;

    gate.intervalOctetLeft = IntervalOctetSpec{0, true};
    gate.opIndex = 0;

    IntervalOctetSpec octetMax;
    simtime_t interval;

    if (table->hasChildren()) {
        for (cXMLElement* elm : table->getChildrenByTagName("StreamGate")) {
            gate.instanceId = readXMLUInt(elm->getFirstChildWithTag("InstanceId"), "InstanceId");
            gate.state = readXMLBool(elm->getFirstChildWithTag("State"), "State");

            ipvS.value = readXMLUIntStr(elm->getFirstChildWithTag("IPV"), "IPV", "null", &ipvS.isNull);
            gate.ipv = ipvS;

            gate.closedDueToInvalidRxEnable = readXMLBool(elm->getFirstChildWithTag("ClosedDueToInvalidRxEnable"), "ClosedDueToInvalidRxEnable");
            gate.closedDueToInvalidRx = readXMLBool(elm->getFirstChildWithTag("ClosedDueToInvalidRx"), "ClosedDueToInvalidRx");
            gate.closedDueToOctetsExceededEnable = readXMLBool(elm->getFirstChildWithTag("ClosedDueToOctetsExceededEnable"), "ClosedDueToOctetsExceededEnable");
            gate.closedDueToOctetsExceeded = readXMLBool(elm->getFirstChildWithTag("ClosedDueToOctetsExceeded"), "ClosedDueToOctetsExceeded");

            gclOps = std::vector<StreamGateControlOperation>();

            gcl = elm->getFirstChildWithTag("GateControlList");
            if (gcl && gcl->hasChildren()) {
                for (cXMLElement* op : gcl->getChildrenByTagName("Operation")) {
                    ipvS.value = readXMLUIntStr(op->getFirstChildWithTag("IPV"), "IPV", "null", &ipvS.isNull);
                    octetMax.value = readXMLUIntStr(op->getFirstChildWithTag("IntervalOctetMax"), "IntervalOctetMax", "", &octetMax.isNull);

                    interval = simTime().parse(op->getFirstChildWithTag("TimeInterval")->getNodeValue());

                    gclOps.push_back(StreamGateControlOperation {
                        readXMLBool(op->getFirstChildWithTag("State"), "State"),
                        ipvS,
                        interval,
                        octetMax
                    });
                }

                if (gclOps.size() > 0)
                {
                    StreamGateControlOperation& gclOp = gclOps[0];

                    gate.opIndex = 0;
                    gate.ipv = gclOp.ipv;
                    gate.state = gclOp.state;
                    gate.intervalOctetLeft = gclOp.intervalOctetMax;

                    mClock->scheduleCall(this, gclOp.timeInterval, gate.instanceId);
                }
            }

            gate.gateControlList = gclOps;

            mList.push_back(gate);
        }

        std::sort(mList.begin(), mList.end(), compareStreamGate);
    }
}

void StreamGateTable::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

simtime_t StreamGateTable::tick(int gateId)
{
    Enter_Method("tick()");

    StreamGate* gate = NULL;

    for (int i=0, listSize = mList.size(); i<listSize; i++) {
        if (mList[i].instanceId == gateId) {
            gate = &mList[i];
            break;
        }
    }

    if (!gate)
        return simTime().ZERO;

    gate->opIndex ++;
    if (gate->opIndex >= gate->gateControlList.size())
        gate->opIndex = 0;

    StreamGateControlOperation& gclOp = gate->gateControlList[gate->opIndex];

    gate->ipv = gclOp.ipv;
    gate->state = gclOp.state;
    gate->intervalOctetLeft = gclOp.intervalOctetMax;

    return gclOp.timeInterval;
}

StreamGate* StreamGateTable::getStreamGate(int gateId) {
    for (int i=0, listSize = mList.size(); i<listSize; i++) {
        if (mList[i].instanceId == gateId) {
            return &mList[i];
        }
    }
    return NULL;
}

bool compareStreamGate(StreamGate g1, StreamGate g2) {
    return (g1.instanceId < g2.instanceId);
}

} //namespace
