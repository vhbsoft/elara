#ifndef COMPRESSION_SENDER_RECEIVER_HELPER_H
#define COMPRESSION_SENDER_RECEIVER_HELPER_H

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/compression-sender.h"
#include "ns3/compression-receiver.h"

namespace ns3 {

class CompressionReceiverHelper
{
public:
  CompressionReceiverHelper ();

  /**
   * Record an attribute to be set in each Application after it is is created.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   */
  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * Create one compression receiver application on each of the Nodes in the
   * NodeContainer.
   *
   * \param c The nodes on which to create the Applications.  The nodes
   *          are specified by a NodeContainer.
   * \returns The applications created, one Application per Node in the
   *          NodeContainer.
   */
  ApplicationContainer Install (NodeContainer c);

  Ptr<CompressionReceiver> GetReceiver (void);
private:
  ObjectFactory m_factory;
  Ptr<CompressionReceiver> m_receiver;
};


class CompressionSenderHelper
{
public:
  CompressionSenderHelper ();

  //Record an attribute to be set in each Application after it is is created.
  void SetAttribute (std::string name, const AttributeValue &value);

  // Create one compression sender application on each of the input nodes,
  // returns the applications created
  ApplicationContainer Install (NodeContainer c);

  // Return a pointer to m_sender
  Ptr<CompressionSender> GetSender (void);

private:
  ObjectFactory m_factory;
  Ptr<CompressionSender> m_sender;

};

} // namespace ns3

#endif /* COMPRESSION_SENDER_RECEIVER_HELPER_H */
