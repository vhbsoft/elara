/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "weighted-fair-queue.h"

#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"

#include "ns3/ppp-header.h"
#include "ns3/udp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/ipv4-header.h"

NS_LOG_COMPONENT_DEFINE("WeightedFairQueue");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(WeightedFairQueue);

TypeId WeightedFairQueue::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::WeightedFairQueue").SetParent<Queue>().AddConstructor<
			        WeightedFairQueue>()

			.AddAttribute("Mode",
					"Whether to use bytes (see MaxBytes) or packets (see MaxPackets) as the maximum queue size metric.",
					EnumValue(QUEUE_MODE_PACKETS),
					MakeEnumAccessor(&WeightedFairQueue::SetMode),
					MakeEnumChecker(QUEUE_MODE_BYTES, "QUEUE_MODE_BYTES",
							QUEUE_MODE_PACKETS, "QUEUE_MODE_PACKETS"))

			.AddAttribute("SecondWeightedQueueMaxPackets",
					"The maximum number of packets accepted by the second weighted queue.",
					UintegerValue(100),
					MakeUintegerAccessor(&WeightedFairQueue::m_secondMaxPackets),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("FirstWeightedQueueMaxPackets",
					"The maximum number of packets accepted by the first weighted queue.",
					UintegerValue(100),
					MakeUintegerAccessor(&WeightedFairQueue::m_firstMaxPackets),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("SecondWeightedQueueMaxBytes",
					"The maximum number of bytes accepted by the second weighted queue.",
					UintegerValue(100 * 65535),
					MakeUintegerAccessor(&WeightedFairQueue::m_secondMaxBytes),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("FirstWeightedQueueMaxBytes",
					"The maximum number of bytes accepted by the first weighted queue.",
					UintegerValue(100 * 65535),
					MakeUintegerAccessor(&WeightedFairQueue::m_firstMaxBytes),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("SecondQueuePort",
					"The destination port number for second queue traffic.",
					UintegerValue(3000),
					MakeUintegerAccessor(&WeightedFairQueue::m_secondQueuePort),
					MakeUintegerChecker<uint32_t>())

	        .AddAttribute("FirstWeight",
	        		"The first queue's weight",
					DoubleValue(0),
					MakeDoubleAccessor(&WeightedFairQueue::m_firstWeight),
					MakeDoubleChecker<double_t>())

			.AddAttribute("SecondWeight",
					"The second queue's weight",
					DoubleValue(0),
					MakeDoubleAccessor(&WeightedFairQueue::m_secondWeight),
					MakeDoubleChecker<double_t>())

			.AddAttribute("TotalExpectedBytes",
					"Total expected bytes passed before end of simulation - used for showing progression percentage",
					UintegerValue(0),
					MakeUintegerAccessor(&WeightedFairQueue::m_totalExpectedBytes),
					MakeUintegerChecker<uint64_t>());

	return tid;
}

WeightedFairQueue::WeightedFairQueue() :
		Queue(), m_virtualTime(0.0), m_firstWeightedQueue(), m_bytesInFirstQueue(0), m_secondWeightedQueue(), m_bytesInSecondQueue(0)
			   , m_finishTimeCollection() {
	NS_LOG_FUNCTION_NOARGS ();

	m_passedBytes = 0;
	m_20PercentCompleted = false;
	m_40PercentCompleted = false;
	m_60PercentCompleted = false;
	m_80PercentCompleted = false;

	max_firstQueue = 0;
	max_secondQueue = 0;
}

WeightedFairQueue::~WeightedFairQueue() {
	NS_LOG_FUNCTION_NOARGS ();
}

void WeightedFairQueue::SetMode(WeightedFairQueue::QueueMode mode) {
	NS_LOG_FUNCTION(mode);
	m_mode = mode;
}

WeightedFairQueue::QueueMode WeightedFairQueue::GetMode(void) {
	NS_LOG_FUNCTION_NOARGS ();
	return m_mode;
}

uint16_t WeightedFairQueue::Classify(Ptr<Packet> p) {
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

bool WeightedFairQueue::DoEnqueue(Ptr<Packet> p) {
	NS_LOG_FUNCTION(this << p);
	double_t currentFinishTime, currentStartTime;

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

		// calculating the virtual finish time
		if (m_secondWeightedQueue.empty())
			currentStartTime = m_virtualTime;
		else
			currentStartTime = m_finishTimeCollection[m_secondWeightedQueue.back()->GetUid()];
		currentFinishTime = CalculateFinishTime(currentStartTime, p->GetSize(), m_secondWeight);
		m_finishTimeCollection[p->GetUid()] = currentFinishTime;

		m_bytesInSecondQueue += p->GetSize();
		m_secondWeightedQueue.push(p);

		NS_LOG_LOGIC("Number packets " << m_secondWeightedQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInSecondQueue);

		//*****************************************
		// temporary, should be removed later
	    if (m_bytesInSecondQueue > max_secondQueue)
	    	max_secondQueue = m_bytesInSecondQueue;
		//*****************************************

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

		// calculating the virtual finish time
		if (m_firstWeightedQueue.empty())
			currentStartTime = m_virtualTime;
		else
			currentStartTime = m_finishTimeCollection[m_firstWeightedQueue.back()->GetUid()];
		currentFinishTime = CalculateFinishTime(currentStartTime, p->GetSize(), m_firstWeight);
		m_finishTimeCollection[p->GetUid()] = currentFinishTime;

		m_bytesInFirstQueue += p->GetSize();
		m_firstWeightedQueue.push(p);

		NS_LOG_LOGIC("Number packets " << m_firstWeightedQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInFirstQueue);



		//*****************************************
		// temporary, should be removed later
		if (m_bytesInFirstQueue > max_firstQueue)
			max_firstQueue = m_bytesInFirstQueue;
		//*****************************************

		return true;
	}

	// This normally never happens unless Classify() has been changed
	else {
		return false;
	}
}

Ptr<Packet> WeightedFairQueue::DoDequeue(void) {
	double_t firstFinishTime, secondFinishTime, minFinishTime = (1.7e+308);
	int16_t minQueue = -1;
	Ptr<Packet> p1, p2;
	NS_LOG_FUNCTION(this);

    if (!m_firstWeightedQueue.empty()) {
    	p1 = m_firstWeightedQueue.front();
    	firstFinishTime = m_finishTimeCollection[p1->GetUid()];
    	if (firstFinishTime <= minFinishTime) {
    		minFinishTime = firstFinishTime;
    		minQueue = 0;
    	}
    }

	if (!m_secondWeightedQueue.empty()) {
		p2 = m_secondWeightedQueue.front();
		secondFinishTime = m_finishTimeCollection[p2->GetUid()];
		if (secondFinishTime <= minFinishTime) {
			minFinishTime = secondFinishTime;
			minQueue = 1;
		}
	}

	if (minQueue == 0) {

		m_virtualTime += 8*p1->GetSize() / CalculateWeightSum();

		m_firstWeightedQueue.pop();
		m_bytesInFirstQueue -= p1->GetSize();
		m_finishTimeCollection.erase(p1->GetUid());

		NS_LOG_LOGIC("Popped " << p1);

		NS_LOG_LOGIC("Number packets " << m_firstWeightedQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInFirstQueue);

		return p1;
	}

	else if (minQueue == 1) {

		m_virtualTime += 8*p2->GetSize() / CalculateWeightSum();

		m_secondWeightedQueue.pop();
		m_bytesInSecondQueue -= p2->GetSize();
		m_finishTimeCollection.erase(p2->GetUid());

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

Ptr<const Packet> WeightedFairQueue::DoPeek(void) const {
	double_t firstFinishTime, secondFinishTime, minFinishTime = (1.7e+308);
	int16_t minQueue = -1;
	Ptr<Packet> p1, p2;
	NS_LOG_FUNCTION(this);

    if (!m_firstWeightedQueue.empty()) {
    	p1 = m_firstWeightedQueue.front();
    	firstFinishTime =  getFinishTimeCollection()[p1->GetUid()];
    	if (firstFinishTime <= minFinishTime) {
    		minFinishTime = firstFinishTime;
    		minQueue = 0;
    	}
    }

	if (!m_secondWeightedQueue.empty()) {
		p2 = m_secondWeightedQueue.front();
		secondFinishTime = getFinishTimeCollection()[p2->GetUid()];
		if (secondFinishTime <= minFinishTime) {
			minFinishTime = secondFinishTime;
			minQueue = 1;
		}
	}

	if (minQueue == 0) {
		NS_LOG_LOGIC("Number packets " << m_firstWeightedQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInFirstQueue);

		return p1;
	}

	else if (minQueue == 1) {
		NS_LOG_LOGIC("Number packets " << m_secondWeightedQueue.size ());
		NS_LOG_LOGIC("Number bytes " << m_bytesInSecondQueue);

		return p2;
	}

	else {
		NS_LOG_LOGIC("all queues empty");
		return 0;
	}
}


std::map<uint64_t, double_t> WeightedFairQueue::getFinishTimeCollection() const {
	return m_finishTimeCollection;
}


double_t WeightedFairQueue::CalculateFinishTime (double_t startTime, uint32_t packetSize, double_t weight) {

	return startTime + (8 * packetSize / weight);
}


double_t WeightedFairQueue::CalculateWeightSum(void) {
	double_t weightSum = 0;

	if (!m_firstWeightedQueue.empty())
		weightSum += m_firstWeight;

	if (!m_secondWeightedQueue.empty())
		weightSum += m_secondWeight;

	return weightSum;
}

} // namespace ns3

