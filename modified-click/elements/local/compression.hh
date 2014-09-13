#ifndef CLICK_COMPRESSION_HH
#define CLICK_COMPRESSION_HH
#include <click/element.hh>

#define MAX_PACKET_SIZE 32768

//
// This element takes a packet and compresses its entire contents
//
CLICK_DECLS
class Compression : public Element
{
public:
  Compression();
  ~Compression();

  const char *class_name() const {return "Compression";}
  const char *port_count() const {return "1/2";}
  const char *processing() const {return PUSH;}

  void push(int, Packet *);

  uint32_t ZlibCompression (uint8_t *srcData, uint8_t *destData, uint16_t srcSize, uint16_t &destSize);

private:
  uint8_t m_buf[MAX_PACKET_SIZE];
};

CLICK_ENDDECLS
#endif
