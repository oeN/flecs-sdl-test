#ifndef SIMPLE_COMPONENTS_H
#define SIMPLE_COMPONENTS_H

#include <flecs.h>

// Component types
typedef struct Vector2 {
  float x;
  float y;
} Position2, Size2, Velocity;

// Global variables that hold component ids
extern ECS_COMPONENT_DECLARE(Position2);
extern ECS_COMPONENT_DECLARE(Size2);
extern ECS_COMPONENT_DECLARE(Velocity);

// Module import function
void SimpleComponentsImport(ecs_world_t *world);

#endif