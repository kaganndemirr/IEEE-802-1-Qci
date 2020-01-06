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
    mTickInterval = simTime().parse(par("tickInterval").stringValue());
}

void Clock::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        callScheduled();
        delete msg;
        return;
    }

    throw omnetpp::cRuntimeError("Invalid packet received!");
}

void Clock::callScheduled() {
    simtime_t currentTick = (simTime() / mTickInterval) * mTickInterval;

    mIsTickScheduled.erase(currentTick.str());

    for (ScheduledCall& sc : mScheduleList) {
        if (currentTick == sc.nextTime) {
            sc.callback->tick();
            sc.nextTime += sc.interval;

            if (mIsTickScheduled.find(sc.nextTime.str()) == mIsTickScheduled.end()) {
                mIsTickScheduled[sc.nextTime.str()] = true;
                scheduleAt(sc.nextTime, new cMessage("tick"));
            }
        }
    }
}

void Clock::scheduleCall(IScheduled* callback, simtime_t interval) {
    simtime_t currentTick = (simTime() / mTickInterval) * mTickInterval;
    simtime_t intervalTick = (interval / mTickInterval) * mTickInterval;
    simtime_t nextTime = currentTick + intervalTick;

    Enter_Method_Silent();

    if (mIsTickScheduled.find(nextTime.str()) == mIsTickScheduled.end()) {
        mIsTickScheduled[nextTime.str()] = true;
        scheduleAt(nextTime, new cMessage("tick"));
    }

    mScheduleList.push_back(ScheduledCall {
        callback,
        intervalTick,
        nextTime
    });
}

} //namespace
