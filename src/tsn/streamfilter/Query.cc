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

#include "Query.h"
#include "Element.h"
#include "../TSNPacket_m.h"
#include "../../application/EthernetFrame_m.h"

namespace ieee_802_1_qci {

Define_Module(Query);

void Query::initialize()
{
    // TODO - Generated method body
}

void Query::handleMessage(cMessage *msg)
{
    TSNPacket* pkt = check_and_cast<TSNPacket *>(msg);

    if (!pkt) {
        throw cRuntimeError("Received message isn't a TSNPacket");
    }

    EthernetFrame* frame = check_and_cast<EthernetFrame *>(pkt->getEncapsulatedPacket());

    if (!frame) {
        throw cRuntimeError("Received ControlPacket doesn't contain EthernetFrame");
    }

    int streamHandle = pkt->getStreamHandle();
    int priority = frame->getPriority();

    cModule* module;
    Element* elm;
    int count = gateSize("out");

    for (int i=1; i<count; i++) {
        module = getParentModule()->getSubmodule("elm", i);
        if (module == nullptr) {
            throw cRuntimeError("Size of gate and number of elm don't match!");
        }

        elm = check_and_cast<Element*>(module);
        if (elm->match(streamHandle, priority)) {
            send(msg, "out", i);
            return;
        }
    }

    // Send to direct connection
    bubble("PASS[No StreamFilter]");
    send(msg, "out", 0);
}

} //namespace
