/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef PRIORITY_QUEUE_SENDER_RECEIVER_HELPER_H
#define PRIORITY_QUEUE_SENDER_RECEIVER_HELPER_H

#include <stdint.h>

#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"

#include "ns3/priority-queue-sender.h"
#include "ns3/priority-queue-receiver.h"

namespace ns3 {

class Queue;
class PointToPointNetDevice;
class Node;

class PriorityQueueReceiverHelper
{
public:
  PriorityQueueReceiverHelper ();

  /**
   * Record an attribute to be set in each Application after it is is created.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   */
  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * Create one receiver application on each of the Nodes in the
   * NodeContainer.
   *
   * \param c The nodes on which to create the Applications.  The nodes
   *          are specified by a NodeContainer.
   * \returns The applications created, one Application per Node in the
   *          NodeContainer.
   */
  ApplicationContainer Install (NodeContainer c);
  // Get rid of this later
  Ptr<PriorityQueueReceiver> GetReceiver (void);

private:
  ObjectFactory m_factory;
  Ptr<PriorityQueueReceiver> m_receiver;
};


class PriorityQueueSenderHelper
{
public:
  PriorityQueueSenderHelper ();

  //Record an attribute to be set in each Application after it is is created.
  void SetAttribute (std::string name, const AttributeValue &value);

  // Create one sender application on each of the input nodes,
  // returns the applications created
  ApplicationContainer Install (NodeContainer c);

  // Return a pointer to m_sender
  Ptr<PriorityQueueSender> GetSender (void);

  // Get rid of this later
private:
  ObjectFactory m_factory;
  Ptr<PriorityQueueSender> m_sender;

};

} // namespace ns3

#endif /* PRIORITYQUEUE_SENDER_RECEIVER_HELPER_H */

