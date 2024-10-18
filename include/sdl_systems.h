#ifndef SDL_SYSTEMS_H
#define SDL_SYSTEMS_H

#include "flecs.h"

void clear_screen(ecs_iter_t *it);
void render(ecs_iter_t *it);
void setup_sdl(ecs_iter_t *it);
void destroy_sdl(ecs_world_t *wold, void *ctx);

#endif // SDL_SYSTEMS_H