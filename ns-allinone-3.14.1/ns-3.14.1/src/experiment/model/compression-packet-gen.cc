/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <stdio.h>

#include "ns3/packet.h"
#include "ns3/log.h"

#include "compression-packet-gen.h"
#include "compression-app-layer.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CompressionPacketGen");

CompressionPacketGenerator::CompressionPacketGenerator ()
{
  m_entropy = 'l';
  m_altL = true;
  m_dataBuf = NULL;
  m_zeroBuf = NULL;
  m_size = 0;
}

CompressionPacketGenerator::~CompressionPacketGenerator ()
{
  free (m_dataBuf);
  free (m_zeroBuf);
}

uint8_t
CompressionPacketGenerator::SetEntropy (uint8_t entropy)
{
  if (entropy == 'l' || entropy == 'h' || entropy == 'a')
    {
      m_entropy = entropy;
      return 0;
    }
  else
    {
      NS_LOG_UNCOND("invalid entropy, using 'l'");
      return -1;
    }
}

void
CompressionPacketGenerator::SetSize (uint32_t size)
{
  m_size = size;
}

uint8_t
CompressionPacketGenerator::Initialize (void)
{
  uint32_t size = m_size - 4; // Reserve 4 bytes to save the size in appData

  free (m_zeroBuf);
  m_zeroBuf = (uint8_t *) calloc (size, 1);

  m_dataBuf = (uint8_t *) realloc ((void*) m_dataBuf, size);

  FILE *urandom;
  urandom = fopen("/dev/urandom", "r");
  if (urandom == NULL) {
    return -1;
  }
  fread(m_dataBuf, size, 1, urandom);
  fclose(urandom);
  return 0; 
}

Ptr<Packet>
CompressionPacketGenerator::GeneratePacket (void)
{

  uint32_t size = m_size - 4; // Reserve 4 bytes to save the size in appData
  
  CompressionAppLayer appData;

  if (m_entropy == 'a')
    {
      if (m_altL == true)
        {
          appData.SetData(m_zeroBuf, size);
          m_altL = false;
        }
      else
        {
          // Just for one experiment
          FILE *urandom;
          urandom = fopen("/dev/urandom", "r");
          if (urandom == NULL)
            NS_LOG_UNCOND("Could not read /dev/urandom");
          else
            {
              fread(m_dataBuf,size, 1, urandom);
              fclose (urandom);
            }
          appData.SetData(m_dataBuf, size);
          m_altL = true;
        }
    }
  if (m_entropy == 'l')
    {
      appData.SetData(m_zeroBuf, size);
      
    }
  if (m_entropy == 'h')
    {
      // Just for one experiment
      FILE *urandom;
      urandom = fopen("/dev/urandom", "r");
      if (urandom == NULL)
        NS_LOG_UNCOND("Could not read /dev/urandom");
      else
        {
          fread(m_dataBuf,size, 1, urandom);
          fclose (urandom);
        }
      appData.SetData(m_dataBuf, size);
    }  

  // Create a new packet and add the Header
  Ptr<Packet> p = Create<Packet>();

  p->AddHeader (appData);        
  
  // Attach the jitter estimation
  m_jitter.PrepareTx (p);
  
  return p;
}

} // namespace ns3
