#include "PlayingNotes.h"

void PlayingNotes::add(Note note)
{
    remove(note.get_pitch());
    remove_hanging_note(note.get_pitch());
    std::lock_guard<std::mutex> guard(myMutex);
    notes.push_back(note);
};

void PlayingNotes::remove(int note)
{
    std::lock_guard<std::mutex> guard(myMutex);
    std::list<Note>::iterator i = notes.begin();
    while (i != notes.end())
    {
        if (i->get_pitch() == note)
        {
            notes.erase(i++);  // alternatively, i = items.erase(i);
        }
        else
        {
            ++i;
        }
    }
};

std::list<Note> PlayingNotes::get_all()
{
    std::lock_guard<std::mutex> guard(myMutex);
    return notes;
};

std::list<int> PlayingNotes::get_notes()
{
    std::lock_guard<std::mutex> guard(myMutex);
    std::list<int> result;

    std::list<Note>::iterator i = notes.begin();
    while (i != notes.end())
    {
        result.push_back(i->get_pitch());
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
            hanging_notes.erase(i++);  // alternatively, i = items.erase(i);
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
    std::list<int> disp = get_notes();
    std::cout << "We have notes:";
    std::list<int>::iterator i = disp.begin();
    while (i != disp.end())
    {
        std::cout << *i << " ";
        ++i;
    }
    std::cout << std::endl;
}