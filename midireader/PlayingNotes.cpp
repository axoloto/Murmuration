#include "PlayingNotes.hpp"
#include "Logging.hpp"
using namespace IO;

void PlayingNotes::add(Note note)
{
  remove(note.getPitch());
  remove_hanging_note(note.getPitch());
  std::lock_guard<std::mutex> guard(myMutex);
  LOG_INFO("ADDDING NOTE");
  notes.push_back(note);
};

void PlayingNotes::remove(int note)
{
  std::lock_guard<std::mutex> guard(myMutex);
  std::list<Note>::iterator i = notes.begin();
  while (i != notes.end())
  {
    if (i->getPitch() == note)
    {
      notes.erase(i++); // alternatively, i = items.erase(i);
    }
    else
    {
      ++i;
    }
  }
};

std::list<Note> PlayingNotes::getAllNotes()
{
  std::lock_guard<std::mutex> guard(myMutex);
  return notes;
};

std::list<int> PlayingNotes::getNotes()
{
  std::lock_guard<std::mutex> guard(myMutex);
  std::list<int> result;

  std::list<Note>::iterator i = notes.begin();
  while (i != notes.end())
  {
    result.push_back(i->getPitch());
    ++i;
  }
  return result;
};

void PlayingNotes::add_hanging_note(int note)
{
  remove_hanging_note(note);
  hanging_notes.push_back(note);
};

void PlayingNotes::remove_hanging_note(int note)
{
  std::list<int>::iterator i = hanging_notes.begin();
  while (i != hanging_notes.end())
  {
    if (*i == note)
    {
      hanging_notes.erase(i++); // alternatively, i = items.erase(i);
    }
    else
    {
      ++i;
    }
  }
};

void PlayingNotes::remove_all_hanging_notes()
{
  std::list<int>::iterator i = hanging_notes.begin();
  std::cout << "Stopped creating particles from note ";
  while (i != hanging_notes.end())
  {
    std::cout << *i << " ";
    remove(*i);
    ++i;
  }
  std::cout << std::endl;
  hanging_notes.clear();
};

void PlayingNotes::print_notes()
{
  std::lock_guard<std::mutex> guard(myMutex);
  std::list<int> disp = getNotes();
  std::cout << "We have notes:";
  std::list<int>::iterator i = disp.begin();
  while (i != disp.end())
  {
    std::cout << *i << " ";
    ++i;
  }
  std::cout << std::endl;
}