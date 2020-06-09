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

#include "Sink.h"
#include "EthernetFrame_m.h"
#include <ostream>
#include <iomanip>

namespace ieee_802_1_qci {

Define_Module(Sink);

void Sink::initialize()
{
    handleParameterChange(nullptr);
}

void Sink::handleMessage(cMessage *msg)
{
    EthernetFrame* pkt = check_and_cast<EthernetFrame *>(msg);

    double delay = (simTime().dbl() - pkt->getCreationTime()) * 1000;
    const char* source = pkt->getSrc();
    int streamId = pkt->getStreamId();
    int payloadSize = pkt->getPayloadSize();

    EV_INFO << "\u001b[32m Packet received src=" << source
            << " streamid=" << streamId
            << " length=" << payloadSize
            << " delay=" << delay << "ms \u001b[0m" << endl;
    if (mSource[0] && strcmp(mSource, source)) {
        EV_ERROR << "Invalid Source" << endl;
    }
    if (mStreamId != -1 && streamId != mStreamId) {
        EV_ERROR << "Invalid StreamId" << endl;
    }

    std::ostringstream bubbleStr;
    bubbleStr << std::fixed << std::setprecision(4) << "Delay " << delay << "ms";
    mBubbleModule->bubble(bubbleStr.str().c_str());

    delete msg;
}

void Sink::handleParameterChange(const char *parname)
{
    if (!parname || strcmp(parname, "bubblePath") == 0) {
        const char* bubblePath = par("bubblePath").stringValue();
        mBubbleModule = getModuleByPath(bubblePath);

        if (!mBubbleModule) {
            mBubbleModule = getModuleByPath(".");
        }
    }

    if (!parname || strcmp(parname, "source") == 0) {
        mSource = par("source").stringValue();
    }

    if (!parname || strcmp(parname, "streamId") == 0) {
        mStreamId = par("streamId").intValue();
    }
}

} //namespace
