#include <zlib.h>

#include <click/config.h>
#include "decompression.hh"

CLICK_DECLS
Decompression::Decompression()
{
};

Decompression::~Decompression()
{
};

void
Decompression::push(int, Packet *p_in)
{

  WritablePacket* p_out = p_in->uniqueify();

  uint8_t* start = (uint8_t*) p_out->data();
  uint16_t comp_size = p_out->length();
  uint16_t uncomp_size = MAX_PACKET_SIZE;

  if (ZlibDecompression (start, m_buf, comp_size, uncomp_size))
    {
      if (uncomp_size > comp_size)
        p_out = p_out->put(uncomp_size - comp_size);
      else
        p_out->take(comp_size - uncomp_size);

      memcpy((uint8_t*) p_out->data(), m_buf, uncomp_size);

      output(0).push(p_out);
    }
  else output(1).push(p_out);
}

uint32_t
Decompression::ZlibDecompression (uint8_t *srcData, uint8_t *destData, uint16_t srcSize, uint16_t &destSize)
{
  z_stream strm;
  strm.zalloc = 0;
  strm.zfree = 0;
  strm.next_in = srcData;
  strm.avail_in = srcSize;
  strm.next_out = destData;
  strm.avail_out = destSize;

  inflateInit(&strm);

  int inflate_res = inflate(&strm, Z_FINISH);
  if (inflate_res != Z_STREAM_END)
    {
      inflateEnd(&strm);
      return -1;
    }

  destSize = strm.total_out;

  inflateEnd(&strm);

  return 1;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Decompression)
ELEMENT_LIBS(-L/usr/local/lib -lz)
ELEMENT_MT_SAFE(Decompression)
