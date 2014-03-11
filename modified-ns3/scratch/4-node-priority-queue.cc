/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Robert Chang 2012
 *
 * Usage:
./waf --run "4-node-priority-queue --procDelay=0.000001 --ReceiveQueueSizeR2=10000 --TXQueueSizeS=400000000 --TXQueueSizeR1=6000000 --TXQueueSizeR2=15000000 --SR1DataRate=10Mbps --R1R2DataRate=10Mbps --R2RDataRate=10Mbps --SR1Delay=5ms --R1R2Delay=5ms --R2RDelay=5ms --packetSize=10 --outputFile=./output_files/temp.dat --interPacketTime=0.00000001 --separationPacketTrainLength=1000 --initialPacketTrainLength=150 --numAptPriorityProbes=500 --aptPriority='H'"
 */

#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/priority-queue-module.h"
#include "ns3/nstime.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Priority-Queue-Test");

int
main (int argc, char *argv[])
{   

  // Packet Train Settings
  uint32_t separationPacketTrainLength = 10;
  uint32_t initialPacketTrainLength = 10;
  uint32_t numAptPriorityProbes = 10;
  uint8_t aptPriority = 'H';

  uint32_t packetSize = 1100;
  double interPacketTime = 0.00000001;

  // Network Settings
  uint32_t ReceiveQueueSizeR2 = 655350000;
  uint32_t highPriorityQueueSize = 655350000;
  uint32_t lowPriorityQueueSize = 655350000;
  double procDelay = 1;


  uint32_t TXQueueSizeS = 655350000;
  uint32_t TXQueueSizeR1 = 655350000;
  uint32_t TXQueueSizeR2 = 655350000;
  std::string SR1DataRate = "4Mbps";
  std::string R1R2DataRate = "4Mbps";
  std::string R2RDataRate = "4Mbps";
  std::string SR1Delay = "30ms";
  std::string R1R2Delay = "30ms";
  std::string R2RDelay = "30ms";
  uint16_t SMtu = 2064;
  uint16_t R1Mtu = 2064;
  uint16_t R2Mtu = 2064;
  
  char topologyNodes = '4';

  // Simulation Settings
  std::string outputFile = "temp.dat";
  double startTime = 0.0;
  double endTime = 72000000.0;
  uint8_t log = 0;

  uint8_t enablePQ = 1;

  // Get command line options
  CommandLine cmd;

  cmd.AddValue ("separationPacketTrainLength", "", separationPacketTrainLength);
  cmd.AddValue ("initialPacketTrainLength", "", initialPacketTrainLength);
  cmd.AddValue ("numAptPriorityProbes", "", numAptPriorityProbes);
  cmd.AddValue ("aptPriority", "", aptPriority);

  cmd.AddValue ("packetSize", "Size of the packets, bounds are 2 and 1500", packetSize);
  cmd.AddValue ("interPacketTime", "Time inbetween packets in seconds", interPacketTime);

  cmd.AddValue ("ReceiveQueueSizeR2", "The size of the incoming queue on R2", ReceiveQueueSizeR2);
  //cmd.AddValue ("highPriorityQueueSize", "", highPriorityQueueSize);
  //cmd.AddValue ("lowPriorityQueueSize", "", lowPriorityQueueSize);
  cmd.AddValue ("procDelay", "processing delay on R1", procDelay);

  cmd.AddValue ("TXQueueSizeS", "The size of the outgoing queue on S", TXQueueSizeS);
  cmd.AddValue ("TXQueueSizeR1", "The size of the outgoing queue on R1", TXQueueSizeR1);
  cmd.AddValue ("TXQueueSizeR2", "The size of the outgoing queue on R2", TXQueueSizeR2);

  cmd.AddValue ("SR1DataRate", "The transmission rate on p2pchannel SR1", SR1DataRate);
  cmd.AddValue ("R1R2DataRate", "The transmission rate on p2pchannel R1R2", R1R2DataRate);
  cmd.AddValue ("R2RDataRate", "The transmission rate on p2pchannel R2R", R2RDataRate);
  cmd.AddValue ("SR1Delay", "The transmission delay of the p2pchannel s0p0", SR1Delay);
  cmd.AddValue ("R1R2Delay", "The transmission delay of the p2pchannel p0p1", R1R2Delay);
  cmd.AddValue ("R2RDelay", "The transmission delay of the p2pchannel p1r0", R2RDelay);
  cmd.AddValue ("SMtu","", SMtu);
  cmd.AddValue ("R1Mtu","", R1Mtu);
  cmd.AddValue ("R2Mtu","", R2Mtu);

  cmd.AddValue ("topologyNodes", "the number of nodes in the simulation (3 or 4)", topologyNodes);

  cmd.AddValue ("outputFile", "The file to save the packet arrival times", outputFile);
  cmd.AddValue ("startTime", "The start time of both the sender and receiver", startTime);
  cmd.AddValue ("endTime", "The end time of the sender, receiver ends 1 second before", endTime);
  cmd.AddValue ("log", "1 to enable logging messages, 0 to disable. Disabled by default", log);

  cmd.AddValue ("enablePQ", "1 enables, 0 disables. Enabled by default", enablePQ);
 
  cmd.Parse (argc,argv);
 
//
// CREATE THE NETWORK TOPOLOGY
//

  // Create the nodes
  NodeContainer allNodes;
  allNodes.Create(4);

  NodeContainer S = NodeContainer (allNodes.Get(0));
  NodeContainer R1 = NodeContainer (allNodes.Get(1));
  NodeContainer R2 = NodeContainer (allNodes.Get(2));
  NodeContainer R = NodeContainer (allNodes.Get(3));  

  NodeContainer SR1 = NodeContainer (allNodes.Get(0), allNodes.Get(1));  
  NodeContainer R1R2 = NodeContainer (allNodes.Get(1), allNodes.Get(2));
  NodeContainer R2R = NodeContainer (allNodes.Get(2), allNodes.Get(3));
  
  // Create the internet stacks
  InternetStackHelper stack;
  stack.Install (allNodes); 

  // Create the point to point net devices and channels
  PointToPointHelper p2p;
  p2p.SetQueue("ns3::DropTailQueue", "Mode", (StringValue) "QUEUE_MODE_BYTES");
  //p2p.SetQueue("ns3::DropTailQueue", "Mode", (StringValue) "QUEUE_MODE_PACKETS");

  //SR1
  p2p.SetChannelAttribute ("Delay", (StringValue) SR1Delay);
  p2p.SetDeviceAttribute ("DataRate", (StringValue) SR1DataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (SMtu));
  p2p.SetQueue("ns3::DropTailQueue", "MaxBytes", UintegerValue (TXQueueSizeS));
  NetDeviceContainer SR1_d = p2p.Install (SR1);

  //p2p.SetQueue("ns3::DropTailQueue", "Mode", (StringValue) "QUEUE_MODE_BYTES");

  //R2R
  p2p.SetChannelAttribute ("Delay", (StringValue) R2RDelay); 
  p2p.SetDeviceAttribute ("DataRate", (StringValue) R2RDataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (R2Mtu));
  p2p.SetQueue("ns3::DropTailQueue", "MaxBytes", UintegerValue (TXQueueSizeR2));
  NetDeviceContainer R2R_d = p2p.Install (R2R);

  //R1R2

  // Set up priority queue model

  Ptr<PriorityQueueModel> spq = CreateObject<PriorityQueueModel> ();
  if (enablePQ==1) spq->Enable(); else spq->Disable();

  spq->SetNodalProcessingTime (Seconds (procDelay));
  
  ObjectFactory queueFactory;
  queueFactory.SetTypeId (DropTailQueue::GetTypeId());
  queueFactory.Set ("Mode", (StringValue) "QUEUE_MODE_BYTES");

  highPriorityQueueSize = ReceiveQueueSizeR2;
  lowPriorityQueueSize = ReceiveQueueSizeR2;

  queueFactory.Set ("MaxBytes", UintegerValue (highPriorityQueueSize));
  Ptr<Queue> highQueue = queueFactory.Create<DropTailQueue> ();
  spq->SetHighQueue(highQueue);

  queueFactory.Set ("MaxBytes", UintegerValue (lowPriorityQueueSize));
  Ptr<Queue> lowQueue = queueFactory.Create<DropTailQueue> ();
  spq->SetLowQueue(lowQueue);  

  p2p.SetDeviceAttribute ("PriorityQueueModel", (PointerValue) spq);

  p2p.SetChannelAttribute ("Delay", (StringValue) R1R2Delay); 
  p2p.SetDeviceAttribute ("DataRate", (StringValue) R1R2DataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (R1Mtu));
  p2p.SetQueue("ns3::DropTailQueue", "MaxBytes", UintegerValue (TXQueueSizeR1));
  NetDeviceContainer R1R2_d = p2p.Install (R1R2);

  // Create addresses
  Ipv4AddressHelper ipv4;

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer SR1_i = ipv4.Assign (SR1_d);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer R1R2_i = ipv4.Assign (R1R2_d);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer R2R_i = ipv4.Assign (R2R_d);

  // Create router nodes
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

//
// Create applications
//

  //Create, configure, and install PriorityQueueReceiver
  PriorityQueueSenderHelper sendHelper;
  sendHelper.SetAttribute ("InitialPacketTrainLength", UintegerValue (initialPacketTrainLength));
  sendHelper.SetAttribute ("SeparationPacketTrainLength", UintegerValue (separationPacketTrainLength));
  sendHelper.SetAttribute ("NumAptPriorityProbes", UintegerValue (numAptPriorityProbes));
  sendHelper.SetAttribute ("AptPriority", UintegerValue (aptPriority));

  sendHelper.SetAttribute ("Interval", TimeValue (Seconds (interPacketTime)));
  sendHelper.SetAttribute ("PacketSize", UintegerValue (packetSize));
  sendHelper.SetAttribute ("RemoteAddress", AddressValue (R2R_i.GetAddress(1)));

  ApplicationContainer sender;

//  if (topologyNodes == '3')
//    sender = sendHelper.Install (R1.Get (0));

//  if (topologyNodes == '4')
  sender = sendHelper.Install (S.Get (0));

  sendHelper.GetSender()->SetLogFileName (outputFile);  // This is terrible programming, fix it later in all 3 modules
  sender.Start (Seconds (startTime));
  sender.Stop (Seconds (endTime-1));

  // Create, configure, and install PriorityQueueReceiver
  PriorityQueueReceiverHelper recvHelper;
  ApplicationContainer receiver = recvHelper.Install (R.Get (0));
  recvHelper.GetReceiver()->SetLogFileName (outputFile); // This is terrible programming, fix it later in all 3 modules
  receiver.Start (Seconds (startTime));
  receiver.Stop (Seconds (endTime));

  // Logging stuff
  LogComponentEnable("PriorityQueueSender", LOG_LEVEL_ERROR);
  LogComponentEnable("PriorityQueueReceiver", LOG_LEVEL_ERROR);
  LogComponentEnable("PriorityQueueModel", LOG_LEVEL_ERROR);

  if (log=='1' || log=='3')
    {
      LogComponentEnable("PriorityQueueSender", LOG_LEVEL_INFO);
      LogComponentEnable("PriorityQueueReceiver", LOG_LEVEL_INFO);
    }
  if (log=='2' || log=='3')
    {
      LogComponentEnable("PriorityQueueModel", LOG_LEVEL_INFO);
      LogComponentEnable("PointToPointNetDevice", LOG_LEVEL_INFO);
    }
      //LogComponentEnable("PointToPointNetDevice", LOG_LEVEL_FUNCTION);
      //LogComponentEnable("ProcessingDelaySender", LOG_LEVEL_FUNCTION);
      //LogComponentEnable("ProcessingDelayReceiver", LOG_LEVEL_FUNCTION);
  
  Simulator::Run ();

  recvHelper.GetReceiver()->Process();

  Simulator::Destroy ();

  return 0;
}
