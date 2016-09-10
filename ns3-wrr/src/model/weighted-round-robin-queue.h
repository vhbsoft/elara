 /* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef WEIGHTED_ROUND_ROBIN_QUEUE_H
#define WEIGHTED_ROUND_ROBIN_QUEUE_H

#include <queue>
#include "ns3/packet.h"
#include "ns3/queue.h"
#include "ns3/nstime.h"

namespace ns3 {

class TraceContainer;

class WeightedRoundRobinQueue: public Queue {
public:
	static TypeId GetTypeId(void);

	WeightedRoundRobinQueue();

	~WeightedRoundRobinQueue();

	void SetMode(WeightedRoundRobinQueue::QueueMode mode);
	WeightedRoundRobinQueue::QueueMode GetMode(void);

private:

	virtual bool DoEnqueue(Ptr<Packet> p);
	virtual Ptr<Packet> DoDequeue(void);
	virtual Ptr<const Packet> DoPeek(void) const;

	// returns 0 for 1st queue
	// returns 1 for 2nd queue
	uint16_t Classify(Ptr<Packet> p);

	// returns the minimum amount of all queues' normalized weight
	double_t GetMinNormalizedWeight();

	// Calculates and sets normalized weight for each queue based on assigned weight and mean packet size
	void CalculateNormalizedWeights();

	// Calculates and sets the number of packets to be served for each queue
	void CalculatePacketsToBeServed();

	// rerurns the queue number to be served
	int32_t GetQueueToBeServed();

	// returns the queue number to be peeked (for DoPeek() method)
	int32_t GetQueueToBePeeked() const;

	// packet classification port
	uint32_t m_secondQueuePort;

	std::queue<Ptr<Packet> > m_firstWeightedQueue;
	uint32_t m_bytesInFirstQueue;
	uint32_t m_firstMaxPackets;
	uint32_t m_firstMaxBytes;
	double_t m_firstWeight;
	double_t m_firstNormalizedWeight;
	uint32_t m_firstPacketsToBeServed;
	uint32_t m_firstMeanPacketSize;
	uint32_t m_firstRemainingPacketsToBeServed;

	std::queue<Ptr<Packet> > m_secondWeightedQueue;
	uint32_t m_bytesInSecondQueue;
	uint32_t m_secondMaxPackets;
	uint32_t m_secondMaxBytes;
	double_t m_secondWeight;
	double_t m_secondNormalizedWeight;
	uint32_t m_secondPacketsToBeServed;
	uint32_t m_secondMeanPacketSize;
	uint32_t m_secondRemainingPacketsToBeServed;

	QueueMode m_mode;

	bool m_isInitialized;


	// These variables are temporarily used for showing progression percentage
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

#endif /* WEIGHTED_ROUND_ROBIN_QUEUE_H */

