#pragma once
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "RtMidi.h"
#include <list>
#include "Note.h"
#include "PlayingNotes.h"
#include <thread>
#include <ctime>

class Cmidiin
{
  public:
      void usage(void);
      void mycallback(double deltatime, std::vector<unsigned char>* message, void* user);
      bool chooseMidiPort(RtMidiIn* rtmidi);
      void midi_thread();
      void read_notes();
      

      RtMidiIn* midiin = 0;
      std::thread midiin_thread;
      int pedal_point = 0;
      PlayingNotes played_notes;
      std::clock_t start = std::clock();
      double duration;
      int beat = 1;
      float luminosity_incr = 150.0f; // must be < 200.0f
      int key = 24; // starting major ionian C #24 midi
      ///////////////////////root/////////m2nd/////////2nd///////////m3rd////////3rd////////4th/////////m5th/////////5th//////////#5th/////////6th//////////m7th////////7th/////
      int scale[12][3] = { { 0, 0, 51 }, { 51, 0, 0 }, { 51, 25, 0 }, { 51, 0, 0 }, { 0, 51, 25 }, { 51, 0, 25 }, { 51, 0, 0 }, { 25, 51, 0 }, { 51, 0, 0 }, { 51, 0, 51 }, { 51, 0, 0 }, { 51, 51, 0 } };
      /*root = blue 0 0 51
        m2nd = red 51 0 0
        2nd = orange 51 25 0
        m3rd = red 51 0 0 
        3rd = green blue 0 51 25
        4th = purple 51 0 25
        m5th = red 51 0 0
        5th = green red 25 51 0
        aug5th = red 51 0 0
        6th = violet 51 0 51
        m7th = red 51 0 0
        7th = yellow 51 51 0*/
};
