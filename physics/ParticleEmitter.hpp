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
  ParticleEmitter(Dimension dim, size_t nbParts, const Math::float3& origin, const Math::float3& col)
      : m_dimension(dim)
      , m_nbParticles(nbParts)
      , m_origin(origin)
      , m_color(col)
      , m_halfSideLength(0.3f) {};

  ~ParticleEmitter() = default;

  const Math::float3& getOriginPosition() const { return m_origin; }

  std::vector<Math::float3> getParticlesPos() const;
  std::vector<Math::float3> getParticlesCol() const;

  size_t getNbParticles() const { return m_nbParticles; }

  private:
  Dimension m_dimension;
  size_t m_nbParticles;
  Math::float3 m_origin;
  Math::float3 m_color;
  float m_halfSideLength;
};
}