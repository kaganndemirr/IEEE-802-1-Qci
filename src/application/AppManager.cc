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

#include "AppManager.h"
#include "EthernetFrame_m.h"

namespace ieee_802_1_qci {

Define_Module(AppManager);

void AppManager::initialize()
{
}

void AppManager::handleMessage(cMessage *msg)
{
    // Message received from apps
    if (msg->arrivedOn("in")) {
        send(msg, "upperOut");
        return;
    }

    EthernetFrame* pkt = check_and_cast<EthernetFrame *>(msg);

    cModule* parent = getParentModule();
    cModule* app;
    int appCount = gateSize("out");
    int pktStreamId = pkt->getStreamId();
    int streamId;

    for (int i=0; i<appCount; i++) {
        app = parent->getSubmodule("app", i);
        if (app->hasPar("streamId")) {
            streamId = app->par("streamId");
        } else {
            streamId = -1;
        }

        if (streamId == -1 || pktStreamId == streamId) {
            send(msg, "out", i);
            return;
        }
    }

    throw cRuntimeError("Received message doesn't target any of the apps");
}

} //namespace
