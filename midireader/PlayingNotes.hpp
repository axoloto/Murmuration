#pragma once
#include "Note.hpp"

#include <iostream>
#include <list>
#include <mutex>

namespace Midi
{
class PlayingNotes
{
  public:
  void add(Note note);
  void add_hanging_note(int note);
  void remove_hanging_note(int note);
  void remove_all_hanging_notes();
  void remove(int note);
  std::list<Note> getAllNotes();
  std::list<int> getNotes();
  void print_notes();

  private:
  // a global instance of std::mutex to protect global variable
  std::mutex myMutex;
  std::list<Note> notes;
  std::list<int> hanging_notes;
};
}