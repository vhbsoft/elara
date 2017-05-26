/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Robert Chang 2012
 * Usage:
> ./waf --run "4node-compression --outputFile=./test.dat --entropy=h --numPackets=1000  --packetSize=1024 --interPacketTime=.0000001 --s0p0Delay=30ms --p0p1Delay=30ms --p1r0Delay=30ms --s0p0DataRate=5Mbps --p0p1DataRate=4Mbps --p1r0DataRate=5Mbps --queueMode=b --s0QueueSize=655350000 --p0QueueSize=655350000 --p1QueueSize=655350000  --compression=1"

> ./waf --run "4node-compression --outputFile=./test.dat --entropy=h --numPackets=1000  --packetSize=1024 --interPacketTime=.0000001 --s0p0Delay=30ms --p0p1Delay=30ms --p1r0Delay=30ms --s0p0DataRate=5Mbps --p0p1DataRate=4Mbps --p1r0DataRate=5Mbps --queueMode=p --s0QueueSize=1000 --p0QueueSize=1000 --p1QueueSize=1000  --compression=1"

 */

#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/compression-module.h"
#include "ns3/experiment-module.h"
#include "ns3/nstime.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Compression-Test");

int
main (int argc, char *argv[])
{   

  // Experiment Settings
  uint32_t compression = 1;
  double startTime = 0.0;
  double endTime = 60000.0;
  uint8_t log = 0;

  // Application Settings  
  uint32_t numPackets = 10;
  double interPacketTime = 0.001;
  uint32_t packetSize = 1072;
  char entropy = 'l';
  std::string outputFile = "ns_output_file";

  // Network Settings
  std::string s0p0Delay = "30ms";
  std::string p0p1Delay = "30ms";
  std::string p1r0Delay = "30ms";

  std::string p0p1DataRate = "5Mbps";
  std::string s0p0DataRate = "4Mbps";
  std::string p1r0DataRate = "5Mbps";

  uint32_t s0p0Mtu = 2 * packetSize;
  uint32_t p0p1Mtu = 2 * packetSize;
  uint32_t p1r0Mtu = 2 * packetSize;
  
  char queueMode = 'p';
  uint32_t s0QueueSize = 2000;
  uint32_t p0QueueSize = 2000;
  uint32_t p1QueueSize = 2000;

  // Parse the Command Line  
  CommandLine cmd;
  cmd.AddValue ("compression", "1 to enable compression, 0 to disable. Enabled by default", compression);
  cmd.AddValue ("outputFile", "The file to save the packet arrival times", outputFile);
  cmd.AddValue ("startTime", "The start time of both the sender and receiver", startTime);
  cmd.AddValue ("endTime", "The end time of the sender, receiver ends 1 second before", endTime);
  cmd.AddValue ("log", "1 to enable logging messages, 0 to disable. Disabled by default", log);


  cmd.AddValue ("entropy", "The entropy of the packets sent", entropy);
  cmd.AddValue ("numPackets", "Number of packets to Send", numPackets);
  cmd.AddValue ("packetSize", "Size of the packets, bounds are 12 and 1500", packetSize);
  cmd.AddValue ("interPacketTime", "Time inbetween packets in seconds", interPacketTime);

  cmd.AddValue ("s0p0Delay", "The transmission delay of the channel s0p0", s0p0Delay);
  cmd.AddValue ("p0p1Delay", "The transmission delay of the channel p0p1", p0p1Delay);
  cmd.AddValue ("p1r0Delay", "The transmission delay of the channel p1r0", p1r0Delay);
 
  cmd.AddValue ("s0p0DataRate", "The transmission rate between net devices on channel ", s0p0DataRate);
  cmd.AddValue ("p0p1DataRate", "The transmission rate between net devices on channel ", p0p1DataRate);
  cmd.AddValue ("p1r0DataRate", "The transmission rate between net devices on channel ", p1r0DataRate);

  cmd.AddValue ("s0p0Mtu", "The max transmission size between net devices on channel ", s0p0Mtu);
  cmd.AddValue ("p0p1Mtu", "The max transmission size between net devices on channel ", p0p1Mtu);
  cmd.AddValue ("p1r0Mtu", "The max transmission size between net devices on channel ", p1r0Mtu);

  cmd.AddValue ("queueMode", "Whether queues are based on bytes or packets. b for bytes, p for packets", queueMode);

  cmd.AddValue ("s0QueueSize", "The size of the queue at s0", s0QueueSize);
  cmd.AddValue ("p0QueueSize", "The size of the queue at p0", p0QueueSize);
  cmd.AddValue ("p1QueueSize", "The size of the queue at p1", p1QueueSize);

  cmd.Parse (argc,argv);
 
//
// CREATE THE NETWORK TOPOLOGY
//

  // Create the nodes
  NodeContainer allNodes;
  allNodes.Create(4);

  NodeContainer sendNodes = NodeContainer (allNodes.Get(0));
  NodeContainer recvNodes = NodeContainer (allNodes.Get(3));
  NodeContainer pathNodes = NodeContainer (allNodes.Get(1), allNodes.Get(2));
  NodeContainer s0p0 = NodeContainer (sendNodes.Get(0), pathNodes.Get(0));
  NodeContainer p0p1 = NodeContainer (pathNodes.Get(0), pathNodes.Get(1));
  NodeContainer p1r0 = NodeContainer (pathNodes.Get(1), recvNodes.Get(0));
  
  // Create the internet stacks
  InternetStackHelper stack;
  stack.Install (allNodes); 

  // Create the point to point net devices and channels
  PointToPointHelper p2p;
  p2p.SetQueue("ns3::DropTailQueue", "Mode", (StringValue) "QUEUE_MODE_BYTES");

  p2p.SetChannelAttribute ("Delay", (StringValue) s0p0Delay); 
  p2p.SetDeviceAttribute ("DataRate", (StringValue) s0p0DataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (s0p0Mtu));
  if (queueMode == 'b') {
    p2p.SetQueue("ns3::DropTailQueue", "Mode", EnumValue(DropTailQueue::QUEUE_MODE_BYTES));
    p2p.SetQueue("ns3::DropTailQueue", "MaxBytes", UintegerValue (s0QueueSize));
  }
  else if (queueMode == 'p') {
    p2p.SetQueue("ns3::DropTailQueue", "Mode", EnumValue(DropTailQueue::QUEUE_MODE_PACKETS));
    p2p.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue (s0QueueSize));
  }
  NetDeviceContainer s0p0_d = p2p.Install (s0p0);

  p2p.SetChannelAttribute ("Delay", (StringValue) p1r0Delay); 
  p2p.SetDeviceAttribute ("DataRate", (StringValue) p1r0DataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (p1r0Mtu));
  if (queueMode == 'b') {
    p2p.SetQueue("ns3::DropTailQueue", "Mode", EnumValue(DropTailQueue::QUEUE_MODE_BYTES));
    p2p.SetQueue("ns3::DropTailQueue", "MaxBytes", UintegerValue (p1QueueSize));
  }
  else if (queueMode == 'p') {
    p2p.SetQueue("ns3::DropTailQueue", "Mode", EnumValue(DropTailQueue::QUEUE_MODE_PACKETS));
    p2p.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue (p1QueueSize));
  }
  NetDeviceContainer p1r0_d = p2p.Install (p1r0);  

  // The middle connection is a compression link
  CompressionHelper comp;
  comp.SetChannelAttribute ("Delay", (StringValue) p0p1Delay); 
  comp.SetDeviceAttribute ("DataRate", (StringValue) p0p1DataRate);
  comp.SetDeviceAttribute ("Mtu", UintegerValue (p0p1Mtu));
  if (queueMode == 'b') {
    comp.SetQueue("ns3::DropTailQueue", "Mode", EnumValue(DropTailQueue::QUEUE_MODE_BYTES));
    comp.SetQueue("ns3::DropTailQueue", "MaxBytes", UintegerValue (p0QueueSize));
  }
  else if (queueMode == 'p') {
    comp.SetQueue("ns3::DropTailQueue", "Mode", EnumValue(DropTailQueue::QUEUE_MODE_PACKETS));
    comp.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue (p0QueueSize));
  }
  NetDeviceContainer p0p1_d = comp.Install (p0p1);

  // Create addresses
  Ipv4AddressHelper ipv4;

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer s0p0_i = ipv4.Assign (s0p0_d);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p0p1_i = ipv4.Assign (p0p1_d);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer p1r0_i = ipv4.Assign (p1r0_d);

  // Create router nodes
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

//
// Create applications
//


  //Create, configure, and install CompressionSender
  CompressionSenderHelper sendHelper;
  sendHelper.SetAttribute ("NumPackets", UintegerValue (numPackets));
  sendHelper.SetAttribute ("Interval", TimeValue (Seconds (interPacketTime)));
  sendHelper.SetAttribute ("PacketSize", UintegerValue (packetSize));
  sendHelper.SetAttribute ("RemoteAddress", AddressValue (p1r0_i.GetAddress(1))); 
  ApplicationContainer sender = sendHelper.Install (sendNodes.Get (0));
  sendHelper.GetSender()->SetEntropy(entropy);
  sendHelper.GetSender()->SetLogFileName (outputFile);  
  sender.Start (Seconds (startTime));
  sender.Stop (Seconds (endTime-1));

  // Create, configure, and install CompressionReceiver
  CompressionReceiverHelper recvHelper;
  ApplicationContainer receiver = recvHelper.Install (recvNodes.Get (0));
  recvHelper.GetReceiver()->SetLogFileName (outputFile);
  receiver.Start (Seconds (startTime));
  receiver.Stop (Seconds (endTime));

  if (log!=0)
    {
      LogComponentEnable("CompressionSender", LOG_LEVEL_INFO);
      LogComponentEnable("CompressionReceiver", LOG_LEVEL_INFO);
      LogComponentEnable("CompressionModel", LOG_LEVEL_INFO);

      //LogComponentEnable("CompressionSender", LOG_LEVEL_FUNCTION);
      //LogComponentEnable("CompressionReceiver", LOG_LEVEL_FUNCTION);

      //LogComponentEnable("PointToPointNetDevice", LOG_LEVEL_INFO);
      //LogComponentEnable("PointToPointNetDevice", LOG_LEVEL_FUNCTION);
    }
  
  Simulator::Run ();

  recvHelper.GetReceiver()->Process();

  Simulator::Destroy ();

  return 0;
}
