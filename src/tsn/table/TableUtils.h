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

#ifndef __IEEE_802_1_QCI_XMLUTILS_H_
#define __IEEE_802_1_QCI_XMLUTILS_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace ieee_802_1_qci {

int readXMLUInt(cXMLElement* elm, const char* tagName);
bool readXMLBool(cXMLElement* elm, const char* tagName);
int readXMLUIntStr(cXMLElement* elm, const char* tagName, const char* str, bool* isStr);

}

#endif
