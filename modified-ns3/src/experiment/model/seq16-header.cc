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
#include "seq16-header.h"

NS_LOG_COMPONENT_DEFINE ("Seq16Header");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Seq16Header);

Seq16Header::Seq16Header ()
  : m_seq (0)
{
}

void
Seq16Header::SetSeq (uint16_t seq)
{
  m_seq = seq;
}

uint16_t
Seq16Header::GetSeq (void) const
{
  return m_seq;
}

TypeId
Seq16Header::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Seq16Header")
    .SetParent<Header> ()
    .AddConstructor<Seq16Header> ()
  ;
  return tid;
}

TypeId
Seq16Header::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
Seq16Header::Print (std::ostream &os) const
{
  os << "(seq=" << m_seq << ")";
}

uint32_t
Seq16Header::GetSerializedSize (void) const
{
  return 2;
}

void
Seq16Header::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU16 (m_seq);
}

uint32_t
Seq16Header::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_seq = i.ReadNtohU16 ();
  return GetSerializedSize ();
}

} // namespace ns3
