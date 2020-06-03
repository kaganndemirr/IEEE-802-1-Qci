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

#ifndef __IEEE_802_1_QCI_STREAMGATE_H_
#define __IEEE_802_1_QCI_STREAMGATE_H_

#include <omnetpp.h>

#include "../../utils/MultiBubbleModule.h"
#include "GateControlOp.h"
#include "../Clock.h"

using namespace omnetpp;

namespace ieee_802_1_qci {

struct StreamGate_s {
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

class StreamGate : public MultiBubbleModule, public IScheduled
{
  private:
    StreamGate_s mPar;
    Clock* mClock;
    bool scheduleStarted = false;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void handleParameterChange(const char *parname);

  public:
    bool match(int streamGateId);
    void initScheduling();
    simtime_t tick(int unused);
};

} //namespace

#endif
