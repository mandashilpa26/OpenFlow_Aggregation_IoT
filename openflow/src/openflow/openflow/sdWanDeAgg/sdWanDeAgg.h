/*
 * sdWanDeAgg.h
 *
 *  Created on: Jul 3, 2019
 *      Author: shilpa
 */

#ifndef OPENFLOW_OPENFLOW_SDWANDEAGG_SDWANDEAGG_H_
#define OPENFLOW_OPENFLOW_SDWANDEAGG_SDWANDEAGG_H_



class sdWanDeAgg : public OF_Switch, public cListener {

public:
    sdWanDeAgg();
    ~sdWanDeAgg();

protected:
    //AggregateBuffer aggregate_buffer;
    //simsignal_t dpAggregatePacketHash;

    long deaggregatedPackets;
    long startUpPhasePackets;

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void deAggregatePacket(EthernetIIFrame *frame);
    virtual void processFrame(EthernetIIFrame *frame);
    bool checkAggregatedPacket(EthernetIIFrame *frame);

    void processQueuedMsg(cMessage *data_msg);
    bool checkAggregatedPacket(cMessage *msg);
    virtual void finish();
};


#endif /* OPENFLOW_OPENFLOW_SDWANDEAGG_SDWANDEAGG_H_ */
