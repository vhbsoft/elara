#ifndef COMPRESSION_MODEL_H
#define COMPRESSION_MODEL_H

#include <zlib.h>

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"

namespace ns3 {

class Packet;

class CompressionModel : public Object
{
public:
  static TypeId GetTypeId (void);

  CompressionModel ();
  ~CompressionModel ();

  void Enable (void);
  void Disable (void);
  bool IsEnabled (void) const;

  // Compresses the data in p, returns true if the compression succeeded
  void CompressPacket (Ptr<Packet> p) const;

  // Decompresses the compressed data in p, returns true if the decompression succeeded
  void DecompressPacket (Ptr<Packet> p) const;

  // Compresses the data in srcData to destData, 
  // reads srcSize bytes and writes no more than destSize bytes
  // afterwards destSize will be updated to reflect the compressed size
  bool ZlibCompression (uint8_t *srcData, uint8_t *destData, uint32_t srcSize, uint32_t &destSize) const;

  // Decompresses the data in srcData to destData,
  // reads srcSize bytes and expects destSize to be exactly the original size of the data
  bool ZlibDecompression (uint8_t *srcData, uint8_t *destData, uint32_t srcSize, uint32_t destSize) const;
  
  // Returns the amount of processing delay for this packet
  double GetProcessingTime (Ptr<Packet> p) const; 

  void DelayEnable (void);
  void DelayDisable (void);
  bool DelayIsEnabled(void) const;

  // Change this eventually
  bool m_processNext;
  bool m_isProcessing;
  double m_procDelay;

private:
  
  bool m_enable;
  bool m_delayEnable;
 
}; // CompressionModel

} // namespace ns3

#endif
