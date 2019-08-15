/*
 * AggregateAggregateBuffer.cc
 *
 *  Created on: Jun 24, 2019
 *      Author: shilpa
 */


#include <omnetpp.h>
#include "openflow/openflow/sdWan/AggregateBuffer.h"
#include "openflow/openflow/protocol/openflow.h"
#include "inet/transportlayer/udp/UDPPacket.h"
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace inet;

typedef std::vector<EthernetIIFrame *>      aggregateBufferSet;
typedef std::map<string, aggregateBufferSet> aggregateBufferMap;


AggregateBuffer::AggregateBuffer(){
    aggregateFactor = 1;
}

AggregateBuffer::AggregateBuffer(int cap){
    capacity = cap;
    aggregateFactor = 1;
}

AggregateBuffer::~AggregateBuffer(){

}

int AggregateBuffer::size(){
    return aggregate_msgs.size();
}

bool AggregateBuffer::isfull(){
    return aggregate_msgs.size() >= capacity;
}


// store message in buffer and return buffer_id.
string AggregateBuffer::storeMessage(EthernetIIFrame *msg){
    string dest_mac = msg->getDest().str();

    vector<EthernetIIFrame *> result = (vector<EthernetIIFrame *>)aggregate_msgs[dest_mac];
    if(result.size()!=0){

        result.push_back(msg);
        aggregate_msgs[dest_mac] = result;
        EV << " size of result is.. " << result.size();
    }
    else{
        aggregate_msgs.insert(pair<string, vector<EthernetIIFrame *>>(dest_mac,aggregateBufferSet(10)));
        vector<EthernetIIFrame *> resultHash = (vector<EthernetIIFrame *>)aggregate_msgs[dest_mac];
        resultHash.push_back(msg);
        //aggregate_msgs.insert(pair<string, vector<EthernetIIFrame *>>(hashValue, resultHash));
        aggregate_msgs[dest_mac] = resultHash;
        EV << " size of result is.. " << resultHash.size();
    }

    EV << '\n' << "PacketTest Buffered Destination " << msg->getDest() <<" " << simTime().inUnit(SIMTIME_US)<<'\n';

    return dest_mac;
}

bool AggregateBuffer::deleteMessage(EthernetIIFrame *msg){
    //TODO
    return true;
}


uint32_t AggregateBuffer::getCapacity(){
    return capacity;
}


vector<EthernetIIFrame *> AggregateBuffer::returnMessages(string dest_mac){
    vector<EthernetIIFrame *> frames = (vector<EthernetIIFrame *>)aggregate_msgs[dest_mac];
    aggregate_msgs.erase(dest_mac);
    return frames;
}



bool AggregateBuffer::reachedAggregateCapacity(string dest_mac) {

    vector<EthernetIIFrame *> result = (vector<EthernetIIFrame *>)aggregate_msgs[dest_mac];
    //EV << "reachedAggregateCapacity" << dest_mac << " size of result is.. " << result.size();
    //EV<<" aggregate factor.." << aggregateFactor;

    if(result.size()==aggregateFactor){
        return true;
    }

    return false;
}


int AggregateBuffer::totalLengthPacketsAggregated(string dest_mac) {

    vector<EthernetIIFrame *> result = (vector<EthernetIIFrame *>)aggregate_msgs[dest_mac];
    if(result.size()==0)
        return 0;

    auto totalLength = 0;

    for(auto i = result.begin();i!=result.end();i++){
        if(dynamic_cast<UDPPacket *>((*i)->getEncapsulatedPacket()->getEncapsulatedPacket()) != NULL){

           UDPPacket *udpPacket = (UDPPacket *) (*i)->getEncapsulatedPacket()->getEncapsulatedPacket();
           totalLength+=udpPacket->getByteLength();
        }

}
    return totalLength;
}


uint16_t AggregateBuffer::getAggregateFactor(){

    return aggregateFactor;
}

void AggregateBuffer::setAggregateFactor(uint16_t factor){

    aggregateFactor = factor;
    EV<<" Set aggregate factor.." << aggregateFactor;
}


uint16_t AggregateBuffer::getThreshold(string dest_mac) {

    vector<EthernetIIFrame *> result = (vector<EthernetIIFrame *>)aggregate_msgs[dest_mac];

    threshold = 1522 -(14+20+((result.size()+1)*4));
    return threshold;


}

