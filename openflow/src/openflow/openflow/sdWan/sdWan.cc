/*
 * sdWan.cc
 *
 *  Created on: Jun 22, 2019
 *      Author: shilpa
 */

#include "openflow/openflow/switch/OF_Switch.h"
#include "openflow/openflow/protocol/openflow.h"
#include "openflow/openflow/sdWan/sdWan.h"

#include "openflow/messages/Open_Flow_Message_m.h"
#include "openflow/messages/OFP_Initialize_Handshake_m.h"
#include "openflow/messages/OFP_Features_Reply_m.h"
#include "openflow/messages/OFP_Hello_m.h"

#include "openflow/messages/OFP_Packet_In_m.h"
#include "openflow/messages/OFP_Packet_Out_m.h"
#include "openflow/messages/OFP_Flow_Mod_m.h"
#include "openflow/messages/OF_Aggregate_m.h"
#include "inet/linklayer/ethernet/EtherMAC.h"

#include "inet/networklayer/ipv4/IPv4Datagram.h"
#include "inet/networklayer/arp/ipv4/ARPPacket_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"

#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/InterfaceTable.h"

#include "inet/applications/pingapp/PingPayload_m.h"
#include "inet/networklayer/ipv4/ICMPMessage.h"
#include "inet/transportlayer/udp/UDPPacket.h"
#include <vector>
#include <omnetpp.h>
#include <iostream>
#include <fstream>

#define MSGKIND_SERVICETIME                 3
#define FILENAME    "timings.txt"

#define MSGKIND_CONNECT                     1

Define_Module(sdWan);

sdWan::sdWan(){

}

sdWan::~sdWan(){

}


void sdWan::initialize(){

    // read ned parameters

    //aggregateTime = par("aggregateTime");
    aggregate_buffer = AggregateBuffer((int)par("aggBufferCapacity"));

    OF_Switch::initialize();
    aggregate = true;
    aggregatedPackets =0l;

    dpPingPacketHash = registerSignal("dpPingPacketHash");
    cpPingPacketHash = registerSignal("cpPingPacketHash");
    queueSize = registerSignal("queueSize");
    bufferSize = registerSignal("bufferSize");
    waitingTime = registerSignal("waitingTime");

}



void sdWan::handleMessage(cMessage *msg){

    OF_Switch::handleMessage(msg);

}


void sdWan::processAggregatePacket(EthernetIIFrame *frame){


    UDPPacket *udpPacket = (UDPPacket *) frame->getEncapsulatedPacket()->getEncapsulatedPacket();
    int currentPacketLength=udpPacket->getByteLength();

    string dest_mac = frame->getDest().str();

    // get and compare with threshold
    uint16_t threshold = aggregate_buffer.getThreshold(dest_mac);
    if(aggregate_buffer.totalLengthPacketsAggregated(dest_mac)+currentPacketLength > threshold){
        createAggregatedPacket(frame);
        string hashValue = aggregate_buffer.storeMessage(frame);
        recordTime("timings.txt",simTime(), "Packet Buffered");

    }
    else{
        string hashValue = aggregate_buffer.storeMessage(frame);
        recordTime("timings.txt",simTime(), "Packet Buffered");
    }

    /*
     * emit signal for aggregate
     */
    if(aggregate_buffer.reachedAggregateCapacity(dest_mac)){
        /*
         * if capacity is reached. call aggregate function.
         */
        //emit(dpAggregatePacketHash,frame);
        EV << "Creating aggregate message." << '\n';
        createAggregatedPacket(frame);
    }

}


void sdWan::processFrame(EthernetIIFrame *frame){

    oxm_basic_match match = oxm_basic_match();

    //EV << " in process frame.. extracting details";
    //extract match fields

    if(dynamic_cast<OF_Aggregate *>(frame->getEncapsulatedPacket()->getEncapsulatedPacket()) != NULL){
        take(frame);
        match.OFB_IN_PORT = 0;

    }
    else{
        match.OFB_IN_PORT = frame->getArrivalGate()->getIndex();
    }
    //EV<< "port is " << match.OFB_IN_PORT;
    match.OFB_ETH_SRC = frame->getSrc();
    match.OFB_ETH_DST = frame->getDest();
    match.OFB_ETH_TYPE = frame->getEtherType();

    //extract ARP specific match fields if present
    if(frame->getEtherType()==ETHERTYPE_ARP){
        ARPPacket *arpPacket = check_and_cast<ARPPacket *>(frame->getEncapsulatedPacket());
        match.OFB_ARP_OP = arpPacket->getOpcode();
        match.OFB_ARP_SHA = arpPacket->getSrcMACAddress();
        match.OFB_ARP_THA = arpPacket->getDestMACAddress();
        match.OFB_ARP_SPA = arpPacket->getSrcIPAddress();
        match.OFB_ARP_TPA = arpPacket->getDestIPAddress();
    }

    unsigned long hash =0;

    //emit id of ping packet to indicate where it was processed
    if(dynamic_cast<ICMPMessage *>(frame->getEncapsulatedPacket()->getEncapsulatedPacket()) != NULL){
        ICMPMessage *icmpMessage = (ICMPMessage *)frame->getEncapsulatedPacket()->getEncapsulatedPacket();

        PingPayload * pingMsg =  (PingPayload * )icmpMessage->getEncapsulatedPacket();
        //generate and emit hash
        std::stringstream hashString;
        hashString << "SeqNo-" << pingMsg->getSeqNo() << "-Pid-" << pingMsg->getOriginatorId();
        hash = std::hash<std::string>()(hashString.str().c_str());
    }



   Flow_Table_Entry *lookup = flowTable.lookup(match);
   if (lookup != NULL){
       //lookup successful
       flowTableHit++;
       //EV << "Found entry in flow table." << '\n';
       ofp_action_output action_output = lookup->getInstructions();
       uint32_t outport = action_output.port;
       if(outport == OFPP_CONTROLLER){
           //send it to the controller
           OFP_Packet_In *packetIn = new OFP_Packet_In("packetIn");
           packetIn->getHeader().version = OFP_VERSION;
           packetIn->getHeader().type = OFPT_PACKET_IN;
           packetIn->setReason(OFPR_ACTION);
           packetIn->setByteLength(32);
           packetIn->encapsulate(frame);
           packetIn->setBuffer_id(OFP_NO_BUFFER);
           socket.send(packetIn);
           if(hash !=0){
               emit(cpPingPacketHash,hash);
           }
       } else {
           /*
            * If not an aggregated packet, send to aggregate buffer with timer values.
            * Aggregate
            * # revisit
            */

           bool isAggregate = checkAggregatedPacket(frame);
           if(aggregate && !isAggregate){
               //EV << "Calling process aggregate." << '\n';
               processAggregatePacket(frame);
           }
           else{
               //EV<<" else block";
               if(hash !=0){
                   emit(dpPingPacketHash,hash);
               }
               //send it out the dataplane on the specific port
               //EV << "Sending packet out";
               send(frame, "dataPlaneOut", outport);
           }
       }
   } else {
       if(hash !=0){
           emit(cpPingPacketHash,hash);
       }
       // lookup failed
       flowTableMiss++;
       //EV << "No Entry Found contacting controller" << '\n';
       handleMissMatchedPacket(frame);
   }
}


// Aggregate

bool sdWan::checkAggregatedPacket(EthernetIIFrame *frame){

    if(dynamic_cast<OF_Aggregate *>(frame->getEncapsulatedPacket()->getEncapsulatedPacket()) == NULL){
        //EV<<" Non aggregated Packet"<<'\n';
        return false;
    }
    //EV << "Aggregated packet"<<'\n';
    return true;
}

bool sdWan::checkAggregatedPacket(cMessage *msg){
    if (dynamic_cast<EthernetIIFrame *>(msg) != NULL){
           EthernetIIFrame *frame = (EthernetIIFrame *)msg;
           if(dynamic_cast<OF_Aggregate *>(frame->getEncapsulatedPacket()->getEncapsulatedPacket()) == NULL){
                   //EV<<" Non aggregated Packet"<<'\n';
                   return false;
           }
           else return true;
    }
    //EV << "Aggregated packet"<<'\n';
    return true;
}


void sdWan::processQueuedMsg(cMessage *data_msg){
    /*
     * the switch has received a message( depending on control/ data, put code in data plane )
     * disaggregation code.
     */
    EV << "reached process queue nessage";

    if(checkAggregatedPacket(data_msg)){
        EthernetIIFrame *frame = (EthernetIIFrame *)data_msg;
        EthernetIIFrame *copy = frame->dup();
        processFrame(copy);
        delete frame;

    }
    else if(data_msg->arrivedOn("dataPlaneIn")){
        dataPlanePacket++;
        //EV<<"data plane"<<'\n';
        if(socket.getState() != TCPSocket::CONNECTED){
            //no yet connected to controller
            //drop packet by returning
            return;
        }
        if (dynamic_cast<EthernetIIFrame *>(data_msg) != NULL){ //msg from dataplane
            EthernetIIFrame *frame = (EthernetIIFrame *)data_msg;

            EthernetIIFrame *copy = frame->dup();
            processFrame(copy);

        }
    } else {
        controlPlanePacket++;
        //EV<<"control plane"<<'\n';
       if (dynamic_cast<Open_Flow_Message *>(data_msg) != NULL) { //msg from controller
            Open_Flow_Message *of_msg = (Open_Flow_Message *)data_msg;
            ofp_type type = (ofp_type)of_msg->getHeader().type;
            switch (type){
                case OFPT_FEATURES_REQUEST:
                    handleFeaturesRequestMessage(of_msg);
                    break;
                case OFPT_FLOW_MOD:
                    EV<< " calling flow mod in sdwan"<<'\n';
                    handleFlowModMessage(of_msg);
                    //#TODO
                    break;
                case OFPT_PACKET_OUT:
                    handlePacketOutMessage(of_msg);
                    break;
                }
        }

    }

}


void sdWan::createAggregatedPacket(EthernetIIFrame *msg) {

    // Extract layer 2 parameters
    int inPort = msg->getArrivalGate()->getIndex();
    int moduleId = msg->getArrivalModule()->getIndex();


    //EV << "cgate" << arrivalGate->str();
    MACAddress src_mac = msg->getSrc();
    MACAddress dest_mac = msg->getDest();
    int etherType = msg->getEtherType();


    // Extract layer 3 parameters
    auto layer3Packet = (IPv4Datagram *)msg->getEncapsulatedPacket();
    IPv4Address src_ip = layer3Packet->getSrcAddress();
    IPv4Address dest_ip = layer3Packet->getDestAddress();
    int protocol = layer3Packet->getTransportProtocol();

    // Get the vector of ethernetframes to be aggregated.
    string hashValue = dest_mac.str();
    vector<EthernetIIFrame *> result = aggregate_buffer.returnMessages(hashValue);

    //Create outer frame( OF_Aggregate message), IP message and Ethernetframe
    auto aggregatedPacket = new EthernetIIFrame("Aggregated");
    auto aggregatedIPPacket = new IPv4Datagram("Aggregated");
    auto ofAggregatedPacket = new OF_Aggregate("Aggregated");


    //EV << "Extracted all values." << '\n';

    ofAggregatedPacket->setSubframesArraySize(result.size());
    auto totalLength = 0;
    int count = 0;

    for(auto i = result.begin();i!=result.end();i++){
        if(dynamic_cast<UDPPacket *>((*i)->getEncapsulatedPacket()->getEncapsulatedPacket()) != NULL){

            // Calculate length and get the IP address
           EV << "packet count " << count <<'\n';
           UDPPacket *udpPacket = (UDPPacket *) (*i)->getEncapsulatedPacket()->getEncapsulatedPacket();

           auto tempPacket = (IPv4Datagram *)(*i)->getEncapsulatedPacket();
           IPv4Address packet_src_ip = tempPacket->getSrcAddress();


           // create aggregated udp packet and encapsulate captured udp packet.
           UDP_Aggregate udpAggregate;
           udpAggregate.setSrcAddress(packet_src_ip);

           udpAggregate.encapsulate(udpPacket->dup());

           totalLength+=udpPacket->getByteLength()+4;

           ofAggregatedPacket->setSubframes(count, udpAggregate);
           count++;

           //EV << "Aggregated Packet length " << count << " " << totalLength <<'\n';
           dataPlanePacket++;

        }
    }
    ofAggregatedPacket->setByteLength(totalLength);
    EV << "Aggregated Packet length " << totalLength <<'\n';

    // Fill IP packet
    /*
     * get the switch own IP Address
     * #revisit
     */
    aggregatedIPPacket->setSrcAddress(src_ip);
    aggregatedIPPacket->setDestAddress(dest_ip);
    aggregatedIPPacket->encapsulate(ofAggregatedPacket);
    aggregatedIPPacket->setTransportProtocol(protocol);

    // Fill ethernet packet
    aggregatedPacket->setSrc(src_mac);
    aggregatedPacket->setDest(dest_mac);
    aggregatedPacket->setEtherType(etherType);
    aggregatedPacket->setArrival(moduleId,inPort);


    //aggregatedPacket->setControlInfo(controlInfo);

    aggregatedPacket->encapsulate(aggregatedIPPacket);

    //EthernetIIFrame *copy = aggregatedPacket->dup();

    cMessage *message = (cMessage *) aggregatedPacket;
    message->setArrival(moduleId,inPort);
    message->setContextPointer(message);


    aggregatedPackets++;

    recordTime("timings.txt",simTime(),"PacketTest Aggregated");
    EV << '\n' << "PacketTest Aggregated Destination " << dest_mac << " " << simTime().inUnit(SIMTIME_US) <<'\n';
    processQueuedMsg(message);
}

void sdWan::finish(){

    OF_Switch::finish();

    // record statistics
    recordScalar("aggregatedPackets", aggregatedPackets);


}

void sdWan::recordTime(std::string filename, simtime_t time, std::string data){

     ofstream  out;
     out.open(FILENAME, std::ios_base::app);
     out << data;
     out << time;
     out.close();
}


void sdWan::handleFlowModMessage(Open_Flow_Message *of_msg){


    //EV << "sdwan_switch::handleFlowModMessage" << '\n';
    OFP_Flow_Mod *flowModMsg = (OFP_Flow_Mod *) of_msg;

    flowTable.addEntry(Flow_Table_Entry(flowModMsg));

    uint16_t aggregateFactor = flowModMsg->getAggregateFactor();
    //EV<<" handle flow mod in sdwan.cc.. value of aggregation factor "<< aggregateFactor;
    if(aggregate){
        aggregate_buffer.setAggregateFactor(aggregateFactor);
    }

}

