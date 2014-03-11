#ifndef EXP_DATA_HEADER_H
#define EXP_DATA_HEADER_H

#include "ns3/header.h"
#include "ns3/nstime.h"

#include <string>

namespace ns3 {

class ExpDataHeader : public Header
{
public:
  ExpDataHeader ();

  void SetNumPackets (uint32_t num);
  void SetInterPacketTime (uint64_t time); 
  void SetProbePacketLen (uint32_t len);

  uint32_t GetNumPackets (void) const;
  uint64_t GetInterPacketTime (void) const;
  uint32_t GetProbePacketLen (void) const;

  void SetInitialNum (uint32_t);
  void SetSeparationNum (uint32_t);
  void SetAptNum (uint32_t);
  void  SetAptPriority (uint8_t);

  uint32_t GetInitialNum (void) const;
  uint32_t GetSeparationNum (void) const;
  uint32_t GetAptNum (void) const;
  uint8_t  GetAptPriority (void) const;

  uint32_t GetResultSize (void) const;

  void SetResults (std::string results);

  std::string GetResults (void) const;

  static TypeId GetTypeId (void);

private:
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

  uint32_t m_num;
  uint64_t m_time;
  uint32_t m_len;

  uint32_t m_initialNum;
  uint32_t m_separationNum;
  uint32_t m_aptNum;
  uint8_t m_aptPriority;

  std::string m_results;
};

} // namespace ns3

#endif /* EXP_DATA_HEADER_H */
