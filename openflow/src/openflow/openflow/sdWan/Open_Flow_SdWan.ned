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

import inet.linklayer.contract.IWiredNic;
import inet.linklayer.ethernet.EthernetInterface;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.contract.INetworkLayer;
import inet.networklayer.contract.IRoutingTable;
import inet.transportlayer.contract.ITCP;


module Open_Flow_SdWan
{
    parameters:
        @networkNode();
        @sdWan();
        @labels(node,ethernet-node);
        @display("i=device/switch;i2=openflow/OpenFlow-Logo-small;bgb=1107,633");
        bool sendCompletePacket = default(false);
        double serviceTime @unit("s") = default(0s);
        bool csmacdSupport = default(true);  // by default use CSMA/CD
        string macType = default(csmacdSupport ? "EtherMAC" : "EtherMACFullDuplex"); // ~EtherMAC or ~EtherMACFullDuplex

        string networkLayerType = default("IPv4NetworkLayer");
        string routingTableType = default("IPv4RoutingTable");

        //submodules
        string tcpType = default("TCP");  // tcp implementation (e.g. TCP, TCP_old, TCP_NSC)
        bool forwarding = default(false);
        bool multicastForwarding = default(false);
        string routingFile = default("");

        routingTable.forwarding = forwarding;
        routingTable.multicastForwarding = multicastForwarding;   // for IPv4, IPv6, Generic
        *.routingTableModule = default(absPath(".routingTable"));
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        etherMAC[*].encapType = "EtherEncapDummy";
        etherMAC[*].csmacdSupport = csmacdSupport;
        etherMAC[*].macType = macType;
        networkLayer.proxyARP = false;

    gates:
        inout gateControlPlane[] @labels(ControlPlane);
        inout gateDataPlane[] @labels(DataPlane);

    submodules:

        interfaceTable: InterfaceTable {
            parameters:
                @display("p=446,203");
        }
        tcp: <tcpType> like ITCP {
            parameters:
                @display("p=462,90");
        }
        routingTable: <routingTableType> like IRoutingTable if routingTableType != "" {
            parameters:
                @display("p=200,203");
        }
        networkLayer: <networkLayerType> like INetworkLayer {
            parameters:
                @display("p=323,90;q=queue");
            gates:
                ifIn[sizeof(gateControlPlane)];
                ifOut[sizeof(gateControlPlane)];
        }
        eth[sizeof(gateControlPlane)]: EthernetInterface {
            parameters:
                @display("p=143,90,row,90;q=txQueue");
        }
        etherMAC[sizeof(gateDataPlane)]: <default("EthernetInterface")> like IWiredNic {
            parameters:
                mac.promiscuous = true;
                @display("p=951,90,c,70");
        }

        sdWan: sdWan {
            @display("p=671,90");
            sendCompletePacket = sendCompletePacket;
            gates:
                dataPlaneOut[sizeof(gateDataPlane)];
                dataPlaneIn[sizeof(gateDataPlane)];
        }

    connections allowunconnected:
        for i=0..sizeof(gateControlPlane)-1 {
            eth[i].phys <--> gateControlPlane[i];
            eth[i].upperLayerOut --> networkLayer.ifIn[i];
            eth[i].upperLayerIn <-- networkLayer.ifOut[i];
        }

        for i=0..sizeof(gateDataPlane)-1 {
            etherMAC[i].phys <--> gateDataPlane[i];
            sdWan.dataPlaneOut[i] --> etherMAC[i].upperLayerIn;
            etherMAC[i].upperLayerOut --> sdWan.dataPlaneIn[i];
        }
        tcp.ipOut --> networkLayer.transportIn++;
        tcp.ipIn <-- networkLayer.transportOut++;

        tcp.appOut++ --> sdWan.controlPlaneIn;
        sdWan.controlPlaneOut --> tcp.appIn++;


}

