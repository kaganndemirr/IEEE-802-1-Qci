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

#ifndef UTILS_MULTIBUBBLEMODULE_H_
#define UTILS_MULTIBUBBLEMODULE_H_

#include <omnetpp.h>
#include <list>

using namespace omnetpp;

namespace ieee_802_1_qci {

class MultiBubbleModule : public cSimpleModule {
private:
    std::list<cModule*> mModules;

protected:
    void bubble(const char *text);
    void addBubbleModule(const char* modulePath);
    void addBubbleModule(cModule* module);
};

} /* namespace ieee_802_1_qci */

#endif /* UTILS_MULTIBUBBLEMODULE_H_ */
