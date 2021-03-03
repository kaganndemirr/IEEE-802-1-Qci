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

#include "SFLoader.h"

namespace ieee_802_1_qci {

Define_Module(SFLoader);

void SFLoader::initialize()
{
    cXMLElement* table = par("table");

    // Read stream filters
    if (table->hasChildren()) {
        cXMLElement* filterList;
        cXMLElement* maxSDUSizeTag;

        cXMLElementList children = table->getChildrenByTagName("StreamFilter");
        int count = children.size();
        int gateIdx = 1;

        std::stringstream name;
        std::string flowMeters;
        std::string maxSDUSize;

        cModuleType* moduleType = cModuleType::get("ieee_802_1_qci.tsn.streamfilter.StreamFilter");
        cModule* tableModule = getModuleByPath("^.relay.streamFilter");
        cModule* queryModule = getModuleByPath("^.relay.streamFilter.query");
        cModule* nextModule = getModuleByPath("^.relay.streamFilter.next");
        cModule* module;

        queryModule->setGateSize("out", count + 1);
        nextModule->setGateSize("in", count + 1);

        for (cXMLElement* elm : children) {
            flowMeters.clear();

            filterList = elm->getFirstChildWithTag("FilterList");
            if (filterList && filterList->hasChildren()) {
                // There is only one MaxSDUSize allowed in filters
                maxSDUSizeTag = filterList->getFirstChildWithTag("MaxSDUSize");
                if (maxSDUSizeTag) {
                    maxSDUSize = maxSDUSizeTag->getNodeValue();
                } else {
                    maxSDUSize = "";
                }

                // Concatenate all flowmeter ids
                for (cXMLElement* filter : filterList->getChildrenByTagName("FlowMeter")) {
                    flowMeters.append(filter->getNodeValue());
                    flowMeters.append(" ");
                }
            }

            // Removes last space char
            if (flowMeters.length() > 0) {
                flowMeters.pop_back();
            }

            module = moduleType->create("elm", tableModule, count, gateIdx-1);

            module->par("instanceId") = elm->getFirstChildWithTag("InstanceId")->getNodeValue();
            module->par("streamHandle") = elm->getFirstChildWithTag("StreamHandle")->getNodeValue();
            module->par("priority") = elm->getFirstChildWithTag("Priority")->getNodeValue();
            module->par("streamGateId") = elm->getFirstChildWithTag("StreamGateId")->getNodeValue();
            module->par("maxSDUSize") = maxSDUSize;
            module->par("flowMeters") = flowMeters;
            module->par("streamBlockedDueToOversizeFrameEnable") = elm->getFirstChildWithTag("StreamBlockedDueToOversizeFrameEnable")->getNodeValue();
            module->par("streamBlockedDueToOversizeFrame") = "false";
            module->finalizeParameters();

            queryModule->gate("out", gateIdx)->connectTo(module->gate("in"));
            module->gate("out")->connectTo(nextModule->gate("in", gateIdx));

            module->buildInside();
            module->scheduleStart(simTime());

            gateIdx++;
        }
    }
}

void SFLoader::handleMessage(cMessage *msg)
{
}

} //namespace
