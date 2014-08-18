#ifndef COMPRESSION_APP_LAYER_H
#define COMPRESSION_APP_LAYER_H

#include "ns3/header.h"

namespace ns3 {

class CompressionAppLayer : public Header 
{
public:

  static TypeId GetTypeId (void);
  CompressionAppLayer ();
  virtual ~CompressionAppLayer ();

  // Takes a buffer and its size, assumed to be correct
  // and copies to buffer to m_data.  
  void SetData (uint8_t* data, uint32_t size);
  
  // Resizes and writes over param data with m_data
  // returns size of m_data
  uint32_t GetData (uint8_t* data);

  // Returns m_size + 4 bytes
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  virtual void Print (void) const;
  
private:
  virtual TypeId GetInstanceTypeId (void) const;

  uint32_t m_size;

  uint8_t* m_data;

}; // CompressionAppLayer

} // namespace ns3

#endif
