#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/seq-ts-header.h"

#include <string>
#include <fstream>

#include "ns3/exp-data-header.h"
#include "ns3/seq-header.h"

#include "compression-receiver.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CompressionReceiver");
NS_OBJECT_ENSURE_REGISTERED (CompressionReceiver);

TypeId
CompressionReceiver::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CompressionReceiver")
    .SetParent<Application> ()
    .AddConstructor<CompressionReceiver> ()
    .AddAttribute ("UdpPort",
                   "Port on which we listen for incoming probe packets.",
                   UintegerValue (9),
                   MakeUintegerAccessor (&CompressionReceiver::m_udpPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("TcpPort",
                   "TcpPort on which we listen for incoming tcp connections.",
                   UintegerValue (10),
                   MakeUintegerAccessor (&CompressionReceiver::m_tcpPort),
                   MakeUintegerChecker<uint16_t> ())

  ;
  return tid;
}

CompressionReceiver::CompressionReceiver()
{
  NS_LOG_FUNCTION (this);
  m_received=0;
  m_initialized=0;
  m_results=NULL;
  m_mode=0;
}

CompressionReceiver::~CompressionReceiver()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
CompressionReceiver::SetMode (uint8_t mode)
{
  NS_LOG_FUNCTION (mode);
  m_mode = mode;
}

void
CompressionReceiver::SetLogFileName (std::string name)
{
  NS_LOG_FUNCTION (name);
  m_name = name;
}

/*
void
CompressionReceiver::SetIncludeTs (uint8_t includeTs)
{
  NS_LOG_FUNCTION (includeTs);
  m_includeTs = includeTs;
}
*/

void
CompressionReceiver::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Application::DoDispose ();
}

void
CompressionReceiver::StartApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Set up udp socket

  if (m_udpSocket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_udpSocket = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (),
                                                   m_udpPort);
      m_udpSocket->Bind (local);
    }

  m_udpSocket->SetRecvCallback (MakeCallback (&CompressionReceiver::HandleUdpRead, this));


  // Set up tcp socket
  if (m_mode == 0)
    {
  
      if (m_tcpSocket == 0)
	{
	  TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
	  m_tcpSocket = Socket::CreateSocket (GetNode (), tid);
	  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (),
						       m_tcpPort);
	  m_tcpSocket->Bind (local);
	  m_tcpSocket->Listen ();
	}
      
      m_tcpSocket->SetAcceptCallback (
				      MakeNullCallback<bool, Ptr<Socket>, const Address &> (),
				      MakeCallback (&CompressionReceiver::HandleTcpAccept, this));
      
      m_tcpSocket->SetCloseCallbacks (
				      MakeCallback (&CompressionReceiver::HandleTcpClose, this),
				      MakeCallback (&CompressionReceiver::HandleTcpError, this));
 
    }     
}

void
CompressionReceiver::StopApplication ()
{
  NS_LOG_FUNCTION_NOARGS ();

  if (m_udpSocket != 0)
    {
      m_udpSocket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }

  if (m_mode == 0)
    {
      if (m_tcpSocket != 0)
	{
	  m_tcpSocket->Close ();
	  m_tcpSocket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
	}
    }
}

void
CompressionReceiver::HandleUdpRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      if (packet->GetSize () > 0)
        {
          uint32_t seqNum;
          if (packet->GetSize () >= 12)
	    {
	      SeqTsHeader seqTs;
              packet->RemoveHeader (seqTs);
	      seqNum = seqTs.GetSeq ();
 	    }
	  else 
	    {
	      SeqHeader seq;
              packet->RemoveHeader (seq);
	      seqNum = seq.GetSeq();
	    }

	  uint64_t ts = Simulator::Now().GetTimeStep();	  

          if (InetSocketAddress::IsMatchingType (from))
	    {
	      if (seqNum < 10)
		NS_LOG_INFO("recv 00" << seqNum << '\t' << ts);
	      else if (seqNum < 100)
		NS_LOG_INFO("recv 0" << seqNum << '\t' << ts);
	      else
		NS_LOG_INFO("recv " << seqNum << '\t' << ts);
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
	  if (m_mode == 0)
	    {
	      m_received++;
	      m_results[seqNum] = ts;
	    }
      }
    }
}

void CompressionReceiver::HandleTcpAccept (Ptr<Socket> s, const Address& from)
{
  NS_LOG_FUNCTION (this << s << from);
  if (m_initialized == 0)
    s->SetRecvCallback (MakeCallback (&CompressionReceiver::Initialize, this));    
  else
    m_nextEvent = Simulator::Schedule (Seconds (0.0), &CompressionReceiver::Process, this);
  m_connection = s;  
}

void
CompressionReceiver::Initialize (Ptr<Socket> socket)
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

  NS_LOG_INFO ("numPackets = " << m_numPackets);
  NS_LOG_INFO ("interPacketTime = " << m_interPacketTime);
  NS_LOG_INFO ("probePacketLen = " << m_probePacketLen);

  m_results = new int64_t[m_numPackets];
  for (uint32_t i = 0; i < m_numPackets; i++)
    m_results[i] = -1;

  Ptr<Packet> p = Create<Packet> (1);  
  socket->Send(p);
}

void CompressionReceiver::HandleTcpError (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}

void CompressionReceiver::HandleTcpClose (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}

void
CompressionReceiver::Process (void)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_INFO ("Processing experiment results");

  std::stringstream ss;
  std::string results;
  std::ofstream output;  
  output.open(m_name.c_str());

  uint32_t chunkSize = 15000;
  for (uint32_t chunk = 0; chunk < 1 + m_numPackets / chunkSize; chunk++)
    {
      for (uint32_t i = chunk*chunkSize; i < (chunk+1)*chunkSize && i < m_numPackets; i++)
	{
	  ss << i;
	  ss << "\t";
	  ss << m_results[i];
	  ss << "\n";
	}
      
      results = ss.str();
      output << results;
      ss.str("");
    }
  output.close(); 
  delete [] m_results;

  NS_LOG_INFO ("Saved results to: " << m_name);

  /*
  // SAVING RESULTS OVER TCP IS BROKEN
  ExpDataHeader expData;
  expData.SetNumPackets(m_numPackets);
  expData.SetInterPacketTime(m_interPacketTime);
  expData.SetProbePacketLen(m_probePacketLen);

  expData.SetResults(results);

  Ptr<Packet> p = Create<Packet> ();
  p->AddHeader(expData);

  NS_LOG_INFO ("Sending results: " << p->GetSize() << " bytes Uid: " << p->GetUid () 
	       << " Time: " << (Simulator::Now ()).GetSeconds ()
	       );

  m_connection->Send(p);
  */
  return;
}

} // Namespace ns3

