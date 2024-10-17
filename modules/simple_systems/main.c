#include "simple_systems.h"
#include <simple_components.h>

void MovePlayer(ecs_iter_t *it) {
  Position2 *pos = ecs_field(it, Position2, 0);
  Velocity *vel = ecs_field(it, Velocity, 1);

  for (int i = 0; i < it->count; i++) {
    pos[i].x += vel[i].x * it->delta_time;
    pos[i].y += vel[i].y * it->delta_time;
  }
}

void SimpleSystemsImport(ecs_world_t *world) {
  ECS_MODULE(world, SimpleSystems);
  ECS_IMPORT(world, SimpleComponents);

  ECS_SYSTEM(world, MovePlayer, EcsOnUpdate, [in] simple.components.Position2,
             [in] simple.components.Velocity);
}