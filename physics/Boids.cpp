#include "Boids.hpp"
#include "Geometry.hpp"
#include "Logging.hpp"
#include "Parameters.hpp"
#include "Utils.hpp"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace Physics;

#define PROGRAM_BOIDS "boids"

// utils.cl
#define KERNEL_INFINITE_POS "infPosVerts"
#define KERNEL_RESET_CAMERA_DIST "resetCameraDist"
#define KERNEL_FILL_CAMERA_DIST "fillCameraDist"

// grid.cl
#define KERNEL_RESET_PART_DETECTOR "resetGridDetector"
#define KERNEL_FILL_PART_DETECTOR "fillGridDetector"
#define KERNEL_RESET_CELL_ID "resetCellIDs"
#define KERNEL_FILL_CELL_ID "fillCellIDs"
#define KERNEL_RESET_START_END_CELL "resetStartEndCell"
#define KERNEL_FILL_START_CELL "fillStartCell"
#define KERNEL_FILL_END_CELL "fillEndCell"
#define KERNEL_ADJUST_END_CELL "adjustEndCell"

// boids.cl
#define KERNEL_RANDOM_POS "randPosVertsBoids"
#define KERNEL_FILL_COLOR "fillBoidsColor"
#define KERNEL_UPDATE_POS_BOUNCING "updatePosWithBouncingWalls"
#define KERNEL_UPDATE_POS_CYCLIC "updatePosWithCyclicWalls"
#define KERNEL_UPDATE_VEL "updateVel"
#define KERNEL_FILL_TEXT "fillBoidsTexture"
#define KERNEL_BOIDS_RULES_GRID_2D "applyBoidsRulesWithGrid2D"
#define KERNEL_BOIDS_RULES_GRID_3D "applyBoidsRulesWithGrid3D"
#define KERNEL_ADD_TARGET_RULE "addTargetRule"
#define KERNEL_UPDATE_LIFE_TIME "updateLifeTime"

Boids::Boids(ModelParams params)
    : Model(params)
    , m_scaleAlignment(1.6f)
    , m_scaleCohesion(1.45f)
    , m_scaleSeparation(1.6f)
    , m_activeAlignment(true)
    , m_activeSeparation(true)
    , m_activeCohesion(true)
    , m_activeLifeTime(true)
    , m_simplifiedMode(true)
    , m_maxNbPartsInCell(3000)
    , m_radixSort(params.maxNbParticles)
    , m_particleLifes(params.maxNbParticles, -1)
    , m_target(std::make_unique<Target>(params.boxSize))
{
  m_currNbParticles = Utils::NbParticles::P0;

  createProgram();

  createBuffers();

  createKernels();

  m_init = true;

  reset();
}

bool Boids::createProgram() const
{
  CL::Context& clContext = CL::Context::Get();

  std::ostringstream clBuildOptions;
  clBuildOptions << "-DEFFECT_RADIUS_SQUARED=" << Utils::FloatToStr(1.0f * m_boxSize * m_boxSize / (m_gridRes * m_gridRes));
  clBuildOptions << " -DABS_WALL_POS=" << Utils::FloatToStr(m_boxSize / 2.0f);
  clBuildOptions << " -DGRID_RES=" << m_gridRes;
  clBuildOptions << " -DGRID_CELL_SIZE=" << Utils::FloatToStr(1.0f * m_boxSize / m_gridRes);
  clBuildOptions << " -DGRID_NUM_CELLS=" << m_nbCells;
  clBuildOptions << " -DNUM_MAX_PARTS_IN_CELL=" << m_maxNbPartsInCell;

  LOG_INFO(clBuildOptions.str());
  clContext.createProgram(PROGRAM_BOIDS, std::vector<std::string>({ "boids.cl", "utils.cl", "grid.cl" }), clBuildOptions.str());

  return true;
}

bool Boids::createBuffers() const
{
  CL::Context& clContext = CL::Context::Get();

  clContext.createGLBuffer("u_cameraPos", m_cameraVBO, CL_MEM_READ_ONLY);
  clContext.createGLBuffer("p_pos", m_particlePosVBO, CL_MEM_READ_WRITE);
  clContext.createGLBuffer("p_col", m_particleColVBO, CL_MEM_READ_WRITE);
  clContext.createGLBuffer("c_partDetector", m_gridVBO, CL_MEM_READ_WRITE);

  clContext.createBuffer("p_vel", 4 * m_maxNbParticles * sizeof(float), CL_MEM_READ_WRITE);
  clContext.createBuffer("p_acc", 4 * m_maxNbParticles * sizeof(float), CL_MEM_READ_WRITE);
  clContext.createBuffer("p_cellID", m_maxNbParticles * sizeof(unsigned int), CL_MEM_READ_WRITE);
  clContext.createBuffer("p_cameraDist", m_maxNbParticles * sizeof(unsigned int), CL_MEM_READ_WRITE);
  clContext.createBuffer("p_lifeTime", m_maxNbParticles * sizeof(int), CL_MEM_READ_WRITE);

  clContext.createBuffer("c_startEndPartID", 2 * m_nbCells * sizeof(unsigned int), CL_MEM_READ_WRITE);

  return true;
}

bool Boids::createKernels() const
{
  CL::Context& clContext = CL::Context::Get();

  // Init only
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_INFINITE_POS, { "p_pos" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_RANDOM_POS, { "p_pos", "p_vel" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_FILL_COLOR, { "p_col" });

  // For rendering purpose only
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_RESET_PART_DETECTOR, { "c_partDetector" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_FILL_PART_DETECTOR, { "p_pos", "c_partDetector" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_RESET_CAMERA_DIST, { "p_cameraDist" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_FILL_CAMERA_DIST, { "p_pos", "u_cameraPos", "p_cameraDist" });

  // Boids Physics
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_UPDATE_VEL, { "p_acc", "", "", "p_vel" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_UPDATE_POS_BOUNCING, { "p_vel", "", "p_pos" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_UPDATE_POS_CYCLIC, { "p_vel", "", "p_pos" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_UPDATE_LIFE_TIME, { "p_lifeTime", "p_pos", "p_col" });

  // Radix Sort based on 3D grid
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_RESET_CELL_ID, { "p_cellID" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_FILL_CELL_ID, { "p_pos", "p_cellID" });

  clContext.createKernel(PROGRAM_BOIDS, KERNEL_RESET_START_END_CELL, { "c_startEndPartID" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_FILL_START_CELL, { "p_cellID", "c_startEndPartID" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_FILL_END_CELL, { "p_cellID", "c_startEndPartID" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_ADJUST_END_CELL, { "c_startEndPartID" });

  clContext.createKernel(PROGRAM_BOIDS, KERNEL_BOIDS_RULES_GRID_2D, { "p_pos", "p_vel", "c_startEndPartID", "", "p_acc" });
  clContext.createKernel(PROGRAM_BOIDS, KERNEL_BOIDS_RULES_GRID_3D, { "p_pos", "p_vel", "c_startEndPartID", "", "p_acc" });

  clContext.createKernel(PROGRAM_BOIDS, KERNEL_ADD_TARGET_RULE, { "p_pos", "", "", "p_acc", "p_acc" });

  return true;
}

void Boids::updateBoidsParamsInKernel()
{
  CL::Context& clContext = CL::Context::Get();

  float dim = (m_dimension == Dimension::dim2D) ? 2.0f : 3.0f;
  clContext.setKernelArg(KERNEL_RANDOM_POS, 2, sizeof(float), &dim);

  float vel = m_velocity;
  clContext.setKernelArg(KERNEL_UPDATE_VEL, 2, sizeof(float), &vel);

  std::array<float, 8> boidsParams;
  boidsParams[0] = m_velocity;
  boidsParams[1] = m_activeCohesion ? m_scaleCohesion : 0.0f;
  boidsParams[2] = m_activeAlignment ? m_scaleAlignment : 0.0f;
  boidsParams[3] = m_activeSeparation ? m_scaleSeparation : 0.0f;
  boidsParams[4] = isTargetActivated() ? 1.0f : 0.0f;
  clContext.setKernelArg(KERNEL_BOIDS_RULES_GRID_2D, 3, sizeof(boidsParams), &boidsParams);
  clContext.setKernelArg(KERNEL_BOIDS_RULES_GRID_3D, 3, sizeof(boidsParams), &boidsParams);

  if (isTargetActivated())
  {
    const auto squaredRadiusEffect = targetRadiusEffect() * targetRadiusEffect();
    const auto signEffect = targetSignEffect();
    clContext.setKernelArg(KERNEL_ADD_TARGET_RULE, 2, sizeof(float), &squaredRadiusEffect);
    clContext.setKernelArg(KERNEL_ADD_TARGET_RULE, 3, sizeof(int), &signEffect);
  }
}

void Boids::reset()
{
  if (!m_init)
    return;

  m_currNbParticles = Utils::NbParticles::P0;
  m_particleLifes = std::vector<int>(m_maxNbParticles, -1);

  updateBoidsParamsInKernel();

  CL::Context& clContext = CL::Context::Get();

  initBoidsParticles();

  clContext.acquireGLBuffers({ "p_pos", "p_col", "c_partDetector" });

  clContext.runKernel(KERNEL_FILL_COLOR, m_maxNbParticles);
  clContext.runKernel(KERNEL_RESET_PART_DETECTOR, m_nbCells);
  clContext.runKernel(KERNEL_FILL_PART_DETECTOR, m_currNbParticles);

  clContext.runKernel(KERNEL_RESET_CELL_ID, m_maxNbParticles);
  clContext.runKernel(KERNEL_RESET_CAMERA_DIST, m_maxNbParticles);

  clContext.releaseGLBuffers({ "p_pos", "p_col", "c_partDetector" });
}

void Boids::initBoidsParticles()
{
  if (m_currNbParticles > m_maxNbParticles)
  {
    LOG_ERROR("Cannot init boids, current number of particles is higher than max limit");
    return;
  }

  CL::Context& clContext = CL::Context::Get();

  clContext.acquireGLBuffers({ "p_pos", "p_col" });

  const float& inf = std::numeric_limits<float>::infinity();
  std::vector<std::array<float, 4>> buffer(m_maxNbParticles, std::array<float, 4>({ inf, inf, inf, 0.0f }));

  clContext.loadBufferFromHost("p_pos", 0, 4 * sizeof(float) * buffer.size(), buffer.data());
  // Using same buffer to initialize vel, giving interesting patterns
  clContext.loadBufferFromHost("p_vel", 0, 4 * sizeof(float) * buffer.size(), buffer.data());

  buffer = std::vector<std::array<float, 4>>(m_maxNbParticles, std::array<float, 4>({ 0.0f, 0.0f, 0.0f, 0.0f }));

  clContext.loadBufferFromHost("p_col", 0, 4 * sizeof(float) * buffer.size(), buffer.data());

  std::vector<int> lifeTime(m_maxNbParticles, -1);
  clContext.loadBufferFromHost("p_lifeTime", 0, sizeof(int) * lifeTime.size(), lifeTime.data());

  clContext.releaseGLBuffers({ "p_pos", "p_col" });
}

void Boids::releaseDeadParticles()
{
  std::for_each(m_particleLifes.begin(), m_particleLifes.end(), [&](int& lifeTime)
      {
        // Particle is alive
        if (lifeTime > 0)
        {
          --lifeTime;
        }
        // Particle is dead
        if (lifeTime == 0)
        {
          --m_currNbParticles;
          // Negative value -1 for buried particle
          --lifeTime;
        }
      });

  auto firstAliveIt = std::find_if(m_particleLifes.cbegin(), m_particleLifes.cend(), [](const int& lifeTime)
      { return lifeTime > 0; });

  // No particle alive, starting index must be 0
  if (firstAliveIt == m_particleLifes.cend())
    firstAliveIt = m_particleLifes.cbegin();

  m_firstParticleAliveIndex = std::distance(m_particleLifes.cbegin(), firstAliveIt);

  //LOG_INFO("first particle alive index {}", m_firstParticleAliveIndex);
}

// Must be called only if OpenGLBuffers p_pos and p_col has been acquired by OpenCL Context
void Boids::emitNewParticles()
{
  CL::Context& clContext = CL::Context::Get();

  //LOG_INFO("number of particle emitters {}", m_particleEmitters.size());

  // Filling global buffers by the end
  size_t nbTotalNewParticles = 0;
  std::vector<size_t> particleOffsetIndex;
  for (const auto& emitter : m_particleEmitters)
  {
    nbTotalNewParticles += emitter.getNbParticles();
    particleOffsetIndex.push_back(m_maxNbParticles - nbTotalNewParticles);
    //LOG_INFO("particle offset index {}", particleOffsetIndex.back());
  }

  size_t particleEmitterIndex = 0;
  for (const auto& emitter : m_particleEmitters)
  {
    size_t particleOffset = particleOffsetIndex[particleEmitterIndex++];

    size_t nbNewParticles = emitter.getNbParticles();
    int lifeTime = emitter.getParticlesLifeTime();

    m_particleLifes.insert(m_particleLifes.cbegin() + m_firstParticleAliveIndex + m_currNbParticles, nbNewParticles, lifeTime);

    std::vector<std::array<float, 4>> tempBuffer(nbNewParticles, std::array<float, 4>({ 0.0f, 0.0f, 0.0f, 0.0f }));

    std::vector<Math::float3> newParticlesPos = emitter.getParticlesPos();
    std::transform(newParticlesPos.cbegin(), newParticlesPos.cend(), tempBuffer.begin(),
        [](const Math::float3& pos) -> std::array<float, 4> { return { pos.x, pos.y, pos.z, 0.0f }; });

    clContext.loadBufferFromHost("p_pos", 4 * sizeof(float) * particleOffset, 4 * sizeof(float) * tempBuffer.size(), tempBuffer.data());

    std::vector<Math::float3> newParticlesVel = emitter.getParticlesVel();
    std::transform(newParticlesVel.cbegin(), newParticlesVel.cend(), tempBuffer.begin(),
        [](const Math::float3& vel) -> std::array<float, 4> { return { vel.x, vel.y, vel.z, 0.0f }; });

    clContext.loadBufferFromHost("p_vel", 4 * sizeof(float) * particleOffset, 4 * sizeof(float) * tempBuffer.size(), tempBuffer.data());

    std::vector<Math::float3> newParticlesCol = emitter.getParticlesCol();
    std::transform(newParticlesCol.cbegin(), newParticlesCol.cend(), tempBuffer.begin(),
        [](const Math::float3& vertCol) -> std::array<float, 4> { return { vertCol.x, vertCol.y, vertCol.z, 0.0f }; });

    clContext.loadBufferFromHost("p_col", 4 * sizeof(float) * particleOffset, 4 * sizeof(float) * tempBuffer.size(), tempBuffer.data());

    std::vector<int> newParticlesLife(nbNewParticles, lifeTime);
    clContext.loadBufferFromHost("p_lifeTime", sizeof(int) * particleOffset, sizeof(int) * newParticlesLife.size(), newParticlesLife.data());

    m_currNbParticles += nbNewParticles;
  }

  clearParticleEmitterList();
}

void Boids::update()
{
  if (!m_init)
    return;

  CL::Context& clContext = CL::Context::Get();

  clContext.acquireGLBuffers({ "p_pos", "p_col", "c_partDetector", "u_cameraPos" });

  if (!m_pause)
  {
    releaseDeadParticles();
    emitNewParticles();

    float timeStep = 0.1f;
    clContext.runKernel(KERNEL_FILL_CELL_ID, m_maxNbParticles);

    m_radixSort.sort("p_cellID", { "p_pos", "p_col", "p_vel", "p_acc" }, { "p_lifeTime" });

    clContext.runKernel(KERNEL_RESET_START_END_CELL, m_nbCells);
    clContext.runKernel(KERNEL_FILL_START_CELL, m_currNbParticles);
    clContext.runKernel(KERNEL_FILL_END_CELL, m_currNbParticles);

    if (m_simplifiedMode)
      clContext.runKernel(KERNEL_ADJUST_END_CELL, m_nbCells);

    if (m_dimension == Dimension::dim2D)
      clContext.runKernel(KERNEL_BOIDS_RULES_GRID_2D, m_currNbParticles);
    else
      clContext.runKernel(KERNEL_BOIDS_RULES_GRID_3D, m_currNbParticles);

    if (isTargetActivated())
    {
      m_target->updatePos(m_dimension, m_velocity);
      auto targetXYZ = m_target->pos();
      std::array<float, 4> targetPos = { targetXYZ.x, targetXYZ.y, targetXYZ.z, 0.0f };
      clContext.setKernelArg(KERNEL_ADD_TARGET_RULE, 1, sizeof(float) * 4, &targetPos);
      clContext.runKernel(KERNEL_ADD_TARGET_RULE, m_currNbParticles);
    }

    clContext.setKernelArg(KERNEL_UPDATE_VEL, 1, sizeof(float), &timeStep);
    clContext.runKernel(KERNEL_UPDATE_VEL, m_currNbParticles);

    switch (m_boundary)
    {
    case Boundary::CyclicWall:
      clContext.setKernelArg(KERNEL_UPDATE_POS_CYCLIC, 1, sizeof(float), &timeStep);
      clContext.runKernel(KERNEL_UPDATE_POS_CYCLIC, m_currNbParticles);
      break;
    case Boundary::BouncingWall:
      clContext.setKernelArg(KERNEL_UPDATE_POS_BOUNCING, 1, sizeof(float), &timeStep);
      clContext.runKernel(KERNEL_UPDATE_POS_BOUNCING, m_currNbParticles);
      break;
    }

    clContext.runKernel(KERNEL_RESET_PART_DETECTOR, m_nbCells);
    clContext.runKernel(KERNEL_FILL_PART_DETECTOR, m_currNbParticles);

    // WIP
    if (m_activeLifeTime)
      clContext.runKernel(KERNEL_UPDATE_LIFE_TIME, m_currNbParticles);
  }

  // WIP
  // clContext.runKernel(KERNEL_FILL_CAMERA_DIST, m_currNbParticles);

  // m_radixSort.sort("p_cameraDist", { "p_pos", "p_col", "p_vel", "p_acc" });

  clContext.releaseGLBuffers({ "p_pos", "p_col", "c_partDetector", "u_cameraPos" });
}