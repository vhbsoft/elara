/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef COMPRESSION_PACKET_GEN_H
#define COMPRESSION_PACKET_GEN_H

#include "ns3/delay-jitter-estimation.h"

namespace ns3 {

class Packet;
class CompressionAppLayer;

class CompressionPacketGenerator
{
public:
  CompressionPacketGenerator ();
  ~CompressionPacketGenerator ();

  void SetSize (uint32_t size);
  uint8_t SetEntropy (uint8_t entropy);

  // Updates buffers to new data of m_size - 4
  uint8_t Initialize (void);

  // Returns the Ptr to a packet generated according to preset specifications
  Ptr<Packet> GeneratePacket (void);
    
private:

//
// Private variables
//

  // Buffer for high entropy data
  uint8_t *m_dataBuf;

  // Buffer for low entropy data
  uint8_t *m_zeroBuf;
  
  // Size of generated packets
  uint32_t m_size;

  // Specifies the type of packets generated. 
  // 'l' = Low
  // 'h' = High
  // 'a' = Alternating
  uint8_t m_entropy;
  
  // Allows for alternating between low / high entropy packets
  bool m_altL;
  
  // Simulates jitter
  DelayJitterEstimation m_jitter;

}; // CompressionPacketGenerator

} // namespace ns3

#endif
