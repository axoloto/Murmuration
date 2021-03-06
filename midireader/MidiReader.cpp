#include "MidiReader.hpp"
#include "RtMidi.h"
#include <ctime>

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

using namespace IO;

MidiReader::MidiReader()
{
}

MidiReader::~MidiReader()
{
  stop();
}

//Ugliness at its finest
int beat = 1;
int pedalPoint = 0;
std::clock_t start = std::clock();
double duration = 0.0f;
void callback(double deltatime, std::vector<unsigned char>* message, void* userData)
{
  //std::cout << "Received midi message" << std::endl;
  unsigned int nBytes = message->size();

  //std::cout << "We received " << (int)message->at(0) << " " << (int)message->at(0)  <<std::endl;

  auto* playingNotes = ((PlayingNotes*)(userData));

  // starting major ionian C #24 midi
  int key = 24;
  float luminosityIncr = 150.0f; // must be < 200.0f
  float sizeBox = 18.0f;
  float rayon = 2.0f;

  //notes range from 21 to 108
  if ((int)message->at(0) == 144) //note ON so we create particles
  {
    beat++;
    duration = (std::clock() - start) / (double)CLOCKS_PER_SEC + 1.0f;
    LOG_INFO("Beat estimated is : {} ", (double)beat / duration);
    LOG_INFO("We are creating particles from note {} at speed {}", (int)message->at(1), (int)message->at(2));
    int degree = ((int)message->at(1) - key) % 12;
    if (degree < 0)
    {
      degree = 12 + degree;
    }
    int octave = (((int)message->at(1) - key) / 12) + 1;

    Math::float3 rgb;
    rgb.x = (((float)scale[degree][0] / 2.0f + ((float)octave / 8.0f) * luminosityIncr)) / 255.0f;
    rgb.y = (((float)scale[degree][1] / 2.0f + ((float)octave / 8.0f) * luminosityIncr)) / 255.0f;
    rgb.z = (((float)scale[degree][2] / 2.0f + ((float)octave / 8.0f) * luminosityIncr)) / 255.0f;

    Math::float3 pos;
    pos.x = std::min(std::max((float)degree / 12.0f + (float)octave / 9.0f * sizeBox - sizeBox / 2.0f + rayon * cos((float)degree / 12.0f * 2.0f * Math::PI_F), -sizeBox / 2.0f), sizeBox / 2.0f);
    pos.z = std::min(std::max((float)degree / 12.0f + (float)octave / 9.0f * sizeBox - sizeBox / 2.0f + rayon * sin((float)degree / 12.0f * 2.0f * Math::PI_F), -sizeBox / 2.0f), sizeBox / 2.0f);
    pos.y = std::min(std::max((float)degree / 12.0f + (float)octave / 9.0f * sizeBox - sizeBox / 2.0f, -sizeBox / 2.0f), sizeBox / 2.0f);

    Note note((int)message->at(1), (int)message->at(2), beat, rgb, pos);

    playingNotes->add(note);

    LOG_INFO("Receiving MIDI input with thread {} ", std::hash<std::thread::id> {}(std::this_thread::get_id()));

    LOG_INFO("We are playing the {} degree of octave, {} of color {} {} {}", degree, octave, rgb.x, rgb.y, rgb.z);

    if (duration > 5.0f)
    {
      beat = 1;
      start = std::clock();
    }
  }
  else if ((int)message->at(0) == 128) // note OFF
  {
    if (pedalPoint == 0) // we stop creating particles
    {
      LOG_INFO("We stopped creating particle from note {} ", (int)message->at(1));
      playingNotes->remove((int)message->at(1));
    }
    // else //we put the note in queue to be stopped when the pedal is released
    {
      playingNotes->add_hanging_note((int)message->at(1));
    }
  }
  else if ((int)message->at(0) == 176 && (int)message->at(1) == 64)
  {
    //std::cout << "Pedal point value is " << (int)message->at(2) << std::endl;
    //pedalPoint = (int)message->at(2);
    pedalPoint = 0;
    if (pedalPoint == 0) // we destroy all the queued notes
    {
      playingNotes->remove_all_hanging_notes();
    }
  }
}

void MidiReader::stop()
{
  m_rtMidiThread->closePort();
}

void MidiReader::start()
{
  try
  {
    m_rtMidiThread = std::make_unique<RtMidiIn>();

    if (chooseMidiPort())
    {
      LOG_INFO("Starting reading MIDI input with thread {} ", std::hash<std::thread::id> {}(std::this_thread::get_id()));

      // Set our callback function. This should be done immediately after
      // opening the port to avoid having incoming messages written to the
      // queue instead of sent to the callback function.
      m_rtMidiThread->setCallback(&callback, &m_playedNotes);

      // Don't ignore sysex, timing, or active sensing messages.
      m_rtMidiThread->ignoreTypes(false, false, false);
    }
  }
  catch (RtMidiError& error)
  {
    error.printMessage();
  }
}

std::list<Note> MidiReader::getAllNotes()
{
  return m_playedNotes.getAllNotes();
}

bool MidiReader::chooseMidiPort()
{
  if (m_rtMidiThread->getPortCount() == 0)
  {
    LOG_INFO("No input MIDI ports available!");
    return false;
  }

  // Always opening port 0 by default for now
  m_rtMidiThread->openPort(0);

  return true;
}
