/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/header.h"

#include "compression-header.h"
#include <iostream>

NS_LOG_COMPONENT_DEFINE ("CompressionHeader");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CompressionHeader);

TypeId
CompressionHeader::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  static TypeId tid = TypeId ("ns3::CompressionHeader")
    .SetParent<Header> ()
    .AddConstructor<CompressionHeader> ()
  ;
  return tid;
}

TypeId
CompressionHeader::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return GetTypeId ();
}

CompressionHeader::CompressionHeader ()
  : m_compressed (false),
    m_size (0),
    m_uncompSize (0),
    m_data (NULL) 
{
  NS_LOG_FUNCTION_NOARGS ();
}

CompressionHeader::~CompressionHeader ()
{
  NS_LOG_FUNCTION_NOARGS ();
  free (m_data);
}

uint32_t
CompressionHeader::GetSize (void) const
{
  return m_size;
}

void
CompressionHeader::SetSize (uint32_t size)
{
  m_size = size;
}

void
CompressionHeader::SetData (uint8_t* data, uint32_t size)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_size = size;
  m_data = (uint8_t*) realloc(m_data, size);
  memcpy(m_data, data, size);
}

uint32_t
CompressionHeader::GetData (uint8_t* data, uint32_t size) const
{
  NS_LOG_FUNCTION_NOARGS ();
  if (size < m_size) {
    NS_LOG_UNCOND ("CompressionHeader::GetData - buffer too small");
    return -1;
  }
  memcpy(data, m_data, m_size);
  return m_size;
}

void
CompressionHeader::SetCompressed (bool compressed)
{
  m_compressed = compressed;
}

void 
CompressionHeader::SetUncompSize (uint32_t size)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_uncompSize = size;
}

uint32_t 
CompressionHeader::GetUncompSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_uncompSize;
}
  
uint32_t
CompressionHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_compressed)
    return m_size + 4;
  return m_size;
}

void
CompressionHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();
  Buffer::Iterator it = start;
  if (m_compressed)
    it.WriteU32 (m_uncompSize);
  it.Write (m_data, m_size);
}

uint32_t
CompressionHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();
  Buffer::Iterator it = start;
  if (m_compressed) 
    m_uncompSize = it.ReadU32 ();
  m_data = (uint8_t*) realloc(m_data, m_size);
  it.Read (m_data, m_size);
  return GetSerializedSize ();
}

void
CompressionHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION_NOARGS ();
  os << "uncompSize=" << m_uncompSize << std::endl;
  os << "size=" << m_size << std::endl;
  os << "data cannot be displayed\n";
}

} //namespace ns3
