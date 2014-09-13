#ifndef CLICK_DECOMPRESSION_HH
#define CLICK_DECOMPRESSION_HH
#include <click/element.hh>

#define MAX_PACKET_SIZE 32768

//
// This element takes a packet and decompresses its entire contents
//
CLICK_DECLS
class Decompression : public Element
{
public:
  Decompression();
  ~Decompression();

  const char *class_name() const {return "Decompression";}
  const char *port_count() const {return "1/2";}
  const char *processing() const {return AGNOSTIC;}

  void push(int, Packet *);

  uint32_t ZlibDecompression (uint8_t *srcData, uint8_t *destData, uint16_t srcSize, uint16_t &destSize);

private:
  uint8_t m_buf[MAX_PACKET_SIZE];
};
CLICK_ENDDECLS
#endif
