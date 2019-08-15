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

network Scenario_Small
{
    @display("bgb=1066,667");

    types:
        channel ethernetline extends ThruputMeteringChannel
        {
            delay = 1us;
            datarate = 100Mbps;
            thruputDisplayFormat = "u";
        }
    submodules:
        client_1: StandardHost {
            @display("p=27,158");
        }

        client_2: StandardHost {
            @display("p=46,259");
        }

        client_3: StandardHost {
            @display("p=119.438,348.92");
        }


        server_1: StandardHost {
            @display("p=787,91");
        }
        //        configurator: FlatNetworkConfigurator {
        configurator: IPv4NetworkConfigurator {
            parameters:
                @display("p=195,236");
        }
        switch_1: Open_Flow_Switch {
            @display("p=361,190");
        }
        of_controller: Open_Flow_Controller {
            @display("p=361,24");
        }

        server_2: StandardHost {
            @display("p=796,152");
        }
        server_3: StandardHost {
            @display("p=796,236");
        }
        switch_2: Open_Flow_Switch {
            @display("p=585.112,189.222");
        }
        etherSwitch: EtherSwitch {
            @display("p=368,77");
        }

        sdwan_1: Open_Flow_SdWan {
            @display("p=129,190");
        }

        sdwan_2: Open_Flow_SdWanDeAgg {
            @display("p=719.312,190.56401");
        }

    connections allowunconnected:

//        client1.ethg++ <--> {delay=1ms;} <--> open_flow_switch.ethg++;
//        open_flow_switch.ethg++ <--> {delay=1ms;} <--> server1.ethg++;
//        controller.ethg++ <--> {delay=1ms;} <--> open_flow_switch.gate_controller++;
//        open_flow_switch.ethg++ <--> {delay=1ms;} <--> server2.ethg++;
//        open_flow_switch.ethg++ <--> {delay=1ms;} <--> server3.ethg++;

//        client_1.ethg++ <--> ethernetline <--> switch_1.gateDataPlane++;
//        client_2.ethg++ <--> ethernetline <--> switch_1.gateDataPlane++;

        client_1.ethg++ <--> ethernetline <--> sdwan_1.gateDataPlane++;
        client_2.ethg++ <--> ethernetline <--> sdwan_1.gateDataPlane++;
        client_3.ethg++ <--> ethernetline <--> sdwan_1.gateDataPlane++;

        sdwan_1.gateDataPlane++ <--> ethernetline <--> switch_1.gateDataPlane++;

        sdwan_2.gateDataPlane++ <--> ethernetline <--> server_1.ethg++;
        sdwan_2.gateDataPlane++ <--> ethernetline <--> server_2.ethg++;
        sdwan_2.gateDataPlane++ <--> ethernetline <--> server_3.ethg++;

        etherSwitch.ethg++ <--> ethernetline <--> switch_1.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> switch_2.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> sdwan_1.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> sdwan_2.gateControlPlane++;

        etherSwitch.ethg++ <--> ethernetline <--> of_controller.ethg++;

        switch_1.gateDataPlane++ <--> ethernetline <--> switch_2.gateDataPlane++;
        switch_2.gateDataPlane++ <--> ethernetline <--> sdwan_2.gateDataPlane++;
}
