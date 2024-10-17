#include "simple_components.h"

ECS_COMPONENT_DECLARE(Position2);
ECS_COMPONENT_DECLARE(Size2);
ECS_COMPONENT_DECLARE(Velocity);

void SimpleComponentsImport(ecs_world_t *world) {
  // Create the module entity. The PascalCase module name is translated to a
  // lower case path for the entity name, like "simple.components".
  ECS_MODULE(world, SimpleComponents);

  // All contents of the module are created inside the module's namespace, so
  // the Position component will be created as simple.components.Position2

  ECS_COMPONENT_DEFINE(world, Position2);
  ECS_COMPONENT_DEFINE(world, Size2);
  ECS_COMPONENT_DEFINE(world, Velocity);
}