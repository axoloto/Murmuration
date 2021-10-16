#pragma once

#include "Logging.hpp"
#include "PlayingNotes.hpp"
#include "RtMidi.h"

namespace IO
{
class MidiReader
{
  public:
  MidiReader();
  ~MidiReader();

  bool chooseMidiPort();
  void start();
  void stop();

  std::list<Note> getAllNotes();

  private:
  std::unique_ptr<RtMidiIn> m_rtMidiThread;
  PlayingNotes m_playedNotes;
};
}