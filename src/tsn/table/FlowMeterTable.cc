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

#include "FlowMeterTable.h"

#include "../../utils/Table.h"

namespace ieee_802_1_qci {

Define_Module(FlowMeterTable);

void FlowMeterTable::initialize()
{
    FlowMeter flowMeter;

    cXMLElement* table = par("table");

    if(table->hasChildren()) {
        for (cXMLElement* elm : table->getChildrenByTagName("FlowMeter")){
            flowMeter.instanceId = readXMLUInt(elm->getFirstChildWithTag("InstanceId"), "InstanceId");

            flowMeter.committedInformationRate = readXMLUInt(elm->getFirstChildWithTag("CommittedInformationRate"), "CommittedInformationRate");
            flowMeter.committedBurstSize = readXMLUInt(elm->getFirstChildWithTag("CommittedBurstSize"), "CommittedBurstSize");
            flowMeter.excessInformationRate = readXMLUInt(elm->getFirstChildWithTag("ExcessInformationRate"), "ExcessInformationRate");
            flowMeter.excessBurstSize = readXMLUInt(elm->getFirstChildWithTag("ExcessBurstSize"), "ExcessBurstSize");

            flowMeter.couplingFlag = readXMLBool(elm->getFirstChildWithTag("CouplingFlag"), "CouplingFlag");
            flowMeter.colorMode = readXMLBool(elm->getFirstChildWithTag("ColorMode"), "ColorMode");
            flowMeter.dropOnYellow = readXMLBool(elm->getFirstChildWithTag("DropOnYellow"), "DropOnYellow");
            flowMeter.markAllFramesRed = false;
            flowMeter.markAllFramesRedEnable = readXMLBool(elm->getFirstChildWithTag("MarkAllFramesRedEnable"), "MarkAllFramesRedEnable");

            greenBuckets[flowMeter.instanceId] = new TokenBucket(flowMeter.committedBurstSize, 0);
            yellowBuckets[flowMeter.instanceId] = new TokenBucket(flowMeter.excessBurstSize, 0);

            mList.push_back(flowMeter);
        }

        std::sort(mList.begin(), mList.end(), compareFlowMeter);
    }

    mLastUpdate = simTime();
}

void FlowMeterTable::handleMessage(cMessage *msg)
{
}


void FlowMeterTable::updateBuckets()
{
    if (mLastUpdate == simTime()) {
        return;
    }

    double delta = (simTime() - mLastUpdate).dbl();
    mLastUpdate = simTime();

    for (FlowMeter meter : mList) {
        TokenBucket* greenBucket = greenBuckets.at(meter.instanceId);
        TokenBucket* yellowBucket = yellowBuckets.at(meter.instanceId);

        double overflow = greenBucket->add(meter.committedInformationRate * delta);

        if (meter.couplingFlag) {
            yellowBucket->add(meter.excessInformationRate * delta + overflow);
        }
        else {
            yellowBucket->add(meter.excessInformationRate * delta);
        }
    }
}

FlowMeter* FlowMeterTable::getFlowMeter(int fmId) {
    for (int i=0, listSize = mList.size(); i<listSize; i++) {
        if (mList[i].instanceId == fmId) {
            return &mList[i];
        }
    }
    return NULL;
}

bool FlowMeterTable::tryGreenBucket(int fmId, int tokens) {
    TokenBucket* bucket = greenBuckets.at(fmId);

    updateBuckets();

    return bucket->remove(tokens) == 0;
}

bool FlowMeterTable::tryYellowBucket(int fmId, int tokens) {
    TokenBucket* bucket = yellowBuckets.at(fmId);

    updateBuckets();

    return bucket->remove(tokens) == 0;
}

bool compareFlowMeter(FlowMeter fm1, FlowMeter fm2) {
    return (fm1.instanceId < fm2.instanceId);
}

} //namespace
