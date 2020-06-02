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

#include "FlowMeter.h"
#include "../TSNPacket_m.h"
#include "../../application/EthernetFrame_m.h"
#include "../../utils/Table.h"

namespace ieee_802_1_qci {

Define_Module(FlowMeter);

void FlowMeter::initialize()
{
    mLastUpdate = simTime();

    WATCH(greenBucket);
    WATCH(yellowBucket);

    handleParameterChange(nullptr);
}

void FlowMeter::handleMessage(cMessage *msg)
{
    TSNPacket* pkt = check_and_cast<TSNPacket *>(msg);

    if (!pkt) {
        throw cRuntimeError("Received message isn't a TSNPacket");
    }

    EthernetFrame* frame = check_and_cast<EthernetFrame *>(pkt->getEncapsulatedPacket());

    if (!frame) {
        throw cRuntimeError("Received TSNPacket doesn't contain EthernetFrame");
    }

    std::ostringstream bubbleText;
    bubbleText << "FlowMeter[" << mPar.instanceId << "]";

    if (mPar.markAllFramesRed) {
        EV_WARN << "Packet dropped because it couldn't pass the meter (MarkAllFramesRed"
                << ", ID: " << mPar.instanceId << ")";

        bubbleText << " DROP[MarkAllFramesRed]";
        bubble(bubbleText.str().c_str());

        delete msg;
        return;
    }

    unsigned int packetSize = frame->getPayloadSize();

    if (tryGreenBucket(packetSize) &&
            (pkt->getColor() == 0
            || pkt->getColor() == 1
            || !mPar.colorMode)) { // color-blind
        pkt->setColor(1);
    }
    else if (tryYellowBucket(packetSize) && !mPar.dropOnYellow) {
        pkt->setColor(2);
        bubbleText << " YELLOW";
    }
    else {
        std::string reason = (mPar.dropOnYellow ? "DropOnYellow" : "PacketSize");

        EV_WARN << "Packet dropped because it couldn't pass the meter (" << reason
                << ", ID: " << mPar.instanceId << ")";

        bubbleText << " DROP[" << reason << "]";
        bubble(bubbleText.str().c_str());

        if (!mPar.dropOnYellow && mPar.markAllFramesRedEnable) {
            mPar.markAllFramesRed = true;

            EV_WARN << "markAllFramesRed = true";
        }

        delete msg;
        return;
    }

    bubbleText << " PASS";
    bubble(bubbleText.str().c_str());

    int meterCount = pkt->getFlowMeterIdsArraySize();
    int meterIdx = pkt->getMeterIdx();

    // This is the last flowmeter
    if (meterIdx >= meterCount) {
        send(msg, "out");
        return;
    }

    cModule* queryModule = getModuleByPath("^.query");
    cGate* directIn = queryModule->gate("directIn");

    // Forward back to query for remaining flowmeters
    sendDirect(msg, directIn);
}

void FlowMeter::handleParameterChange(const char *parname)
{
    if (!parname || !strcmp(parname, "instanceId")) {
        mPar.instanceId = readUInt(par("instanceId"), "instanceId");
    }

    if (!parname || !strcmp(parname, "committedInformationRate")) {
        updateBucket();
        mPar.committedInformationRate = readUInt(par("committedInformationRate"), "committedInformationRate");
    }

    if (!parname || !strcmp(parname, "committedBurstSize")) {
        mPar.committedBurstSize = readUInt(par("committedBurstSize"), "committedBurstSize");
        greenBucket.setCapacity(mPar.committedBurstSize);
    }

    if (!parname || !strcmp(parname, "excessInformationRate")) {
        updateBucket();
        mPar.excessInformationRate = readUInt(par("excessInformationRate"), "excessInformationRate");
    }

    if (!parname || !strcmp(parname, "excessBurstSize")) {
        mPar.excessBurstSize = readUInt(par("excessBurstSize"), "excessBurstSize");
        yellowBucket.setCapacity(mPar.excessBurstSize);
    }

    if (!parname || !strcmp(parname, "couplingFlag")) {
        mPar.couplingFlag = readBool(par("couplingFlag"), "couplingFlag");
    }

    if (!parname || !strcmp(parname, "colorMode")) {
        mPar.colorMode = readBool(par("colorMode"), "colorMode");
    }

    if (!parname || !strcmp(parname, "dropOnYellow")) {
        mPar.dropOnYellow = readBool(par("dropOnYellow"), "dropOnYellow");
    }

    if (!parname || !strcmp(parname, "markAllFramesRed")) {
        mPar.markAllFramesRed = readBool(par("markAllFramesRed"), "markAllFramesRed");
    }

    if (!parname || !strcmp(parname, "markAllFramesRedEnable")) {
        mPar.markAllFramesRedEnable = readBool(par("markAllFramesRedEnable"), "markAllFramesRedEnable");
    }
}

bool FlowMeter::match(int meterId) {
    return (mPar.instanceId == meterId);
}

void FlowMeter::updateBucket()
{
    if (mLastUpdate == simTime()) {
        return;
    }

    double delta = (simTime() - mLastUpdate).dbl();
    mLastUpdate = simTime();

    double overflow = greenBucket.add(mPar.committedInformationRate * delta);

    if (mPar.couplingFlag) {
        yellowBucket.add(mPar.excessInformationRate * delta + overflow);
    }
    else {
        yellowBucket.add(mPar.excessInformationRate * delta);
    }
}

bool FlowMeter::tryGreenBucket(int tokens) {
    updateBucket();
    return greenBucket.remove(tokens) == 0;
}

bool FlowMeter::tryYellowBucket(int tokens) {
    updateBucket();
    return yellowBucket.remove(tokens) == 0;
}

} //namespace
