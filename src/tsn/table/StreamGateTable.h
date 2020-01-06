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

#ifndef __IEEE_802_1_QCI_STREAMGATETABLE_H_
#define __IEEE_802_1_QCI_STREAMGATETABLE_H_

#include <omnetpp.h>
#include <vector>
#include <bits/stdc++.h>

#include "TableUtils.h"
#include "../Clock.h"

using namespace omnetpp;

namespace ieee_802_1_qci {

struct IPVSpec {
    int value;
    bool isNull;
};

struct IntervalOctetSpec {
    int value;
    bool isNull;
};

struct StreamGateControlOperation {
    bool state;
    IPVSpec ipv;
    simtime_t timeInterval;
    IntervalOctetSpec intervalOctetMax;
};

struct StreamGate {
    int instanceId;
    bool state;
    IPVSpec ipv;

    bool closedDueToInvalidRxEnable;
    bool closedDueToInvalidRx;
    bool closedDueToOctetsExceededEnable;
    bool closedDueToOctetsExceeded;

    std::vector<StreamGateControlOperation> gateControlList;

    IntervalOctetSpec intervalOctetLeft;
    unsigned int opIndex;
};

class StreamGateTable : public cSimpleModule, public IScheduled
{
private:
    Clock* mClock;
    std::vector<StreamGate> mList;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

public:
    simtime_t tick(int gateId);
    StreamGate* getStreamGate(int gateId);
};

bool compareStreamGate(StreamGate g1, StreamGate g2);

} //namespace

#endif
