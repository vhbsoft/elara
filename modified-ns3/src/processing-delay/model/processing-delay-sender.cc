/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/seq-ts-header.h"
#include "ns3/pointer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>

#include "ns3/exp-data-header.h"
#include "ns3/seq-header.h"

#include "processing-delay-sender.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ProcessingDelaySender");
NS_OBJECT_ENSURE_REGISTERED (ProcessingDelaySender);

TypeId
ProcessingDelaySender::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ProcessingDelaySender")
    .SetParent<Application> ()
    .AddConstructor<ProcessingDelaySender> ()
    .AddAttribute ("NumPackets",
                   "Number of probe packets in the train",
                   UintegerValue (100),
                   MakeUintegerAccessor (&ProcessingDelaySender::m_count),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Interval",
                   "The time to wait between probe packets in the train", 
		   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&ProcessingDelaySender::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("PacketSize",
                   "Size of packets generated. If less than 12 the timestamp will be ommitted. The minimum packet size is 2 for just the sequence header",
                   UintegerValue (1024),
                   MakeUintegerAccessor (&ProcessingDelaySender::m_size),
                   MakeUintegerChecker<uint32_t> (2, 10000))
    .AddAttribute ("RemoteAddress",
                   "The Address of the ProcessingDelayReceiver",
                   AddressValue (),
                   MakeAddressAccessor (&ProcessingDelaySender::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemoteUdpPort", "The destination port of the outbound probe packets",
                   UintegerValue (9),
                   MakeUintegerAccessor (&ProcessingDelaySender::m_peerUdpPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("RemoteTcpPort", "The destination port of the outbound TCP connections",
                   UintegerValue (10),
                   MakeUintegerAccessor (&ProcessingDelaySender::m_peerTcpPort),
                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

ProcessingDelaySender::ProcessingDelaySender ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_initialized = 0;
  m_sent = 0;
  m_udpSocket = 0;
  m_tcpSocket = 0;
  m_mode = 0;
  m_nextEvent = EventId ();
}

ProcessingDelaySender::~ProcessingDelaySender ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
ProcessingDelaySender::SetRemote (Ipv4Address ip, uint16_t udpPort, uint16_t tcpPort)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_peerAddress = Address(ip);
  m_peerUdpPort = udpPort;
  m_peerTcpPort = tcpPort;
}

void
ProcessingDelaySender::SetRemote (Address ip, uint16_t udpPort, uint16_t tcpPort)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_peerAddress = ip;
  m_peerUdpPort = udpPort;
  m_peerTcpPort = tcpPort;
}

void 
ProcessingDelaySender::SetNumPackets (uint32_t count)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_count = count;
}

void 
ProcessingDelaySender::SetInterval (Time interval)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_interval = interval;
}

void 
ProcessingDelaySender::SetPacketLen (uint32_t size)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_size = size;
}

void
ProcessingDelaySender::SetLogFileName (std::string name)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_name = name;
}

void
ProcessingDelaySender::SetMode (uint8_t mode)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_mode = mode;
}

void
ProcessingDelaySender::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Application::DoDispose ();
}

void
ProcessingDelaySender::StartApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Set up UDP socket
  if (m_udpSocket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_udpSocket = Socket::CreateSocket (GetNode (), tid);
      if (Ipv4Address::IsMatchingType(m_peerAddress) == true)
        {
          m_udpSocket->Bind ();
          m_udpSocket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), 
				m_peerUdpPort));
        }
    }

  // Change to HandleUdpRead() once it has been implemented
  m_udpSocket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());

  // Check mode
  if (m_mode == 0)
    {
      // Set up TCP socket
      if (m_tcpSocket == 0)
        {
          TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
          m_tcpSocket = Socket::CreateSocket (GetNode (), tid);
          m_tcpSocket->Bind ();
          m_tcpSocket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_peerTcpPort));
          m_tcpSocket->SetConnectCallback (
				           MakeCallback (&ProcessingDelaySender::ConnectionSucceeded, this), // This is the next step
				           MakeCallback (&ProcessingDelaySender::ConnectionFailed, this)
				          );
        }
      m_tcpSocket->SetCloseCallbacks (
				      MakeCallback (&ProcessingDelaySender::HandleTcpClose, this),
				      MakeCallback (&ProcessingDelaySender::HandleTcpError, this)
				     );
      
      m_tcpSocket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
  else 
    {
      m_nextEvent = Simulator::Schedule (Seconds (0.0), &ProcessingDelaySender::SendICMP, this);
    }
}

void
ProcessingDelaySender::StopApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_nextEvent);
}

void
ProcessingDelaySender::InitializeTCP (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (m_nextEvent.IsExpired ());

  m_initialized = 1;

  ExpDataHeader expData;
  expData.SetNumPackets(m_count);
  expData.SetInterPacketTime(m_interval.GetTimeStep());
  expData.SetProbePacketLen(m_size);

  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader(expData);

  NS_LOG_INFO ("Sending initializing packet:" <<
	       "\n\tnumPackets = " << m_count <<
  	       "\n\tinterPacketTime = " << m_interval.GetTimeStep() <<
	       "\n\tprobePacketLen = " << m_size);

  m_tcpSocket->Send(p);
  
  m_tcpSocket->SetRecvCallback (MakeCallback (&ProcessingDelaySender::StartTrain, this));
}

void
ProcessingDelaySender::SendICMP (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  // Add ICMP stuff here

  if (m_initialized == 0)
    {
      m_nextEvent = Simulator::Schedule (Seconds (0.0), &ProcessingDelaySender::Send, this);  
      m_initialized = 1;
    }
}

void
ProcessingDelaySender::StartTrain (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (m_nextEvent.IsExpired ());

  m_tcpSocket->Close();
  
  m_nextEvent = Simulator::Schedule (Seconds (0.0), &ProcessingDelaySender::Send, this);

}

void
ProcessingDelaySender::Send (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (m_nextEvent.IsExpired ());

  // Create packet
  uint32_t size = m_size;
  Ptr<Packet> p;

  // Add sequence/timestamp
  if (size >= 12)
    {
      // Leave 12 bytes for the SeqTsHeader
      p = Create<Packet>(size - 12);
      SeqTsHeader seqTs;
      seqTs.SetSeq (m_sent);
      p->AddHeader (seqTs);
    }
  else
    {
      // Leave 2 bytes for the SeqHeader
      p = Create<Packet>(size - 2);
      SeqHeader seq;
      seq.SetSeq (m_sent);
      p->AddHeader (seq); 
    }

  // Verify the packet's size
  if (p->GetSize() != size) {NS_LOG_UNCOND ("Error! Packet Size is incorrect!"); exit(1);}

  // Covert m_peerAddress for logging
  std::stringstream peerAddressStringStream;
  if (Ipv4Address::IsMatchingType (m_peerAddress))
    {
      peerAddressStringStream << Ipv4Address::ConvertFrom (m_peerAddress);
    }
  
  // Send the packet to the socket
  //if (m_sent == 5800) NS_LOG_UNCOND("packet sequence: " << m_sent << "\nUID: " << p->GetUid());
  uint32_t bytesSent = m_udpSocket->Send (p->Copy());

  if (bytesSent >= 0)
    {
      //NS_LOG_UNCOND ("bytesSent=" << bytesSent);
      //NS_LOG_UNCOND("sender::send "<<p->GetUid()<<" at "<<Simulator::Now().GetTimeStep());
      //NS_LOG_UNCOND ("sent " << m_sent << "\t" << Simulator::Now().GetTimeStep());
      //if (m_sent == 11678) NS_LOG_UNCOND ("uid: "<<p->GetUid()<<"\n");
	      if (m_sent < 10)
		NS_LOG_INFO("sent 00" << m_sent << '\t' << Simulator::Now().GetTimeStep());
	      else if (m_sent < 100)
		NS_LOG_INFO("sent 0" << m_sent << '\t' << Simulator::Now().GetTimeStep());
	      else
		NS_LOG_INFO("sent " << m_sent << '\t' << Simulator::Now().GetTimeStep());
      m_sent++;
      /*
      NS_LOG_INFO ("TraceDelay TX " << size << " bytes to "
		   << peerAddressStringStream.str () << " Uid: "
		   << p->GetUid () << " Time: "
		   << (Simulator::Now ()).GetSeconds ()
		   );
      */
    }
  else
    {
      NS_LOG_INFO ("Error while sending " << size << " bytes to "
		   << peerAddressStringStream.str ()
		   );
    }
  
  // Check against total number of packets
  if (m_sent < m_count)
    {
      m_nextEvent = Simulator::Schedule (m_interval, &ProcessingDelaySender::Send, this);
    }
  else
    {
      if (m_mode == 0)
	{
          return; 
          // SENDING DATA OVER TCP IS BROKEN
          //m_nextEvent = Simulator::Schedule (Seconds (0.0), &ProcessingDelaySender::RequestData, this);
	}
      else
	{
	  m_nextEvent = Simulator::Schedule (Seconds (0.0), &ProcessingDelaySender::SendICMP, this);
	}
    }
}

void
ProcessingDelaySender::RequestData (void)
{
  NS_LOG_FUNCTION_NOARGS ();  

  TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
  m_tcpSocket = Socket::CreateSocket (GetNode (), tid);
  m_tcpSocket->Bind ();
  m_tcpSocket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), 
			m_peerTcpPort)
			);

  m_tcpSocket->SetConnectCallback (
				   MakeCallback (&ProcessingDelaySender::ConnectionSucceeded, this), // This is the next step 
				   MakeCallback (&ProcessingDelaySender::ConnectionFailed, this)
				   );

  m_tcpSocket->SetCloseCallbacks (
    MakeCallback (&ProcessingDelaySender::HandleTcpClose, this),
    MakeCallback (&ProcessingDelaySender::HandleTcpError, this)
				  );

  m_tcpSocket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());

}

// SENDING DATA OVER TCP IS BROKEN
void
ProcessingDelaySender::ReceiveData (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  packet = socket->RecvFrom (from);
  if (packet->GetSize() == 0)
    return;

  NS_LOG_INFO ("Receiving results: " << packet->GetSize() << " bytes" << 
	       " Uid: " << packet->GetUid () << " Time: " << (Simulator::Now ()).GetSeconds ()
               );

  ExpDataHeader expData;
  NS_LOG_INFO ("calling RemoveHeader");
  packet->RemoveHeader(expData);

  NS_LOG_INFO ("calling GetResults");
  std::string results = expData.GetResults();
  NS_LOG_INFO (results);

  std::ofstream output;
  output.open(m_name.c_str());
  output << results;
  output.close();
}

void
ProcessingDelaySender::HandleUdpRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION_NOARGS ();
  // TO BE IMPLEMENTED
  // to get the ping backs in icmp mode
}

void 
ProcessingDelaySender::ConnectionSucceeded (Ptr<Socket> socket)
{
  //NS_LOG_UNCOND ("TCP Connection succeeded");

  NS_LOG_FUNCTION (this << socket);
  NS_LOG_INFO ("TCP Connection succeeded");
  if (m_initialized == 0)
    m_nextEvent = Simulator::Schedule (Seconds (1.0), &ProcessingDelaySender::InitializeTCP, this);
  else
    socket->SetRecvCallback (MakeCallback (&ProcessingDelaySender::ReceiveData, this));
}

void 
ProcessingDelaySender::ConnectionFailed (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  NS_LOG_INFO ("TCP Connection Failed");
}

void 
ProcessingDelaySender::HandleTcpClose (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}
 
void 
ProcessingDelaySender::HandleTcpError (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}

} // Namespace ns3
