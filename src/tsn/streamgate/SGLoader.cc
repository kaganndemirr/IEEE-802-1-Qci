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

#include "SGLoader.h"

namespace ieee_802_1_qci {

Define_Module(SGLoader);

void SGLoader::initialize()
{
    cXMLElement* table = par("table");

    if (table->hasChildren()) {
        cXMLElementList children = table->getChildrenByTagName("StreamGate");
        cXMLElement* gcl;
        cXMLElementList gclOps;

        int count = children.size();
        int gateIdx = 1;
        int opCount;
        int opIdx;

        cModuleType* moduleType = cModuleType::get("ieee_802_1_qci.tsn.streamgate.StreamGate");
        cModuleType* opModuleType = cModuleType::get("ieee_802_1_qci.tsn.streamgate.GateControlOp");
        cModule* tableModule = getModuleByPath("^.relay.streamGate");
        cModule* queryModule = getModuleByPath("^.relay.streamGate.query");
        cModule* nextModule = getModuleByPath("^.relay.streamGate.next");
        cModule* gclModule;
        cModule* module;

        queryModule->setGateSize("out", count + 1);
        nextModule->setGateSize("in", count + 1);

        for (cXMLElement* elm : children) {
            // Create StreamGate
            module = moduleType->create("elm", tableModule, count, gateIdx-1);

            module->par("instanceId") = elm->getFirstChildWithTag("InstanceId")->getNodeValue();
            module->par("state") = elm->getFirstChildWithTag("State")->getNodeValue();
            module->par("ipv") = elm->getFirstChildWithTag("IPV")->getNodeValue();
            module->par("closedDueToInvalidRxEnable") = elm->getFirstChildWithTag("ClosedDueToInvalidRxEnable")->getNodeValue();
            module->par("closedDueToInvalidRx") = "false";
            module->par("closedDueToOctetsExceededEnable") = elm->getFirstChildWithTag("ClosedDueToOctetsExceededEnable")->getNodeValue();
            module->par("closedDueToOctetsExceeded") = "false";
            module->par("intervalOctetLeft") = "";
            module->par("opIndex") = "0";
            module->finalizeParameters();

            queryModule->gate("out", gateIdx)->connectTo(module->gate("in"));
            module->gate("out")->connectTo(nextModule->gate("in", gateIdx));

            // Create GateControlOps
            gcl = elm->getFirstChildWithTag("GateControlList");
            if (gcl && gcl->hasChildren()) {
                gclOps = gcl->getChildrenByTagName("Operation");
                opCount = gclOps.size();
                opIdx = 0;

                for (cXMLElement* op : gclOps) {
                    // Create StreamGate
                    gclModule = opModuleType->create("op", module, opCount, opIdx);

                    gclModule->par("state") = op->getFirstChildWithTag("State")->getNodeValue();
                    gclModule->par("ipv") = op->getFirstChildWithTag("IPV")->getNodeValue();
                    gclModule->par("timeInterval") = op->getFirstChildWithTag("TimeInterval")->getNodeValue();
                    gclModule->par("intervalOctetMax") = op->getFirstChildWithTag("IntervalOctetMax")->getNodeValue();

                    gclModule->finalizeParameters();

                    opIdx ++;
                }
            }

            // Finish StreamGate
            module->buildInside();
            module->scheduleStart(simTime());

            gateIdx++;
        }
    }
}

void SGLoader::handleMessage(cMessage *msg)
{
}

} //namespace
