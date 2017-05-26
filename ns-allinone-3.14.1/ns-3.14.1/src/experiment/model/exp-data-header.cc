/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/header.h"
#include "ns3/simulator.h"
#include "exp-data-header.h"

NS_LOG_COMPONENT_DEFINE ("ExpDataHeader");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (ExpDataHeader);

ExpDataHeader::ExpDataHeader ()
  : m_results("")
{
  m_num = 0;
  m_time = 0;
  m_len = 0;

  m_initialNum = 0;
  m_separationNum = 0;
  m_aptNum = 0;
  m_aptPriority = 0;
}

void 
ExpDataHeader::SetNumPackets (uint32_t num)
{
  m_num = num;
}

void 
ExpDataHeader::SetInterPacketTime (uint64_t time)
{
  m_time = time;
}
void 
ExpDataHeader::SetProbePacketLen (uint32_t len)
{
  m_len = len;
}

uint32_t 
ExpDataHeader::GetNumPackets (void) const
{
  return m_num;
}

uint64_t
ExpDataHeader::GetInterPacketTime (void) const
{
  return m_time;
}

uint32_t 
ExpDataHeader::GetProbePacketLen (void) const
{
  return m_len;
}

void 
ExpDataHeader::SetInitialNum (uint32_t initialNum)
{
  m_initialNum = initialNum;
}

void 
ExpDataHeader::SetSeparationNum (uint32_t separationNum)
{
  m_separationNum = separationNum;
}

void 
ExpDataHeader::SetAptNum (uint32_t aptNum)
{
  m_aptNum = aptNum;
}

void  
ExpDataHeader::SetAptPriority (uint8_t aptPriority)
{
  m_aptPriority = aptPriority;
}

uint32_t 
ExpDataHeader::GetInitialNum (void) const
{
  return m_initialNum;
}

uint32_t 
ExpDataHeader::GetSeparationNum (void) const
{
  return m_separationNum;
}

uint32_t 
ExpDataHeader::GetAptNum (void) const
{
  return m_aptNum;
}

uint8_t  
ExpDataHeader::GetAptPriority (void) const
{
  return m_aptPriority;
}

uint32_t
ExpDataHeader::GetResultSize (void) const
{
  return m_results.size();
}

void 
ExpDataHeader::SetResults (std::string results)
{
  m_results = results;
}

std::string
ExpDataHeader::GetResults (void) const
{
  return m_results;
}

TypeId
ExpDataHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ExpDataHeader")
    .SetParent<Header> ()
    .AddConstructor<ExpDataHeader> ()
  ;
  return tid;
}

TypeId
ExpDataHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
ExpDataHeader::Print (std::ostream &os) const
{
}

uint32_t
ExpDataHeader::GetSerializedSize (void) const
{
  return m_results.size() + 4+8+4 + 4+4+4+1 + 4;
}

void
ExpDataHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator it = start;
  it.WriteHtonU32 (m_num);
  it.WriteHtonU64 (m_time);
  it.WriteHtonU32 (m_len);

  it.WriteHtonU32 (m_initialNum);
  it.WriteHtonU32 (m_separationNum);
  it.WriteHtonU32 (m_aptNum);
  it.WriteU8 (m_aptPriority);

  uint32_t size = (uint32_t) m_results.size();
  it.WriteHtonU32 (size);

  //NS_LOG_UNCOND("Serialize::size = " << size);

  for (uint32_t i=0; i < size; i++)
    {
      it.WriteU8 ((uint8_t) m_results[i]);
    }
}

uint32_t
ExpDataHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator it = start;
  m_num = it.ReadNtohU32 ();
  m_time = it.ReadNtohU64 ();
  m_len = it.ReadNtohU32 ();

  m_initialNum = it.ReadNtohU32 ();
  m_separationNum = it.ReadNtohU32 ();
  m_aptNum = it.ReadNtohU32 ();
  m_aptPriority = it.ReadU8 ();

  uint32_t size = it.ReadNtohU32 ();

  //NS_LOG_UNCOND("Deserialize::size = " << size);
  uint32_t i;

  m_results = "";
  for (i=0; i < size; i++)
    {
      m_results += (char) it.ReadU8 ();
      //if (i > 500) NS_LOG_UNCOND(i);
    }

  return GetSerializedSize ();
}

} // namespace ns3
