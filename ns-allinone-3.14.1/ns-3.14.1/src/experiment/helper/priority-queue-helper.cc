/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/uinteger.h"
#include "ns3/string.h"

#include "ns3/priority-queue-sender.h"
#include "ns3/priority-queue-receiver.h"

#include "priority-queue-helper.h"

namespace ns3 {
//
// PriorityQueueReceiverHelper functions
//
PriorityQueueReceiverHelper::PriorityQueueReceiverHelper ()
{
  m_factory.SetTypeId (PriorityQueueReceiver::GetTypeId ());
}

void
PriorityQueueReceiverHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
PriorityQueueReceiverHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      m_receiver = m_factory.Create<PriorityQueueReceiver> ();
      node->AddApplication (m_receiver);
      apps.Add (m_receiver);

    }
  return apps;
}

Ptr<PriorityQueueReceiver>
PriorityQueueReceiverHelper::GetReceiver (void)
{
  return m_receiver;
}

//
// PriorityQueueSenderHelper functions
//
PriorityQueueSenderHelper::PriorityQueueSenderHelper ()
{
  m_factory.SetTypeId (PriorityQueueSender::GetTypeId ());
}

void
PriorityQueueSenderHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
PriorityQueueSenderHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      m_sender = m_factory.Create<PriorityQueueSender> ();
      node->AddApplication (m_sender);
      apps.Add (m_sender);
    }
  return apps;
}

Ptr<PriorityQueueSender>
PriorityQueueSenderHelper::GetSender (void)
{
  return m_sender;
}


} // namespace ns3

