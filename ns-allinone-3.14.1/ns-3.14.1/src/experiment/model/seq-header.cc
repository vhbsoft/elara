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

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/header.h"
#include "ns3/simulator.h"
#include "seq-header.h"

NS_LOG_COMPONENT_DEFINE ("SeqHeader");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SeqHeader);

SeqHeader::SeqHeader ()
  : m_seq (0)
{
}

void
SeqHeader::SetSeq (uint16_t seq)
{
  m_seq = seq;
}

uint16_t
SeqHeader::GetSeq (void) const
{
  return m_seq;
}

TypeId
SeqHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SeqHeader")
    .SetParent<Header> ()
    .AddConstructor<SeqHeader> ()
  ;
  return tid;
}

TypeId
SeqHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
SeqHeader::Print (std::ostream &os) const
{
  os << "(seq=" << m_seq << ")";
}

uint32_t
SeqHeader::GetSerializedSize (void) const
{
  return 2;
}

void
SeqHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU16 (m_seq);
}

uint32_t
SeqHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_seq = i.ReadNtohU16 ();
  return GetSerializedSize ();
}

} // namespace ns3
