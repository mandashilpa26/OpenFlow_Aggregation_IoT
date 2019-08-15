/*
 * AggregateBuffer.h
 *
 *  Created on: Jun 24, 2019
 *      Author: shilpa
 */

#ifndef OPENFLOW_OPENFLOW_SDWAN_AGGREGATEBUFFER_H_
#define OPENFLOW_OPENFLOW_SDWAN_AGGREGATEBUFFER_H_


#include <deque>
#include <map>
#include "inet/linklayer/ethernet/EtherFrame_m.h"
#include "inet/linklayer/common/MACAddress.h"


using namespace std;
using namespace inet;

typedef std::vector<EthernetIIFrame *>      aggregateBufferSet;
typedef std::map<string, aggregateBufferSet> aggregateBufferMap;


class AggregateBuffer {

public:
    AggregateBuffer();
    AggregateBuffer(int cap);
    ~AggregateBuffer();
    bool isfull();
    string storeMessage(EthernetIIFrame *msg);
    bool deleteMessage(EthernetIIFrame *msg);
    aggregateBufferSet returnMessages(string dest_mac);
    uint32_t getCapacity();
    int size();
    bool reachedAggregateCapacity(string dest_mac);
    int totalLengthPacketsAggregated(string dest_mac);
    uint16_t getAggregateFactor();
    void setAggregateFactor(uint16_t factor);

    uint16_t getThreshold(string dest_mac);




protected:
    aggregateBufferMap aggregate_msgs;
    uint32_t capacity;
    uint16_t aggregateFactor;
    uint16_t threshold;
};




#endif /* OPENFLOW_OPENFLOW_SDWAN_AGGREGATEBUFFER_H_ */
