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

package ieee_802_1_qci.tsn;

import ieee_802_1_qci.tsn.Forwarder;
import ieee_802_1_qci.tsn.StreamIdentifier;
import ieee_802_1_qci.tsn.FilteringUnit;

module Relay
{
    parameters:
        @display("i=block/dispatch;bgb=240,220,white");

        int portCount;

    gates:
        input in[portCount];
        output out[portCount];

    submodules:
        forwarder: Forwarder {
            @display("p=70,150");
            portCount = sizeof(in);
        }
        identifier: StreamIdentifier {
            @display("p=120,50");
            portCount = sizeof(in);
        }
        filtering: FilteringUnit {
            @display("p=170,150");
            portCount = sizeof(in);
        }

    connections:
        for i=0..portCount-1 {
            in[i] --> forwarder.in[i];
            forwarder.out[i] --> identifier.in[i];
            identifier.out[i] --> filtering.in[i];
            filtering.out[i] --> out[i];
        }
}
