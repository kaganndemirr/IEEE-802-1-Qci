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
#include <list>

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

    int portIn = pkt->getPortIn();

    // Forward packet to matching port
    const char* dst = frame->getDst();
    int port = m_fdb->getPort(std::string(dst));
    int portCount = gateSize("out");

    if (port != -1) {
        if (port < 0 || port >= portCount) {
            EV_ERROR << "Invalid port: " << msg->getDisplayString();
            delete msg;
            return;
        }

        send(msg, "out", port);
        return;
    }

    // Broadcast message to other routers
    std::list<int> routerPorts = m_fdb->getRouterPorts();
    for (int port : routerPorts) {
        if (port < 0 || port >= portCount) {
            EV_ERROR << "Invalid port: " << msg->getDisplayString();
            delete msg;
            return;
        }

        // Skip that the message received from
        if (port == portIn) {
            continue;
        }

        // Send duplicate message
        send(msg->dup(), "out", port);
    }

    // Delete original message
    delete msg;
}

} //namespace
