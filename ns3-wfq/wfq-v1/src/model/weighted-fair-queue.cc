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

#include <algorithm>

#include <float.h>

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
					MakeUintegerAccessor(&WeightedFairQueue::m_inputSecondMaxPackets),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("FirstWeightedQueueMaxPackets",
					"The maximum number of packets accepted by the first weighted queue.",
					UintegerValue(100),
					MakeUintegerAccessor(&WeightedFairQueue::m_inputFirstMaxPackets),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("SecondWeightedQueueMaxBytes",
					"The maximum number of bytes accepted by the second weighted queue.",
					UintegerValue(100 * 65535),
					MakeUintegerAccessor(&WeightedFairQueue::m_inputSecondMaxBytes),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("FirstWeightedQueueMaxBytes",
					"The maximum number of bytes accepted by the first weighted queue.",
					UintegerValue(100 * 65535),
					MakeUintegerAccessor(&WeightedFairQueue::m_inputFirstMaxBytes),
					MakeUintegerChecker<uint32_t>())

			.AddAttribute("SecondQueuePort",
					"The destination port number for second queue traffic.",
					UintegerValue(3000),
					MakeUintegerAccessor(&WeightedFairQueue::m_secondQueuePort),
					MakeUintegerChecker<uint32_t>())

	        .AddAttribute("FirstWeight",
	        		"The first queue's weight",
					DoubleValue(0),
					MakeDoubleAccessor(&WeightedFairQueue::m_inputFirstWeight),
					MakeDoubleChecker<double_t>())

			.AddAttribute("SecondWeight",
					"The second queue's weight",
					DoubleValue(0),
					MakeDoubleAccessor(&WeightedFairQueue::m_inputSecondWeight),
					MakeDoubleChecker<double_t>())

			.AddAttribute("TotalExpectedBytes",
					"Total expected bytes passed before end of simulation - used for showing progression percentage",
					UintegerValue(0),
					MakeUintegerAccessor(&WeightedFairQueue::m_totalExpectedBytes),
					MakeUintegerChecker<uint64_t>())

			.AddAttribute("LinkCapacity",
					"Link Capacity",
					DoubleValue(0),
					MakeDoubleAccessor(&WeightedFairQueue::m_linkCapacity),
					MakeDoubleChecker<double_t>());

	return tid;
}

WeightedFairQueue::WeightedFairQueue() :
		Queue(), m_virtualTime(0.0), v_chk(0), t_chk(0) {
	NS_LOG_FUNCTION_NOARGS ();


	// temporary
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

	uint16_t classIndex;

	uint32_t protocol = ip.GetProtocol();
	if (protocol == 17) {
		UdpHeader udp;
		p->PeekHeader(udp);

		if (udp.GetDestinationPort() == m_secondQueuePort) {
			NS_LOG_INFO("\tclassifier: second queue udp");
			classIndex = 1;
		} else {
			NS_LOG_INFO("\tclassifier: first queue udp");
			classIndex = 0;
		}
	}

	else if (protocol == 6) {
		TcpHeader tcp;
		p->PeekHeader(tcp);
		if (tcp.GetDestinationPort() == m_secondQueuePort) {
			NS_LOG_INFO("\tclassifier: second queue tcp");
			classIndex = 1;
		} else {
			NS_LOG_INFO("\tclassifier: first queue tcp");
			classIndex = 0;
		}

	} else {
		NS_LOG_INFO("\tclassifier: unrecognized transport protocol");
		classIndex = 0;
	}

	p->AddHeader(ip);
	p->AddHeader(ppp);


	//*************************************
	// this is temporary, used for showing progression, should be removed later
	if (classIndex == 1)
	{
		m_passedBytes = m_passedBytes + p->GetSize() - 54;
		if (m_passedBytes > m_totalExpectedBytes/5 and m_20PercentCompleted == false) {
			std::cout << "20 percent" << std::endl;
			m_20PercentCompleted = true;
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
		}
	}
	//*************************************

	return classIndex;
}

bool WeightedFairQueue::DoEnqueue(Ptr<Packet> p) {
	NS_LOG_FUNCTION(this << p);
	double_t packetFinishTime, packetStartTime;

	if (m_isInitialized == false)
	{
		InitializeClassMaps();
		m_isInitialized = true;
	}

	uint16_t classIndex = Classify(p);

	if (m_mode == QUEUE_MODE_PACKETS
			&& (m_classList[classIndex].m_packetsInWFQQueue >= m_classList[classIndex].m_maxPackets)) {
		NS_LOG_LOGIC("Queue full (at max packets) -- dropping pkt");
		Drop(p);
		return false;
	}

	if (m_mode == QUEUE_MODE_BYTES
			&& (m_classList[classIndex].m_bytesInWFQQueue + p->GetSize() >= m_classList[classIndex].m_maxBytes)) {
		NS_LOG_LOGIC("Queue full (packet would exceed max bytes) -- dropping pkt");
		Drop(p);
		return false;
	}


	// updating virtual time based on iterated deletion
	bool fin = false;
	int16_t minActiveClassIndex;
	double_t minActiveLastFinishTime;
	double_t delta;
	//double_t t_chk = m_lastVirtualTimeUpdate;
	double_t t_now = Simulator::Now().GetSeconds();
	//double_t v_chk = m_virtualTime;
	while (!fin)
	{
		delta = t_now - t_chk;
		minActiveClassIndex = GetMinActiveClass();
		if (minActiveClassIndex == -1)
			fin = true;
		else
		{
			minActiveLastFinishTime = m_classList[minActiveClassIndex].m_lastFinishTime;
			if (!(minActiveLastFinishTime <= (v_chk + delta / CalculateActiveSum())))
				fin = true;
		}

		if (fin == false)
		{
			t_chk = t_chk + (minActiveLastFinishTime - v_chk) * CalculateActiveSum();
			v_chk = minActiveLastFinishTime;
			m_classList[minActiveClassIndex].m_active = false;
		}

		else
		{
			if (minActiveClassIndex == -1)
			{
				m_virtualTime = v_chk;// = 0;
				//ClearLastFinishTimes();
			}
			else
				m_virtualTime = v_chk + delta / CalculateActiveSum();
			v_chk = m_virtualTime;
			t_chk = t_now;
			//break;
		}
	}

	m_classList[classIndex].m_active = true;


	//packetStartTime = std::max(m_virtualTime , m_classList[classIndex].m_lastFinishTime);
	packetStartTime = m_classList[classIndex].m_lastFinishTime > m_virtualTime ? m_classList[classIndex].m_lastFinishTime : m_virtualTime;
	packetFinishTime = CalculateFinishTime(packetStartTime, p->GetSize(), m_classList[classIndex].m_weight);

	m_classList[classIndex].m_lastFinishTime = packetFinishTime;

	QueueItem queueItem1 = QueueItem();
	queueItem1.m_packet = p;
	queueItem1.m_finishTime = packetFinishTime;
	queueItem1.m_classIndex = classIndex;
	m_wfqPQ.push(queueItem1);

	m_classList[classIndex].m_bytesInWFQQueue += p->GetSize();
	m_classList[classIndex].m_packetsInWFQQueue++;
	NS_LOG_LOGIC("Number packets " << m_classList[classIndex].m_packetsInWFQQueue);
	NS_LOG_LOGIC("Number bytes " << m_classList[classIndex].m_bytesInWFQQueue);


	return true;
}

Ptr<Packet> WeightedFairQueue::DoDequeue(void) {
	NS_LOG_FUNCTION(this);
	uint16_t minClassIndex;
	Ptr<Packet> p;

	if (!m_wfqPQ.empty())
	{
		QueueItem queueItem = m_wfqPQ.top();
		minClassIndex = queueItem.m_classIndex;
		p = queueItem.m_packet;
		m_wfqPQ.pop();
		m_classList[minClassIndex].m_bytesInWFQQueue -= p->GetSize();
		m_classList[minClassIndex].m_packetsInWFQQueue--;

		NS_LOG_LOGIC("Popped " << p);
		NS_LOG_LOGIC("Number packets " << m_classList[minClassIndex].m_packetsInWFQQueue);
		NS_LOG_LOGIC("Number bytes " << m_classList[minClassIndex].m_bytesInWFQQueue);

		return p;
	}

	else
	{
		NS_LOG_LOGIC("all queues empty");
		return 0;
	}
}

Ptr<const Packet> WeightedFairQueue::DoPeek(void) const {
	NS_LOG_FUNCTION(this);
	/*uint16_t minClassIndex;
	Ptr<Packet> p;

	if (!m_wfqPQ.empty())
	{
		QueueItem queueItem = m_wfqPQ.top();
		minClassIndex = queueItem.m_classIndex;
		p = queueItem.m_packet;
		//m_wfqPQ.pop();
		//m_classList[minClassIndex].m_bytesInWFQQueue -= p->GetSize();
		//m_classList[minClassIndex].m_packetsInWFQQueue--;

		//NS_LOG_LOGIC("Popped " << p);
		NS_LOG_LOGIC("Number packets " << m_classList[minClassIndex].m_packetsInWFQQueue);
		NS_LOG_LOGIC("Number bytes " << m_classList[minClassIndex].m_bytesInWFQQueue);

		//std::cout << "dequeue at: " << Simulator::Now().GetSeconds() << std::endl;

		return p;
	}

	else
	{
		NS_LOG_LOGIC("all queues empty");
		return 0;
	}*/

	return 0;
}

double_t WeightedFairQueue::CalculateFinishTime (double_t startTime, uint32_t packetSize, double_t weight) {

	return startTime + (8 * packetSize / (weight * m_linkCapacity));
}


double_t WeightedFairQueue::CalculateActiveSum(void) {
	double_t weightSum = 0;

	for (unsigned int i=0; i<m_classList.size(); i++) {

		if (m_classList[i].m_active == true)
			weightSum += m_classList[i].m_weight;
	}

	return weightSum;
}

void WeightedFairQueue::InitializeClassMaps(void) {

	ClassMap classMap1 = ClassMap();
	classMap1.m_maxPackets = m_inputFirstMaxPackets;
	classMap1.m_maxBytes = m_inputFirstMaxBytes;
	classMap1.m_weight = m_inputFirstWeight;

	ClassMap classMap2 = ClassMap();
	classMap2.m_maxPackets = m_inputSecondMaxPackets;
	classMap2.m_maxBytes = m_inputSecondMaxBytes;
	classMap2.m_weight = m_inputSecondWeight;

	m_classList.push_back(classMap1);
	m_classList.push_back(classMap2);

	//ClearLastFinishTimes();
}

/*void WeightedFairQueue::ClearLastFinishTimes(void) {
	for (unsigned int i=0; i<m_classList.size(); i++)
		m_classList[i].m_lastFinishTime = 0;
}*/

int16_t WeightedFairQueue::GetMinActiveClass(void) {

	double_t minLastFinishTime = DBL_MAX;
	int16_t minActiveClassIndex = -1;

	for (unsigned int i=0; i<m_classList.size(); i++) {

		if (m_classList[i].m_active == true)
			if (m_classList[i].m_lastFinishTime < minLastFinishTime)
			{
				minLastFinishTime = m_classList[i].m_lastFinishTime;
				minActiveClassIndex = i;
			}
	}

	return minActiveClassIndex;
}

ClassMap::ClassMap() {

	m_bytesInWFQQueue = 0;
	m_packetsInWFQQueue = 0;
	m_lastFinishTime = 0;
	m_active = false;
}

} // namespace ns3
