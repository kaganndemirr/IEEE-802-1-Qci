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

#include "../TSNPacket_m.h"
#include "../../application/EthernetFrame_m.h"
#include "../../utils/Table.h"
#include "StreamGate.h"

namespace ieee_802_1_qci {

Define_Module(StreamGate);

void StreamGate::initialize()
{
    mClock = check_and_cast<Clock*> (getParentModule()->getParentModule()->getParentModule()->getSubmodule("clk"));

    handleParameterChange(nullptr);

    addBubbleModule("^");
    addBubbleModule("^.^");
    addBubbleModule("^.^.^");

    std::ostringstream text;
    text << "InstanceId: " << mPar.instanceId;
    getDisplayString().setTagArg("t", 0, text.str().c_str());
}

void StreamGate::handleMessage(cMessage *msg)
{
    TSNPacket* pkt = check_and_cast<TSNPacket *>(msg);
    EthernetFrame* frame = check_and_cast<EthernetFrame *>(pkt->getEncapsulatedPacket());

    std::ostringstream bubbleText;

    // Drop packets when gate is closed
    if (mPar.closedDueToInvalidRx || mPar.closedDueToOctetsExceeded || !mPar.state) {
        std::string reason = (
            mPar.closedDueToInvalidRx ? "InvalidRx"
            : (mPar.closedDueToOctetsExceeded ? "OctetsExceeded" : "ClosedState")
        );

        EV_WARN << "\u001b[31;1m Packet dropped due to arrival at a closed gate (" << reason
                << ", GCL OP: " << mPar.opIndex << ") \u001b[0m" << endl;

        bubbleText << "DROP[" << reason << "]";
        bubble(bubbleText.str().c_str());

        if (!mPar.state && mPar.closedDueToInvalidRxEnable && !mPar.closedDueToInvalidRx) {
            mPar.closedDueToInvalidRx = true;

            EV_WARN << "\u001b[31m closedDueToInvalidRx = true \u001b[0m" << endl;
        }

        delete msg;
        return;
    }

    if (!mPar.intervalOctetLeft.isNull) {
        int pktSize = frame->getPayloadSize();

        // Drop packets when its bigger than allowed octets
        if (pktSize > mPar.intervalOctetLeft.value) {
            EV_WARN << "\u001b[31;1m Packet dropped due to arrival at a gate that has not enough octet left "
                    << "(GCL OP: " << mPar.opIndex << ") \u001b[0m" << endl;

            bubbleText << "DROP[OctetsExceed]";
            bubble(bubbleText.str().c_str());

            if (mPar.closedDueToOctetsExceededEnable) {
                mPar.closedDueToOctetsExceeded = true;

                EV_WARN << "\u001b[31m closedDueToOctetsExceeded = true \u001b[0m" << endl;
            }

            delete msg;
            return;
        }

        mPar.intervalOctetLeft.value -= pktSize;
    }

    if (!mPar.ipv.isNull) {
        pkt->setIpv(mPar.ipv.value);
        bubbleText << "IPV[" << mPar.ipv.value << "]";
        bubble(bubbleText.str().c_str());
    }

    send(msg, "out");
}

void StreamGate::handleParameterChange(const char *parname)
{
    if (!parname || !strcmp(parname, "instanceId")) {
        mPar.instanceId = readUInt(par("instanceId"), "instanceId");
    }

    if (!parname || !strcmp(parname, "state")) {
        mPar.state = readBool(par("state"), "state");

        cDisplayString& displayString = getDisplayString();
        if (mPar.state) {
            displayString.setTagArg("i", 0, par("iconOpen").stringValue());
        } else {
            displayString.setTagArg("i", 0, par("iconClosed").stringValue());
        }
    }

    if (!parname || !strcmp(parname, "ipv")) {
        mPar.ipv.value = readUIntStr(par("ipv"), "ipv", "null", &mPar.ipv.isNull);
    }

    if (!parname || !strcmp(parname, "closedDueToInvalidRxEnable")) {
        mPar.closedDueToInvalidRxEnable = readBool(par("closedDueToInvalidRxEnable"), "closedDueToInvalidRxEnable");
    }

    if (!parname || !strcmp(parname, "closedDueToInvalidRx")) {
        mPar.closedDueToInvalidRx = readBool(par("closedDueToInvalidRx"), "closedDueToInvalidRx");
    }

    if (!parname || !strcmp(parname, "closedDueToOctetsExceededEnable")) {
        mPar.closedDueToOctetsExceededEnable = readBool(par("closedDueToOctetsExceededEnable"), "closedDueToOctetsExceededEnable");
    }

    if (!parname || !strcmp(parname, "closedDueToOctetsExceeded")) {
        mPar.closedDueToOctetsExceeded = readBool(par("closedDueToOctetsExceeded"), "closedDueToOctetsExceeded");
    }

    if (!parname || !strcmp(parname, "intervalOctetLeft")) {
        mPar.intervalOctetLeft.value = readUIntStr(par("intervalOctetLeft"), "intervalOctetLeft", "", &mPar.intervalOctetLeft.isNull);
    }

    if (!parname || !strcmp(parname, "opIndex")) {
        mPar.opIndex = readUInt(par("opIndex"), "opIndex");
    }
}

bool StreamGate::match(int streamGateId)
{
    return (mPar.instanceId == streamGateId);
}

void StreamGate::initScheduling()
{
    if (scheduleStarted) {
        return;
    }

    scheduleStarted = true;

    cModule* module = getSubmodule("op", 0);
    if (module == nullptr) {
        return;
    }

    GateControlOp* op = check_and_cast<GateControlOp*>(module);
    StreamGateControlOperation gclOp = op->getParams();

    par("opIndex") = "0";
    par("state") = gclOp.state ? "true" : "false";

    if (gclOp.ipv.isNull) {
        par("ipv") = "null";
    } else {
        par("ipv") = std::to_string(gclOp.ipv.value).c_str();
    }

    if (gclOp.intervalOctetMax.isNull) {
        par("intervalOctetLeft") = "";
    } else {
        par("intervalOctetLeft") = std::to_string(gclOp.intervalOctetMax.value).c_str();
    }

    mClock->scheduleCall(this, gclOp.timeInterval, 0);
}

simtime_t StreamGate::tick(int unused)
{
    Enter_Method("tick()");

    int idx = mPar.opIndex + 1;

    cModule* module = getSubmodule("op", idx);
    if (module == nullptr) {
        idx = 0;
        module = getSubmodule("op", idx);
    }

    par("opIndex") = std::to_string(idx).c_str();

    // No GCL found
    if (module == nullptr) {
        scheduleStarted = false;
        return simTime().ZERO;
    }

    GateControlOp* op = check_and_cast<GateControlOp*>(module);
    StreamGateControlOperation gclOp = op->getParams();

    par("state") = gclOp.state ? "true" : "false";

    if (gclOp.ipv.isNull) {
        par("ipv") = "null";
    } else {
        par("ipv") = std::to_string(gclOp.ipv.value).c_str();
    }

    if (gclOp.intervalOctetMax.isNull) {
        par("intervalOctetLeft") = "";
    } else {
        par("intervalOctetLeft") = std::to_string(gclOp.intervalOctetMax.value).c_str();
    }

    return gclOp.timeInterval;
}

} //namespace
