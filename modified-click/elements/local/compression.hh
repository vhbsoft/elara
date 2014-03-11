#ifndef CLICK_COMPRESSION_HH
#define CLICK_COMPRESSION_HH
#include <click/element.hh>
#include <zlib.h>

#define MAX_PACKET_SIZE 10000

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

  bool ZlibCompression (uint8_t *srcData, uint8_t *destData, uint16_t srcSize, uint16_t &destSize);

private:
  uint8_t buf[MAX_PACKET_SIZE];
};
CLICK_ENDDECLS
#endif