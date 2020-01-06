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

#ifndef __IEEE_802_1_QCI_CLOCK_H_
#define __IEEE_802_1_QCI_CLOCK_H_

#include <omnetpp.h>
#include <vector>
#include <unordered_map>

using namespace omnetpp;

namespace ieee_802_1_qci {

class IScheduled {
public:
    virtual void tick() = 0;
};

struct ScheduledCall {
    IScheduled* callback;
    simtime_t interval;
    simtime_t nextTime;
};

class Clock : public cSimpleModule
{
  private:
    simtime_t mTickInterval;

    std::unordered_map<std::string, bool> mIsTickScheduled;
    std::vector<ScheduledCall> mScheduleList;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    void callScheduled();

  public:
    void scheduleCall(IScheduled* callback, simtime_t interval);
};

} //namespace

#endif
