#ifndef CLICK_TCPCOMPRESSION_HH
#define CLICK_TCPCOMPRESSION_HH
#include <click/element.hh>
#include <zlib.h>

#define MAX_PACKET_SIZE 10000

//
// This element takes an IP packet and compresses everything after the tcp header
//
CLICK_DECLS
class TcpCompression : public Element
{
public:
  TcpCompression();
  ~TcpCompression();

  const char *class_name() const {return "TcpCompression";}
  const char *port_count() const {return "1/2";}
  const char *processing() const {return PUSH;}

  void push(int, Packet *);

  bool ZlibCompression (uint8_t *srcData, uint8_t *destData, uint16_t srcSize, uint16_t &destSize);

private:
  uint8_t m_buf[MAX_PACKET_SIZE];
};
CLICK_ENDDECLS
#endif
