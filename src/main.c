#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear.h"

#include <stdio.h>

#define NK_SDL_RENDERER_SDL_H <SDL2/SDL.h>
#include "nuklear_sdl_renderer.h"

#include "simple_components.h"
#include "simple_systems.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef struct SDLContext {
  SDL_Window *window;
  SDL_Renderer *renderer;
  struct nk_context *nkctx;
} SDLContext;

ecs_world_t *world = NULL;
float font_scale = 1;

// dependent on SDL
void draw_player(ecs_iter_t *it);
void clear_screen(ecs_iter_t *it);
void render(ecs_iter_t *it);
void setup_sdl(ecs_iter_t *it);
void destroy_sdl(ecs_world_t *wold, void *ctx);
void setup_nuklear(ecs_iter_t *it);
void draw_gui(ecs_iter_t *it);
void handle_input(ecs_iter_t *it);

ECS_COMPONENT_DECLARE(SDLContext);
#define SDLContextInst (ecs_id(SDLContext))

void setup_ecs() {
  // world = ecs_init();
  ecs_log_set_level(0);
  ECS_IMPORT(world, SimpleSystems);

  // Set ecs phases to accommodate SDL Draw Functions
  ecs_entity_t AfterSdlSetup = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t BeforeDraw = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t OnDraw = ecs_new_w_id(world, EcsPhase);
  ecs_entity_t AfterDraw = ecs_new_w_id(world, EcsPhase);

  ecs_add_pair(world, AfterSdlSetup, EcsDependsOn, EcsOnStart);
  ecs_add_pair(world, BeforeDraw, EcsDependsOn, EcsOnUpdate);
  ecs_add_pair(world, OnDraw, EcsDependsOn, BeforeDraw);
  ecs_add_pair(world, AfterDraw, EcsDependsOn, OnDraw);

  ECS_COMPONENT_DEFINE(world, SDLContext);

  ECS_SYSTEM(world, setup_sdl, EcsOnStart, [out] SDLContext());
  ECS_SYSTEM(world, setup_nuklear,
             AfterSdlSetup, [in] SDLContext(), [out] SDLContext());
  ECS_SYSTEM(world, handle_input, EcsOnUpdate, [in] SDLContext());
  ECS_SYSTEM(world, clear_screen, BeforeDraw, [in] SDLContext());
  ECS_SYSTEM(world, draw_player, OnDraw, simple.components.Position2,
             simple.components.Size2, [in] SDLContext());
  ECS_SYSTEM(world, draw_gui, OnDraw, [in] SDLContext());
  ECS_SYSTEM(world, render, AfterDraw, [in] SDLContext());

  ecs_entity_t e = ecs_insert(world, ecs_value(Position2, {100, 100}));
  ecs_set(world, e, Size2, {50, 50});
  ecs_set(world, e, Velocity, {200, 0});
}

int main(int argc, char *argv[]) {
  world = ecs_init_w_args(argc, argv);
  ECS_IMPORT(world, FlecsStats);

  setup_ecs();

  ecs_atfini(world, destroy_sdl, NULL);
  return ecs_app_run(world, &(ecs_app_desc_t){
                                // Optional, gather statistics for explorer
                                .enable_stats = 1,
                                .enable_rest = 1,
                            });

  // ecs_singleton_set(world, EcsRest, {0});
  // ecs_set_target_fps(world, 60);

  // return run_game();

  // return 0;
}

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
  ecs_set(world, SDLContextInst, SDLContext,
          {.window = window, .renderer = renderer});
  ecs_trace("Window: %p\n", window);
  ecs_trace("Renderer: %p\n", renderer);
  ecs_trace("SDL Context: %p\n", ecs_get(world, SDLContextInst, SDLContext));

  // TODO: Understand this code and use if necessary (it seems), copied from
  // https://github.com/Immediate-Mode-UI/Nuklear/blob/master/demo/sdl_renderer/main.c
  /* scale the renderer output for High-DPI displays */
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
}

void setup_nuklear(ecs_iter_t *it) {
  ecs_trace("Setting up Nuklear\n");
  ecs_world_t *world = it->world;
  const SDLContext *sdl_ctx = ecs_get(world, SDLContextInst, SDLContext);
  ecs_trace("SDL Context: %p\n", sdl_ctx);
  SDL_Window *win = sdl_ctx->window;
  SDL_Renderer *renderer = sdl_ctx->renderer;
  struct nk_context *ctx;

  /* GUI */
  ctx = nk_sdl_init(win, renderer);
  ecs_set(world, SDLContextInst, SDLContext,
          {.window = win, .renderer = renderer, .nkctx = ctx});
  /* Load Fonts: if none of these are loaded a default font will be used  */
  /* Load Cursor: if you uncomment cursor loading please hide the cursor */
  {
    struct nk_font_atlas *atlas;
    struct nk_font_config config = nk_font_config(0);
    struct nk_font *font;

    /* set up the font atlas and add desired font; note that font sizes are
     * multiplied by font_scale to produce better results at higher DPIs */
    nk_sdl_font_stash_begin(&atlas);
    font = nk_font_atlas_add_default(atlas, 13 * font_scale, &config);
    /*font = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/DroidSans.ttf", 14 * font_scale, &config);*/
    /*font = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/Roboto-Regular.ttf", 16 * font_scale, &config);*/
    /*font = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/kenvector_future_thin.ttf", 13 * font_scale,
     * &config);*/
    /*font = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/ProggyClean.ttf", 12 * font_scale, &config);*/
    /*font = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/ProggyTiny.ttf", 10 * font_scale, &config);*/
    /*font = nk_font_atlas_add_from_file(atlas,
     * "../../../extra_font/Cousine-Regular.ttf", 13 * font_scale, &config);*/
    nk_sdl_font_stash_end();

    /* this hack makes the font appear to be scaled down to the desired
     * size and is only necessary when font_scale > 1 */
    font->handle.height /= font_scale;
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    nk_style_set_font(ctx, &font->handle);
  }
}

void draw_player(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  Position2 *pos = ecs_field(it, Position2, 0);
  Size2 *size = ecs_field(it, Size2, 1);

  const SDLContext *ctx = ecs_get(world, SDLContextInst, SDLContext);
  SDL_Renderer *renderer = ctx->renderer;

  SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

  // Drawing each player
  for (int i = 0; i < it->count; i++) {
    // Creating a new Rectangle
    SDL_Rect r;
    r.x = pos[i].x;
    r.y = pos[i].y;
    r.w = size[i].x;
    r.h = size[i].y;
    SDL_RenderFillRect(renderer, &r);
  }
}

void draw_gui(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  const SDLContext *sdlctx = ecs_get(world, SDLContextInst, SDLContext);
  struct nk_context *ctx = sdlctx->nkctx;

  struct nk_colorf bg;

  bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
  /* GUI */
  if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
               NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                   NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
    enum { EASY, HARD };
    static int op = EASY;
    static int property = 20;

    nk_layout_row_static(ctx, 30, 80, 1);
    if (nk_button_label(ctx, "button"))
      fprintf(stdout, "button pressed\n");
    nk_layout_row_dynamic(ctx, 30, 2);
    if (nk_option_label(ctx, "easy", op == EASY))
      op = EASY;
    if (nk_option_label(ctx, "hard", op == HARD))
      op = HARD;
    nk_layout_row_dynamic(ctx, 25, 1);
    nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

    nk_layout_row_dynamic(ctx, 20, 1);
    nk_label(ctx, "background:", NK_TEXT_LEFT);
    nk_layout_row_dynamic(ctx, 25, 1);
    if (nk_combo_begin_color(ctx, nk_rgb_cf(bg),
                             nk_vec2(nk_widget_width(ctx), 400))) {
      nk_layout_row_dynamic(ctx, 120, 1);
      bg = nk_color_picker(ctx, bg, NK_RGBA);
      nk_layout_row_dynamic(ctx, 25, 1);
      bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
      bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
      bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
      bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
      nk_combo_end(ctx);
    }
  }
  nk_end(ctx);
}

void clear_screen(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  const SDLContext *ctx = ecs_get(world, SDLContextInst, SDLContext);
  SDL_Renderer *renderer = ctx->renderer;

  SDL_SetRenderDrawColor(renderer, 173, 216, 230, 255); // light blue color
  SDL_RenderClear(renderer);

  // TODO: move me in another system?
  nk_sdl_render(NK_ANTI_ALIASING_ON);
}

void render(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  const SDLContext *ctx = ecs_get(world, SDLContextInst, SDLContext);
  SDL_Renderer *renderer = ctx->renderer;

  SDL_RenderPresent(renderer);
}

void handle_input(ecs_iter_t *it) {
  ecs_world_t *world = it->world;
  const SDLContext *ctx = ecs_get(world, SDLContextInst, SDLContext);
  struct nk_context *nkctx = ctx->nkctx;

  SDL_Event e;
  nk_input_begin(nkctx);
  // since this whole method is called every frame we don't need to
  while (SDL_PollEvent(&e)) {
    // SDL_PollEvent(&e);
    switch (e.type) {
    case SDL_QUIT:
      // ecs_trace("Quitting\n");
      // FIXME: it works with ecs_app_run but it doesn't call the destroy_sdl
      // function registered with ecs_atfini
      ecs_quit(world);
      // ecs_fini(world);
      break;
    default:
      nk_sdl_handle_event(&e);
    }
  }
  nk_sdl_handle_grab();
  nk_input_end(nkctx);
}

void destroy_sdl(ecs_world_t *world, void *ctx) {
  ecs_trace("Destroying SDL\n");
  const SDLContext *sdl_ctx = ecs_get(world, SDLContextInst, SDLContext);

  ecs_trace("Window: %p\n", sdl_ctx->window);
  ecs_trace("Renderer: %p\n", sdl_ctx->renderer);

  nk_sdl_shutdown();
  SDL_DestroyRenderer(sdl_ctx->renderer);
  SDL_DestroyWindow(sdl_ctx->window);
  SDL_Quit();
}