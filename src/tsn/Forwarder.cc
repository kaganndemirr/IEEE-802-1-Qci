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

#include "Forwarder.h"
#include "../application/EthernetFrame_m.h"
#include "TSNPacket_m.h"

namespace ieee_802_1_qci {

Define_Module(Forwarder);

void Forwarder::initialize()
{
    m_fdb = check_and_cast<FilteringDatabase*> (getParentModule()->getParentModule()->getSubmodule("fdb"));
}

void Forwarder::handleMessage(cMessage *msg)
{
    TSNPacket* pkt = check_and_cast<TSNPacket *>(msg);

    if (!pkt) {
        throw cRuntimeError("Received message isn't a TSNPacket");
    }

    EthernetFrame* frame = check_and_cast<EthernetFrame *>(pkt->getEncapsulatedPacket());

    if (!frame) {
        throw cRuntimeError("Received TSNPacket doesn't contain EthernetFrame");
    }

    // Forward packet to matching port
    const char* dst = frame->getDst();
    int port = m_fdb->getPort(std::string(dst));

    if (port != -1) {
        pkt->setPortOut(port);
        send(pkt, "out");
    } else {
        EV_WARN << "No matching entry found: " << msg->getDisplayString();
        delete msg;
    }
}

} //namespace
