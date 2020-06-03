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

#include "TSNEncapsulator.h"
#include "../application/EthernetFrame_m.h"
#include "TSNPacket_m.h"

namespace ieee_802_1_qci {

Define_Module(TSNEncapsulator);

void TSNEncapsulator::initialize()
{
}

void TSNEncapsulator::handleMessage(cMessage *msg)
{
    EthernetFrame* pkt = dynamic_cast<EthernetFrame *>(msg);
    if (pkt) {
        TSNPacket* tsnPkt = new TSNPacket();

        tsnPkt->encapsulate(pkt);
        tsnPkt->setPortIn(msg->getArrivalGate()->getIndex());

        send(tsnPkt, "out");
    } else {
        EV_WARN << "Unknown message received: " << msg->getDisplayString();
        delete msg;
    }
}

} //namespace
