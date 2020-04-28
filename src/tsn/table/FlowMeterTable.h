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

#include "TableUtils.h"

using namespace omnetpp;

namespace ieee_802_1_qci {

struct CommittedInformationRate{
    int value;
    bool isNull;
};

struct CommittedBurstSize{
    int value;
    bool isNull;
};

struct ExcessInformationRate{
    int value;
    bool isNull;
};

struct ExcessBurstSize{
    int value;
    bool isNull;
};

struct FlowMeter{
    unsigned int id;
    CommittedInformationRate cir;
    CommittedBurstSize cbs;
    ExcessInformationRate eir;
    ExcessBurstSize ebs;

    bool couplingFlag;
    bool colorMode; // ?
    bool dropOnYellow;
    bool markAllFramesRedEnable;
};

class FlowMeterTable : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

} //namespace

#endif
