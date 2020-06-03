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

#include "NetworkController.h"
#include "../application/EthernetFrame_m.h"

namespace ieee_802_1_qci {

Define_Module(NetworkController);

void NetworkController::initialize()
{
    addr = par("addr").stringValue();
}

void NetworkController::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("upperIn")) {
        EthernetFrame* frame = dynamic_cast<EthernetFrame *>(msg);

        // Set source MAC address if null
        if (frame) {
            const char* src = frame->getSrc();
            if (!src || !src[0]) {
                frame->setSrc(addr);
            }
        }

        send(msg, "phys$o");
    } else if (msg->arrivedOn("phys$i")) {
        send(msg, "upperOut");
    }
}

} //namespace
