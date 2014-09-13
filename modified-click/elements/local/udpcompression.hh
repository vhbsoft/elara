#ifndef CLICK_UDPCOMPRESSION_HH
#define CLICK_UDPCOMPRESSION_HH
#include <click/element.hh>
#include <zlib.h>

#define MAX_PACKET_SIZE 10000

//
// This element takes an IP packet and compresses everything after the udp header
//
CLICK_DECLS
class UdpCompression : public Element
{
public:
  UdpCompression();
  ~UdpCompression();

  const char *class_name() const {return "UdpCompression";}
  const char *port_count() const {return "1/2";}
  const char *processing() const {return PUSH;}

  void push(int, Packet *);

  bool ZlibCompression (uint8_t *srcData, uint8_t *destData, uint16_t srcSize, uint16_t &destSize);

private:
  uint8_t m_buf[MAX_PACKET_SIZE];
};
CLICK_ENDDECLS
#endif
