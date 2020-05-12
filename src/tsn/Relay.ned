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
import ieee_802_1_qci.tsn.StreamFilterHandler;
import ieee_802_1_qci.tsn.StreamGateHandler;
import ieee_802_1_qci.tsn.FlowMeterHandler;
import ieee_802_1_qci.tsn.TSNEncapsulator;
import ieee_802_1_qci.tsn.TSNDecapsulator;

module Relay
{
    parameters:
        @display("i=block/dispatch;bgb=500,300,white");

        int portCount;

    gates:
        input in[portCount];
        output out[portCount];

    submodules:
        encap: TSNEncapsulator {
            @display("p=80,225");
            portCount = sizeof(in);
        }
        forwarder: Forwarder {
            @display("p=80,140");
            portCount = sizeof(in);
        }
        identifier: StreamIdentifier {
            @display("p=80,50");
            portCount = sizeof(in);
        }
        streamFilter: StreamFilterHandler {
            @display("p=250,50");
            portCount = sizeof(in);
        }
        streamGate: StreamGateHandler {
            @display("p=450,50");
            portCount = sizeof(in);
        }
        flowMeter: FlowMeterHandler {
            @display("p=450,225");
            portCount = sizeof(in);
        }
        decap: TSNDecapsulator {
            @display("p=250,225");
            portCount = sizeof(in);
        }

    connections:
        for i=0..portCount-1 {
            in[i] --> encap.in[i];
            encap.out[i] --> forwarder.in[i];

            forwarder.out[i] --> identifier.in[i];
            identifier.out[i] --> streamFilter.in[i];

            streamFilter.next[i] --> streamGate.in[i];
            streamFilter.out[i] --> decap.filterIn[i];

            streamGate.next[i] --> flowMeter.in[i];
            streamGate.out[i] --> decap.gateIn[i];

            flowMeter.out[i] --> decap.meterIn[i];
            decap.out[i] --> out[i];
        }
}
