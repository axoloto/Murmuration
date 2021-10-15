#pragma once

#include "Logging.hpp"
#include "PlayingNotes.hpp"
#include "RtMidi.h"

namespace Midi
{
class Reader
{
  public:
  Reader();
  ~Reader();

  bool chooseMidiPort();
  void start();
  void stop();

  std::list<Note> getAllNotes();

  private:
  std::unique_ptr<RtMidiIn> m_rtMidiThread;
  PlayingNotes m_playedNotes;
};
}