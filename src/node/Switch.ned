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

package ieee_802_1_qci.node;

import ieee_802_1_qci.tsn.Clock;

import ieee_802_1_qci.tsn.FilteringDatabase;
import ieee_802_1_qci.tsn.table.StreamFilterTable;
import ieee_802_1_qci.tsn.table.StreamGateTable;
import ieee_802_1_qci.tsn.table.FlowMeterTable;

import ieee_802_1_qci.link.NetworkController;
import ieee_802_1_qci.tsn.Relay;

module Switch
{
    parameters:
        @display("i=device/switch;bgb=300,300,white");

    gates:
        inout port[];

    types:
    submodules:
        clk: Clock {
            @display("p=270,25");
        }

        fdb: FilteringDatabase {
            @display("p=50,25");
        }
        streamfilters: StreamFilterTable {
            @display("p=150,25");
        }
        streamgates: StreamGateTable {
            @display("p=50,125");
        }
        flowmeters: FlowMeterTable {
            @display("p=150,125");
        }

        nc[sizeof(port)]: NetworkController {
            @display("p=60,230");
        }
        relay: Relay {
            @display("p=250,230");
            portCount = sizeof(port);
        }

    connections:
        for i=0..sizeof(port)-1 {
            port[i] <--> nc[i].phys;
            nc[i].upperOut --> relay.in[i];
            relay.out[i] --> nc[i].upperIn;
        }
}
