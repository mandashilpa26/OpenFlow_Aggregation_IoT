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

package openflow.networks;
import inet.examples.wireless.dynamic.DynamicHost;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.common.misc.ThruputMeteringChannel;
import inet.networklayer.configurator.ipv4.FlatNetworkConfigurator;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator; //use IPv4NetworkConfigurator instead of FlatNetworkConfigurator

//import inet.common.misc.ThruputMeteringChannel;
//import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
//import inet.node.ethernet.EtherSwitch;
//import inet.node.inet.StandardHost;
import openflow.hyperflow.Hyper_Flow_Synchronizer;
import openflow.openflow.controller.Open_Flow_Controller;
import openflow.openflow.switch.Open_Flow_Switch;
import openflow.utility.ControllerInvolvementFilter;
import openflow.utility.StaticSpanningTree;

import openflow.openflow.sdWan.Open_Flow_SdWan;
import openflow.openflow.sdWanDeAgg.Open_Flow_SdWanDeAgg;

network NonAggregate_1_5
{
    parameters:
        int numClientsSet1 @display("bgb=768,465") = default(1);

        int numServers = default(1);

        @display("bgb=1066,667");

    types:
        channel ethernetline extends ThruputMeteringChannel
        {
            delay = 1us;
            datarate = 100Mbps;
            thruputDisplayFormat = "u";
        }
    submodules:
        client1[numClientsSet1]: StandardHost {
            @display("p=295.24002,331.474");
        }
        client2[numClientsSet1]: StandardHost {
            @display("p=38.918,343.552");
        }
        client3[numClientsSet1]: StandardHost {
            @display("p=295.24002,458.96402");
        }
        client4[numClientsSet1]: StandardHost {
            @display("p=76.494,432.12402");
        }
        client5[numClientsSet1]: StandardHost {
            @display("p=37.576,260.348");
        }

        servers[numServers]: StandardHost {
            @display("p=787,91");
        }

        //        configurator: FlatNetworkConfigurator {
        configurator: IPv4NetworkConfigurator {
            parameters:
                @display("p=195,236");
        }
        switch_2: Open_Flow_Switch {
            @display("p=444.202,189.222");
        }

        of_controller: Open_Flow_Controller {
            @display("p=361,24");
        }

        switch_3: Open_Flow_Switch {
            @display("p=585.112,189.222");
        }
        switch_1: Open_Flow_Switch {
            @display("p=315.37,189.222");
        }
        etherSwitch: EtherSwitch {
            @display("p=368,77");
        }

        wan_1: Open_Flow_Switch {
            @display("p=129,190");
        }

        wan_2: Open_Flow_Switch {
            @display("p=719.312,190.56401");
        }
    connections allowunconnected:

        //client_1.ethg++ <--> ethernetline <--> sdwan_1.gateDataPlane++;
        for i=0..(numClientsSet1)-1 {
            client1[i].ethg++ <--> ethernetline <--> wan_1.gateDataPlane++;
            client2[i].ethg++ <--> ethernetline <--> wan_1.gateDataPlane++;
            client3[i].ethg++ <--> ethernetline <--> wan_1.gateDataPlane++;
            client4[i].ethg++ <--> ethernetline <--> wan_1.gateDataPlane++;
            client5[i].ethg++ <--> ethernetline <--> wan_1.gateDataPlane++;
        }

//        for i=0..(numClientsSet2)-1 {
//            client2[i].ethg++ <--> ethernetline <--> sdwan_1.gateDataPlane++;
//        }

        for i=0..(numServers)-1 {
            wan_2.gateDataPlane++ <--> ethernetline <--> servers[i].ethg++;
        }
        wan_1.gateDataPlane++ <--> ethernetline <--> switch_1.gateDataPlane++;

        //sdwan_2.gateDataPlane++ <--> ethernetline <--> server_1.ethg++;
        //sdwan_2.gateDataPlane++ <--> ethernetline <--> server_2.ethg++;


        etherSwitch.ethg++ <--> ethernetline <--> switch_1.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> switch_2.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> switch_3.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> wan_1.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> wan_2.gateControlPlane++;

        etherSwitch.ethg++ <--> ethernetline <--> of_controller.ethg++;

        switch_1.gateDataPlane++ <--> ethernetline <--> switch_2.gateDataPlane++;
        switch_2.gateDataPlane++ <--> ethernetline <--> switch_3.gateDataPlane++;
        switch_3.gateDataPlane++ <--> ethernetline <--> wan_2.gateDataPlane++;
}
