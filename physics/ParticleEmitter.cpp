#include "ParticleEmitter.hpp"
#include "Geometry.hpp"
#include "Utils.hpp"

using namespace Physics;

std::vector<Math::float3> ParticleEmitter::getParticlesPos() const
{
  std::vector<Math::float3> positions;

  if (m_dimension == Dimension::dim2D)
  {
    const auto& subdiv2D = Utils::GetNbParticlesSubdiv2D((Utils::NbParticles)m_nbParticles);
    Math::int2 grid2DRes = { subdiv2D[0], subdiv2D[1] };
    Math::float3 start2D = { 0.0f, m_initPos.y - m_halfSideLength, m_initPos.z - m_halfSideLength };
    Math::float3 end2D = { 0.0f, m_initPos.y + m_halfSideLength, m_initPos.z + m_halfSideLength };

    positions = Geometry::Generate2DGrid(Geometry::Shape2D::Circle, Geometry::Plane::YZ, grid2DRes, start2D, end2D);
  }
  else if (m_dimension == Dimension::dim3D)
  {
    const auto& subdiv3D = Utils::GetNbParticlesSubdiv3D((Utils::NbParticles)m_nbParticles);
    Math::int3 grid3DRes = { subdiv3D[0], subdiv3D[1], subdiv3D[2] };
    Math::float3 start3D = { m_initPos.x - m_halfSideLength, m_initPos.y - m_halfSideLength, m_initPos.z - m_halfSideLength };
    Math::float3 end3D = { m_initPos.x + m_halfSideLength, m_initPos.y + m_halfSideLength, m_initPos.z + m_halfSideLength };

    positions = Geometry::Generate3DGrid(Geometry::Shape3D::Sphere, grid3DRes, start3D, end3D);
  }

  return std::move(positions);
}

std::vector<Math::float3> ParticleEmitter::getParticlesVel() const
{
  std::vector<Math::float3> velocities(m_nbParticles, m_initVel);

  return std::move(velocities);
}

std::vector<Math::float3> ParticleEmitter::getParticlesCol() const
{
  std::vector<Math::float3> colors(m_nbParticles, m_color);

  return std::move(colors);
}