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

#include "MultiBubbleModule.h"

namespace ieee_802_1_qci {

void MultiBubbleModule::bubble(const char *text) {
    cSimpleModule::bubble(text);

    for (cModule* module : mModules) {
        module->bubble(text);
    }
}

void MultiBubbleModule::addBubbleModule(cModule* module) {
    if (module != nullptr) {
        mModules.push_back(module);
    }
}

void MultiBubbleModule::addBubbleModule(const char* modulePath) {
    cModule* module = getModuleByPath(modulePath);

    if (module != nullptr) {
        addBubbleModule(module);
    }
}

} /* namespace ieee_802_1_qci */
