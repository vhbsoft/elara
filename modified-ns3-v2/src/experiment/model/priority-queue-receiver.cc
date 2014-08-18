/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <string>
#include <fstream>

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"

#include "ns3/exp-data-header.h"
#include "ns3/seq16-header.h"
#include "ns3/seq32-header.h"


#include "priority-queue-receiver.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PriorityQueueReceiver");
NS_OBJECT_ENSURE_REGISTERED (PriorityQueueReceiver);

TypeId
PriorityQueueReceiver::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PriorityQueueReceiver")
    .SetParent<Application> ()
    .AddConstructor<PriorityQueueReceiver> ()
    .AddAttribute ("UdpPortHigh",
                   "Port on which we listen for incoming probe packets.",
                   UintegerValue (3000),
                   MakeUintegerAccessor (&PriorityQueueReceiver::m_udpPortHigh),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("UdpPortLow",
                   "Port on which we listen for incoming probe packets.",
                   UintegerValue (2000),
                   MakeUintegerAccessor (&PriorityQueueReceiver::m_udpPortLow),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("TcpPort",
                   "TcpPort on which we listen for incoming tcp connections.",
                   UintegerValue (10),
                   MakeUintegerAccessor (&PriorityQueueReceiver::m_tcpPort),
                   MakeUintegerChecker<uint16_t> ())

  ;
  return tid;
}

PriorityQueueReceiver::PriorityQueueReceiver()
{
  NS_LOG_FUNCTION (this);
  m_received=0;
  m_initialized=0;
  m_resultsLow=NULL;
  m_resultsHigh=NULL;
  m_mode=0;
  m_stage=-1;
  m_lastHighSeq = 0;
  m_lastLowSeq = 0;
  m_lastHighTs = 0;
  m_lastLowTs = 0;
}

PriorityQueueReceiver::~PriorityQueueReceiver()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
PriorityQueueReceiver::SetMode (uint8_t mode)
{
  NS_LOG_FUNCTION (mode);
  m_mode = mode;
}

void
PriorityQueueReceiver::SetLogFileName (std::string name)
{
  NS_LOG_FUNCTION (name);
  m_name = name;
}

void
PriorityQueueReceiver::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Application::DoDispose ();
}

void
PriorityQueueReceiver::StartApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Set up udp socket for high priority packets
  if (m_udpSocketHigh == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_udpSocketHigh = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 3000);
      m_udpSocketHigh->Bind (local);
    }

  m_udpSocketHigh->SetRecvCallback (MakeCallback (&PriorityQueueReceiver::HandleUdpReadHigh, this));

  // Set up udp socket for low priority packets
  if (m_udpSocketLow == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_udpSocketLow = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 2000);
      m_udpSocketLow->Bind (local);
    }

  m_udpSocketLow->SetRecvCallback (MakeCallback (&PriorityQueueReceiver::HandleUdpReadLow, this));

  // Set up tcp socket
  if (m_mode == 0)
    {
      if (m_tcpSocket == 0)
        {
          TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
          m_tcpSocket = Socket::CreateSocket (GetNode (), tid);
          InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_tcpPort);
          m_tcpSocket->Bind (local);
          m_tcpSocket->Listen ();
        }
      
      m_tcpSocket->SetAcceptCallback (
				      MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
				      MakeCallback (&PriorityQueueReceiver::HandleTcpAccept, this)
				     );
      
      m_tcpSocket->SetCloseCallbacks (
				      MakeCallback (&PriorityQueueReceiver::HandleTcpClose, this),
				      MakeCallback (&PriorityQueueReceiver::HandleTcpError, this)
				     );
    }
}

void
PriorityQueueReceiver::Initialize (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  m_initialized = 1;

  Address from;
  Ptr<Packet> packet = socket->RecvFrom (from);
 
  if (packet->GetSize() == 0)
    return;

  ExpDataHeader expData;
  packet->RemoveHeader(expData);
  m_numPackets = expData.GetNumPackets();
  m_interPacketTime = expData.GetInterPacketTime();
  m_probePacketLen = expData.GetProbePacketLen();

  m_initialNum = expData.GetInitialNum ();
  m_separationNum = expData.GetSeparationNum ();
  m_aptNum = expData.GetAptNum ();
  m_aptPriority = expData.GetAptPriority ();

  NS_LOG_INFO ("Receiving initializing packet:" <<
	       "\n\tnumPackets = " << m_numPackets <<
  	       "\n\tinterPacketTime = " << m_interPacketTime <<
	       "\n\tprobePacketLen = " << m_probePacketLen <<
               "\n\tinitialPacketTrainLength = " << m_initialNum <<
               "\n\tseparationPacketTrainLength = " << m_separationNum <<
               "\n\tnumAptPriorityProbes = " << m_aptNum <<
               "\n\taptPriority = " << m_aptPriority);

  if (m_aptPriority == 'H')
    {
      m_numHighPriority = m_initialNum + m_aptNum;
      m_numLowPriority = m_numPackets - m_numHighPriority;
    }
  else if (m_aptPriority == 'L')
    {
      m_numLowPriority = m_aptNum;
      m_numHighPriority = m_numPackets - m_numLowPriority;
    }

  m_resultsLow = new int64_t[m_numLowPriority];
  m_resultsHigh = new int64_t[m_numHighPriority];
  //m_resultsBoth = new int64_t[m_numPackets];
  //m_resultsBothType = new uint8_t[m_numPackets];

  for (uint32_t i = 0; i < m_numPackets; i++)
    {
      //m_resultsBoth[i] = -1;
      //m_resultsBothType[i] = ' ';
      if (i < m_numLowPriority)
        m_resultsLow[i] = -1;
      if (i < m_numHighPriority)
        m_resultsHigh[i] = -1;
    }

  Ptr<Packet> p = Create<Packet> (1);  
  socket->Send(p);
}

void
PriorityQueueReceiver::HandleUdpReadHigh (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () > 0)
        {
          uint32_t seqNum;
          if (packet->GetSize () >= 4)
	    {
	      Seq32Header seq;
              packet->RemoveHeader (seq);
	      seqNum = seq.GetSeq ();
 	    }
	  else 
	    {
	      Seq16Header seq;
              packet->RemoveHeader (seq);
	      seqNum = seq.GetSeq();
	    }
          /*
          if (m_stage == -1 && seqNum < m_lastHighSeq)
            {
              m_stage = 0;
              seqNum += m_initialNum;
            }
          else if (m_stage > -1)
            {
              seqNum += m_initialNum;
            }

          if (m_aptPriority == 'L')
            {
              seqNum += m_stage*m_separationNum;
              if (seqNum < m_lastHighSeq)
                {
                  m_stage++;
            }
          */
	  int64_t ts = Simulator::Now().GetTimeStep();

          if (InetSocketAddress::IsMatchingType (from))
	    {
	      if (seqNum < 10)
                {
		  NS_LOG_INFO("recv 00" << seqNum << '\t' << ts);
                }
	      else if (seqNum < 100)
                {
		  NS_LOG_INFO("recv 0" << seqNum << '\t' << ts);
                }
	      else
                {
		  NS_LOG_INFO("recv " << seqNum << '\t' << ts);
                }
	      /*
	      NS_LOG_INFO ("TraceDelay: RX " << packet->GetSize () <<
			   " bytes from "<< InetSocketAddress::ConvertFrom (from).GetIpv4 () <<
			   " Sequence Number: " << seq <<
			   " Uid: " << packet->GetUid () <<
			   //" TXtime: " << seqTs.GetTs () <<
			   " RXtime: " << Simulator::Now (); 
			   //<< " Delay: " << Simulator::Now () - seqTs.GetTs ());
	      */
	    }

          if (seqNum < m_lastHighSeq || ts < m_lastHighTs)
            {
              NS_LOG_ERROR ("H Packet " << seqNum << " received out of order! last packet = " << m_lastHighSeq);
            }

          m_lastHighSeq = seqNum;
          m_lastHighTs = ts;

	  if (m_mode == 0)
	    {
	      m_received++;
	      m_resultsHigh[seqNum] = ts;
	    }
        }
    }
}

void
PriorityQueueReceiver::HandleUdpReadLow (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () > 0)
        {
          uint32_t seqNum;
          if (packet->GetSize () >= 4)
	    {
	      Seq32Header seq;
              packet->RemoveHeader (seq);
	      seqNum = seq.GetSeq ();
 	    }
	  else 
	    {
	      Seq16Header seq;
              packet->RemoveHeader (seq);
	      seqNum = seq.GetSeq();
	    }

	  int64_t ts = Simulator::Now().GetTimeStep();	  

          if (InetSocketAddress::IsMatchingType (from))
	    {
	      if (seqNum < 10)
                {
		  NS_LOG_INFO("recv 00" << seqNum << '\t' << ts);
                }
	      else if (seqNum < 100)
                {
		  NS_LOG_INFO("recv 0" << seqNum << '\t' << ts);
                }
	      else
                {
		  NS_LOG_INFO("recv " << seqNum << '\t' << ts);
                }
	      /*
	      NS_LOG_INFO ("TraceDelay: RX " << packet->GetSize () <<
			   " bytes from "<< InetSocketAddress::ConvertFrom (from).GetIpv4 () <<
			   " Sequence Number: " << seq <<
			   " Uid: " << packet->GetUid () <<
			   //" TXtime: " << seqTs.GetTs () <<
			   " RXtime: " << Simulator::Now (); 
			   //<< " Delay: " << Simulator::Now () - seqTs.GetTs ());
	      */
	    }

          if (seqNum < m_lastLowSeq || ts < m_lastLowTs)
            {
              NS_LOG_ERROR ("L Packet " << seqNum << " received out of order!");
            }

          m_lastLowSeq = seqNum;
          m_lastLowTs = ts;

	  if (m_mode == 0)
	    {
	      m_received++;
	      m_resultsLow[seqNum] = ts;
	    }
        }
    }
}

void PriorityQueueReceiver::HandleTcpAccept (Ptr<Socket> s, const Address& from)
{
  NS_LOG_INFO ("Accepting TCP request");

  NS_LOG_FUNCTION (this << s << from);
  if (m_initialized == 0)
    s->SetRecvCallback (MakeCallback (&PriorityQueueReceiver::Initialize, this));    
  else
    m_nextEvent = Simulator::Schedule (Seconds (0.0), &PriorityQueueReceiver::Process, this);
  m_connection = s;  
}

void PriorityQueueReceiver::HandleTcpError (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}

void PriorityQueueReceiver::HandleTcpClose (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}

void
PriorityQueueReceiver::Process (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_INFO ("Processing experiment results");

  std::stringstream ssHigh;
  std::string resultsHigh;
  std::ofstream outputHigh;

  std::string outfileHigh = m_name + "-H";

  outputHigh.open(outfileHigh.c_str());

  uint32_t chunkSize = 15000;
  for (uint32_t chunk = 0; chunk < 1 + m_numHighPriority / chunkSize; chunk++)
    {
      for (uint32_t i = chunk*chunkSize; i < (chunk+1)*chunkSize && i < m_numHighPriority; i++)
	{
	  ssHigh << i;
	  ssHigh << "\t";
	  ssHigh << m_resultsHigh[i];
	  ssHigh << "\n";
	}
      
      resultsHigh = ssHigh.str();
      outputHigh << resultsHigh;
      ssHigh.str("");
    }
  outputHigh.close(); 

  delete [] m_resultsHigh;

  NS_LOG_INFO ("Saved high priority results to: " << outfileHigh);

  std::stringstream ssLow;
  std::string resultsLow;
  std::ofstream outputLow;

  std::string outfileLow = m_name + "-L";

  outputLow.open(outfileLow.c_str());

  for (uint32_t chunk = 0; chunk < 1 + m_numLowPriority / chunkSize; chunk++)
    {
      for (uint32_t i = chunk*chunkSize; i < (chunk+1)*chunkSize && i < m_numLowPriority; i++)
	{
	  ssLow << i;
	  ssLow << "\t";
	  ssLow << m_resultsLow[i];
	  ssLow << "\n";
	}
      
      resultsLow = ssLow.str();
      outputLow << resultsLow;
      ssLow.str("");
    }
  outputLow.close(); 

  delete [] m_resultsLow;

  NS_LOG_INFO ("Saved low priority results to: " << outfileLow);

  return;
}

void
PriorityQueueReceiver::StopApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();

  if (m_udpSocketHigh != 0)
    {
      m_udpSocketHigh->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }

  if (m_udpSocketLow != 0)
    {
      m_udpSocketLow->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }

  if (m_mode == 0 && m_tcpSocket != 0)
    {
      m_tcpSocket->Close ();
      m_tcpSocket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
}

} // Namespace ns3

