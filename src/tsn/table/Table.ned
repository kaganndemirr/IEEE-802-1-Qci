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

package ieee_802_1_qci.tsn.table;

import ieee_802_1_qci.tsn.table.IQuery;
import ieee_802_1_qci.utils.Join;

module Table
{
    parameters:
        @display("i=block/table");
        
        string queryNode;

    gates:
        input in;
        output out;

    submodules:
        query: <queryNode> like IQuery {
        }
        next: Join {
        }

    connections:
        in --> query.in;
        query.out[0] --> next.in[0];
        next.out --> out;
}
