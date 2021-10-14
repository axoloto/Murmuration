#pragma once

#include <map>
#include <string>

namespace Physics
{
// List of supported physical models
enum ModelType
{
  BOIDS = 0,
  FLUIDS = 1
};

struct CompareModelType
{
  bool operator()(const ModelType& modelA, const ModelType& modelB) const
  {
    return (int)modelA < (int)modelB;
  }
};

static const std::map<ModelType, std::string, CompareModelType> ALL_MODELS {
  { ModelType::BOIDS, "Boids" }, // Craig Reynolds laws
  { ModelType::FLUIDS, "Fluids" }, // Position Based Fluids by NVIDIA team (Macklin and Muller)
};

enum class Dimension
{
  dim2D,
  dim3D
};

enum class Boundary
{
  BouncingWall,
  CyclicWall
};
}