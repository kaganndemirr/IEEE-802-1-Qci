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

#include "Element.h"
#include "../TSNPacket_m.h"
#include "../../application/EthernetFrame_m.h"
#include "../../utils/Table.h"

namespace ieee_802_1_qci {

Define_Module(Element);

void Element::initialize()
{
    handleParameterChange(nullptr);
}

void Element::handleMessage(cMessage *msg)
{
    TSNPacket* pkt = check_and_cast<TSNPacket *>(msg);

    if (!pkt) {
        throw cRuntimeError("Received message isn't a TSNPacket");
    }

    EthernetFrame* frame = check_and_cast<EthernetFrame *>(pkt->getEncapsulatedPacket());

    if (!frame) {
        throw cRuntimeError("Received ControlPacket doesn't contain EthernetFrame");
    }

    std::ostringstream bubbleText;

    bubbleText << "StreamFilter[" << mPar.instanceId << "]";

    // Drop packets when stream blocked
    if (mPar.streamBlockedDueToOversizeFrame) {
        EV_WARN << "Packet dropped because the stream is blocked (OversizeFrame"
                << ", ID: " << mPar.instanceId << ")";

        bubbleText << " DROP[streamBlockedDueToOversizeFrame]";
        bubble(bubbleText.str().c_str());

        delete msg;
        return;
    }

    // Drop packets > maxSDUSize
    if (mPar.maxSDUSize.isActive) {
        unsigned int pktSize = frame->getPayloadSize();
        if (pktSize > mPar.maxSDUSize.value) {
            EV_WARN << "Packet dropped because it exceeds MaxSDUSize (ID: " << mPar.instanceId
                    << ", " << pktSize << " > " << mPar.maxSDUSize.value << ")";

            bubbleText << " DROP[MaxSDUSize]";
            bubble(bubbleText.str().c_str());

            if (mPar.streamBlockedDueToOversizeFrameEnable) {
                mPar.streamBlockedDueToOversizeFrame = true;

                EV_WARN << "streamBlockedDueToOversizeFrame = true";
            }

            delete msg;
            return;
        }
    }

    pkt->setStreamGateId(mPar.streamGateId);
    pkt->setFlowMeterIdsArraySize(mPar.flowMeters.size());

    int idx = 0;
    for (auto meterId : mPar.flowMeters) {
        pkt->setFlowMeterIds(idx++, meterId);
    }

    bubbleText << " PASS";
    bubble(bubbleText.str().c_str());

    send(msg, "out");
}

void Element::handleParameterChange(const char *parname)
{
    if (!parname || !strcmp(parname, "instanceId")) {
        mPar.instanceId = readUInt(par("instanceId"), "instanceId");
    }

    if (!parname || !strcmp(parname, "streamHandle")) {
        mPar.streamHandle.value = readUIntStr(par("streamHandle"), "streamHandle", "*", &mPar.streamHandle.isWildcard);
    }

    if (!parname || !strcmp(parname, "priority")) {
        mPar.priority.value = readUIntStr(par("priority"), "priority", "*", &mPar.priority.isWildcard);
    }

    if (!parname || !strcmp(parname, "streamGateId")) {
        mPar.streamGateId = readUInt(par("streamGateId"), "streamGateId");
    }

    if (!parname || !strcmp(parname, "maxSDUSize")) {
        std::string maxSDUSize = par("maxSDUSize");

        if (maxSDUSize.size() > 0) {
            mPar.maxSDUSize.value = readUInt(par("maxSDUSize"), "maxSDUSize");
            mPar.maxSDUSize.value = true;
        } else {
            mPar.maxSDUSize.value = 0;
            mPar.maxSDUSize.isActive = false;
        }
    }

    if (!parname || !strcmp(parname, "flowMeters")) {
        std::string str = par("flowMeters");
        size_t pos = 0;
        std::string token;

        mPar.flowMeters.clear();

        while ((pos = str.find(" ")) != std::string::npos) {
            token = str.substr(0, pos);

            if (token.size() > 0) {
                mPar.flowMeters.push_back(readUInt(token.c_str(), "FlowMeter id"));
            }

            str.erase(0, pos + 1);
        }

        mPar.flowMeters.push_back(readUInt(str.c_str(), "FlowMeter id"));
    }

    if (!parname || !strcmp(parname, "streamBlockedDueToOversizeFrameEnable")) {
        const char* val = par("streamBlockedDueToOversizeFrameEnable");
        mPar.streamBlockedDueToOversizeFrameEnable = readBool(val, "streamBlockedDueToOversizeFrameEnable");
    }

    if (!parname || !strcmp(parname, "streamBlockedDueToOversizeFrame")) {
        const char* val = par("streamBlockedDueToOversizeFrame");
        mPar.streamBlockedDueToOversizeFrame = readBool(val, "streamBlockedDueToOversizeFrame");
    }
}

bool Element::match(int streamId, int priority) {
    return (mPar.streamHandle.isWildcard || mPar.streamHandle.value == streamId)
            && (mPar.priority.isWildcard || mPar.priority.value == priority);
}

} //namespace
