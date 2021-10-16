#include "OscReader.hpp"
#include "Logging.hpp"

#include <iostream>

using namespace IO;

OscReader::OscReader()
    : osc::OscPacketListener()
    , m_thread {}
{
}

OscReader::~OscReader() { stop(); }

#define PORT 7000

void OscReader::start()
{
  LOG_INFO("Main thread {} ", std::hash<std::thread::id> {}(std::this_thread::get_id()));

  m_thread = std::thread(&OscReader::run, this);
}

void OscReader::run()
{
  m_listener = std::make_unique<UdpListeningReceiveSocket>(IpEndpointName(IpEndpointName::ANY_ADDRESS, PORT), this);
  m_listener->Run();
}

void OscReader::stop()
{
  m_listener->AsynchronousBreak();
  m_thread.join();
}

void OscReader::ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint)
{
  LOG_INFO("Receiving OSC input with thread {} ", std::hash<std::thread::id> {}(std::this_thread::get_id()));

  (void)remoteEndpoint; // suppress unused parameter warning
  try
  {
    // example of parsing single messages. osc::OsckPacketListener
    // handles the bundle traversal.
    if (std::strcmp(m.AddressPattern(), "/test1") == 0)
    {
      // example #1 -- argument stream interface
      osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
      float a1;
      float a2;
      float a3;
      args >> a1 >> a2 >> a3 >> osc::EndMessage;
      std::cout << "received '/test1' message with arguments: "
                << a1 << " " << a2 << " " << a3 << " "
                << "\n";
    }
    else if (std::strcmp(m.AddressPattern(), "/test2") == 0)
    {
      // example #2 -- argument iterator interface, supports
      // reflection for overloaded messages (eg you can call
      // (*arg)->IsBool() to check if a bool was passed etc).
      osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
      bool a1 = (arg++)->AsBool();
      int a2 = (arg++)->AsInt32();
      float a3 = (arg++)->AsFloat();
      const char* a4 = (arg++)->AsString();
      if (arg != m.ArgumentsEnd())
        throw osc::ExcessArgumentException();
      std::cout << "received '/test2' message with arguments: "
                << a1 << " " << a2 << " " << a3 << " " << a4 << "\n";
    }
    else if (std::strcmp(m.AddressPattern(), "/midi") == 0)
    { //parsing osc message to get notes from noise
      // example #1 -- argument stream interface
      osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
      int a1; // note on/off
      int a2; // note pitch
      int a3; // note velocity
      args >> a1 >> a2 >> a3 >> osc::EndMessage;
      std::cout << "received midi message with arguments: "
                << a1 << " " << a2 << " " << a3 << " "
                << "\n";
    }
    else if (std::strcmp(m.AddressPattern(), "/max-2-c") == 0)
    { // parsing osc message from the ring
      // example #1 -- argument stream interface
      osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
      float a1; // ring orientation 1
      float a2; // ring orientation 2
      float a3; // ring orientation 3
      int a4; //tap
      args >> a1 >> a2 >> a3 >> a4 >> osc::EndMessage;
      std::cout << "received ring message with arguments : "
                << a1 << " " << a2 << " " << a3 << " " << a4 << "\n";
    }
    else
    {
      std::cout << "Received unknown" << std::endl;
    }
  }
  catch (osc::Exception& e)
  {
    // any parsing errors such as unexpected argument types, or
    // missing arguments get thrown as exceptions.
    std::cout << "error while parsing message: "
              << m.AddressPattern() << ": " << e.what() << "\n";
  }
}