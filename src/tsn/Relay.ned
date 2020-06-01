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
import ieee_802_1_qci.tsn.table.Table;
import ieee_802_1_qci.tsn.RelayOut;

module Relay
{
    parameters:
        @display("i=block/dispatch;bgb=500,300,white");

        int portCount;

    gates:
        input in;
        output out[portCount];

    submodules:
        forwarder: Forwarder {
            @display("p=80,225");
        }
        identifier: StreamIdentifier {
            @display("p=80,50");
        }
        streamFilter: Table {
            @display("i=block/filter;p=250,50");
            queryNode = "ieee_802_1_qci.tsn.streamfilter.SFQuery";
        }
        streamGate: Table {
            @display("i=block/layer;p=450,50");
            queryNode = "ieee_802_1_qci.tsn.streamgate.SGQuery";
        }
        flowMeter: Table {
            @display("i=block/bucket;p=450,225");
            queryNode = "ieee_802_1_qci.tsn.flowmeter.FMQuery";
        }
        relayOut: RelayOut {
            @display("p=250,225");

            portCount = portCount;
        }

    connections:
        in --> forwarder.in;

        forwarder.out --> identifier.in;
        identifier.out --> streamFilter.in;

        streamFilter.out --> streamGate.in;
        streamGate.out --> flowMeter.in;

        flowMeter.out --> relayOut.in;

        for i=0..portCount-1 {
            relayOut.out[i] --> out[i];
        }
}
