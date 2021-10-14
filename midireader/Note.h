#pragma once

class Note
{
public:
    Note(int not, int strength, double deltatime, float * colors)
    {
        note = not;
        velocity = strength;
        beat = deltatime;
        rgb[0] = colors[0];
        rgb[1] = colors[1];
        rgb[2] = colors[2];
        //std::time_t result = std::time(nullptr);
        //time = std::localtime(&result);
    };
    int get_pitch();
    int get_velocity();
    float* get_rgb();
private:
    int note;
    int velocity;
    double beat;
    float rgb[3];
    //int time;
};
