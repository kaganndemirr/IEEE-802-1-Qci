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

package ieee_802_1_qci.tsn.streamgate;

simple StreamGate
{
    parameters:
        @display("i=block/layer;p=,,r");
        
        string iconClosed = "icon/gateclosed"; // block/encrypt
        string iconOpen = "block/layer"; // block/decrypt
        
        string instanceId;
        string state;
        string ipv;
    
        string closedDueToInvalidRxEnable;
        string closedDueToInvalidRx;
        string closedDueToOctetsExceededEnable;
        string closedDueToOctetsExceeded;
    
        string intervalOctetLeft;
        string opIndex;
    
    gates:
        input in;
        output out;
}
