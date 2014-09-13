#include <zlib.h>

#include <click/config.h>
#include "compression.hh"

CLICK_DECLS
Compression::Compression()
{
};

Compression::~Compression()
{
};

void
Compression::push(int, Packet *p_in)
{

  WritablePacket* p_out = p_in->uniqueify();

  uint8_t* start = (uint8_t*) p_out->data();
  uint16_t uncomp_size = p_out->length();
  uint16_t comp_size = MAX_PACKET_SIZE;

  if (ZlibCompression (start, m_buf, uncomp_size, comp_size))
    {
      if (uncomp_size > comp_size)
        p_out->take(uncomp_size - comp_size);
      else
        p_out = p_out->put(comp_size - uncomp_size);

      memcpy((uint8_t*) p_out->data(), m_buf, comp_size);

      output(0).push(p_out);
    }
  else output(1).push(p_out);
}

uint32_t
Compression::ZlibCompression (uint8_t *srcData, uint8_t *destData, uint16_t srcSize, uint16_t &destSize)
{

  z_stream strm;
  strm.zalloc = 0;
  strm.zfree = 0;
  strm.next_in = srcData;
  strm.avail_in = srcSize;
  strm.next_out = destData;
  strm.avail_out = destSize;
  
  deflateInit(&strm, Z_BEST_COMPRESSION);
  
  while (strm.avail_in != 0) 
    {
      int res = deflate(&strm, Z_NO_FLUSH);
      if (res != Z_OK)
	{
	  deflateEnd(&strm);
	  return -1;
	}
    }
  
  int deflate_res = Z_OK;
  while (deflate_res == Z_OK)
    {
      deflate_res = deflate(&strm, Z_FINISH);
    }
  if (deflate_res != Z_STREAM_END)
    {
      deflateEnd(&strm);
      return -1;
    }
  
  destSize = strm.total_out;
  deflateEnd(&strm);

  return 1;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Compression)
ELEMENT_LIBS(-L/usr/local/lib -lz)
ELEMENT_MT_SAFE(Compression)
