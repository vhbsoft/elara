#ifndef COMPRESSION_HEADER_H
#define COMPRESSION_HEADER_H

#include "ns3/header.h"

namespace ns3 {

class CompressionHeader : public Header 
{
public:

  static TypeId GetTypeId (void);
  CompressionHeader ();
  virtual ~CompressionHeader ();

  uint32_t GetSize (void) const;
 
  void SetSize (uint32_t size);

  void SetData (uint8_t* data, uint32_t size);

  // Writes internal data to buffer pointed by param data, 
  // appication must ensure the buffer is at least param size
  uint32_t GetData (uint8_t* data, uint32_t size) const;

  void SetCompressed (bool compressed);

  void SetUncompSize (uint32_t size);
  uint32_t GetUncompSize (void) const;
  
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
private:
  virtual TypeId GetInstanceTypeId (void) const;

  // true if this header holds, or is meant to hold, compressed data. False otherwise. 
  bool m_compressed;

  // size of the buffer data
  uint32_t m_size;
 
  // size of the buffer data when uncompressed
  uint32_t m_uncompSize;

  // buffer to store data
  uint8_t *m_data;

}; // CompressedHeader

} // namespace ns3

#endif
