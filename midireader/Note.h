#pragma once

class Note
{
public:
    Note(int not, int strength, double deltatime, float * colors, float * position)
    {
        note = not;
        velocity = strength;
        beat = deltatime;
        rgb[0] = colors[0];
        rgb[1] = colors[1];
        rgb[2] = colors[2];
        pos[0] = position[0];
        pos[1] = position[1];
        pos[2] = position[2];
        //std::time_t result = std::time(nullptr);
        //time = std::localtime(&result);
    };
    int get_pitch();
    int get_velocity();
    float* get_rgb();
    float* get_pos();

private:
    int note;
    int velocity;
    double beat;
    float rgb[3];
    float pos[3];
    //int time;
};
