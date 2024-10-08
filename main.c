#include <SDL2/SDL.h>
#include <flecs.h>
#include <stdio.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef struct Vector2 {
  float x;
  float y;
} Position2, Size2;

typedef struct SDLContext {
  SDL_Window *window;
  SDL_Renderer *renderer;
} SDLContext;

ecs_world_t *world = NULL;

void move_player(ecs_iter_t *it);
void draw_player(ecs_iter_t *it);
void setup_draw(ecs_iter_t *it);
void render(ecs_iter_t *it);
void setup_sdl(ecs_iter_t *it);
void destroy_sdl(ecs_world_t *wold, void *ctx);

ECS_COMPONENT_DECLARE(SDLContext);
#define SDLContextInst (ecs_id(SDLContext))

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

int run_game() {
  // world = ecs_init();
  ecs_log_set_level(0);

  // Set ecs phases to accommodate SDL Draw Functions
  ecs_entity_t BeforeDraw = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t OnDraw = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t AfterDraw = ecs_new_w_id(world, EcsPhase);

  ecs_add_pair(world, BeforeDraw, EcsDependsOn, EcsOnUpdate);
  ecs_add_pair(world, OnDraw, EcsDependsOn, BeforeDraw);
  ecs_add_pair(world, AfterDraw, EcsDependsOn, OnDraw);

  ECS_COMPONENT_DEFINE(world, SDLContext);
  ECS_COMPONENT(world, Position2);
  ECS_COMPONENT(world, Size2);
  ECS_COMPONENT(world, SDLContext);

  ECS_SYSTEM(world, setup_sdl, EcsOnStart, 0);
  ECS_SYSTEM(world, move_player, EcsOnUpdate, Position2);
  ECS_SYSTEM(world, setup_draw, BeforeDraw, 0);
  ECS_SYSTEM(world, draw_player, OnDraw, Position2, Size2);
  ECS_SYSTEM(world, render, AfterDraw, 0);

  ECS_ENTITY(world, Player, 0);
  ecs_set(world, Player, Position2, {100, 100});
  ecs_set(world, Player, Size2, {50, 50});

  ecs_trace("Running Game\n");
  main_loop();
  ecs_trace("Game Over\n");

  ecs_atfini(world, destroy_sdl, NULL);

  return ecs_fini(world);
}

int main(int argc, char *argv[]) {
  world = ecs_init_w_args(argc, argv);
  ECS_IMPORT(world, FlecsStats);

  ecs_singleton_set(world, EcsRest, {0});

  run_game();

  return 0;
}

void setup_sdl(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  ecs_trace("Setting up SDL");
  SDL_Window *window;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    ecs_err("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
  }

  window = SDL_CreateWindow("Flecs Demo", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    ecs_err("Window could not be created! SDL_Error: %s\n", SDL_GetError());
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    ecs_err("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
  }

  ecs_trace("Context ready");
  ecs_set(world, SDLContextInst, SDLContext,
          {.window = window, .renderer = renderer});
  ecs_trace("Window: %p\n", window);
  ecs_trace("Renderer: %p\n", renderer);

  // ecs_singleton_set(it->world, SDLContext,
  //                   {.window = window, .renderer = renderer});
}

void destroy_sdl(ecs_world_t *world, void *ctx) {
  (void)ctx;
  ecs_trace("Destroying SDL\n");
  const SDLContext *sdl_ctx = ecs_get(world, SDLContextInst, SDLContext);

  ecs_trace("Window: %p\n", sdl_ctx->window);
  ecs_trace("Renderer: %p\n", sdl_ctx->renderer);

  SDL_DestroyWindow(sdl_ctx->window);
  SDL_DestroyRenderer(sdl_ctx->renderer);
  SDL_Quit();
}

void draw_player(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  Position2 *pos = ecs_field(it, Position2, 0);
  Size2 *size = ecs_field(it, Size2, 1);

  const SDLContext *ctx = ecs_get(world, SDLContextInst, SDLContext);
  SDL_Renderer *renderer = ctx->renderer;

  // Drawing each player
  for (int i = 0; i < it->count; i++) {
    // Creating a new Rectangle
    SDL_Rect r;
    r.x = pos[i].x;
    r.y = pos[i].y;
    r.w = size[i].x;
    r.h = size[i].y;

    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &r);
  }
}

void move_player(ecs_iter_t *it) {
  Position2 *pos = ecs_field(it, Position2, 0);
  // printf("Time: %f\n", it->delta_time);
  for (int i = 0; i < it->count; i++) {
    pos[i].x += 10 * it->delta_time;
    // pos[i].y += 10;
  }
}

void setup_draw(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  const SDLContext *ctx = ecs_get(world, SDLContextInst, SDLContext);
  SDL_Renderer *renderer = ctx->renderer;

  SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255); // light blue color
  SDL_RenderClear(renderer);
}

void render(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  const SDLContext *ctx = ecs_get(world, SDLContextInst, SDLContext);
  SDL_Renderer *renderer = ctx->renderer;

  SDL_RenderPresent(renderer);
}
