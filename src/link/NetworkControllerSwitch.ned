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

package ieee_802_1_qci.link;

import ieee_802_1_qci.link.NetworkController;
import ieee_802_1_qci.queue.PriorityQueue;

module NetworkControllerSwitch
{
    parameters:
        @display("i=block/ifcard");
        string addr = default("");
    
    gates:
        input upperIn;
        output upperOut;
        inout phys;
        
   submodules:
        nc: NetworkController {
            @display("p=400,300");
            addr = addr;
        }
        queue: PriorityQueue {
            @display("p=250,150");
        }

    connections:
        phys <--> nc.phys;
        nc.upperOut --> upperOut;
        upperIn --> queue.in;
        queue.out --> nc.upperIn;
}
