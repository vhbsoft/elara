/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <assert.h>
#include <zlib.h>
#include <iostream>

#include "compression-model.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/compression-header.h"

#include "ns3/header.h"
#include "ns3/ipv4-header.h"
#include "ns3/udp-header.h"
#include "ns3/seq-ts-header.h"
//#include "ns3/compression-app-layer.h"

NS_LOG_COMPONENT_DEFINE ("CompressionModel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CompressionModel);

TypeId 
CompressionModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CompressionModel")
    .SetParent<Object> ()
    .AddConstructor<CompressionModel> ()
    .AddAttribute ("isEnabled", "Whether this CompressionModel is enabled or not.",
		   BooleanValue (true),
		   MakeBooleanAccessor (&CompressionModel::m_enable),
		   MakeBooleanChecker ())
    .AddAttribute ("isDelayEnabled", "Whether this ProcessingDelayModel is enabled or not.",
		   BooleanValue (false),
		   MakeBooleanAccessor (&CompressionModel::m_delayEnable),
		   MakeBooleanChecker ())
 ;
  return tid;
}

CompressionModel::CompressionModel ()
  :   m_processNext (true),
      m_isProcessing (false),
      m_enable (true), 
      m_delayEnable (false)
{
  NS_LOG_FUNCTION_NOARGS ();
}

CompressionModel::~CompressionModel () 
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
CompressionModel::Enable (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_enable = true;
}

void
CompressionModel::Disable (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_enable = false;
}

bool
CompressionModel::IsEnabled (void) const
{
  return m_enable;
}

void
CompressionModel::CompressPacket (Ptr<Packet> p) const
{
  NS_LOG_FUNCTION_NOARGS ();

  Ipv4Header peek;
  p->PeekHeader(peek);

  uint32_t decompSize = p->GetSize();

  NS_LOG_INFO ("Received " << decompSize << " bytes for compression UID: " << p->GetUid()
               << " destination: " << peek.GetDestination());

  // Compress the packet

  CompressionHeader compHead;
  compHead.SetSize(decompSize);
  compHead.SetCompressed(false);

  p->RemoveHeader (compHead);
  
  uint32_t srcSize = compHead.GetSize();
  uint8_t *srcData = (uint8_t*) malloc(srcSize);
  compHead.GetData (srcData, srcSize);
  
  uint32_t destSize = srcSize*2;
  uint8_t *destData = (uint8_t*) malloc(destSize);  
  
  ZlibCompression (srcData, destData, srcSize, destSize);
   
  compHead.SetCompressed(true);
  compHead.SetUncompSize (srcSize);
  compHead.SetData (destData, destSize);

  p->AddHeader (compHead);

  NS_LOG_INFO("Compression to " << p->GetSize() << " bytes");
  
  free (srcData);
  free (destData);
  return;
}

void
CompressionModel::DecompressPacket (Ptr<Packet> p) const
{
  NS_LOG_FUNCTION_NOARGS ();

  uint32_t compSize = p->GetSize() - 4;

  // Remove compression header
  CompressionHeader compHead;
  compHead.SetSize (compSize);
  compHead.SetCompressed(true);

  p->RemoveHeader (compHead);
  
  uint32_t srcSize = compHead.GetSize ();
  uint8_t *srcData = (uint8_t*) malloc(srcSize);
  compHead.GetData (srcData, srcSize); 

  uint32_t destSize = compHead.GetUncompSize ();
  uint8_t *destData = (uint8_t*) malloc(destSize);
  
  ZlibDecompression (srcData, destData, srcSize, destSize);

  compHead.SetCompressed(false);
  compHead.SetSize (destSize);
  compHead.SetData (destData, destSize);
  
  p->AddHeader (compHead);

  free (srcData);
  free (destData);
  NS_LOG_INFO ("Decompressed packet from " << compSize + 4 << " to " 
                << p->GetSize() << " bytes UID: " << p->GetUid());
  return;
}

bool
CompressionModel::ZlibCompression (uint8_t *srcData, uint8_t *destData, uint32_t srcSize, uint32_t &destSize) const
{
  //NS_LOG_UNCOND ("CompressionModel::ZlibCompression recieved " << srcSize << " bytes.");

  //NS_LOG_UNCOND ("CompressionModel::ZlibCompression building z_stream");  

  z_stream strm;
  strm.zalloc = 0;
  strm.zfree = 0;
  strm.next_in = srcData;
  strm.avail_in = srcSize;
  strm.next_out = destData;
  strm.avail_out = destSize;

  deflateInit(&strm, Z_BEST_COMPRESSION);
  
  while (strm.avail_in != 0) 
  {
    //NS_LOG_UNCOND ("CompressionModel::ZlibCompression calling deflate() no flush");
    int res = deflate(&strm, Z_NO_FLUSH);
    //NS_LOG_UNCOND ("CompressionModel::ZlibCompression deflate() no flush sucess");
    assert(res == Z_OK);
  }

  int deflate_res = Z_OK;
  while (deflate_res == Z_OK)
  {
    //NS_LOG_UNCOND ("CompressionModel::ZlibCompression calling deflate() finish");
    deflate_res = deflate(&strm, Z_FINISH);
  }
  assert(deflate_res == Z_STREAM_END);

  destSize = strm.total_out;

  //NS_LOG_UNCOND ("CompressionModel::ZlibCompression calling deflateEnd()");  


  deflateEnd(&strm);

  return true;
}

bool
CompressionModel::ZlibDecompression (uint8_t *srcData, uint8_t *destData, uint32_t srcSize, uint32_t destSize) const
{

  z_stream strm;
  strm.zalloc = 0;
  strm.zfree = 0;
  strm.next_in = srcData;
  strm.avail_in = srcSize;
  strm.next_out = destData;
  strm.avail_out = destSize;

  inflateInit(&strm);

  int inflate_res = inflate(&strm, Z_FINISH);
  assert(inflate_res == Z_STREAM_END);  
  
  inflateEnd(&strm);

  return true;
}

void
CompressionModel::DelayEnable (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_delayEnable = true;
}

void
CompressionModel::DelayDisable (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_delayEnable = false;
}

bool
CompressionModel::DelayIsEnabled (void) const
{
  return m_delayEnable;
}
double
CompressionModel::GetProcessingTime (Ptr<Packet> p) const
{
  return m_procDelay;
}

} //namespace ns3
