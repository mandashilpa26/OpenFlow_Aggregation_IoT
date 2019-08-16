# OpenFlow-based Aggregation Mechanism for Communication in the Internet of Things

Internet of Things(IoT) is a novel technology which polls data at high frequencies and produces packets with small payloads. IoT devices make distinct measurements thus producing heterogeneous in a small area. These densely deployed devices transmit their data through the access network to store their values on the cloud, leading to flooding of packets in the access networks. IoT networks need mechanisms to handle the scale issue in future. OpenFlow is a Software-Defined Networking(SDN) approach which separates the control plane from data plane offering centralised control. Flow aggregation gathers and aggregates data based on custom flows such as source address and destination address, port numbers. This research aims at integrating support for data aggregation in OpenFlow for communication in IoT devices. This study proposed the design of an aggregator and deaggregator located at the boundary of the access network to aggregate IoT packets from edge networks into the access network and deaggregate at the receiving end. The results of this research provide a detailed analysis of the percentage of packets reduced in the access network and the latency values observed due to aggregation and how aggregation with OpenFlow can help overcome the scale issue in IoT network.


**Installation Steps**  <br/>
1. Install OMNet++ version 5.4.1 suitable for your Operating System. <br/> 
2. Clone the github code from https://github.com/mandashilpa26/OpenFlow_Aggregation_IoT.git <br/>

**Install INET**  <br/>
3. Start the OMNeT++ IDE, and import the inet project via File -> Import -> Existing Projects to the Workspace. A project named INET should appear. This is Inet version 3.6.5. <br/>
4. Build with Project -> Build, or hit Ctrl+B <br/>


**Install OpenFlow** <br/>
5. Import the openflow project via File -> Import -> Existing Projects to the Workspace. A project named INET should appear. This is openflow code with aggregation for IoT packets. <br/>
6. Build with Project -> Build, or hit Ctrl+B <br/>


**Run Tests** <br/>
7. openflow/scenarios/testcases lists the initialisation files for the tests cases for this project. Run the examples with by right clicking on them. <br/>
8. Files Aggregate-S-D.ini are files with S number of sources and D number of destinations. These files use the openflow aggregated sdwan switches. Similarly, NonAggregate-S-D.ini files contain traditional OpenFlow switches without aggregation with S sources and D destinations. <br/>


**Change Aggregation Factor** <br/>
9. The aggregation factor is a configrable parameter communicated by the controller to the aggregate switches. It can be changed in openflow/src/openflow/controllerApps. <br/>


