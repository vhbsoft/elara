/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *
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
 *
 * Author:
 */

#ifndef SEQ16_HEADER_H
#define SEQ16_HEADER_H

#include "ns3/header.h"

namespace ns3 {
/**
 * \class SeqHeader
 * The header is made of a 16bits sequence number
 */
class Seq16Header : public Header
{
public:
  Seq16Header ();

  /**
   * \param seq the sequence number
   */
  void SetSeq (uint16_t seq);

  /**
   * \return the sequence number
   */
  uint16_t GetSeq (void) const;

  static TypeId GetTypeId (void);

private:
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

  uint16_t m_seq;
};

} // namespace ns3

#endif /* SEQ16_HEADER_H */
