#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char* args[]) {
    SDL_Window* window;

    SDL_Init(SDL_INIT_VIDEO);

    window =
        SDL_CreateWindow("Basic app", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_VULKAN);

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

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

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
