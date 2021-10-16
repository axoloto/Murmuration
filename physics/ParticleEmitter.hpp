#pragma once

#include <array>
#include <vector>

#include "Math.hpp"
#include "utils/Define.hpp"

namespace Physics
{
class ParticleEmitter
{
  public:
  ParticleEmitter(Dimension dim, const Math::float3& initPos, const Math::float3& initVel, const Math::float3& col)
      : m_dimension(dim)
      , m_nbParticles(512)
      , m_initPos(initPos)
      , m_initVel(initVel)
      , m_color(col)
      , m_halfSideLength(0.03f) {};

  ~ParticleEmitter() = default;

  std::vector<Math::float3> getParticlesPos() const;
  std::vector<Math::float3> getParticlesVel() const;
  std::vector<Math::float3> getParticlesCol() const;
  size_t getNbParticles() const { return m_nbParticles; }

  private:
  Dimension m_dimension;
  size_t m_nbParticles;
  Math::float3 m_initPos;
  Math::float3 m_initVel;
  Math::float3 m_color;
  float m_halfSideLength;
};
}