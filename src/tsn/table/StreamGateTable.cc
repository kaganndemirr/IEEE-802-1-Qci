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
    StreamGate gate;
    IPVSpec ipvS;
    std::vector<StreamGateControlOperation> gclOps;

    cXMLElement* table = par("table");
    cXMLElement* gcl;

    gate.intervalOctetLeft = -1;
    gate.opIndex = -1;

    IntervalOctetMaxSpec octetMax;

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
                    octetMax.value = readXMLUIntStr(op->getFirstChildWithTag("IntervalOctetMax"), "IntervalOctetMax", "", &octetMax.isOmitted);

                    gclOps.push_back(StreamGateControlOperation {
                        readXMLBool(op->getFirstChildWithTag("State"), "State"),
                        ipvS,
                        readXMLUInt(op->getFirstChildWithTag("TimeInterval"), "TimeInterval"),
                        octetMax
                    });
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
