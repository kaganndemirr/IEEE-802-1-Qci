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

#include "TrafficGenerator.h"
#include "EthernetFrame_m.h"

namespace ieee_802_1_qci {

Define_Module(TrafficGenerator);

void TrafficGenerator::initialize()
{
    const char* target = par("target").stringValue();
    int streamId = par("streamId").intValue();
    int priority = par("priority").intValue();

    EthernetFrame* msg = new EthernetFrame();
    msg->setStreamId(streamId);
    msg->setDst(target);
    msg->setPayload("demo message");
    msg->setPriority(priority);
    send(msg, "out");

    // TODO implement repeating packet generation (with self-messages?)
}

void TrafficGenerator::handleMessage(cMessage *msg)
{
    EthernetFrame* pkt = check_and_cast<EthernetFrame *>(msg);
    EV_INFO << "Packet received src=" << pkt->getSrc()
            << " length=" << pkt->getByteLength()
            << " payload=" << pkt->getPayload();
    delete msg;
}

} //namespace
