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

namespace ieee_802_1_qci;

packet TSNPacket {
    int streamHandle;
    uint8_t ipv;
    uint8_t color; // 8 bits ; 0 => No Color, 1 => Green, 2 => Yellow
    int streamGateId;
    int flowMeterIds[];
}
