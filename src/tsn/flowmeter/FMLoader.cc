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

#include "FMLoader.h"

namespace ieee_802_1_qci {

Define_Module(FMLoader);

void FMLoader::initialize()
{
    cXMLElement* table = par("table");

    if (table->hasChildren()) {
        cXMLElementList children = table->getChildrenByTagName("FlowMeter");

        int count = children.size();
        int gateIdx = 1;

        cModuleType* moduleType = cModuleType::get("ieee_802_1_qci.tsn.flowmeter.FlowMeter");
        cModule* tableModule = getModuleByPath("^.relay.flowMeter");
        cModule* queryModule = getModuleByPath("^.relay.flowMeter.query");
        cModule* nextModule = getModuleByPath("^.relay.flowMeter.next");
        cModule* module;

        queryModule->setGateSize("out", count + 1);
        nextModule->setGateSize("in", count + 1);

        for (cXMLElement* elm : children) {
            module = moduleType->create("elm", tableModule, count, gateIdx-1);

            module->par("instanceId") = elm->getFirstChildWithTag("InstanceId")->getNodeValue();
            module->par("committedInformationRate") = elm->getFirstChildWithTag("CommittedInformationRate")->getNodeValue();
            module->par("committedBurstSize") = elm->getFirstChildWithTag("CommittedBurstSize")->getNodeValue();
            module->par("excessInformationRate") = elm->getFirstChildWithTag("ExcessInformationRate")->getNodeValue();
            module->par("excessBurstSize") = elm->getFirstChildWithTag("ExcessBurstSize")->getNodeValue();
            module->par("couplingFlag") = elm->getFirstChildWithTag("CouplingFlag")->getNodeValue();
            module->par("colorMode") = elm->getFirstChildWithTag("ColorMode")->getNodeValue();
            module->par("dropOnYellow") = elm->getFirstChildWithTag("DropOnYellow")->getNodeValue();
            module->par("markAllFramesRedEnable") = elm->getFirstChildWithTag("MarkAllFramesRedEnable")->getNodeValue();
            module->par("markAllFramesRed") = "false";
            module->finalizeParameters();

            queryModule->gate("out", gateIdx)->connectTo(module->gate("in"));
            module->gate("out")->connectTo(nextModule->gate("in", gateIdx));

            module->buildInside();
            module->scheduleStart(simTime());

            gateIdx++;
        }
    }
}

void FMLoader::handleMessage(cMessage *msg)
{
}

} //namespace
