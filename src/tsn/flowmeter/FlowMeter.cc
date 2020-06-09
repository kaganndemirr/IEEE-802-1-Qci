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

    addBubbleModule("^");
    addBubbleModule("^.^");
    addBubbleModule("^.^.^");

    std::ostringstream text;
    text << "InstanceId: " << mPar.instanceId;
    getDisplayString().setTagArg("t", 0, text.str().c_str());
}

void FlowMeter::handleMessage(cMessage *msg)
{
    TSNPacket* pkt = check_and_cast<TSNPacket *>(msg);
    EthernetFrame* frame = check_and_cast<EthernetFrame *>(pkt->getEncapsulatedPacket());

    if (mPar.markAllFramesRed) {
        EV_WARN << "\u001b[31;1m Packet dropped because it couldn't pass the meter (MarkAllFramesRed) \u001b[0m" << endl;
        bubble("DROP[MarkAllFramesRed]");

        delete msg;
        return;
    }

    unsigned int packetSize = frame->getPayloadSize();

    if (tryGreenBucket(packetSize) &&
            (frame->getColor() == 0
            || frame->getColor() == 1
            || !mPar.colorMode)) { // color-blind
        send(msg, "greenOut");
        return;
    }
    else if (tryYellowBucket(packetSize)) {
        if (mPar.dropOnYellow) {
            // Mark red
            frame->setColor(3);
        }

        send(msg, "yellowOut");
        return;
    }

    EV_WARN << "\u001b[31;1m Packet dropped because it couldn't pass the meter (PacketSize) \u001b[0m" << endl;
    bubble("DROP[PacketSize]");

    if (mPar.markAllFramesRedEnable) {
        mPar.markAllFramesRed = true;
        EV_WARN << "\u001b[31m markAllFramesRed = true \u001b[0m" << endl;
    }

    delete msg;
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
