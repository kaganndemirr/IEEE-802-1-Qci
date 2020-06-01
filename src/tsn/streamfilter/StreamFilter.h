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

#ifndef __IEEE_802_1_QCI_STREAMFILTER_H_
#define __IEEE_802_1_QCI_STREAMFILTER_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace ieee_802_1_qci {

struct StreamHandleSpec {
    int value;
    bool isWildcard;
};

struct PrioritySpec {
    int value;
    bool isWildcard;
};

struct MaxSDUSizeSpec {
    size_t value;
    bool isActive;
};

struct StreamFilter_s {
    int instanceId;
    StreamHandleSpec streamHandle;
    PrioritySpec priority;
    int streamGateId;
    MaxSDUSizeSpec maxSDUSize;
    std::vector<int> flowMeters;

    bool streamBlockedDueToOversizeFrameEnable;
    bool streamBlockedDueToOversizeFrame;
};

class StreamFilter : public cSimpleModule
{
  private:
    StreamFilter_s mPar;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void handleParameterChange(const char *parname);

  public:
    bool match(int streamId, int priority);
};

} //namespace

#endif
