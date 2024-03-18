#include <SDL2/SDL_video.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const char* WINDOW_TITLE = "Simple Button Example";

bool isInsideButton(int x, int y, SDL_Rect button) {
    return (x >= button.x && x <= button.x + button.w &&
            y >= button.y && y <= button.y + button.h);
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;
    bool quit = false;
    int clickCount = 0;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set background color to white
    SDL_RenderClear(renderer);

    SDL_Rect buttonRect = { SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 25, 100, 50 };

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (isInsideButton(x, y, buttonRect)) {
                    clickCount++;
                    printf("Button clicked! Click count: %d\n", clickCount);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Clear screen
        SDL_RenderClear(renderer);

        // Draw the button
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Set button color to green
        SDL_RenderFillRect(renderer, &buttonRect);

        SDL_RenderPresent(renderer); // Update screen
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
