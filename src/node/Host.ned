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

import ieee_802_1_qci.application.TrafficGenerator;
import ieee_802_1_qci.link.NetworkController;

module Host
{
    parameters:
        @display("i=device/pc3;bgb=300,100,white");

    gates:
        inout port;

    submodules:
        nc: NetworkController {

            @display("p=100,50");
        }
        app: TrafficGenerator {
            @display("p=200,50");
        }

    connections:
        nc.upperOut --> app.in;
        nc.upperIn <-- app.out;
        port <--> nc.phys;
}
