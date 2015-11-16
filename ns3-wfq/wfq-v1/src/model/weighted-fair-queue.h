 /* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef WEIGHTED_FAIR_QUEUE_H
#define WEIGHTED_FAIR_QUEUE_H

#include <queue>
#include "ns3/packet.h"
#include "ns3/queue.h"
#include "ns3/nstime.h"

#include "ns3/event-id.h"

namespace ns3 {

class TraceContainer;

class ClassMap {
public:

	ClassMap();

	uint32_t m_bytesInWFQQueue;
	uint32_t m_packetsInWFQQueue;
	uint32_t m_maxPackets;
	uint32_t m_maxBytes;
	double_t m_weight;
	double_t m_lastFinishTime;
	bool m_active;
};

class QueueItem {
public:
	Ptr<Packet> m_packet;
	double_t m_finishTime;
	uint16_t m_classIndex;
};

struct CompareFinishTime {
    bool operator()(QueueItem const & qi1, QueueItem const & qi2) {
        return qi1.m_finishTime > qi2.m_finishTime;
    }
};

class WeightedFairQueue: public Queue {
public:
	static TypeId GetTypeId(void);

	WeightedFairQueue();

	~WeightedFairQueue();

	void SetMode(WeightedFairQueue::QueueMode mode);
	WeightedFairQueue::QueueMode GetMode(void);

private:

	virtual bool DoEnqueue(Ptr<Packet> p);
	virtual Ptr<Packet> DoDequeue(void);
	virtual Ptr<const Packet> DoPeek(void) const;

	// returns 0 for 1st queue
	// returns 1 for 2nd queue
	uint16_t Classify(Ptr<Packet> p);

	// collection of finish times of all the packets
	std::map<uint64_t, double_t> getFinishTimeCollection() const;

	// calculates the packet's virtual finish time:
	// finish_time(packet i) = max(finish_time(i-1) , virtual_time(arrival_time(i))) + (packet_size_in_bits / weight)
	double_t CalculateFinishTime(double_t startTime, uint32_t packetSize, double_t weight);

	// calculates the sum of all active queues under GPS system
	double_t CalculateActiveSum(void);

	void InitializeClassMaps(void);

	int16_t GetMinActiveClass(void);

	//void ClearLastFinishTimes(void);

	//void HandleGPSDeparture(void);

	//void ScheduleNextGPSEvent(void);

	// current virtual time
	double_t m_virtualTime;

	// packet classification port
	uint32_t m_secondQueuePort;

	std::vector<ClassMap> m_classList;

	std::priority_queue<QueueItem, std::vector<QueueItem>, CompareFinishTime> m_wfqPQ;

	//double_t m_lastVirtualTimeUpdate;

	double_t m_linkCapacity;

	//EventId m_nextGPSEvent;

	bool m_isInitialized;

	double_t v_chk;
	double_t t_chk;

	//std::queue<Ptr<Packet> > m_firstWeightedQueue;
	//uint32_t m_BytesInFirstQueue;
	uint32_t m_inputFirstMaxPackets;
	uint32_t m_inputFirstMaxBytes;
	double_t m_inputFirstWeight;

	//std::queue<Ptr<Packet> > m_secondWeightedQueue;
	//uint32_t m_BytesInSecondQueue;
	uint32_t m_inputSecondMaxPackets;
	uint32_t m_inputSecondMaxBytes;
	double_t m_inputSecondWeight;

	QueueMode m_mode;

	// collection of virtual finish times of all the packets enqueued
	//std::map<uint64_t, double_t> m_finishTimeCollection;


	// These variables are temporarily used for showing progression percentage, should be removed later
	uint64_t m_passedBytes;
	uint8_t m_20PercentCompleted;
	uint8_t m_40PercentCompleted;
	uint8_t m_60PercentCompleted;
	uint8_t m_80PercentCompleted;
	uint64_t m_totalExpectedBytes;
	uint64_t max_firstQueue;
	uint64_t max_secondQueue;
};

} // namespace ns3

#endif /* WEIGHTED_FAIR_QUEUE_H */

