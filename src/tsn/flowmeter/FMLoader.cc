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

        int meterCount = children.size();
        int meterIdx = 0;

        int bucketCount = meterCount * 2;
        int bucketIdx = 0;

        int queryOutIdx = 1;
        int nextInIdx = 1;

        cModuleType* moduleType = cModuleType::get("ieee_802_1_qci.tsn.flowmeter.FlowMeter");
        cModuleType* bucketType = cModuleType::get("ieee_802_1_qci.tsn.flowmeter.Bucket");
        cModule* tableModule = getModuleByPath("^.relay.flowMeter");
        cModule* queryModule = getModuleByPath("^.relay.flowMeter.query");
        cModule* nextModule = getModuleByPath("^.relay.flowMeter.next");
        cModule* meter;
        cModule* greenBucket;
        cModule* yellowBucket;

        queryModule->setGateSize("out", meterCount + 1);
        nextModule->setGateSize("in", bucketCount + 1);

        for (cXMLElement* elm : children) {
            // Create meter module
            meter = moduleType->create("meter", tableModule, meterCount, meterIdx++);
            meter->par("instanceId") = elm->getFirstChildWithTag("InstanceId")->getNodeValue();
            meter->par("committedInformationRate") = elm->getFirstChildWithTag("CommittedInformationRate")->getNodeValue();
            meter->par("committedBurstSize") = elm->getFirstChildWithTag("CommittedBurstSize")->getNodeValue();
            meter->par("excessInformationRate") = elm->getFirstChildWithTag("ExcessInformationRate")->getNodeValue();
            meter->par("excessBurstSize") = elm->getFirstChildWithTag("ExcessBurstSize")->getNodeValue();
            meter->par("couplingFlag") = elm->getFirstChildWithTag("CouplingFlag")->getNodeValue();
            meter->par("colorMode") = elm->getFirstChildWithTag("ColorMode")->getNodeValue();
            meter->par("dropOnYellow") = elm->getFirstChildWithTag("DropOnYellow")->getNodeValue();
            meter->par("markAllFramesRedEnable") = elm->getFirstChildWithTag("MarkAllFramesRedEnable")->getNodeValue();
            meter->par("markAllFramesRed") = "false";
            meter->finalizeParameters();

            // Create green bucket module
            greenBucket = bucketType->create("bucket", tableModule, bucketCount, bucketIdx++);
            greenBucket->par("color") = "green";
            greenBucket->finalizeParameters();

            // Create yellow bucket module
            yellowBucket = bucketType->create("bucket", tableModule, bucketCount, bucketIdx++);
            yellowBucket->par("color") = "yellow";
            yellowBucket->finalizeParameters();

            // Connect modules
            queryModule->gate("out", queryOutIdx++)->connectTo(meter->gate("in"));
            meter->gate("greenOut")->connectTo(greenBucket->gate("in"));
            meter->gate("yellowOut")->connectTo(yellowBucket->gate("in"));
            greenBucket->gate("out")->connectTo(nextModule->gate("in", nextInIdx++));
            yellowBucket->gate("out")->connectTo(nextModule->gate("in", nextInIdx++));

            // Build green bucket module
            greenBucket->buildInside();
            greenBucket->scheduleStart(simTime());

            // Build yellow bucket module
            yellowBucket->buildInside();
            yellowBucket->scheduleStart(simTime());

            // Build meter module
            meter->buildInside();
            meter->scheduleStart(simTime());
        }
    }
}

void FMLoader::handleMessage(cMessage *msg)
{
}

} //namespace
