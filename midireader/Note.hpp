#pragma once

#include "Math.hpp"

namespace IO
{
class Note
{
  public:
  Note(int note, int strength, double deltatime, Math::float3 color, Math::float3 pos)
  {
    m_note = note;
    m_velocity = strength;
    m_beat = deltatime;
    m_rgb = color;
    m_pos = pos;
  };
  int getPitch() const { return m_note; }
  int getVelocity() const { return m_velocity; }
  Math::float3 getRgb() const { return m_rgb; }
  Math::float3 getPos() const { return m_pos; }

  private:
  int m_note;
  int m_velocity;
  double m_beat;
  Math::float3 m_rgb;
  Math::float3 m_pos;
};
}
