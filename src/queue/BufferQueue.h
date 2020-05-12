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

#ifndef __IEEE_802_1_QCI_BUFFERQUEUE_H_
#define __IEEE_802_1_QCI_BUFFERQUEUE_H_

#include <omnetpp.h>
#include "../tsn/Clock.h"

using namespace omnetpp;

namespace ieee_802_1_qci {

class BufferQueue : public cSimpleModule, public IScheduled
{
  private:
    Clock* mClock;
    cPacketQueue* mQueue;
    simtime_t mInterval;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void handleParameterChange(const char *parname);

  public:
    simtime_t tick(int unused);
};

} //namespace

#endif
