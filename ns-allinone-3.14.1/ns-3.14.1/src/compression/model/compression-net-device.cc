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

#include "ns3/log.h"
#include "ns3/queue.h"
#include "ns3/simulator.h"
#include "ns3/mac48-address.h"
#include "ns3/llc-snap-header.h"
#include "ns3/error-model.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"
#include "ns3/pointer.h"
#include "ns3/boolean.h"
#include "ns3/mpi-interface.h"
#include "compression-net-device.h"
#include "compression-channel.h"
#include "compression-header.h"
#include "ns3/ppp-header.h"

NS_LOG_COMPONENT_DEFINE ("CompressionNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CompressionNetDevice);

TypeId 
CompressionNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CompressionNetDevice")
    .SetParent<NetDevice> ()
    .AddConstructor<CompressionNetDevice> ()
    .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                   UintegerValue (DEFAULT_MTU),
                   MakeUintegerAccessor (&CompressionNetDevice::SetMtu,
                                         &CompressionNetDevice::GetMtu),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("Address", 
                   "The MAC address of this device.",
                   Mac48AddressValue (Mac48Address ("ff:ff:ff:ff:ff:ff")),
                   MakeMac48AddressAccessor (&CompressionNetDevice::m_address),
                   MakeMac48AddressChecker ())
    .AddAttribute ("DataRate", 
                   "The default data rate for point to point links",
                   DataRateValue (DataRate ("32768b/s")),
                   MakeDataRateAccessor (&CompressionNetDevice::m_bps),
                   MakeDataRateChecker ())
    .AddAttribute ("ReceiveErrorModel", 
                   "The receiver error model used to simulate packet loss",
                   PointerValue (),
                   MakePointerAccessor (&CompressionNetDevice::m_receiveErrorModel),
                   MakePointerChecker<ErrorModel> ())
    .AddAttribute ("InterframeGap", 
                   "The time to wait between packet (frame) transmissions",
                   TimeValue (Seconds (0.0)),
                   MakeTimeAccessor (&CompressionNetDevice::m_tInterframeGap),
                   MakeTimeChecker ())

    //
    // Transmit queueing discipline for the device which includes its own set
    // of trace hooks.
    //
    .AddAttribute ("TxQueue", 
                   "A queue to use as the transmit queue in the device.",
                   PointerValue (),
                   MakePointerAccessor (&CompressionNetDevice::m_queue),
                   MakePointerChecker<Queue> ())

    //
    // Trace sources at the "top" of the net device, where packets transition
    // to/from higher layers.
    //
    .AddTraceSource ("MacTx", 
                     "Trace source indicating a packet has arrived for transmission by this device",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_macTxTrace))
    .AddTraceSource ("MacTxDrop", 
                     "Trace source indicating a packet has been dropped by the device before transmission",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_macTxDropTrace))
    .AddTraceSource ("MacPromiscRx", 
                     "A packet has been received by this device, has been passed up from the physical layer "
                     "and is being forwarded up the local protocol stack.  This is a promiscuous trace,",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_macPromiscRxTrace))
    .AddTraceSource ("MacRx", 
                     "A packet has been received by this device, has been passed up from the physical layer "
                     "and is being forwarded up the local protocol stack.  This is a non-promiscuous trace,",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_macRxTrace))
#if 0
    // Not currently implemented for this device
    .AddTraceSource ("MacRxDrop", 
                     "Trace source indicating a packet was dropped before being forwarded up the stack",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_macRxDropTrace))
#endif
    //
    // Trace souces at the "bottom" of the net device, where packets transition
    // to/from the channel.
    //
    .AddTraceSource ("PhyTxBegin", 
                     "Trace source indicating a packet has begun transmitting over the channel",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_phyTxBeginTrace))
    .AddTraceSource ("PhyTxEnd", 
                     "Trace source indicating a packet has been completely transmitted over the channel",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_phyTxEndTrace))
    .AddTraceSource ("PhyTxDrop", 
                     "Trace source indicating a packet has been dropped by the device during transmission",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_phyTxDropTrace))
#if 0
    // Not currently implemented for this device
    .AddTraceSource ("PhyRxBegin", 
                     "Trace source indicating a packet has begun being received by the device",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_phyRxBeginTrace))
#endif
    .AddTraceSource ("PhyRxEnd", 
                     "Trace source indicating a packet has been completely received by the device",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_phyRxEndTrace))
    .AddTraceSource ("PhyRxDrop", 
                     "Trace source indicating a packet has been dropped by the device during reception",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_phyRxDropTrace))

    //
    // Trace sources designed to simulate a packet sniffer facility (tcpdump).
    // Note that there is really no difference between promiscuous and 
    // non-promiscuous traces in a point-to-point link.
    //
    .AddTraceSource ("Sniffer", 
                     "Trace source simulating a non-promiscuous packet sniffer attached to the device",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_snifferTrace))
    .AddTraceSource ("PromiscSniffer", 
                     "Trace source simulating a promiscuous packet sniffer attached to the device",
                     MakeTraceSourceAccessor (&CompressionNetDevice::m_promiscSnifferTrace))

    //
    // TODO: Add comments
    // 
    //
    .AddAttribute ("EnableCompression",
                   "Whether this device will compress / decompress packets",
                   BooleanValue (true),
                   MakeBooleanAccessor (&CompressionNetDevice::SetEnableCompression,
                                        &CompressionNetDevice::GetEnableCompression),
                   MakeBooleanChecker())
  ;
  return tid;
}


CompressionNetDevice::CompressionNetDevice () 
  :
    m_txMachineState (READY),
    m_channel (0),
    m_linkUp (false),
    m_currentPkt (0),
    m_enableCompression (true)
{
  NS_LOG_FUNCTION (this);
}

CompressionNetDevice::~CompressionNetDevice ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
CompressionNetDevice::AddHeader (Ptr<Packet> p, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION_NOARGS ();
  PppHeader ppp;
  ppp.SetProtocol (EtherToPpp (protocolNumber));
  p->AddHeader (ppp);
}

bool
CompressionNetDevice::ProcessHeader (Ptr<Packet> p, uint16_t& param)
{
  NS_LOG_FUNCTION_NOARGS ();
  PppHeader ppp;
  p->RemoveHeader (ppp);
  param = PppToEther (ppp.GetProtocol ());
  return true;
}

void
CompressionNetDevice::DoDispose ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_node = 0;
  m_channel = 0;
  m_receiveErrorModel = 0;
  m_currentPkt = 0;
  NetDevice::DoDispose ();
}

void
CompressionNetDevice::SetDataRate (DataRate bps)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_bps = bps;
}

void
CompressionNetDevice::SetInterframeGap (Time t)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_tInterframeGap = t;
}

bool
CompressionNetDevice::TransmitStart (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this << p);
  NS_LOG_LOGIC ("UID is " << p->GetUid () << ")");

  //
  // This function is called to start the process of transmitting a packet.
  // We need to tell the channel that we've started wiggling the wire and
  // schedule an event that will be executed when the transmission is complete.
  //
  NS_ASSERT_MSG (m_txMachineState == READY, "Must be READY to transmit");
  m_txMachineState = BUSY;
  m_currentPkt = p;
  m_phyTxBeginTrace (m_currentPkt);

  Time txTime = Seconds (m_bps.CalculateTxTime (p->GetSize ()));
  Time txCompleteTime = txTime + m_tInterframeGap;

  NS_LOG_LOGIC ("Schedule TransmitCompleteEvent in " << txCompleteTime.GetSeconds () << "sec");
  Simulator::Schedule (txCompleteTime, &CompressionNetDevice::TransmitComplete, this);

  bool result = m_channel->TransmitStart (p, this, txTime);
  if (result == false)
    {
      m_phyTxDropTrace (p);
    }
  return result;
}

void
CompressionNetDevice::TransmitComplete (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  //
  // This function is called to when we're all done transmitting a packet.
  // We try and pull another packet off of the transmit queue.  If the queue
  // is empty, we are done, otherwise we need to start transmitting the
  // next packet.
  //
  NS_ASSERT_MSG (m_txMachineState == BUSY, "Must be BUSY if transmitting");
  m_txMachineState = READY;

  NS_ASSERT_MSG (m_currentPkt != 0, "CompressionNetDevice::TransmitComplete(): m_currentPkt zero");

  m_phyTxEndTrace (m_currentPkt);
  m_currentPkt = 0;

  Ptr<Packet> p = m_queue->Dequeue ();
  if (p == 0)
    {
      //
      // No packet was on the queue, so we just exit.
      //
      return;
    }

  //
  // Got another packet off of the queue, so start the transmit process agin.
  //
  m_snifferTrace (p);
  m_promiscSnifferTrace (p);
  TransmitStart (p);
}

bool
CompressionNetDevice::Attach (Ptr<CompressionChannel> ch)
{
  NS_LOG_FUNCTION (this << &ch);

  m_channel = ch;

  m_channel->Attach (this);

  //
  // This device is up whenever it is attached to a channel.  A better plan
  // would be to have the link come up when both devices are attached, but this
  // is not done for now.
  //
  NotifyLinkUp ();
  return true;
}

void
CompressionNetDevice::SetQueue (Ptr<Queue> q)
{
  NS_LOG_FUNCTION (this << q);
  m_queue = q;
}

void
CompressionNetDevice::SetReceiveErrorModel (Ptr<ErrorModel> em)
{
  NS_LOG_FUNCTION (this << em);
  m_receiveErrorModel = em;
}

void
CompressionNetDevice::Receive (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);
  uint16_t protocol = 0;

  if (m_receiveErrorModel && m_receiveErrorModel->IsCorrupt (packet) ) 
    {
      // 
      // If we have an error model and it indicates that it is time to lose a
      // corrupted packet, don't forward this packet up, let it go.
      //
      m_phyRxDropTrace (packet);
    }
  else 
    {
      // 
      // Hit the trace hooks.  All of these hooks are in the same place in this 
      // device becuase it is so simple, but this is not usually the case in 
      // more complicated devices.
      //
      m_snifferTrace (packet);
      m_promiscSnifferTrace (packet);
      m_phyRxEndTrace (packet);

      //
      // Strip off the point-to-point protocol header and forward this packet
      // up the protocol stack.  Since this is a simple point-to-point link,
      // there is no difference in what the promisc callback sees and what the
      // normal receive callback sees.
      //
      ProcessHeader (packet, protocol);

      //
      // TODO: Add comments
      // 
      //
      if (m_enableCompression)
        {
          // DecompressPacket
          NS_LOG_INFO ("Decompressing Packet");
          DecompressPacket(packet);
        }

      if (!m_promiscCallback.IsNull ())
        {
          m_macPromiscRxTrace (packet);
          m_promiscCallback (this, packet, protocol, GetRemote (), GetAddress (), NetDevice::PACKET_HOST);
        }

      m_macRxTrace (packet);
      m_rxCallback (this, packet, protocol, GetRemote ());
    }
}

Ptr<Queue>
CompressionNetDevice::GetQueue (void) const
{ 
  NS_LOG_FUNCTION_NOARGS ();
  return m_queue;
}

void
CompressionNetDevice::NotifyLinkUp (void)
{
  m_linkUp = true;
  m_linkChangeCallbacks ();
}

void
CompressionNetDevice::SetIfIndex (const uint32_t index)
{
  m_ifIndex = index;
}

uint32_t
CompressionNetDevice::GetIfIndex (void) const
{
  return m_ifIndex;
}

Ptr<Channel>
CompressionNetDevice::GetChannel (void) const
{
  return m_channel;
}

//
// This is a point-to-point device, so we really don't need any kind of address
// information.  However, the base class NetDevice wants us to define the
// methods to get and set the address.  Rather than be rude and assert, we let
// clients get and set the address, but simply ignore them.

void
CompressionNetDevice::SetAddress (Address address)
{
  m_address = Mac48Address::ConvertFrom (address);
}

Address
CompressionNetDevice::GetAddress (void) const
{
  return m_address;
}

bool
CompressionNetDevice::IsLinkUp (void) const
{
  return m_linkUp;
}

void
CompressionNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  m_linkChangeCallbacks.ConnectWithoutContext (callback);
}

//
// This is a point-to-point device, so every transmission is a broadcast to
// all of the devices on the network.
//
bool
CompressionNetDevice::IsBroadcast (void) const
{
  return true;
}

//
// We don't really need any addressing information since this is a 
// point-to-point device.  The base class NetDevice wants us to return a
// broadcast address, so we make up something reasonable.
//
Address
CompressionNetDevice::GetBroadcast (void) const
{
  return Mac48Address ("ff:ff:ff:ff:ff:ff");
}

bool
CompressionNetDevice::IsMulticast (void) const
{
  return true;
}

Address
CompressionNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  return Mac48Address ("01:00:5e:00:00:00");
}

Address
CompressionNetDevice::GetMulticast (Ipv6Address addr) const
{
  NS_LOG_FUNCTION (this << addr);
  return Mac48Address ("33:33:00:00:00:00");
}

bool
CompressionNetDevice::IsPointToPoint (void) const
{
  return true;
}

bool
CompressionNetDevice::IsBridge (void) const
{
  return false;
}

bool
CompressionNetDevice::Send (
  Ptr<Packet> packet, 
  const Address &dest, 
  uint16_t protocolNumber)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_LOG_LOGIC ("p=" << packet << ", dest=" << &dest);
  NS_LOG_LOGIC ("UID is " << packet->GetUid ());

  //
  // If IsLinkUp() is false it means there is no channel to send any packet 
  // over so we just hit the drop trace on the packet and return an error.
  //
  if (IsLinkUp () == false)
    {
      m_macTxDropTrace (packet);
      return false;
    }

  //
  // TODO: Add comments
  //
  //
  if (m_enableCompression)
    {
      // Compress Packet
      NS_LOG_INFO ("Compressing Packet");
      CompressPacket(packet);
    }

  //
  // Stick a point to point protocol header on the packet in preparation for
  // shoving it out the door.
  //
  AddHeader (packet, protocolNumber);

  m_macTxTrace (packet);

  //
  // If there's a transmission in progress, we enque the packet for later
  // transmission; otherwise we send it now.
  //
  if (m_txMachineState == READY) 
    {
      // 
      // Even if the transmitter is immediately available, we still enqueue and
      // dequeue the packet to hit the tracing hooks.
      //
      if (m_queue->Enqueue (packet) == true)
        {
          packet = m_queue->Dequeue ();
          m_snifferTrace (packet);
          m_promiscSnifferTrace (packet);
          return TransmitStart (packet);
        }
      else
        {
          // Enqueue may fail (overflow)
          m_macTxDropTrace (packet);
          return false;
        }
    }
  else
    {
      return m_queue->Enqueue (packet);
    }
}

bool
CompressionNetDevice::SendFrom (Ptr<Packet> packet, 
                                 const Address &source, 
                                 const Address &dest, 
                                 uint16_t protocolNumber)
{
  return false;
}

Ptr<Node>
CompressionNetDevice::GetNode (void) const
{
  return m_node;
}

void
CompressionNetDevice::SetNode (Ptr<Node> node)
{
  m_node = node;
}

bool
CompressionNetDevice::NeedsArp (void) const
{
  return false;
}

void
CompressionNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
  m_rxCallback = cb;
}

void
CompressionNetDevice::SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb)
{
  m_promiscCallback = cb;
}

bool
CompressionNetDevice::SupportsSendFrom (void) const
{
  return false;
}

void
CompressionNetDevice::DoMpiReceive (Ptr<Packet> p)
{
  Receive (p);
}

Address 
CompressionNetDevice::GetRemote (void) const
{
  NS_ASSERT (m_channel->GetNDevices () == 2);
  for (uint32_t i = 0; i < m_channel->GetNDevices (); ++i)
    {
      Ptr<NetDevice> tmp = m_channel->GetDevice (i);
      if (tmp != this)
        {
          return tmp->GetAddress ();
        }
    }
  NS_ASSERT (false);
  // quiet compiler.
  return Address ();
}

bool
CompressionNetDevice::SetMtu (uint16_t mtu)
{
  NS_LOG_FUNCTION (this << mtu);
  m_mtu = mtu;
  return true;
}

uint16_t
CompressionNetDevice::GetMtu (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_mtu;
}

uint16_t
CompressionNetDevice::PppToEther (uint16_t proto)
{
  switch(proto)
    {
    case 0x0021: return 0x0800;   //IPv4
    case 0x0057: return 0x86DD;   //IPv6
    default: NS_ASSERT_MSG (false, "PPP Protocol number not defined!");
    }
  return 0;
}

uint16_t
CompressionNetDevice::EtherToPpp (uint16_t proto)
{
  switch(proto)
    {
    case 0x0800: return 0x0021;   //IPv4
    case 0x86DD: return 0x0057;   //IPv6
    default: NS_ASSERT_MSG (false, "PPP Protocol number not defined!");
    }
  return 0;
}

void
CompressionNetDevice::SetEnableCompression(bool enable)
{
  m_enableCompression = enable;
}

bool
CompressionNetDevice::GetEnableCompression(void) const
{
  return m_enableCompression;
}

void
CompressionNetDevice::CompressPacket (Ptr<Packet> p)
{
  NS_LOG_FUNCTION_NOARGS ();
  uint32_t srcSize = p->GetSize();

  if (srcSize > MAX_PACKET_SIZE)
    {
      NS_LOG_UNCOND ("Packet is too large for compression!\nMAX_PACKET_SIZE = " << MAX_PACKET_SIZE 
                     << "\npacket size = " << srcSize 
                     << "\nChange MAX_PACKET_SIZE in compression-net-device.h");
    }

  NS_LOG_INFO ("Received " << srcSize << " bytes for compression UID: " << p->GetUid());
  NS_LOG_UNCOND ("Received " << srcSize << " bytes for compression UID: " << p->GetUid());

  CompressionHeader compHead;
  compHead.SetSize(srcSize);
  p->RemoveHeader (compHead);

  uint8_t *srcData = m_srcData;
  compHead.GetData (srcData, srcSize);
  uint32_t destSize = MAX_PACKET_SIZE;
  uint8_t *destData = m_dstData;
  destSize = ZlibCompression (srcData, destData, srcSize, destSize);

  compHead.SetData (destData, destSize);
  p->AddHeader (compHead);

  NS_LOG_INFO("Compressed data to " << destSize << " bytes");
  NS_LOG_UNCOND("Compressed data to " << destSize << " bytes");

  return;
}

void
CompressionNetDevice::DecompressPacket (Ptr<Packet> p)
{
  NS_LOG_FUNCTION_NOARGS ();
  uint32_t srcSize = p->GetSize();

  if (srcSize > MAX_PACKET_SIZE)
    {
      NS_LOG_UNCOND ("Packet is too large for decompression!\nMAX_PACKET_SIZE = " << MAX_PACKET_SIZE 
                     << "\npacket size = " << srcSize 
                     << "\nChange MAX_PACKET_SIZE in compression-net-device.h");
    }

  NS_LOG_INFO ("Received " << srcSize << " bytes for decompression UID: " << p->GetUid());
  NS_LOG_UNCOND ("Received " << srcSize << " bytes for decompression UID: " << p->GetUid());

  CompressionHeader compHead;
  compHead.SetSize (srcSize);
  p->RemoveHeader (compHead);

  uint8_t *srcData = m_srcData;
  compHead.GetData (srcData, srcSize);
  uint32_t destSize = MAX_PACKET_SIZE;
  uint8_t *destData = m_dstData;
  destSize = ZlibDecompression (srcData, destData, srcSize, destSize);

  compHead.SetData (destData, destSize);
  p->AddHeader (compHead);

  NS_LOG_INFO ("Decompressed data to " << destSize << " bytes");
  NS_LOG_UNCOND ("Decompressed data to " << destSize << " bytes");

  return;
}

uint32_t
CompressionNetDevice::ZlibCompression (uint8_t *srcData, uint8_t *destData, uint32_t srcSize, uint32_t destSize) const
{
  z_stream strm;
  strm.zalloc = 0;
  strm.zfree = 0;
  strm.next_in = srcData;
  strm.avail_in = srcSize;
  strm.next_out = destData;
  strm.avail_out = destSize;
  deflateInit2(&strm,
               Z_BEST_COMPRESSION,
               Z_DEFLATED,
               15,
               8,
               Z_FIXED);
  while (strm.avail_in != 0)
    {
      int res = deflate(&strm, Z_NO_FLUSH);
      NS_ASSERT(res == Z_OK);
      if (strm.avail_out == 0)
        {
          NS_LOG_UNCOND ("Not enough room in packet buffer!\n"
                         << "Increase MAX_PACKET_SIZE in compression-net-device.h");
        } 
    }
  int deflate_res = Z_OK;
  while (deflate_res == Z_OK)
    {
      deflate_res = deflate(&strm, Z_FINISH);
    }
  NS_ASSERT(deflate_res == Z_STREAM_END);
  destSize = strm.total_out;
  deflateEnd(&strm);
  return destSize;
}

uint32_t
CompressionNetDevice::ZlibDecompression (uint8_t *srcData, uint8_t *destData, uint32_t srcSize, uint32_t destSize) const
{
  z_stream strm;
  strm.zalloc = 0;
  strm.zfree = 0;
  strm.next_in = srcData;
  strm.avail_in = srcSize;
  strm.next_out = destData;
  strm.avail_out = destSize;
  inflateInit2(&strm, 15);
  int inflate_res = inflate(&strm, Z_FINISH);
  if (strm.avail_out == 0)
    {
      NS_LOG_UNCOND ("Not enough room in packet buffer!\n"
                     << "Increase MAX_PACKET_SIZE in compression-net-device.h");
    } 
  NS_ASSERT(inflate_res == Z_STREAM_END);
  destSize = strm.total_out;
  inflateEnd(&strm);
  return destSize;
}

} // namespace ns3
