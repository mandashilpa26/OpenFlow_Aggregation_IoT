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

network Aggregate_4_1
{
    parameters:
        int numClients @display("bgb=768,465") = default(1);

        @display("bgb=1066,667");

    types:
        channel ethernetline extends ThruputMeteringChannel
        {
            delay = 1us;
            datarate = 100Mbps;
            thruputDisplayFormat = "u";
        }
    submodules:
        client[numClients]: StandardHost {
            @display("p=100.65,488.488");
        }

        server_1: StandardHost {
            @display("p=956.846,279.13602");
        }
        //        configurator: FlatNetworkConfigurator {
        configurator: IPv4NetworkConfigurator {
            parameters:
                @display("p=69.784004,67.1");
        }
        switch_2: Open_Flow_Switch {
            @display("p=496.54,370.392");
        }

        of_controller: Open_Flow_Controller {
            @display("p=450.91202,81.862");
        }

        switch_3: Open_Flow_Switch {
            @display("p=661.606,370.392");
        }
        switch_1: Open_Flow_Switch {
            @display("p=342.21002,370.392");
        }
        etherSwitch: EtherSwitch {
            @display("p=450.91202,222.772");
        }

        sdwan_1: Open_Flow_SdWan {
            @display("p=152.988,370.392");
        }

        sdwan_2: Open_Flow_SdWanDeAgg {
            @display("p=819.96204,370.392");
        }

    connections allowunconnected:

        //client_1.ethg++ <--> ethernetline <--> sdwan_1.gateDataPlane++;
        for i=0..(numClients)-1 {
            client[i].ethg++ <--> ethernetline <--> sdwan_1.gateDataPlane++;
        }

        sdwan_1.gateDataPlane++ <--> ethernetline <--> switch_1.gateDataPlane++;

        sdwan_2.gateDataPlane++ <--> ethernetline <--> server_1.ethg++;


        etherSwitch.ethg++ <--> ethernetline <--> switch_1.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> switch_2.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> switch_3.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> sdwan_1.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> sdwan_2.gateControlPlane++;

        etherSwitch.ethg++ <--> ethernetline <--> of_controller.ethg++;

        switch_1.gateDataPlane++ <--> ethernetline <--> switch_2.gateDataPlane++;
        switch_2.gateDataPlane++ <--> ethernetline <--> switch_3.gateDataPlane++;
        switch_3.gateDataPlane++ <--> ethernetline <--> sdwan_2.gateDataPlane++;
}
