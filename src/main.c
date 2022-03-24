#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "./lib/vulkan/core/state.h"
#include "./lib/vulkan/initializer/initializer.h"

int main(int argc, char* args[]) {
    SDL_Window* window;

    SDL_Init(SDL_INIT_VIDEO);

    window =
        SDL_CreateWindow("Basic app", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_VULKAN);

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    VulkanState state = vulkan_state_create();
    vulkan_state_init(&state);

    bool is_running = true;

    while (is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    is_running = false;
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        is_running = false;
                    }
                    break;
            }
        }
    }

    vulkan_state_destroy(&state);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
