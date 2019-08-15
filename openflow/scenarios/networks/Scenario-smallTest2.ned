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
import openflow.utility.OpenFlowGraphAnalyzer;

network Scenario_SmallTest2
{
    @display("bgb=1581.4263,864.47626");

    types:
        channel ethernetline extends ThruputMeteringChannel
        {
            delay = 1us;
            datarate = 100Mbps;
            thruputDisplayFormat = "u";
        }
    submodules:
        spanningTree: StaticSpanningTree {
            @display("p=1028,111");
        }
        openflowGraphAnalyzer: OpenFlowGraphAnalyzer {
            @display("p=1028,30");
        }

        of_controller_knroot: Open_Flow_Controller {
            @display("p=392.94376,37.22625");
        }
        client_1: StandardHost {
            @display("p=57.9075,660.42126");
        }
        client_2: StandardHost {
            @display("p=57.9075,328.1425");
        }
        client_3: StandardHost {
            @display("p=776.23627,278.5075");
        }

        client_4: StandardHost {
            @display("p=92.37625,784.5087");
        }
        client_5: StandardHost {
            @display("p=57.9075,216.46375");
        }
        client_6: StandardHost {
            @display("p=722.465,381.91376");
        }

        client_7: StandardHost {
            @display("p=210.94875,798.29626");
        }
        client_8: StandardHost {
            @display("p=67.55875,126.845");
        }
        client_9: StandardHost {
            @display("p=787.26624,180.61626");
        }

        server_1: StandardHost {
            @display("p=776.23627,643.8762");
        }
        //        configurator: FlatNetworkConfigurator {
        configurator: IPv4NetworkConfigurator {
            parameters:
                @display("p=1410.4613,49.635");
        }
        switch_1: Open_Flow_Switch {
            @display("p=202.67625,614.9225");
        }
        of_controller_1: Open_Flow_Controller {
            @display("p=202.67625,126.845");
        }

        of_controller_2: Open_Flow_Controller {
            @display("p=616.3013,104.785");
        }

        server_2: StandardHost {
            @display("p=787.26624,521.1675");
        }
        server_3: StandardHost {
            @display("p=683.86,744.525");
        }
        switch_2: Open_Flow_Switch {
            @display("p=616.3013,614.9225");
        }
        switch_3: Open_Flow_Switch {
            @display("p=202.67625,268.85626");
        }
        switch_4: Open_Flow_Switch {
            @display("p=616.3013,268.85626");
        }
        etherSwitch: EtherSwitch {
            @display("p=392.94376,268.85626");
        }
    connections allowunconnected:

//        client1.ethg++ <--> {delay=1ms;} <--> open_flow_switch.ethg++;
//        open_flow_switch.ethg++ <--> {delay=1ms;} <--> server1.ethg++;
//        controller.ethg++ <--> {delay=1ms;} <--> open_flow_switch.gate_controller++;
//        open_flow_switch.ethg++ <--> {delay=1ms;} <--> server2.ethg++;
//        open_flow_switch.ethg++ <--> {delay=1ms;} <--> server3.ethg++;

        client_1.ethg++ <--> ethernetline <--> switch_1.gateDataPlane++;
        client_2.ethg++ <--> ethernetline <--> switch_3.gateDataPlane++;
        client_3.ethg++ <--> ethernetline <--> switch_4.gateDataPlane++;

        client_4.ethg++ <--> ethernetline <--> switch_1.gateDataPlane++;
        client_5.ethg++ <--> ethernetline <--> switch_3.gateDataPlane++;
        client_6.ethg++ <--> ethernetline <--> switch_4.gateDataPlane++;

        client_7.ethg++ <--> ethernetline <--> switch_1.gateDataPlane++;
        client_8.ethg++ <--> ethernetline <--> switch_3.gateDataPlane++;
        client_9.ethg++ <--> ethernetline <--> switch_4.gateDataPlane++;


        switch_2.gateDataPlane++ <--> ethernetline <--> server_1.ethg++;
        switch_2.gateDataPlane++ <--> ethernetline <--> server_2.ethg++;
        switch_2.gateDataPlane++ <--> ethernetline <--> server_3.ethg++;

        etherSwitch.ethg++ <--> ethernetline <--> switch_1.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> switch_2.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> switch_3.gateControlPlane++;
        etherSwitch.ethg++ <--> ethernetline <--> switch_4.gateControlPlane++;


        switch_1.gateDataPlane++ <--> ethernetline <--> switch_2.gateDataPlane++;
        switch_3.gateDataPlane++ <--> ethernetline <--> switch_1.gateDataPlane++;
        switch_4.gateDataPlane++ <--> ethernetline <--> switch_2.gateDataPlane++;
        //switch_3.gateDataPlane++ <--> ethernetline <--> switch_2.gateDataPlane++;
		//switch_1.gateDataPlane++ <--> ethernetline <--> switch_4.gateDataPlane++;

        etherSwitch.ethg++ <--> ethernetline <--> of_controller_1.ethg++;
        etherSwitch.ethg++ <--> ethernetline <--> of_controller_2.ethg++;
        etherSwitch.ethg++ <--> ethernetline <--> of_controller_knroot.ethg++;


}
