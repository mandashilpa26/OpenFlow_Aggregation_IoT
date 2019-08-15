/*
 * sdWan.h
 *
 *  Created on: Jun 22, 2019
 *      Author: shilpa
 */

#include "openflow/openflow/sdWan/AggregateBuffer.h"
#include "openflow/messages/OF_Aggregate_m.h"
#include <omnetpp.h>
#include <vector>


#ifndef OPENFLOW_OPENFLOW_SDWAN_SDWAN_H_
#define OPENFLOW_OPENFLOW_SDWAN_SDWAN_H_


class sdWan : public OF_Switch, public cListener {

public:
    sdWan();
    ~sdWan();

protected:

    //stats
    simsignal_t dpPingPacketHash;
    simsignal_t cpPingPacketHash;
    simsignal_t queueSize;
    simsignal_t bufferSize;
    simsignal_t waitingTime;
    simsignal_t dpAggregatePacketHash;


    AggregateBuffer aggregate_buffer;
    long aggregatedPackets;

    //long totalPackets;
    TCPSocket socket;

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void processAggregatePacket(EthernetIIFrame *frame);
    virtual void processFrame(EthernetIIFrame *frame);
    bool checkAggregatedPacket(EthernetIIFrame *frame);
    void createAggregatedPacket(EthernetIIFrame *frame);

    void processQueuedMsg(cMessage *data_msg);
    bool checkAggregatedPacket(cMessage *msg);
    //void aggregateAndSendPacket(vector<EthernetIIFrame *> result);
    virtual void finish();
    void recordTime(std::string filename, simtime_t time, std::string data);
    virtual void handleFlowModMessage(Open_Flow_Message *of_msg);

};


#endif /* OPENFLOW_OPENFLOW_SDWAN_SDWAN_H_ */
