#include "Note.h"

int Note::get_pitch()
{
	return note;
};

int Note::get_velocity()
{
	return velocity;
};

float* Note::get_rgb()
{
	return rgb;
};

float* Note::get_pos()
{
  return pos;
};
