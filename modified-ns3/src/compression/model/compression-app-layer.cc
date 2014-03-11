/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/header.h"
#include "ns3/log.h"

#include "compression-app-layer.h"
#include <iostream>
#include <stdio.h>

NS_LOG_COMPONENT_DEFINE ("CompressionAppLayer");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CompressionAppLayer);

TypeId
CompressionAppLayer::GetTypeId (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  static TypeId tid = TypeId ("ns3::CompressionAppLayer")
    .SetParent<Header> ()
    .AddConstructor<CompressionAppLayer> ()
  ;
  return tid;
}

TypeId
CompressionAppLayer::GetInstanceTypeId (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return GetTypeId ();
}

CompressionAppLayer::CompressionAppLayer ()
  : m_size (0),
    m_data (NULL)
{
  NS_LOG_FUNCTION_NOARGS ();
}

CompressionAppLayer::~CompressionAppLayer ()
{
  NS_LOG_FUNCTION_NOARGS ();
  free (m_data);
}

void
CompressionAppLayer::SetData (uint8_t* data, uint32_t size)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_size = size;
  m_data = (uint8_t*) realloc(m_data, size);
  memcpy(m_data, data, size);
}

uint32_t
CompressionAppLayer::GetData (uint8_t*data)
{
  NS_LOG_FUNCTION_NOARGS ();
  data = (uint8_t*) realloc(data, m_size);
  memcpy(data, m_data, m_size);
  return m_size;
}

uint32_t
CompressionAppLayer::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_size + 4;
}

void
CompressionAppLayer::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();

  Buffer::Iterator it = start; 
  it.WriteU32 (m_size);
  it.Write (m_data, m_size);
}

uint32_t
CompressionAppLayer::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();

  Buffer::Iterator it = start;
  m_size = it.ReadU32 ();
  m_data = (uint8_t*) realloc(m_data, m_size);
  it.Read (m_data, m_size);
    
  return GetSerializedSize ();
}

void
CompressionAppLayer::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION_NOARGS ();

  os << "size=" << m_size << std::endl;
}

void
CompressionAppLayer::Print (void) const
{
  printf("buffer: \n\n%.*s\n", m_size, m_data);
}

} //namespace ns3
