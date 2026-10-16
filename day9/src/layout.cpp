#include "layout.hpp"

namespace perf9 {

ParticlesAoS MakeAoS(std::size_t n) {
  ParticlesAoS p;
  p.items.resize(n);
  for (std::size_t i = 0; i < n; ++i) {
    p.items[i].x = static_cast<float>(i);
    p.items[i].vx = 1.0f;
    p.items[i].tag[0] = 'A';
  }
  return p;
}

ParticlesSoA MakeSoA(std::size_t n) {
  ParticlesSoA p;
  p.x.resize(n);
  p.y.resize(n, 0.0f);
  p.z.resize(n, 0.0f);
  p.vx.resize(n, 1.0f);
  p.vy.resize(n, 0.0f);
  p.vz.resize(n, 0.0f);
  for (std::size_t i = 0; i < n; ++i) p.x[i] = static_cast<float>(i);
  return p;
}

std::vector<EntityHotColdBad> MakeEntitiesBad(std::size_t n) {
  std::vector<EntityHotColdBad> v(n);
  for (std::size_t i = 0; i < n; ++i) {
    v[i].pos[0] = static_cast<float>(i);
    v[i].name[0] = 'x';
  }
  return v;
}

std::vector<EntityHotColdGood> MakeEntitiesGood(std::size_t n) {
  std::vector<EntityHotColdGood> v(n);
  for (std::size_t i = 0; i < n; ++i) {
    v[i].pos[0] = static_cast<float>(i);
    v[i].name[0] = 'x';
  }
  return v;
}

void IntegrateAoS(ParticlesAoS& p, float dt) {
  for (auto& item : p.items) {
    item.x += item.vx * dt;
    item.y += item.vy * dt;
    item.z += item.vz * dt;
  }
}

void IntegrateSoA(ParticlesSoA& p, float dt) {
  const std::size_t n = p.x.size();
  for (std::size_t i = 0; i < n; ++i) {
    p.x[i] += p.vx[i] * dt;
    p.y[i] += p.vy[i] * dt;
    p.z[i] += p.vz[i] * dt;
  }
}

float SumPositionsBad(const std::vector<EntityHotColdBad>& e) {
  float s = 0.0f;
  for (const auto& item : e) s += item.pos[0] + item.pos[1] + item.pos[2];
  return s;
}

float SumPositionsGood(const std::vector<EntityHotColdGood>& e) {
  float s = 0.0f;
  for (const auto& item : e) s += item.pos[0] + item.pos[1] + item.pos[2];
  return s;
}

}  // namespace perf9
