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

#include "RelayOut.h"
#include "TSNPacket_m.h"

namespace ieee_802_1_qci {

Define_Module(RelayOut);

void RelayOut::initialize()
{
}

void RelayOut::handleMessage(cMessage *msg)
{
    TSNPacket* tsnPkt = check_and_cast<TSNPacket *>(msg);
    if (!tsnPkt) {
        throw cRuntimeError("Received message isn't a TSNPacket");
    }

    int port = tsnPkt->getPortOut();
    int portCount = par("portCount");
    if (port < 0 || port >= portCount) {
        EV_WARN << "Invalid port: " << msg->getDisplayString();
        delete msg;
        return;
    }

    send(msg, "out", port);
}

} //namespace
