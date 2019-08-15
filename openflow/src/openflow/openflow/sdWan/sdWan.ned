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

package openflow.openflow.sdWan;
import openflow.openflow.switch.OF_Switch;

simple sdWan extends OF_Switch {
    
 	parameters:
 	    @signal[cpPingPacketHash](type="unsigned long");
        @statistic[cpPingPacketHash](title="CpPingPacketHash"; record=vector?,stats; interpolationmode=none);
        @signal[dpPingPacketHash](type="unsigned long");
        @statistic[dpPingPacketHash](title="DpPingPacketHash"; record=vector?,stats; interpolationmode=none);
        @signal[queueSize](type="unsigned long");
        @statistic[queueSize](title="QueueSize"; record=vector?,stats; interpolationmode=none);
        @signal[bufferSize](type="long");
        @statistic[bufferSize](title="BufferSize"; record=vector?,stats; interpolationmode=none);
        @signal[waitingTime](type="simtime_t");
        @statistic[waitingTime](title="WaitingTime"; record=vector?,stats?; interpolationmode=none);
        @display("i=block/app");
        
        
        @signal[dpAggregatePacketHash](type="unsigned long");
        @statistic[dpAggregatePacketHash](title="dpAggregatePacketHash"; record=vector?,stats; interpolationmode=none);
 	    int aggBufferCapacity = default(3712);
		//int bufferCapacity = default(0);

		@class(sdWan);
 	        
}
