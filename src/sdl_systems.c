#include "sdl_systems.h"
#include "game_common.h"
#include "sdl_components.h"

void setup_sdl(ecs_iter_t *it) {
  ecs_trace("Setting up SDL");

  ecs_world_t *world = it->world;

  SDL_Window *window;
  int flags = 0;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    ecs_err("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
  }

  window = SDL_CreateWindow("Flecs Demo", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    ecs_err("Window could not be created! SDL_Error: %s\n", SDL_GetError());
  }

  flags |= SDL_RENDERER_ACCELERATED;
  flags |= SDL_RENDERER_PRESENTVSYNC;

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, flags);
  if (renderer == NULL) {
    ecs_err("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
  }

  ecs_trace("Context ready");

  // TODO: Understand this code and use if necessary (it seems), copied from
  // https://github.com/Immediate-Mode-UI/Nuklear/blob/master/demo/sdl_renderer/main.c
  /* scale the renderer output for High-DPI displays */
  float font_scale;
  {
    int render_w, render_h;
    int window_w, window_h;
    float scale_x, scale_y;
    SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
    SDL_GetWindowSize(window, &window_w, &window_h);
    scale_x = (float)(render_w) / (float)(window_w);
    scale_y = (float)(render_h) / (float)(window_h);
    SDL_RenderSetScale(renderer, scale_x, scale_y);
    font_scale = scale_y;
  }

  // struct nk_colorf bg;
  // bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

  ecs_set(world, SDLContextInst, SDLContext,
          {
              .window = window, .renderer = renderer, .font_scale = font_scale,
              //  .bg = bg
          });
  ecs_trace("Window: %p\n", window);
  ecs_trace("Renderer: %p\n", renderer);
  ecs_trace("SDL Context: %p\n", ecs_get(world, SDLContextInst, SDLContext));
}

void clear_screen(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  const SDLContext *ctx = ecs_get(world, SDLContextInst, SDLContext);
  SDL_Renderer *renderer = ctx->renderer;
  // struct nk_colorf bg = ctx->bg;

  // SDL_SetRenderDrawColor(renderer, bg.r * 255, bg.g * 255, bg.b * 255,
  //                        bg.a * 255);
  SDL_SetRenderDrawColor(renderer, 144, 238, 144, 255); // Light green color
  SDL_RenderClear(renderer);

  // TODO: move me in another system
  // nk_sdl_render(NK_ANTI_ALIASING_ON);
}

void render(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  const SDLContext *ctx = ecs_get(world, SDLContextInst, SDLContext);
  SDL_Renderer *renderer = ctx->renderer;

  SDL_RenderPresent(renderer);
}

void destroy_sdl(ecs_world_t *world, void *ctx) {
  ecs_trace("Destroying SDL\n");
  const SDLContext *sdl_ctx = ecs_get(world, SDLContextInst, SDLContext);

  ecs_trace("Window: %p\n", sdl_ctx->window);
  ecs_trace("Renderer: %p\n", sdl_ctx->renderer);

  // TODO: move me in another system
  // nk_sdl_shutdown();
  SDL_DestroyRenderer(sdl_ctx->renderer);
  SDL_DestroyWindow(sdl_ctx->window);
  SDL_Quit();
}