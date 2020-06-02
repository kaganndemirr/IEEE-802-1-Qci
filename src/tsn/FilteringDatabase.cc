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

#include "FilteringDatabase.h"

namespace ieee_802_1_qci {

Define_Module(FilteringDatabase);

void FilteringDatabase::initialize()
{
    cXMLElement* db = par("db");

    // Read static entries
    if (db->hasChildren()) {
        const char* isRouter;
        const char* mac;
        const char* attr_port;
        int port;

        for (cXMLElement* elm : db->getChildrenByTagName("entry")) {
            // Skip invalid entry
            if (!elm->hasAttributes())
                continue;

            mac = elm->getAttribute("mac");
            isRouter = elm->getAttribute("router");
            attr_port = elm->getAttribute("port");
            port = atoi(attr_port);

            // Skip invalid entry
            if (!mac || !attr_port || port < 0)
                continue;

            add(std::string(mac), port);

            if (isRouter && !strcmp(isRouter, "1")) {
                addRouter(port);
            }
        }
    }

    WATCH_LIST(mRouterList);
}

void FilteringDatabase::handleMessage(cMessage *msg)
{
}

// Returns matching port number if MAC is on db, otherwise -1
int FilteringDatabase::getPort(std::string mac) {
    auto it = m_db.find(mac);
    if (it == m_db.end())
        return -1;
    return it->second;
}

std::list<int> FilteringDatabase::getRouterPorts() {
    return mRouterList;
}

void FilteringDatabase::add(std::string mac, int port) {
    m_db[mac] = port;
}

void FilteringDatabase::addRouter(int port) {
    mRouterList.push_back(port);
}

} //namespace
