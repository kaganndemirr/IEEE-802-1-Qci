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

#include "StreamFilterTable.h"

namespace ieee_802_1_qci {

Define_Module(StreamFilterTable);

void StreamFilterTable::initialize()
{
    StreamFilter sf;
    StreamHandleSpec sh;
    PrioritySpec ps;
    MaxSDUSizeSpec maxSDUSize;
    std::vector<int> flowMeters;

    cXMLElement* table = par("table");
    cXMLElement* filterList;
    cXMLElement* maxSDUSizeTag;

    // Read stream filters
    if (table->hasChildren()) {
        for (cXMLElement* elm : table->getChildrenByTagName("StreamFilter")) {
            sf.instanceId = readXMLUInt(elm->getFirstChildWithTag("InstanceId"), "InstanceId");

            sh.value = readXMLUIntStr(elm->getFirstChildWithTag("StreamHandle"), "StreamHandle", "*", &sh.isWildcard);
            sf.streamHandle = sh;

            ps.value = readXMLUIntStr(elm->getFirstChildWithTag("Priority"), "Priority", "*", &ps.isWildcard);
            sf.priority = ps;

            sf.streamGateId = readXMLUInt(elm->getFirstChildWithTag("StreamGateId"), "StreamGateId");

            flowMeters = std::vector<int>();

            filterList = elm->getFirstChildWithTag("FilterList");
            if (filterList && filterList->hasChildren()) {
                // There is only one MaxSDUSize allowed in filters
                maxSDUSizeTag = filterList->getFirstChildWithTag("MaxSDUSize");
                if (maxSDUSizeTag) {
                    maxSDUSize.value = readXMLUInt(maxSDUSizeTag, "MaxSDUSize");
                    maxSDUSize.isActive = true;
                } else {
                    maxSDUSize.value = 0;
                    maxSDUSize.isActive = false;
                }

                for (cXMLElement* filter : filterList->getChildrenByTagName("FlowMeter")) {
                    flowMeters.push_back(readXMLUInt(filter, "FlowMeter id"));
                }
            }

            sf.streamBlockedDueToOversizeFrameEnable = readXMLBool(elm->getFirstChildWithTag("StreamBlockedDueToOversizeFrameEnable"), "StreamBlockedDueToOversizeFrameEnable");
            sf.streamBlockedDueToOversizeFrame = false;

            sf.maxSDUSize = maxSDUSize;
            sf.flowMeters = flowMeters;

            mList.push_back(sf);
        }

        std::sort(mList.begin(), mList.end(), compareStreamFilter);
    }
}

void StreamFilterTable::handleMessage(cMessage *msg)
{
}

StreamFilter* StreamFilterTable::getStreamFilter(int streamId, int priority) {
    StreamFilter* f;
    for (int i=0, listSize = mList.size(); i<listSize; i++) {
        f = &mList[i];
        if (f->streamHandle.isWildcard || f->streamHandle.value == streamId) {
            if (f->priority.isWildcard || f->priority.value == priority) {
                return f;
            }
        }
    }
    return NULL;
}

bool compareStreamFilter(StreamFilter f1, StreamFilter f2) {
    return (f1.instanceId < f2.instanceId);
}

} //namespace
