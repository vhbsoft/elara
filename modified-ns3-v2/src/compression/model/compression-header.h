/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef COMPRESSION_HEADER_H
#define COMPRESSION_HEADER_H

#include "ns3/header.h"

namespace ns3 {

class CompressionHeader : public Header 
{
public:

  static TypeId GetTypeId (void);
  CompressionHeader ();
  virtual ~CompressionHeader ();

  uint32_t GetSize (void) const; 
  void SetSize (uint32_t size);

  // Writes internal data to buffer pointed by param data, 
  // appication must ensure the buffer is at least param size
  uint32_t GetData (uint8_t* data, uint32_t size) const;
  void SetData (uint8_t* data, uint32_t size);

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  // If Deserialize() is called before SetSize() then 
  // the resulting packet will have size 0
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  
private:
  virtual TypeId GetInstanceTypeId (void) const;

  // size of the buffer data
  uint32_t m_size;
 
  // buffer to store data
  uint8_t *m_data;

}; // CompressedHeader

} // namespace ns3

#endif
