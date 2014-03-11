/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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
 * Author: Mohamed Amine Ismail <amine.ismail@sophia.inria.fr>
 */

#include "ns3/uinteger.h"
#include "ns3/string.h"

#include "ns3/compression-packet-gen.h"
#include "ns3/compression-sender.h"
#include "ns3/compression-receiver.h"
#include "compression-sender-receiver-helper.h"

namespace ns3 {

//
// CompressionReceiverHelper functions
//
CompressionReceiverHelper::CompressionReceiverHelper ()
{
  m_factory.SetTypeId (CompressionReceiver::GetTypeId ());
}

void
CompressionReceiverHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
CompressionReceiverHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      m_receiver = m_factory.Create<CompressionReceiver> ();
      node->AddApplication (m_receiver);
      apps.Add (m_receiver);

    }
  return apps;
}

Ptr<CompressionReceiver>
CompressionReceiverHelper::GetReceiver (void)
{
  return m_receiver;
}

//
// CompressionSenderHelper functions
//
CompressionSenderHelper::CompressionSenderHelper ()
{
  m_factory.SetTypeId (CompressionSender::GetTypeId ());
}

void
CompressionSenderHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
CompressionSenderHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      m_sender = m_factory.Create<CompressionSender> ();
      node->AddApplication (m_sender);
      apps.Add (m_sender);
    }
  return apps;
}

Ptr<CompressionSender>
CompressionSenderHelper::GetSender (void)
{
  return m_sender;
}


} // namespace ns3
