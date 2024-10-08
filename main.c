#include <SDL2/SDL.h>
#include <flecs.h>
#include <stdio.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef struct Vector2 {
  float x;
  float y;
} Position2, Size2;

ecs_world_t *world = NULL;

void move_player(ecs_iter_t *it);
void draw_player(ecs_iter_t *it);
void setup_draw(ecs_iter_t *it);
void render(ecs_iter_t *it);

void main_loop() {
  int quit = 0;
  SDL_Event e;

  while (quit == 0) {
    ecs_progress(world, 0);
    while (SDL_PollEvent(&e)) {
      printf("Event Type: %d\n", e.type);
      switch (e.type) {
      case SDL_QUIT:
        quit = 1;
        break;
      }
    }
  }
}

int run_game(SDL_Window *window) {
  world = ecs_init();
  ecs_set_ctx(world, window, NULL);

  // Set ecs phases to accommodate SDL Draw Functions
  ecs_entity_t BeforeDraw = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t OnDraw = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t AfterDraw = ecs_new_w_id(world, EcsPhase);

  ecs_add_pair(world, BeforeDraw, EcsDependsOn, EcsOnUpdate);
  ecs_add_pair(world, OnDraw, EcsDependsOn, BeforeDraw);
  ecs_add_pair(world, AfterDraw, EcsDependsOn, OnDraw);

  ECS_COMPONENT(world, Position2);
  ECS_COMPONENT(world, Size2);

  ECS_SYSTEM(world, move_player, EcsOnUpdate, Position2);
  ECS_SYSTEM(world, setup_draw, BeforeDraw, 0);
  ECS_SYSTEM(world, draw_player, OnDraw, Position2, Size2);
  ECS_SYSTEM(world, render, AfterDraw, 0);

  ECS_ENTITY(world, Player, 0);
  ecs_set(world, Player, Position2, {100, 100});
  ecs_set(world, Player, Size2, {50, 50});

  printf("Running Game\n");
  main_loop();
  printf("Game Over\n");

  return ecs_fini(world);
}

int main(int argc, char *args[]) {
  SDL_Window *window;
  // TODO: use the SDL_Renderer instead of SDL_Surface
  SDL_Surface *screenSurface;

  // TODO: initialize the SDL stuffs in a OnStart system
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return -1;
  }

  window = SDL_CreateWindow("Flecs Demo", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return -2;
  }

  run_game(window);

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void draw_player(ecs_iter_t *it) {
  Position2 *pos = ecs_field(it, Position2, 0);
  Size2 *size = ecs_field(it, Size2, 1);

  SDL_Window *window = ecs_get_ctx(it->world);
  SDL_Surface *screenSurface = SDL_GetWindowSurface(window);

  // Drawing each player
  for (int i = 0; i < it->count; i++) {
    // Creating a new Rectangle
    SDL_Rect r;
    r.x = pos[i].x;
    r.y = pos[i].y;
    r.w = size[i].x;
    r.h = size[i].y;

    SDL_FillRect(screenSurface, &r,
                 SDL_MapRGB(screenSurface->format, 0xFF, 0x00, 0x00));
  }
}

void move_player(ecs_iter_t *it) {
  Position2 *pos = ecs_field(it, Position2, 0);
  for (int i = 0; i < it->count; i++) {
    pos[i].x += 5;
    // pos[i].y += 10;
  }
}

void setup_draw(ecs_iter_t *it) {
  SDL_Window *window = ecs_get_ctx(it->world);
  SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
  SDL_FillRect(screenSurface, NULL,
               SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
  // SDL_RenderClear(ctx->screenSurface);
}

void render(ecs_iter_t *it) {
  SDL_Window *window = ecs_get_ctx(it->world);
  // SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
  // SDL_RenderPresent(ctx->screenSurface);
  SDL_UpdateWindowSurface(window);
}
