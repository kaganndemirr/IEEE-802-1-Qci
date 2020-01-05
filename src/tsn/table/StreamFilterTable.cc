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
    std::vector<FilterSpecification> filters;

    cXMLElement* table = par("table");
    cXMLElement* filterList;

    // Read stream filters
    if (table->hasChildren()) {
        for (cXMLElement* elm : table->getChildrenByTagName("StreamFilter")) {
            int instanceId = atoi(elm->getFirstChildWithTag("InstanceId")->getNodeValue());
            if (instanceId < 0)
                throw omnetpp::cRuntimeError("InstanceId cannot be negative, given: %d", instanceId);

            sf.instanceId = instanceId;

            const char* streamHandle = elm->getFirstChildWithTag("StreamHandle")->getNodeValue();
            int streamHandleN;
            if (strcmp(streamHandle, "*") == 0) {
                streamHandleN = atoi(streamHandle);
                if (streamHandleN < 0)
                    throw omnetpp::cRuntimeError("StreamHandle cannot be negative, given: %d", streamHandleN);

                sh.value = streamHandleN;
                sh.wildcard = 0;
            } else {
                sh.wildcard = '*';
            }

            sf.streamHandle = sh;

            const char* priority = elm->getFirstChildWithTag("Priority")->getNodeValue();
            int priorityN;
            if (strcmp(priority, "*") == 0) {
                priorityN = atoi(priority);
                if (priorityN < 0)
                    throw omnetpp::cRuntimeError("Priority cannot be negative, given: %d", priorityN);

                ps.value = priorityN;
                ps.wildcard = 0;
            } else {
                ps.wildcard = '*';
            }

            sf.priority = ps;

            int streamGateId = atoi(elm->getFirstChildWithTag("StreamGateId")->getNodeValue());
            if (streamGateId < 0)
                throw omnetpp::cRuntimeError("StreamGateId cannot be negative, given: %d", streamGateId);

            sf.streamGateId = streamGateId;

            filters = std::vector<FilterSpecification>();

            filterList = elm->getFirstChildWithTag("FilterList");
            if (filterList && filterList->hasChildren()) {
                for (cXMLElement* filter : filterList->getChildrenByTagName("Filter")) {
                    if (!filter->hasAttributes())
                        continue;

                    const char* maxSDUSize = filter->getAttribute("maxSDUSize");
                    if (maxSDUSize) {
                        int maxSDUSizeN = atoi(maxSDUSize);
                        if (maxSDUSizeN < 0)
                            throw omnetpp::cRuntimeError("maxSDUSize cannot be negative, given: %d", maxSDUSizeN);

                        filters.push_back(FilterSpecification{
                            maxSDUSizeN, -1
                        });
                    } else {
                        const char* flowMeterId = filter->getAttribute("flowMeterId");
                        if (flowMeterId) {
                            int flowMeterIdN = atoi(flowMeterId);
                            if (flowMeterIdN < 0)
                                throw omnetpp::cRuntimeError("flowMeterId cannot be negative, given: %d", flowMeterIdN);

                            filters.push_back(FilterSpecification{
                                -1, flowMeterIdN
                            });
                        }

                    }
                }
            }

            sf.filters = filters;

            mList.push_back(sf);
        }
    }

    std::sort(mList.begin(), mList.end(), compareStreamFilter);
}

void StreamFilterTable::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

StreamFilter* StreamFilterTable::getStreamFilter(int streamId, int priority) {
    StreamFilter* f;
    for (int i=0, listSize = mList.size(); i<listSize; i++) {
        f = &mList[i];
        if (f->streamHandle.wildcard == '*' || f->streamHandle.value == streamId) {
            if (f->priority.wildcard == '*' || f->priority.value == priority) {
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
