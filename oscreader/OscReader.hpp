#pragma once

#include "OscPacketListener.h"
#include "OscReceivedElements.h"
#include "UdpSocket.h"

#include <thread>

namespace IO
{
class OscReader : public osc::OscPacketListener
{
  public:
  OscReader();
  ~OscReader();

  void ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint) override;

  void start();
  void stop();

  private:
  void run();
  bool m_enableThread;
  std::unique_ptr<UdpListeningReceiveSocket> m_listener;
  std::thread m_thread;
};
}