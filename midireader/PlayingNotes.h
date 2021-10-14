#pragma once
#include <iostream>
#include "Note.h"
#include <list>
#include<mutex>

class PlayingNotes
{
public:
    void add(Note note);
    void add_hanging_note(int note);
    void remove_hanging_note(int note);
    void remove_all_hanging_notes();
    void remove(int note);
    std::list<Note> get_all();
    std::list<int> get_notes();
    void print_notes();
private:
    // a global instance of std::mutex to protect global variable
    std::mutex myMutex;
    std::list<Note> notes;
    std::list<int> hanging_notes;

};