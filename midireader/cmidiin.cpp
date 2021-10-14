//*****************************************//
//  cmidiin.cpp
//  by Gary Scavone, 2003-2004.
//
//  Simple program to test MIDI input and
//  use of a user callback function.
//
//*****************************************//

#include "cmidiin.h"

/*#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"*/




//#define PORT 7000
/*
class ExamplePacketListener : public osc::OscPacketListener {
protected:

    virtual void ProcessMessage(const osc::ReceivedMessage& m,
        const IpEndpointName& remoteEndpoint)
    {
        (void)remoteEndpoint; // suppress unused parameter warning

        try {
            // example of parsing single messages. osc::OsckPacketListener
            // handles the bundle traversal.

            if (std::strcmp(m.AddressPattern(), "/test1") == 0) {
                // example #1 -- argument stream interface
                osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
                bool a1;
                osc::int32 a2;
                float a3;
                const char* a4;
                args >> a1 >> a2 >> a3 >> a4 >> osc::EndMessage;

                std::cout << "received '/test1' message with arguments: "
                    << a1 << " " << a2 << " " << a3 << " " << a4 << "\n";

            }
            else if (std::strcmp(m.AddressPattern(), "/test2") == 0) {
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
        }
        catch (osc::Exception& e) {
            // any parsing errors such as unexpected argument types, or 
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }
    }
};*/

void Cmidiin::usage(void) {
    // Error function in case of incorrect command-line
    // argument specifications.
    std::cout << "\nuseage: cmidiin <port>\n";
    std::cout << "    where port = the device to use (first / default = 0).\n\n";
    exit(0);
}

void Cmidiin::mycallback(double deltatime, std::vector<unsigned char>* message, void* user)
{
    //std::cout << "Received midi message" << std::endl;
    unsigned int nBytes = message->size();

    //notes range from 21 to 108
    if ((int)message->at(0) == 144)//note on so we create particles
    {
        /* Your algorithm here */
        beat++;
        duration = (std::clock() - start) / (double)CLOCKS_PER_SEC + 1.0f;
        std::cout << "Beat estimated is : " << (double)beat / duration << '\n';
        std::cout << "We are creating particles from note " << (int)message->at(1) << " at speed " << (int)message->at(2) << std::endl;
        int degree = ((int)message->at(1) - key) % 12;
        int octave = (((int)message->at(1) - key) / 12) + 1;
        float rgb[3];
        rgb[0] = (((float) scale[degree][0] + ((float) octave / 8) * luminosity_incr)) / 255.0f;
        rgb[1] = (((float)scale[degree][1] + ((float)octave / 8) * luminosity_incr)) / 255.0f;
        rgb[2] = (((float)scale[degree][2] + ((float)octave / 8) * luminosity_incr)) / 255.0f;
        Note note((int)message->at(1), (int)message->at(2), beat, rgb);
        played_notes.add(note);
        std::cout << "We are playing the " << degree << " degree of octave " << octave << " of color " << rgb[0] <<" " << rgb[1] << " " << rgb[2] << std::endl;
        
        if (duration > 5.0f)
        {
            beat = 1;
            start = std::clock();
        }
    }
    else if ((int)message->at(0) == 128)// note off
    {
        if (pedal_point == 0) // we stop creating particles
        {
            std::cout << "We stopped creating particle from note " << (int)message->at(1) << std::endl;
            played_notes.remove((int)message->at(1));
        }
        else //we put the note in queue to be stopped when the pedal is released
        {
            played_notes.add_hanging_note((int)message->at(1));
        }
    }
    else if ((int)message->at(0) == 176 && (int)message->at(1) == 64)
    {
        //std::cout << "Pedal point value is " << (int)message->at(2) << std::endl;
        pedal_point = (int)message->at(2);
        if (pedal_point == 0)// we destroy all the queued notes
        {
            played_notes.remove_all_hanging_notes();
        }
    }
}

bool Cmidiin::chooseMidiPort(RtMidiIn* rtmidi)
{
    /*std::cout << "\nWould you like to open a virtual input port? [y/N] ";

    std::string keyHit;
    std::getline( std::cin, keyHit );
    if ( keyHit == "y" ) {
      rtmidi->openVirtualPort();
      return true;
    }*/

    std::string portName;
    unsigned int i = 0, nPorts = rtmidi->getPortCount();
    if (nPorts == 0) {
        std::cout << "No input ports available!" << std::endl;
        return false;
    }

    if (nPorts == 1) {
        std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
    }
    else {
        for (i = 0; i < nPorts; i++) {
            portName = rtmidi->getPortName(i);
            std::cout << "  Input port #" << i << ": " << portName << '\n';
        }

        do {
            std::cout << "\nChoose a port number: ";
            std::cin >> i;
        } while (i >= nPorts);
        //std::getline( std::cin, keyHit );  // used to clear out stdin
    }

    rtmidi->openPort(i);

    return true;
}

void Cmidiin::midi_thread()
{

    // Minimal command-line check.
    //if (argc > 2) usage();
    //std::cout << "Choose your key from Cmaj (0) to Bmaj (11):\n";
    //i
    // nt num = 0;
    //std::cin >> num;
    //key = num + 24;

    try {

        // RtMidiIn constructor
        midiin = new RtMidiIn();

        // Call function to select port.
        if (chooseMidiPort(midiin) == false) goto cleanup;

        // Set our callback function.  This should be done immediately after
        // opening the port to avoid having incoming messages written to the
        // queue instead of sent to the callback function.
        //midiin->setCallback(&Cmidiin::mycallback);

        // Don't ignore sysex, timing, or active sensing messages.
        midiin->ignoreTypes(false, false, false);

        std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
        char input;
        std::cin.get(input);

    }
    catch (RtMidiError& error) {
        error.printMessage();
    }

cleanup:

    delete midiin;
}


void Cmidiin::read_notes()
{
    while (true)
    {
        played_notes.print_notes();
    }
}

/*
int main( int argc, char ** /*argv[] )
{

   // ExamplePacketListener listener;
   // UdpListeningReceiveSocket s(
     //   IpEndpointName(IpEndpointName::ANY_ADDRESS, PORT),
       // &listener);

    std::cout << "press ctrl-c to end\n";

    
    //std::thread t1(read_notes);
    std::thread t2(midi_thread);
    //s.RunUntilSigInt();
    //t1.join();
    t2.join();


  return 0;
}
*/

