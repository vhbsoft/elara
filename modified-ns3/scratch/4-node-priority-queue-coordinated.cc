/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Robert Chang 2012
 *
 * Usage:

./waf --run "4-node-priority-queue-coordinated --ReceiveQueueSizeR2=10000 --TXQueueSizeR2=4000 --TXThresholdR2Packets=3996 --procDelay=0.000001 --TXQueueSizeS=4000 --TXQueueSizeR1=4000 --SR1Delay=5ms --R1R2Delay=5ms --R2RDelay=5ms --SR1DataRate=10Mbps --R1R2DataRate=10Mbps --R2RDataRate=5Mbps --outputFile=./output_files/temp.dat --packetSize=4 --interPacketTime=0.00000001 --initialPacketTrainLength=100 --separationPacketTrainLength=10000 --numAptPriorityProbes=100 --aptPriority='H' --queueMode='p'"

./waf --run "4-node-priority-queue-coordinated --ReceiveQueueSizeR2=10000 --TXQueueSizeR2=4000 --TXThresholdR2Bytes=3996 --procDelay=0.000001 --TXQueueSizeS=4000000 --TXQueueSizeR1=4000 --SR1Delay=5ms --R1R2Delay=5ms --R2RDelay=5ms --SR1DataRate=10Mbps --R1R2DataRate=10Mbps --R2RDataRate=5Mbps --outputFile=./output_files/temp.dat --packetSize=4 --interPacketTime=0.00000001 --initialPacketTrainLength=100 --separationPacketTrainLength=10000 --numAptPriorityProbes=100 --aptPriority='H' --queueMode='b'" 

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

  uint32_t packetSize = 4;
  double interPacketTime = 0.00000001;
  bool backToBackPackets = false;

  // Network Settings
  uint8_t queueMode = 'p';
  uint32_t ReceiveQueueSizeR2 = 655350000;
  uint32_t highPriorityQueueSize = 655350000;
  uint32_t lowPriorityQueueSize = 655350000;
  double procDelay = 1;

  uint32_t TXQueueSizeS = 655350000;
  uint32_t TXQueueSizeR1 = 655350000;
  uint32_t TXQueueSizeR2 = 655350000;
  uint32_t thresholdBytes = 655348900;
  uint32_t thresholdPackets = 655348900;

  std::string SR1DataRate = "10Mbps";
  std::string R1R2DataRate = "10Mbps";
  std::string R2RDataRate = "5Mbps";
  std::string SR1Delay = "5ms";
  std::string R1R2Delay = "5ms";
  std::string R2RDelay = "5ms";
  uint16_t SMtu = 2064;
  uint16_t R1Mtu = 2064;
  uint16_t R2Mtu = 2064;
  
  char topologyNodes = '4';

  // Simulation Settings
  std::string outputFile = "./output_files/temp.dat";
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
  cmd.AddValue ("backToBackPackets", "overwrites interPacketTime so packets are sent exactly back to back", backToBackPackets);

  cmd.AddValue ("queueMode", "Whether queues are based on bytes or packets. b for bytes, p for packets", queueMode);
  cmd.AddValue ("ReceiveQueueSizeR2", "The size of the incoming queue on R2", ReceiveQueueSizeR2);
  //cmd.AddValue ("highPriorityQueueSize", "", highPriorityQueueSize);
  //cmd.AddValue ("lowPriorityQueueSize", "", lowPriorityQueueSize);
  cmd.AddValue ("procDelay", "processing delay on R1", procDelay);
  cmd.AddValue ("TXQueueSizeR2", "The size of the outgoing queue on R2", TXQueueSizeR2);
  cmd.AddValue ("TXThresholdR2Bytes", "The capacity of the tx queue on R2 before the scheduler starts waiting", thresholdBytes);
  cmd.AddValue ("TXThresholdR2Packets", "The capacity of the tx queue on R2 before the scheduler starts waiting", thresholdPackets);

  cmd.AddValue ("TXQueueSizeS", "The size of the outgoing queue on S", TXQueueSizeS);
  cmd.AddValue ("TXQueueSizeR1", "The size of the outgoing queue on R1", TXQueueSizeR1);

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

  if (queueMode == 'p') {
    p2p.SetQueue("ns3::DropTailQueue", "Mode", (StringValue) "QUEUE_MODE_PACKETS");
  }
  else if (queueMode == 'b') {
    p2p.SetQueue("ns3::DropTailQueue", "Mode", (StringValue) "QUEUE_MODE_BYTES");
  }
  else {
    NS_LOG_UNCOND("invalid queueMode, value must be 'b' or 'p'");
    exit(1);
  }

  //S->R1
  p2p.SetChannelAttribute ("Delay", (StringValue) SR1Delay);
  p2p.SetDeviceAttribute ("DataRate", (StringValue) SR1DataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (SMtu));
  if (queueMode == 'p') {
    p2p.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue (TXQueueSizeS));
  }
  else if (queueMode == 'b') {
    p2p.SetQueue("ns3::DropTailQueue", "MaxBytes", UintegerValue (TXQueueSizeS));
  }
  NetDeviceContainer SR1_d = p2p.Install (SR1);

  //R2->R
  p2p.SetChannelAttribute ("Delay", (StringValue) R2RDelay); 
  p2p.SetDeviceAttribute ("DataRate", (StringValue) R2RDataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (R2Mtu));
  if (queueMode == 'p') {
    p2p.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue (TXQueueSizeR2));    
  }
  else if (queueMode == 'b') {
    p2p.SetQueue("ns3::DropTailQueue", "MaxBytes", UintegerValue (TXQueueSizeR2));
  }

  // Coordinated
  Ptr<PointToPointNetDevice> netDev;
  NetDeviceContainer R2R_d = p2p.InstallCoordinated (R2R, netDev);
  //NS_LOG_UNCOND("MTU = " << netDev->GetMtu());

  //R1->R2

  // Set up priority queue model
  Ptr<PriorityQueueModel> spq = CreateObject<PriorityQueueModel> ();
  if (enablePQ==1) spq->Enable(); else spq->Disable();
  spq->SetNodalProcessingTime (Seconds (procDelay));

  // Coordinated
  spq->EnableThreshold();
  spq->SetQueueMode(queueMode);
  if (queueMode == 'p') {
    spq->SetThresholdPackets(thresholdPackets);
  }
  else if (queueMode == 'b') {
    spq->SetThresholdBytes(thresholdBytes);
  }
  spq->SetTxQueue(netDev->GetQueue());
  
  // Queues
  ObjectFactory queueFactory;
  queueFactory.SetTypeId (DropTailQueue::GetTypeId());
  highPriorityQueueSize = ReceiveQueueSizeR2;
  lowPriorityQueueSize = ReceiveQueueSizeR2;

  Ptr<Queue> highQueue;
  Ptr<Queue> lowQueue;
  if (queueMode == 'p') {
    queueFactory.Set ("Mode", (StringValue) "QUEUE_MODE_PACKETS");

    queueFactory.Set ("MaxPackets", UintegerValue (highPriorityQueueSize));    
    highQueue = queueFactory.Create<DropTailQueue> ();

    queueFactory.Set ("MaxPackets", UintegerValue (lowPriorityQueueSize));
    lowQueue = queueFactory.Create<DropTailQueue> ();

    p2p.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue (TXQueueSizeR1));
  }
  else if (queueMode =='b') {
    queueFactory.Set ("Mode", (StringValue) "QUEUE_MODE_BYTES");    

    queueFactory.Set ("MaxBytes", UintegerValue (highPriorityQueueSize));
    highQueue = queueFactory.Create<DropTailQueue> ();

    queueFactory.Set ("MaxBytes", UintegerValue (lowPriorityQueueSize));
    lowQueue = queueFactory.Create<DropTailQueue> ();

    p2p.SetQueue("ns3::DropTailQueue", "MaxBytes", UintegerValue (TXQueueSizeR1));
  }

  spq->SetHighQueue(highQueue);
  spq->SetLowQueue(lowQueue);

  p2p.SetDeviceAttribute ("PriorityQueueModel", (PointerValue) spq);

  p2p.SetChannelAttribute ("Delay", (StringValue) R1R2Delay); 
  p2p.SetDeviceAttribute ("DataRate", (StringValue) R1R2DataRate);
  p2p.SetDeviceAttribute ("Mtu", UintegerValue (R1Mtu));
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

  //Create, configure, and install ProcessingDelaySender
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

  // ugly hacks, fix it later in all 3 modules
  sendHelper.GetSender()->SetLogFileName (outputFile);
  if (backToBackPackets)
    sendHelper.GetSender()->SetIntervalBackToBack (SR1DataRate);
  sender.Start (Seconds (startTime));
  sender.Stop (Seconds (endTime-1));

  // Create, configure, and install ProcessingDelayReceiver
  PriorityQueueReceiverHelper recvHelper;
  ApplicationContainer receiver = recvHelper.Install (R.Get (0));
  recvHelper.GetReceiver()->SetLogFileName (outputFile);
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
