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

#include "Table.h"

namespace ieee_802_1_qci {

int readUInt(const char* str, const char* name) {
    int val = atoi(str);
    if (val < 0)
        throw omnetpp::cRuntimeError("%s cannot be negative, given: %d", name, val);
    return val;
}

bool readBool(const char* str, const char* name) {
    if (strcmp(str, "true") == 0) {
        return true;
    }
    if (strcmp(str, "false") == 0) {
        return false;
    }

    throw omnetpp::cRuntimeError("%s must be true or false, given: %s", name, str);
}

int readUIntStr(const char* str, const char* name, const char* flagStr, bool* flag) {
    int num;
    if (strcmp(str, flagStr) == 0) {
        *flag = true;
        return 0;
    }

    num = atoi(str);
    if (num < 0)
        throw omnetpp::cRuntimeError("%s cannot be negative, given: %d", name, num);

    *flag = false;
    return num;
}

int readXMLUInt(cXMLElement* elm, const char* tagName) {
    int val = atoi(elm->getNodeValue());
    if (val < 0)
        throw omnetpp::cRuntimeError("%s cannot be negative, given: %d", tagName, val);
    return val;
}

bool readXMLBool(cXMLElement* elm, const char* tagName) {
    const char* val = elm->getNodeValue();
    if (strcmp(val, "true") == 0) {
        return true;
    }
    if (strcmp(val, "false") == 0) {
        return false;
    }

    throw omnetpp::cRuntimeError("%s must be true or false, given: %s", tagName, val);
}

int readXMLUIntStr(cXMLElement* elm, const char* tagName, const char* str, bool* isStr) {
    const char* val = elm->getNodeValue();
    int valN;
    if (strcmp(val, str) == 0) {
        *isStr = true;
        return 0;
    }

    valN = atoi(val);
    if (valN < 0)
        throw omnetpp::cRuntimeError("%s cannot be negative, given: %d", tagName, valN);

    *isStr = false;
    return valN;
}

}
