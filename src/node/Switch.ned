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
import ieee_802_1_qci.tsn.streamfilter.TableLoader;
import ieee_802_1_qci.tsn.table.StreamFilterTable;
import ieee_802_1_qci.tsn.table.StreamGateTable;
import ieee_802_1_qci.tsn.table.FlowMeterTable;

import ieee_802_1_qci.link.NetworkController;
import ieee_802_1_qci.queue.BufferQueue;
import ieee_802_1_qci.tsn.Relay;
import ieee_802_1_qci.tsn.TSNEncapsulator;
import ieee_802_1_qci.tsn.TSNDecapsulator;
import ieee_802_1_qci.queue.PriorityQueue;

module Switch
{
    parameters:
        @display("i=device/switch;bgb=800,400,white");

    gates:
        inout port[];

    types:
    submodules:
        clk: Clock {
            @display("p=50,50");
        }
        fdb: FilteringDatabase {
            @display("p=50,125");
        }

        streamfilters: ieee_802_1_qci.tsn.streamfilter.TableLoader {
            @display("p=750,50");
        }
        streamgates: StreamGateTable {
            @display("p=750,125");
        }
        flowmeters: FlowMeterTable {
            @display("p=750,200");
        }

        nc[sizeof(port)]: NetworkController {
            @display("p=400,300");
        }
        buffer: BufferQueue {
            @display("p=250,150");
        }
        relay: Relay {
            @display("p=400,150");
            
            portCount = sizeof(port);
        }
        encap: TSNEncapsulator {
            @display("p=250,300");

            portCount = sizeof(port);
        }
        decap[sizeof(port)]: TSNDecapsulator {
            @display("p=550,300");
        }
        queue[sizeof(port)]: PriorityQueue {
            @display("p=550,150");
        }

    connections:
        for i=0..sizeof(port)-1 {
            port[i] <--> nc[i].phys;

            nc[i].upperOut --> encap.in[i];

            relay.out[i] --> queue[i].in;
            queue[i].out --> decap[i].in;
            decap[i].out --> nc[i].upperIn;
        }

        encap.out --> buffer.in;
        buffer.out --> relay.in;
}
