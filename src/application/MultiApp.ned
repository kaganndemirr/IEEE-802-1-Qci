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

module MultiApp like IApp
{
    parameters:
        @display("i=block/app2");
        string appPath = default("ieee_802_1_qci.application.TalkerListener");
        int appCount;

    gates:
        input in;
        output out;

    submodules:
        man: AppManager {
            gates:
                in[appCount];
                out[appCount];
        }
        app[appCount]: <appPath> like IApp {
        }

    connections:
        in --> man.upperIn;
        man.upperOut --> out;
        
        for i=0..appCount-1 {
        	man.out[i] --> app[i].in;
        	app[i].out --> man.in[i];
        }
}
