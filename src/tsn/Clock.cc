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

#include "Clock.h"

namespace ieee_802_1_qci {

Define_Module(Clock);

void Clock::initialize()
{
    mTickInterval = par("tickInterval");
}

void Clock::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        tick();
        delete msg;
        return;
    }

    throw omnetpp::cRuntimeError("Invalid packet received!");
}

void Clock::tick() {
    simtime_t currentTick = (simTime() / mTickInterval) * mTickInterval;
    simtime_t interval = simTime().ZERO;

    mIsTickScheduled.erase(currentTick.str());

    for (ScheduledCall& sc : mScheduleList) {
        if (currentTick == sc.nextTime) {
            interval = sc.listener->tick(sc.param);
            if (interval.isZero()) {
                // TODO remove from list
                continue;
            }

            sc.nextTime += interval;
            scheduleTick(sc.nextTime);
        }
    }
}

void Clock::scheduleTick(simtime_t tick)
{
    std::string tickStr = tick.str();
    if (mIsTickScheduled.find(tickStr) == mIsTickScheduled.end()) {
        mIsTickScheduled[tickStr] = true;
        scheduleAt(tick, new cMessage("tick"));
    }
}

void Clock::scheduleCall(IScheduled* listener, simtime_t interval, int param) {
    simtime_t currentTime = (simTime() / mTickInterval) * mTickInterval;
    simtime_t intervalTime = (interval / mTickInterval) * mTickInterval;
    simtime_t nextTime = currentTime + intervalTime;

    Enter_Method_Silent();
    scheduleTick(nextTime);
    mScheduleList.push_back(ScheduledCall {
        listener,
        nextTime,
        param
    });
}

} //namespace
