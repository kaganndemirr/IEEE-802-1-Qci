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

package ieee_802_1_qci.tsn.flowmeter;

simple FlowMeter
{
    parameters:
        @display("i=block/bucket");
        
        string instanceId;
        
        string committedInformationRate; // green tokens (octets) per second
        string committedBurstSize; // green bucket size
        string excessInformationRate; // yellow tokens (octets) per second
        string excessBurstSize; // yellow bucket size
        string couplingFlag; // true: use overflowing green tokens as yellow tokens
    
        string colorMode; // true: color aware, false: color blind
        string dropOnYellow;
        string markAllFramesRed;
        string markAllFramesRedEnable;

    gates:
        input in;
        output out;
}
