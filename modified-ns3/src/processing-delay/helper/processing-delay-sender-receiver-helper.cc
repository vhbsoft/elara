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

#include "ns3/processing-delay-sender.h"
#include "ns3/processing-delay-receiver.h"
#include "processing-delay-sender-receiver-helper.h"

namespace ns3 {

//
// ProcessingDelayReceiverHelper functions
//
ProcessingDelayReceiverHelper::ProcessingDelayReceiverHelper ()
{
  m_factory.SetTypeId (ProcessingDelayReceiver::GetTypeId ());
}

void
ProcessingDelayReceiverHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
ProcessingDelayReceiverHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      m_receiver = m_factory.Create<ProcessingDelayReceiver> ();
      node->AddApplication (m_receiver);
      apps.Add (m_receiver);

    }
  return apps;
}

Ptr<ProcessingDelayReceiver>
ProcessingDelayReceiverHelper::GetReceiver (void)
{
  return m_receiver;
}

//
// ProcessingDelaySenderHelper functions
//
ProcessingDelaySenderHelper::ProcessingDelaySenderHelper ()
{
  m_factory.SetTypeId (ProcessingDelaySender::GetTypeId ());
}

void
ProcessingDelaySenderHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
ProcessingDelaySenderHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      m_sender = m_factory.Create<ProcessingDelaySender> ();
      node->AddApplication (m_sender);
      apps.Add (m_sender);
    }
  return apps;
}

Ptr<ProcessingDelaySender>
ProcessingDelaySenderHelper::GetSender (void)
{
  return m_sender;
}


} // namespace ns3
