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
#include "seq32-header.h"

NS_LOG_COMPONENT_DEFINE ("Seq32Header");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Seq32Header);

Seq32Header::Seq32Header ()
  : m_seq (0)
{
}

void
Seq32Header::SetSeq (uint32_t seq)
{
  m_seq = seq;
}

uint32_t
Seq32Header::GetSeq (void) const
{
  return m_seq;
}

TypeId
Seq32Header::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Seq32Header")
    .SetParent<Header> ()
    .AddConstructor<Seq32Header> ()
  ;
  return tid;
}

TypeId
Seq32Header::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
Seq32Header::Print (std::ostream &os) const
{
  os << "(seq=" << m_seq << ")";
}

uint32_t
Seq32Header::GetSerializedSize (void) const
{
  return 4;
}

void
Seq32Header::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU32 (m_seq);
}

uint32_t
Seq32Header::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_seq = i.ReadNtohU32 ();
  return GetSerializedSize ();
}

} // namespace ns3
