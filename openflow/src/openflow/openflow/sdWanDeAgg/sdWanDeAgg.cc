/*
 * sdWanDeAggDeAgg.cc
 *
 *  Created on: Jul 3, 2019
 *      Author: shilpa
 */



#include "openflow/openflow/switch/OF_Switch.h"
#include "openflow/openflow/protocol/openflow.h"
#include "openflow/openflow/sdWanDeAgg/sdWanDeAgg.h"

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


#define MSGKIND_SERVICETIME                 3

Define_Module(sdWanDeAgg);

sdWanDeAgg::sdWanDeAgg(){

}

sdWanDeAgg::~sdWanDeAgg(){

}


void sdWanDeAgg::initialize(){

    // read ned parameters
    OF_Switch::initialize();
    aggregate = true;
    deaggregatedPackets = 0l;
    startUpPhasePackets = 0l;

}

void sdWanDeAgg::handleMessage(cMessage *msg){

    OF_Switch::handleMessage(msg);

}


void sdWanDeAgg::processFrame(EthernetIIFrame *frame){
    oxm_basic_match match = oxm_basic_match();

    EV << " in process frame.. extracting details";
    //extract match fields

    if(dynamic_cast<OF_Aggregate *>(frame->getEncapsulatedPacket()->getEncapsulatedPacket()) != NULL){
        take(frame);

    }
    match.OFB_IN_PORT = 0;
    EV<< "port is " << match.OFB_IN_PORT;
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

    /*
     * Add code for udp packet.
     * #TODO
     */

   Flow_Table_Entry *lookup = flowTable.lookup(match);
   if (lookup != NULL){
       //lookup successful
       flowTableHit++;
       EV << "Found entry in flow table." << '\n';
       ofp_action_output action_output = lookup->getInstructions();
       uint32_t outport = action_output.port;
       /*
        * If not an aggregated packet, send to aggregate buffer with timer values.
        * Aggregate
        * # revisit
        */

       bool isAggregate = checkAggregatedPacket(frame);
       if(aggregate && isAggregate){
           EV << "Calling deaggregate." << '\n';
           deAggregatePacket(frame);
       }

       else if(outport == OFPP_CONTROLLER){
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

               if(hash !=0){
                   emit(dpPingPacketHash,hash);
               }
               //send it out the dataplane on the specific port
               send(frame, "dataPlaneOut", outport);

       }
   } else {


       bool isAggregate = checkAggregatedPacket(frame);
        if(aggregate && isAggregate){
            EV << "Calling deaggregate." << '\n';
            deAggregatePacket(frame);
        }
        else{

           if(hash !=0){
               emit(cpPingPacketHash,hash);
           }
           // lookup failed
           flowTableMiss++;
           EV << "No Entry Found contacting controller" << '\n';
           handleMissMatchedPacket(frame);
        }
}
}


// Aggregate

bool sdWanDeAgg::checkAggregatedPacket(EthernetIIFrame *frame){

    if(dynamic_cast<OF_Aggregate *>(frame->getEncapsulatedPacket()->getEncapsulatedPacket()) == NULL){
        EV<<" Non aggregated Packet"<<'\n';
        return false;
    }
    EV << "Aggregated packet"<<'\n';
    return true;
}

bool sdWanDeAgg::checkAggregatedPacket(cMessage *msg){
    if (dynamic_cast<EthernetIIFrame *>(msg) != NULL){
           EthernetIIFrame *frame = (EthernetIIFrame *)msg;
           if(dynamic_cast<OF_Aggregate *>(frame->getEncapsulatedPacket()->getEncapsulatedPacket()) == NULL){
                   EV<<" Non aggregated Packet"<<'\n';
                   return false;
           }
           else return true;
    }
    EV << "Aggregated packet"<<'\n';
    return true;
}


void sdWanDeAgg::processQueuedMsg(cMessage *data_msg){
    /*
     * the switch has received a message( depending on control/ data, put code in data plane )
     * disaggregation code.
     */

    //EV << "reached process queue nessage";
    OF_Switch::processQueuedMsg(data_msg);


}


void sdWanDeAgg::deAggregatePacket(EthernetIIFrame *frame){

    if(dynamic_cast<OF_Aggregate *>(frame->getEncapsulatedPacket()->getEncapsulatedPacket()) != NULL){
        IPv4Datagram *aggregatedIPMessage = (IPv4Datagram *)frame->getEncapsulatedPacket();
        OF_Aggregate *ofaggregatedMessage = (OF_Aggregate *)frame->getEncapsulatedPacket()->getEncapsulatedPacket();

        int inPort = frame->getArrivalGate()->getIndex();
        int moduleId = frame->getArrivalModule()->getIndex();

        int numberOfPackets = ofaggregatedMessage->getSubframesArraySize();
        //std::vector<EthernetIIFrame *> *frames = new std::vector<EthernetIIFrame *>();
        for(int i=0;i<numberOfPackets;i++){

            UDP_Aggregate udpAggregatePacket = ofaggregatedMessage->getSubframes(i);
            IPv4Address src_ip = udpAggregatePacket.getSrcAddress();
            UDPPacket *udpPacket = (UDPPacket *)udpAggregatePacket.decapsulate();

            //take(udpPacket);
            // Create new packet
            auto ethernetPacket = new EthernetIIFrame("NonAggregated");
            auto IPPacket = new IPv4Datagram("NonAggregated");

            ethernetPacket->setSrc(frame->getSrc());
            ethernetPacket->setDest(frame->getDest());
            ethernetPacket->setEtherType(frame->getEtherType());
            ethernetPacket->setArrival(moduleId,inPort);

            IPPacket->setSrcAddress(src_ip);
            IPPacket->setDestAddress(aggregatedIPMessage->getDestAddress());
            IPPacket->setTransportProtocol(aggregatedIPMessage->getTransportProtocol());

            IPPacket->encapsulate(udpPacket);
            ethernetPacket->encapsulate(IPPacket);

            take(ethernetPacket);
            EV << " Sending packet";
            EthernetIIFrame *copy = ethernetPacket->dup();
            deaggregatedPackets++;
            processFrame(copy);

            delete ethernetPacket;

        }

    }
    //drop(frame);
    delete frame;

    //return frames;
}


void sdWanDeAgg::finish(){

    OF_Switch::finish();

    // record statistics
    recordScalar("deaggregatedPackets", deaggregatedPackets);
    recordScalar("startUpPhasePackets", startUpPhasePackets);

}




