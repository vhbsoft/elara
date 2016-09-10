/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "weighted-round-robin-queue.h"

#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"

#include "ns3/ppp-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/ipv4-header.h"

#include <math.h>

NS_LOG_COMPONENT_DEFINE("WeightedRoundRobinQueue");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(WeightedRoundRobinQueue);

TypeId WeightedRoundRobinQueue::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::WeightedRoundRobinQueue").SetParent<Queue>().AddConstructor<
			        WeightedRoundRobinQueue>()

			.AddAttribute("Mode",
					"Whether to use bytes (see MaxBytes) or packets (see MaxPackets) as the maximum queue size metric.",
					EnumValue(QUEUE_MODE_PACKETS),
					MakeEnumAccessor(&WeightedRoundRobinQueue::SetMode),
					MakeEnumChecker(QUEUE_MODE_BYTES, "QUEUE_MODE_BYTES",
							QUEUE_MODE_PACKETS, "QUEUE_MODE_PACKETS"))

			.AddAttribute("SecondWeightedQueueMaxPackets",
					"The maximum number of packets accepted by the second weighted queue.",
					UintegerValue(100),
					MakeUintegerAccessor(&WeightedRoundRobinQueue::m_secondMaxPackets),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("FirstWeightedQueueMaxPackets",
					"The maximum number of packets accepted by the first weighted queue.",
					UintegerValue(100),
					MakeUintegerAccessor(&WeightedRoundRobinQueue::m_firstMaxPackets),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("SecondWeightedQueueMaxBytes",
					"The maximum number of bytes accepted by the second weighted queue.",
					UintegerValue(100 * 65535),
					MakeUintegerAccessor(&WeightedRoundRobinQueue::m_secondMaxBytes),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("FirstWeightedQueueMaxBytes",
					"The maximum number of bytes accepted by the first weighted queue.",
					UintegerValue(100 * 65535),
					MakeUintegerAccessor(&WeightedRoundRobinQueue::m_firstMaxBytes),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("SecondQueuePort",
					"The destination port number for second queue traffic.",
					UintegerValue(3000),
					MakeUintegerAccessor(&WeightedRoundRobinQueue::m_secondQueuePort),
					MakeUintegerChecker<uint32_t>())

	        .AddAttribute("FirstWeight",
	        		"The first queue's weight",
					DoubleValue(0),
					MakeDoubleAccessor(&WeightedRoundRobinQueue::m_firstWeight),
					MakeDoubleChecker<double_t>())

			.AddAttribute("SecondWeight",
					"The second queue's weight",
					DoubleValue(0),
					MakeDoubleAccessor(&WeightedRoundRobinQueue::m_secondWeight),
					MakeDoubleChecker<double_t>())

			.AddAttribute("TotalExpectedBytes",
					"Total expected bytes passed before end of simulation - used for showing progression percentage",
					UintegerValue(0),
					MakeUintegerAccessor(&WeightedRoundRobinQueue::m_totalExpectedBytes),
					MakeUintegerChecker<uint64_t>())

			.AddAttribute("FirstMeanPacketSize",
					"The mean packet size of first queue",
					UintegerValue(1),
					MakeUintegerAccessor(&WeightedRoundRobinQueue::m_firstMeanPacketSize),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("SecondMeanPacketSize",
					"The mean packet size of second queue",
					UintegerValue(1),
					MakeUintegerAccessor(&WeightedRoundRobinQueue::m_secondMeanPacketSize),
					MakeUintegerChecker<uint32_t>());

	return tid;
}

WeightedRoundRobinQueue::WeightedRoundRobinQueue() :
		Queue(), m_firstWeightedQueue(), m_bytesInFirstQueue(0), m_secondWeightedQueue(), m_bytesInSecondQueue(0)
			    {
	NS_LOG_FUNCTION_NOARGS ();

	m_isInitialized = false;

	m_passedBytes = 0;
	m_20PercentCompleted = false;
	m_40PercentCompleted = false;
	m_60PercentCompleted = false;
	m_80PercentCompleted = false;

	max_firstQueue = 0;
	max_secondQueue = 0;
}

WeightedRoundRobinQueue::~WeightedRoundRobinQueue() {
	NS_LOG_FUNCTION_NOARGS ();
}

void WeightedRoundRobinQueue::SetMode(WeightedRoundRobinQueue::QueueMode mode) {
	NS_LOG_FUNCTION(mode);
	m_mode = mode;
}

WeightedRoundRobinQueue::QueueMode WeightedRoundRobinQueue::GetMode(void) {
	NS_LOG_FUNCTION_NOARGS ();
	return m_mode;
}

uint16_t WeightedRoundRobinQueue::Classify(Ptr<Packet> p) {
	NS_LOG_FUNCTION(this << p);
	PppHeader ppp;
	p->RemoveHeader(ppp);
	Ipv4Header ip;
	p->RemoveHeader(ip);

	uint16_t weightedQueue;

	uint32_t protocol = ip.GetProtocol();
	if (protocol == 17) {
		UdpHeader udp;
		p->PeekHeader(udp);

		if (udp.GetDestinationPort() == m_secondQueuePort) {
			NS_LOG_INFO("\tclassifier: second queue udp");
			weightedQueue = 1;
		} else {
			NS_LOG_INFO("\tclassifier: first queue udp");
			weightedQueue = 0;
		}
	}

	else if (protocol == 6) {
		TcpHeader tcp;
		p->PeekHeader(tcp);
		if (tcp.GetDestinationPort() == m_secondQueuePort) {
			NS_LOG_INFO("\tclassifier: second queue tcp");
			weightedQueue = 1;
		} else {
			NS_LOG_INFO("\tclassifier: first queue tcp");
			weightedQueue = 0;
		}

	} else {
		NS_LOG_INFO("\tclassifier: unrecognized transport protocol");
		weightedQueue = 0;
	}

	p->AddHeader(ip);
	p->AddHeader(ppp);


	//*************************************
	// this is temporary, used for showing progression, should be removed later
	if (weightedQueue == 1)
	{
		m_passedBytes = m_passedBytes + p->GetSize() - 54;
		if (m_passedBytes > m_totalExpectedBytes/5 and m_20PercentCompleted == false) {
			std::cout << "20 percent" << std::endl;
			m_20PercentCompleted = true;
			//std::cout << "Max first queue used: " << max_firstQueue << std::endl;
			//std::cout << "Max second queue used: " << max_secondQueue << std::endl;
		}
		else if (m_passedBytes > (2*m_totalExpectedBytes/5) and m_40PercentCompleted == false) {
			std::cout << "40 percent" << std::endl;
			m_40PercentCompleted = true;
		}
		else if (m_passedBytes > (3*m_totalExpectedBytes/5) and m_60PercentCompleted == false) {
			std::cout << "60 percent" << std::endl;
			m_60PercentCompleted = true;
		}
		else if (m_passedBytes > (4*m_totalExpectedBytes/5) and m_80PercentCompleted == false) {
			std::cout << "80 percent" << std::endl;
			m_80PercentCompleted = true;
			//std::cout << "Max first queue used: " << max_firstQueue << std::endl;
			//std::cout << "Max second queue used: " << max_secondQueue << std::endl;
		}
	}
	//*************************************

	return weightedQueue;
}

bool WeightedRoundRobinQueue::DoEnqueue(Ptr<Packet> p) {
	NS_LOG_FUNCTION(this << p);

	if (m_isInitialized == false)
	{
		CalculateNormalizedWeights();
		CalculatePacketsToBeServed();
		m_isInitialized = true;
	}

	uint16_t weightedQueue = Classify(p);

	// Second Queue
	if (weightedQueue == 1) {
		if (m_mode == QUEUE_MODE_PACKETS
				&& (m_secondWeightedQueue.size() >= m_secondMaxPackets)) {
			NS_LOG_LOGIC("Queue full (at max packets) -- dropping pkt");
			Drop(p);
			return false;
		}

		if (m_mode == QUEUE_MODE_BYTES
				&& (m_bytesInSecondQueue + p->GetSize() >= m_secondMaxBytes)) {
			NS_LOG_LOGIC(
					"Queue full (packet would exceed max bytes) -- dropping pkt");
			Drop(p);
			return false;
		}

		m_bytesInSecondQueue += p->GetSize();
		m_secondWeightedQueue.push(p);

		NS_LOG_LOGIC("Number packets " << m_secondWeightedQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInSecondQueue);

		return true;
	}

	// First Queue
	else if (weightedQueue == 0) {
		if (m_mode == QUEUE_MODE_PACKETS
				&& (m_firstWeightedQueue.size() >= m_firstMaxPackets)) {
			NS_LOG_LOGIC("Queue full (at max packets) -- dropping pkt");
			Drop(p);
			return false;
		}

		if (m_mode == QUEUE_MODE_BYTES
				&& (m_bytesInFirstQueue + p->GetSize() >= m_firstMaxBytes)) {
			NS_LOG_LOGIC(
					"Queue full (packet would exceed max bytes) -- dropping pkt");
			Drop(p);
			return false;
		}

		m_bytesInFirstQueue += p->GetSize();
		m_firstWeightedQueue.push(p);

		NS_LOG_LOGIC("Number packets " << m_firstWeightedQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInFirstQueue);

		return true;
	}

	// This normally never happens unless Classify() has been changed
	else {
		return false;
	}
}

Ptr<Packet> WeightedRoundRobinQueue::DoDequeue(void) {
	NS_LOG_FUNCTION(this);

	int32_t queueToBeServed = GetQueueToBeServed();

	if (queueToBeServed == 0) {

		Ptr<Packet> p1 = m_firstWeightedQueue.front ();
		m_firstWeightedQueue.pop();
		m_bytesInFirstQueue -= p1->GetSize();
		m_firstRemainingPacketsToBeServed--;

		NS_LOG_LOGIC("Popped " << p1);

		NS_LOG_LOGIC("Number packets " << m_firstWeightedQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInFirstQueue);

		return p1;
	}

	else if (queueToBeServed == 1) {

		Ptr<Packet> p2 = m_secondWeightedQueue.front ();
		m_secondWeightedQueue.pop();
		m_bytesInSecondQueue -= p2->GetSize();
		m_secondRemainingPacketsToBeServed--;

		NS_LOG_LOGIC("Popped " << p2);

		NS_LOG_LOGIC("Number packets " << m_secondWeightedQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInSecondQueue);

		return p2;
	}

	else {
		NS_LOG_LOGIC("all queues empty");
		return 0;
	}
}

Ptr<const Packet> WeightedRoundRobinQueue::DoPeek(void) const {
	NS_LOG_FUNCTION(this);

	int32_t queueToBeServed = GetQueueToBePeeked();

	if (queueToBeServed == 0) {

		Ptr<Packet> p1 = m_firstWeightedQueue.front ();

		NS_LOG_LOGIC("Number packets " << m_firstWeightedQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInFirstQueue);

		return p1;
	}

	else if (queueToBeServed == 1) {

		Ptr<Packet> p2 = m_secondWeightedQueue.front ();

		NS_LOG_LOGIC("Number packets " << m_secondWeightedQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInSecondQueue);

		return p2;
	}

	else {
		NS_LOG_LOGIC("all queues empty");
		return 0;
	}
}

double_t WeightedRoundRobinQueue::GetMinNormalizedWeight()
{
	if (m_firstNormalizedWeight < m_secondNormalizedWeight)
		return m_firstNormalizedWeight;
	else
		return m_secondNormalizedWeight;
}

void WeightedRoundRobinQueue::CalculateNormalizedWeights()
{
	m_firstNormalizedWeight = m_firstWeight / double_t(m_firstMeanPacketSize);
	m_secondNormalizedWeight = m_secondWeight / double_t(m_secondMeanPacketSize);
}

void WeightedRoundRobinQueue::CalculatePacketsToBeServed()
{
	double_t minNormalizedWeight = GetMinNormalizedWeight();

	m_firstPacketsToBeServed = round (m_firstNormalizedWeight / minNormalizedWeight);
	m_secondPacketsToBeServed = round (m_secondNormalizedWeight / minNormalizedWeight);
}

int32_t WeightedRoundRobinQueue::GetQueueToBeServed()
{
	if (m_firstRemainingPacketsToBeServed > 0)
	{
		if (!m_firstWeightedQueue.empty())
		{
			return 0;
		}
		else
		{
			m_firstRemainingPacketsToBeServed = 0;
		}
	}

	if (m_secondRemainingPacketsToBeServed > 0)
	{
		if (!m_secondWeightedQueue.empty())
		{
			return 1;
		}
		else
		{
			m_secondRemainingPacketsToBeServed = 0;
		}
	}

	m_firstRemainingPacketsToBeServed = m_firstPacketsToBeServed;
	m_secondRemainingPacketsToBeServed = m_secondPacketsToBeServed;


	if (m_firstRemainingPacketsToBeServed > 0)
	{
		if (!m_firstWeightedQueue.empty())
		{
			return 0;
		}
		else
		{
			m_firstRemainingPacketsToBeServed = 0;
		}
	}

	if (m_secondRemainingPacketsToBeServed > 0)
	{
		if (!m_secondWeightedQueue.empty())
		{
			return 1;
		}
		else
		{
			m_secondRemainingPacketsToBeServed = 0;
		}
	}

	m_firstRemainingPacketsToBeServed = m_firstPacketsToBeServed;
	m_secondRemainingPacketsToBeServed = m_secondPacketsToBeServed;

	return -1;
}

int32_t WeightedRoundRobinQueue::GetQueueToBePeeked() const
{
	if (m_firstRemainingPacketsToBeServed > 0)
	{
		if (!m_firstWeightedQueue.empty())
		{
			return 0;
		}
	}

	if (m_secondRemainingPacketsToBeServed > 0)
	{
		if (!m_secondWeightedQueue.empty())
		{
			return 1;
		}
	}

	if (!m_firstWeightedQueue.empty())
	{
		return 0;
	}

	if (!m_secondWeightedQueue.empty())
	{
		return 1;
	}

	return -1;
}

} // namespace ns3
