#ifndef CLICK_UDPDECOMPRESSION_HH
#define CLICK_UDPDECOMPRESSION_HH
#include <click/element.hh>
#include <zlib.h>

#define MAX_PACKET_SIZE 10000

//
// This element takes an IP packet and decompresses everything after the tcp header
//
CLICK_DECLS
class UdpDecompression : public Element
{
public:
  UdpDecompression();
  ~UdpDecompression();

  const char *class_name() const {return "UdpDecompression";}
  const char *port_count() const {return "1/2";}
  const char *processing() const {return AGNOSTIC;}

  void push(int, Packet *);

  bool ZlibDecompression (uint8_t *srcData, uint8_t *destData, uint16_t srcSize, uint16_t &destSize);

private:
  uint8_t buf[MAX_PACKET_SIZE];
};
CLICK_ENDDECLS
#endif
