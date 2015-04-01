/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// Network topology
//       S1 ------|                |-------R1
//                |                |
//                N0 ---- N1 ---- N2
//                |                |
//       S2-------|                |-------R2
//
// - Flow from S1 to R1 and S2 to R2 using FTP (BulkSendApplication).
// - WFQ implemented on N1

#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("WFQ_Validation");

Ptr<PointToPointNetDevice> NetDeviceDynamicCast (Ptr<NetDevice> const&p)
{
  return Ptr<PointToPointNetDevice> (dynamic_cast<PointToPointNetDevice *> (PeekPointer (p)));
}

int
main (int argc, char *argv[])
{

  bool tracing = true;
  uint16_t secondQueuePort = 3001;
  uint16_t R1port = 3000;
  uint16_t R2port = 3001;
  uint32_t tcp_adu_size = 536;

  uint32_t DEFAULT_DATA_BYTES = 1000000;
  uint32_t ftp1_data_bytes = DEFAULT_DATA_BYTES;
  uint32_t ftp2_data_bytes = DEFAULT_DATA_BYTES;

  double DEFAULT_START_TIME = 1.0;
  double DEFAULT_END_TIME = 19.0;

  double simEndTime = DEFAULT_END_TIME;

  double ftpApp1StartTime = DEFAULT_START_TIME;
  double ftpApp1EndTime = DEFAULT_END_TIME;
  double ftpApp2StartTime = DEFAULT_START_TIME;
  double ftpApp2EndTime = DEFAULT_END_TIME;

  double sinkApps1StartTime = DEFAULT_START_TIME;
  double sinkApps1EndTime = DEFAULT_END_TIME;
  double sinkApps2StartTime = DEFAULT_START_TIME;
  double sinkApps2EndTime = DEFAULT_END_TIME;

  std::string DEFAULT_DELAY = "5ms";
  std::string S1N0Delay = DEFAULT_DELAY;
  std::string S2N0Delay = DEFAULT_DELAY;
  std::string N0N1Delay = DEFAULT_DELAY;
  std::string N1N2Delay = DEFAULT_DELAY;
  std::string N2R1Delay = DEFAULT_DELAY;
  std::string N2R2Delay = DEFAULT_DELAY;

  std::string DEFAULT_DATARATE = "1Mbps";
  std::string S1N0DataRate = DEFAULT_DATARATE;
  std::string S2N0DataRate = DEFAULT_DATARATE;
  std::string N0N1DataRate = DEFAULT_DATARATE;
  std::string N1N2DataRate = "0.5Mbps";
  std::string N2R1DataRate = DEFAULT_DATARATE;
  std::string N2R2DataRate = DEFAULT_DATARATE;

  uint16_t DEFAULT_MTU = 2064;
  uint16_t S1N0Mtu = DEFAULT_MTU;
  uint16_t S2N0Mtu = DEFAULT_MTU;
  uint16_t N0N1Mtu = DEFAULT_MTU;
  uint16_t N1N2Mtu = DEFAULT_MTU;
  uint16_t N2R1Mtu = DEFAULT_MTU;
  uint16_t N2R2Mtu = DEFAULT_MTU;

  uint32_t DEFAULT_QUEUE_SIZE = 655350000;
  uint32_t TXQueueSizeS1N0 = DEFAULT_QUEUE_SIZE;
  uint32_t TXQueueSizeS2N0 = DEFAULT_QUEUE_SIZE;
  uint32_t TXQueueSizeN0N1 = DEFAULT_QUEUE_SIZE;
  uint32_t TXQueueSizeN2N1 = DEFAULT_QUEUE_SIZE;
  uint32_t TXQueueSizeN2R1 = DEFAULT_QUEUE_SIZE;
  uint32_t TXQueueSizeN2R2 = DEFAULT_QUEUE_SIZE;

  uint32_t firstWeightedQueueSize = DEFAULT_QUEUE_SIZE;
  uint32_t secondWeightedQueueSize = DEFAULT_QUEUE_SIZE;

  uint32_t firstWeight = 1;
  uint32_t secondWeight = 2;


// Allow the user to override any of the defaults at
// run-time, via command-line arguments
  CommandLine cmd;
  cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing);

  cmd.AddValue ("secondQueuePort", "Destination port for second queue traffic", secondQueuePort);
  cmd.AddValue ("R1port", "R1 port", R1port);
  cmd.AddValue ("R2port", "R2 port", R2port);
  cmd.AddValue ("tcp_adu_size", "Packet payload size", tcp_adu_size); // Actually, this is the maximum payload size, but the BulkSender application sends packets only at maximum size. So this technically sets the size of our packet's payload.

  cmd.AddValue ("ftp1_dataBytes", "Total number of bytes for first FTP application to send", ftp1_data_bytes);
  cmd.AddValue ("ftp2_dataBytes", "Total number of bytes for second FTP application to send", ftp2_data_bytes);

  cmd.AddValue ("simEndTime", "Simulation end time", simEndTime);
  cmd.AddValue ("ftpApp1StartTime", "First FTP application start time", ftpApp1StartTime);
  cmd.AddValue ("ftpApp1EndTime", "First FTP application end time", ftpApp1EndTime);
  cmd.AddValue ("ftpApp2StartTime", "Second FTP application start time", ftpApp2StartTime);
  cmd.AddValue ("ftpApp2EndTime", "Second FTP application end time", ftpApp2EndTime);
  cmd.AddValue ("sinkApps1StartTime", "First receiver application start time", sinkApps1StartTime);
  cmd.AddValue ("sinkApps1EndTime", "First receiver application end time", sinkApps1EndTime);
  cmd.AddValue ("sinkApps2StartTime", "Second receiver application start time", sinkApps2StartTime);
  cmd.AddValue ("sinkApps2EndTime", "Second receiver application end time", sinkApps2EndTime);

  cmd.AddValue ("S1N0Delay", "The transmission delay of the p2pchannel S1N0", S1N0Delay);
  cmd.AddValue ("S2N0Delay", "The transmission delay of the p2pchannel S2N0", S2N0Delay);
  cmd.AddValue ("N0N1Delay", "The transmission delay of the p2pchannel N0N1", N0N1Delay);
  cmd.AddValue ("N1N2Delay", "The transmission delay of the p2pchannel N1N2", N1N2Delay);
  cmd.AddValue ("N2R1Delay", "The transmission delay of the p2pchannel N2R1", N2R1Delay);
  cmd.AddValue ("N2R2Delay", "The transmission delay of the p2pchannel N2R2", N2R2Delay);

  cmd.AddValue ("S1N0DataRate", "The transmission rate on p2pchannel S1N0", S1N0DataRate);
  cmd.AddValue ("S2N0DataRate", "The transmission rate on p2pchannel S2N0", S2N0DataRate);
  cmd.AddValue ("N0N1DataRate", "The transmission rate on p2pchannel N0N1", N0N1DataRate);
  cmd.AddValue ("N1N2DataRate", "The transmission rate on p2pchannel N1N2", N1N2DataRate);
  cmd.AddValue ("N2R1DataRate", "The transmission rate on p2pchannel N2R1", N2R1DataRate);
  cmd.AddValue ("N2R2DataRate", "The transmission rate on p2pchannel N2R2", N2R2DataRate);

  cmd.AddValue ("S1N0Mtu", "S1N0 MTU", S1N0Mtu);
  cmd.AddValue ("S2N0Mtu", "S2N0 MTU", S2N0Mtu);
  cmd.AddValue ("N0N1Mtu", "N0N1 MTU", N0N1Mtu);
  cmd.AddValue ("N1N2Mtu", "N1N2 MTU", N1N2Mtu);
  cmd.AddValue ("N2R1Mtu", "N2R1 MTU", N2R1Mtu);
  cmd.AddValue ("N2R2Mtu", "N2R2 MTU", N2R2Mtu);

  cmd.AddValue ("TXQueueSizeS1N0", "The size of the outgoing queue on S1N0", TXQueueSizeS1N0);
  cmd.AddValue ("TXQueueSizeS2N0", "The size of the outgoing queue on S2N0", TXQueueSizeS2N0);
  cmd.AddValue ("TXQueueSizeN0N1", "The size of the outgoing queue on N0N1", TXQueueSizeN0N1);
  cmd.AddValue ("TXQueueSizeN2N1", "The size of the outgoing queue on N2N1", TXQueueSizeN2N1);
  cmd.AddValue ("TXQueueSizeN2R1", "The size of the outgoing queue on N2R1", TXQueueSizeN2R1);
  cmd.AddValue ("TXQueueSizeN2R2", "The size of the outgoing queue on N2R2", TXQueueSizeN2R2);

  cmd.AddValue ("firstWeightedQueueSize", "The size of the first weighted queue", firstWeightedQueueSize);
  cmd.AddValue ("secondWeightedQueueSize", "The size of the second weighted queue", secondWeightedQueueSize);

  cmd.AddValue ("firstWeight", "The weight of the first queue", firstWeight);
  cmd.AddValue ("secondWeight", "The weight of the second queue", secondWeight);

  cmd.Parse (argc, argv);


// creating the topology
  NodeContainer allNodes;
  allNodes.Create(7);

  NodeContainer S1 = NodeContainer (allNodes.Get(0));
  NodeContainer S2 = NodeContainer (allNodes.Get(1));
  NodeContainer N0 = NodeContainer (allNodes.Get(2));
  NodeContainer N1 = NodeContainer (allNodes.Get(3));
  NodeContainer N2 = NodeContainer (allNodes.Get(4));
  NodeContainer R1 = NodeContainer (allNodes.Get(5));
  NodeContainer R2 = NodeContainer (allNodes.Get(6));

  NodeContainer S1N0 = NodeContainer (S1, N0);
  NodeContainer S2N0 = NodeContainer (S2, N0);
  NodeContainer N0N1 = NodeContainer (N0, N1);
  NodeContainer N1N2 = NodeContainer (N1, N2);
  NodeContainer N2R1 = NodeContainer (N2, R1);
  NodeContainer N2R2 = NodeContainer (N2, R2);


  // Create the point to point net devices and channels
  PointToPointHelper p2p;
  p2p.SetQueue("ns3::DropTailQueue", "Mode", (StringValue) "QUEUE_MODE_PACKETS");

  //S1N0
  p2p.SetChannelAttribute ("Delay", (StringValue) S1N0Delay);
  p2p.SetDeviceAttribute ("DataRate", (StringValue) S1N0DataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (S1N0Mtu));
  p2p.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue (TXQueueSizeS1N0));
  NetDeviceContainer S1N0_d = p2p.Install (S1N0);

  //S2N0
  p2p.SetChannelAttribute ("Delay", (StringValue) S2N0Delay);
  p2p.SetDeviceAttribute ("DataRate", (StringValue) S2N0DataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (S2N0Mtu));
  p2p.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue (TXQueueSizeS2N0));
  NetDeviceContainer S2N0_d = p2p.Install (S2N0);

  //N0N1
  p2p.SetChannelAttribute ("Delay", (StringValue) N0N1Delay);
  p2p.SetDeviceAttribute ("DataRate", (StringValue) N0N1DataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (N0N1Mtu));
  p2p.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue (TXQueueSizeN0N1));
  NetDeviceContainer N0N1_d = p2p.Install (N0N1);


  //N1N2: WFQ on N1, regular tail drop queue on N2
  p2p.SetChannelAttribute ("Delay", (StringValue) N1N2Delay);
  p2p.SetDeviceAttribute ("DataRate", (StringValue) N1N2DataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (N1N2Mtu));
  p2p.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue (TXQueueSizeN2N1));
  NetDeviceContainer N1N2_d = p2p.Install (N1N2);

  ObjectFactory m_queueFactory;
  m_queueFactory.SetTypeId ("ns3::WeightedFairQueue");
  m_queueFactory.Set ("FirstWeightedQueueMaxPackets", UintegerValue (firstWeightedQueueSize));
  m_queueFactory.Set ("SecondWeightedQueueMaxPackets", UintegerValue (secondWeightedQueueSize));
  m_queueFactory.Set ("SecondQueuePort", UintegerValue (secondQueuePort));
  m_queueFactory.Set ("FirstWeight", UintegerValue (firstWeight));
  m_queueFactory.Set ("SecondWeight", UintegerValue (secondWeight));

  Ptr<Queue> queueN1 = m_queueFactory.Create<Queue> ();
  Ptr<PointToPointNetDevice>  devN1 = NetDeviceDynamicCast(N1N2_d.Get(0));
  devN1->SetQueue (queueN1);


  //N2R1
  p2p.SetChannelAttribute ("Delay", (StringValue) N2R1Delay);
  p2p.SetDeviceAttribute ("DataRate", (StringValue) N2R1DataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (N2R1Mtu));
  p2p.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue (TXQueueSizeN2R1));
  NetDeviceContainer N2R1_d = p2p.Install (N2R1);

  //N2R2
  p2p.SetChannelAttribute ("Delay", (StringValue) N2R2Delay);
  p2p.SetDeviceAttribute ("DataRate", (StringValue) N2R2DataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (N2R2Mtu));
  p2p.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue (TXQueueSizeN2R2));
  NetDeviceContainer N2R2_d = p2p.Install (N2R2);


  // Create the Internet stacks
  InternetStackHelper stack;
  stack.Install (allNodes);


  // Create addresses
  Ipv4AddressHelper ipv4;

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer S1N0_i = ipv4.Assign (S1N0_d);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer S2N0_i = ipv4.Assign (S2N0_d);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer N0N1_i = ipv4.Assign (N0N1_d);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer N1_i = ipv4.Assign (N1N2_d);

  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer N2R1_i = ipv4.Assign (N2R1_d);

  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer N2R2_i = ipv4.Assign (N2R2_d);


  // Enable routing
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  // Create applications

  // create a FTP application and install it on S1
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (tcp_adu_size));
  AddressValue R1Address (InetSocketAddress (N2R1_i.GetAddress (1), R1port));
  BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
  ftp.SetAttribute ("Remote", R1Address);
  ftp.SetAttribute ("SendSize", UintegerValue (tcp_adu_size));
  ftp.SetAttribute ("MaxBytes", UintegerValue (ftp1_data_bytes));
  ApplicationContainer ftpApp1 = ftp.Install (S1.Get (0));

  ftpApp1.Start (Seconds (ftpApp1StartTime));
  ftpApp1.Stop (Seconds (ftpApp1EndTime));


  // create a FTP application and install it on S2
  AddressValue R2Address (InetSocketAddress (N2R2_i.GetAddress (1), R2port));
  ftp.SetAttribute ("Remote", R2Address);
  ftp.SetAttribute ("MaxBytes", UintegerValue (ftp2_data_bytes));
  ApplicationContainer ftpApp2 = ftp.Install (S2.Get (0));

  ftpApp2.Start (Seconds (ftpApp2StartTime));
  ftpApp2.Stop (Seconds (ftpApp2EndTime));


// Create a PacketSinkApplication and install it on R1
  PacketSinkHelper sink1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), R1port));
  ApplicationContainer sinkApps1 = sink1.Install (R1.Get (0));
  sinkApps1.Start (Seconds (sinkApps1StartTime));
  sinkApps1.Stop (Seconds (sinkApps1EndTime));


// Create a PacketSinkApplication and install it on R2
  PacketSinkHelper sink2 ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), R2port));
  ApplicationContainer sinkApps2 = sink2.Install (R2.Get (0));
  sinkApps2.Start (Seconds (sinkApps2StartTime));
  sinkApps2.Stop (Seconds (sinkApps2EndTime));


// Set up tracing if enabled
  if (tracing)
    {
      //AsciiTraceHelper ascii;
      //p2p.EnableAscii (ascii.CreateFileStream ("wfq-r1.tr"), R1);
      //p2p.EnableAscii (ascii.CreateFileStream ("wfq-r2.tr"), R2);

      p2p.EnablePcap ("wfq-r1", R1, false);
      p2p.EnablePcap ("wfq-r2", R2, false);
    }


// Now, do the actual simulation.
  Simulator::Stop (Seconds (simEndTime));
  Simulator::Run ();
  Simulator::Destroy ();


  Ptr<PacketSink> packetSink1 = DynamicCast<PacketSink> (sinkApps1.Get (0));
  std::cout << "Total Bytes Received at R1: " << packetSink1->GetTotalRx () << std::endl;

  Ptr<PacketSink> packetSink2 = DynamicCast<PacketSink> (sinkApps2.Get (0));
  std::cout << "Total Bytes Received at R2: " << packetSink2->GetTotalRx () << std::endl;
}
