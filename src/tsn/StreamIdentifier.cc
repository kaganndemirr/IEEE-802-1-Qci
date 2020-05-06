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

#include "StreamIdentifier.h"
#include "ControlPacket_m.h"
#include "../application/EthernetFrame_m.h"

namespace ieee_802_1_qci {

Define_Module(StreamIdentifier);

void StreamIdentifier::initialize()
{
}

void StreamIdentifier::handleMessage(cMessage *msg)
{
    ControlPacket* pkt = check_and_cast<ControlPacket *>(msg);

    if (!pkt) {
        throw cRuntimeError("Received message isn't a ControlPacket");
    }

    EthernetFrame* frame = check_and_cast<EthernetFrame *>(pkt->getEncapsulatedPacket());

    if (!frame) {
        throw cRuntimeError("Received ControlPacket doesn't contain EthernetFrame");
    }

    const char* src = frame->getSrc();
    const char* dst = frame->getDst();
    int streamId = frame->getStreamId();

    // TODO set streamHandle using dst,streamId or src,streamId (see 802.1CB Table-6.1)
    pkt->setStreamHandle(streamId);

    send(msg, "out", msg->getArrivalGate()->getIndex());
}

} //namespace
