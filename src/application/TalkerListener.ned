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

package ieee_802_1_qci.application;

module TalkerListener like IApp
{
    parameters:
        @display("i=block/app");

    gates:
        input in;
        output out;

    submodules:
        talker: TrafficGenerator {
            clockPath = default("^.^.clk");
        }
        listener: Sink {
        }

    connections:
        in --> listener.in;
        talker.out --> out;
}
