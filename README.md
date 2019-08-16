# OpenFlow_Aggregation_IoT
# OpenFlow-based Aggregation Mechanism for Communication in the Internet of Things

Internet of Things(IoT) is a novel technology which polls data at high frequencies and produces packets with small payloads. IoT devices make distinct measurements
thus producing heterogeneous in a small area. These densely deployed devices transmit their data through the access network to store their values on the cloud, leading to
flooding of packets in the access networks. IoT networks need mechanisms to handle the scale issue in future. OpenFlow is a Software-Defined Networking(SDN) approach
which separates the control plane from data plane offering centralised control. Flow aggregation gathers and aggregates data based on custom flows such as source address
and destination address, port numbers. This research aims at integrating support for data aggregation in OpenFlow for communication in IoT devices. This study proposed
the design of an aggregator and deaggregator located at the boundary of the access network to aggregate IoT packets from edge networks into the access network and
deaggregate at the receiving end. The results of this research provide a detailed analysis of the percentage of packets reduced in the access network and the latency values
observed due to aggregation and how aggregation with OpenFlow can help overcome the scale issue in IoT network.
