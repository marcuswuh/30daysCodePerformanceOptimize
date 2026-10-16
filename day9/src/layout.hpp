#pragma once

#include <cstddef>
#include <vector>

namespace perf9 {

// AoS：更新速度时仍加载 tag 等冷数据
struct ParticleAoS {
  float x, y, z;
  float vx, vy, vz;
  char tag[32];
};

struct ParticlesAoS {
  std::vector<ParticleAoS> items;
};

// SoA：仅触摸速度/位置数组
struct ParticlesSoA {
  std::vector<float> x, y, z;
  std::vector<float> vx, vy, vz;
};

// 冷热分离：热字段打包在前
struct EntityHotColdBad {
  char name[64];
  float pos[3];
  float vel[3];
  int id;
  char padding[128];
};

struct EntityHotColdGood {
  float pos[3];
  float vel[3];
  int id;
  char name[64];
};

ParticlesAoS MakeAoS(std::size_t n);
ParticlesSoA MakeSoA(std::size_t n);
std::vector<EntityHotColdBad> MakeEntitiesBad(std::size_t n);
std::vector<EntityHotColdGood> MakeEntitiesGood(std::size_t n);

void IntegrateAoS(ParticlesAoS& p, float dt);
void IntegrateSoA(ParticlesSoA& p, float dt);
float SumPositionsBad(const std::vector<EntityHotColdBad>& e);
float SumPositionsGood(const std::vector<EntityHotColdGood>& e);

}  // namespace perf9
