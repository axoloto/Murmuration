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
  ParticleEmitter(Dimension dim, const Math::float3& initPos, const Math::float3& initVel, const Math::float3& col, int lifeTime = 500)
      : m_dimension(dim)
      , m_nbParticles(64)
      , m_initPos(initPos)
      , m_initVel(initVel)
      , m_color(col)
      , m_lifeTime(lifeTime)
      , m_halfSideLength(0.3f) {};

  ~ParticleEmitter() = default;

  std::vector<Math::float3> getParticlesPos() const;
  std::vector<Math::float3> getParticlesVel() const;
  std::vector<Math::float3> getParticlesCol() const;
  int getParticlesLifeTime() const { return m_lifeTime; }
  size_t getNbParticles() const { return m_nbParticles; }

  private:
  Dimension m_dimension;
  size_t m_nbParticles;
  Math::float3 m_initPos;
  Math::float3 m_initVel;
  Math::float3 m_color;
  int m_lifeTime;
  float m_halfSideLength;
};
}