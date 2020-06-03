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

package ieee_802_1_qci.simulations.network2;

import ned.DelayChannel;
import ieee_802_1_qci.node.Host;
import ieee_802_1_qci.node.Switch;

network Network2
{
    parameters:
        @display("bgb=600,200,white");

    types:
        channel C extends DelayChannel
        {
            delay = 2ms;
        }
        channel CS extends DelayChannel
        {
			@display("ls=,3");
            delay = 60ms;
        }

    submodules:
        VOIP1: Host {
            @display("p=100,50");
        }
        VOIP2: Host {
            @display("p=500,50");
        }
        hostC: Host {
            @display("p=100,150");
        }
        hostD: Host {
            @display("p=500,150");
        }
        switch1: Switch {
            @display("p=250,100");
        }
        switch2: Switch {
            @display("p=350,100");
        }

    connections:
        switch1.port++ <--> CS <--> switch2.port++;

        VOIP1.port <--> C <--> switch1.port++;
        hostC.port <--> C <--> switch1.port++;

        VOIP2.port <--> C <--> switch2.port++;
        hostD.port <--> C <--> switch2.port++;
}
