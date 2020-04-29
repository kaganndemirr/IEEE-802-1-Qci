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

#ifndef __IEEE_802_1_QCI_FLOWMETERTABLE_H_
#define __IEEE_802_1_QCI_FLOWMETERTABLE_H_

#include <omnetpp.h>
#include <vector>
#include <bits/stdc++.h>

using namespace omnetpp;

namespace ieee_802_1_qci {

struct FlowMeter {
    int instanceId;

    int committedInformationRate; // green tokens (octets) per second
    int committedBurstSize; // green bucket size
    int excessInformationRate; // yellow tokens (octets) per second
    int excessBurstSize; // yellow bucket size

    bool couplingFlag; // true: use overflowing green tokens as yellow tokens
    bool colorMode; // true: color aware, false: color blind
    bool dropOnYellow;
    bool markAllFramesRedEnable;
};

class FlowMeterTable : public cSimpleModule
{
private:
    std::vector<FlowMeter> mList;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

public:
    FlowMeter* getFlowMeter(int fmId);
};

bool compareFlowMeter(FlowMeter fm1, FlowMeter fm2);

} //namespace

#endif
