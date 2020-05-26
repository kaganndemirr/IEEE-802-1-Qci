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

#include "GateControlOp.h"
#include "../../utils/Table.h"
#include "StreamGate.h"

namespace ieee_802_1_qci {

Define_Module(GateControlOp);

void GateControlOp::initialize()
{
    handleParameterChange(nullptr);

    cModule* parent = getParentModule();
    StreamGate* gate = check_and_cast<StreamGate*>(parent);
    gate->initScheduling();
}

void GateControlOp::handleMessage(cMessage *msg)
{
}

void GateControlOp::handleParameterChange(const char *parname)
{
    if (!parname || !strcmp(parname, "state")) {
        mPar.state = readBool(par("state"), "state");
    }

    if (!parname || !strcmp(parname, "ipv")) {
        mPar.ipv.value = readUIntStr(par("ipv"), "ipv", "null", &mPar.ipv.isNull);
    }

    if (!parname || !strcmp(parname, "timeInterval")) {
        mPar.timeInterval = simTime().parse(par("timeInterval"));
    }

    if (!parname || !strcmp(parname, "intervalOctetMax")) {
        mPar.intervalOctetMax.value = readUIntStr(par("intervalOctetMax"), "intervalOctetMax", "", &mPar.intervalOctetMax.isNull);
    }
}

StreamGateControlOperation GateControlOp::getParams()
{
    return mPar;
}

} //namespace
