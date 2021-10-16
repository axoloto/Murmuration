#pragma once

#include "OscPacketListener.h"
#include "OscReceivedElements.h"
#include "UdpSocket.h"
#include "Math.hpp"
#include "mutex"

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
  Math::float3 get_ring_orientation();
  float get_ring_tap();
  float get_ring_acceleration();

  private:
  std::mutex myMutex;
  //max2c args
  float max2c1 = 0.0f; // ring orientation 1
  float max2c2 = 0.0f; // ring orientation 2
  float max2c3 = 0.0f; // ring orientation 3
  float max2c4 = 0.0f; //tap
  float max2c5 = 0.0f; // acceleration
  void run();
  bool m_enableThread;
  std::unique_ptr<UdpListeningReceiveSocket> m_listener;
  std::thread m_thread;
};
}