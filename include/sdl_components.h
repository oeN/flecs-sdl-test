#ifndef SDL_COMPONENTS_H
#define SDL_COMPONENTS_H

// /* Convenience macro for exporting symbols */
// #ifndef sdl_components_STATIC
// #if defined(sdl_components_EXPORTS) && \
//     (defined(_MSC_VER) || defined(__MINGW32__))
// #define SDL_COMPONENTS_API __declspec(dllexport)
// #elif defined(sdl_components_EXPORTS)
// #define SDL_COMPONENTS_API __attribute__((__visibility__("default")))
// #elif defined(_MSC_VER)
// #define SDL_COMPONENTS_API __declspec(dllimport)
// #else
// #define SDL_COMPONENTS_API
// #endif
// #else
// #define SDL_COMPONENTS_API
// #endif

#include "flecs.h"
#include <SDL2/SDL.h>

// Your code here
typedef struct SDLContext {
  SDL_Window *window;
  SDL_Renderer *renderer;
  struct nk_context *nkctx;
  float font_scale;
} SDLContext;

// SDL_COMPONENTS_API
extern ECS_COMPONENT_DECLARE(SDLContext);

#define SDLContextInst (ecs_id(SDLContext))

#endif // SDL_COMPONENTS_H