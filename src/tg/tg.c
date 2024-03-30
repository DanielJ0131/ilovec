#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const char* WINDOW_TITLE = "TrackGuesser";

// Taken from lg.c
// Structure to hold both array and number of elements
typedef struct {
    char **array;
    int count;
} LyricsStruct;

// Function to generate a list and amount of entries in a directory
LyricsStruct generateTrackList(const char *directory) {
    DIR *dir;
    struct dirent *entry;
    
    // Open the directory
    dir = opendir(directory);
    if (!dir) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // Count the number of files in the directory
    int fileCount = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            fileCount++;
        }
    }
    printf("Files found: %d\n", fileCount);
    printf("========================\n\n");

    // Allocate memory for an array of strings
    char **lyricsList = (char**)malloc((size_t)fileCount * sizeof(char*));
    if (!lyricsList) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Reset the directory stream and iterate through files again
    rewinddir(dir);

    int index = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            // Allocate memory for each string without .mp3
            lyricsList[index] = (char*)malloc((strlen(entry->d_name) - 3) * sizeof(char));
            if (!lyricsList[index]) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }

            // Construct the string without .mp3
            size_t filenameLength = strlen(entry->d_name);
            entry->d_name[filenameLength - 4] = '\0';

            strcpy(lyricsList[index], entry->d_name);

            index++;
        }
    }

    closedir(dir);

    LyricsStruct result;
    result.array = lyricsList;
    result.count = fileCount;

    return result;
}

bool isInsideButton(int x, int y, SDL_Rect button) {
    return (x >= button.x && x <= button.x + button.w &&
            y >= button.y && y <= button.y + button.h);
}

void renderText(SDL_Renderer* renderer, int x, int y, const char *str, TTF_Font* font) {

    //int length = strlen(str);
    //if (length > 15) {
    //    int start = 0; // Starting index of current substring
    //    int end = 0;   // Ending index of current substring
    //    int substringIndex = 0;
    //    char *substrings[2];

    //    while (start < length) {
    //        // Move end to last possible character or end of string, whichever comes first
    //        end = start + 14;
    //        if (end >= length) {
    //            end = length - 1;
    //        } else {
    //            // Move end backward until a space is found or the next character is a space
    //            end++;
    //            while (end > start && str[end] != ' ') {
    //                end--;
    //            }
    //        }
    //        // Allocate memory for the substring
    //        substrings[substringIndex] = malloc((end - start + 2) * sizeof(char)); // +2 for the null terminator
    //    
    //        // Copy the substring into the allocated memory
    //        strncpy(substrings[substringIndex], str + start, end - start + 1);
    //        substrings[substringIndex][end - start + 1] = '\0'; // Null-terminate the substring
    //    
    //        // Move start to the next non-space character after end
    //        while (end < length && str[end] == ' ') {
    //            end++;
    //        }
    //        start = end; // Set start to end for the next substring
    //        substringIndex++; // Move to the next index in the substrings array
    //    }
    //}

    SDL_Color color = {0, 0, 0}; // Black color
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, str, color);
    if (!textSurface) {
        perror("Error rendering surface");
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface); // Free the surface after creating the texture

    // Get the dimensions of the rendered text
    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

    // Create a destination rectangle for rendering the text texture
    SDL_Rect textRect = {x, y, textWidth, textHeight};

    // Render the text texture onto the screen
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);

}

int main() {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* backgroundTexture = NULL;
    SDL_Surface* backgroundSurface = NULL;
    SDL_Event event;
    bool quit = false;
    int clickCountTopLeft = 0;
    int clickCountTopRight = 0;
    int clickCountBotLeft = 0;
    int clickCountBotRight = 0;

    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3) {
        perror("Error initializing SDL_mixer");
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        perror("Error initializing audio device");
    }

    if (TTF_Init() == -1) {
        perror("Error initializing SDL_ttf");
    }

    TTF_Font* font = TTF_OpenFont("fonts/PublicPixel-z84yD.ttf", 12);
    if (!font) {
        perror("Error loading font");
    }

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    backgroundSurface = IMG_Load("img/backgroundv2.jpg");
    if (!backgroundSurface) {
        printf("Error loading background image: %s\n", IMG_GetError());
        return 1;
    }

    // Convert surface to texture
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);

    // Render background texture
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    SDL_Rect buttonTopLeft = { WINDOW_WIDTH * 9/160, WINDOW_HEIGHT * 9/16, WINDOW_WIDTH * 71/160, WINDOW_HEIGHT * 11/80 };
    SDL_Rect buttonTopRight = { WINDOW_WIDTH * 41/80, WINDOW_HEIGHT * 9/16, WINDOW_WIDTH * 71/160, WINDOW_HEIGHT * 11/80 };
    SDL_Rect buttonBotLeft = { WINDOW_WIDTH * 3/80, WINDOW_HEIGHT * 35/48, WINDOW_WIDTH * 71/160, WINDOW_HEIGHT * 11/80 };
    SDL_Rect buttonBotRight = { WINDOW_WIDTH * 1/2, WINDOW_HEIGHT * 35/48, WINDOW_WIDTH * 71/160, WINDOW_HEIGHT * 11/80 };

    // int buttonPositions[4][4] = {
    // {WINDOW_WIDTH / 2 - 250, WINDOW_HEIGHT / 2 - 75, textWidth + 32, textHeight + 32},   // Button 1
    // {WINDOW_WIDTH / 2 + 25, WINDOW_HEIGHT / 2 - 75, 100, 50},  // Button 2
    // {WINDOW_WIDTH / 2 - 125, WINDOW_HEIGHT / 2 + 25, 100, 50},   // Button 3
    // {WINDOW_WIDTH / 2 + 25, WINDOW_HEIGHT / 2 + 25, 100, 50}   // Button 4
    // };

    // SDL_Rect buttonRects[4];
    // for (int i = 0; i < 4; ++i) {
    // buttonRects[i].x = buttonPositions[i][0];
    // buttonRects[i].y = buttonPositions[i][1];
    // buttonRects[i].w = buttonPositions[i][2];
    // buttonRects[i].h = buttonPositions[i][3];
    // }

    // Define the paths to the lyrics
    const char *directory = "tracks"; // Set directory path
    LyricsStruct trackList = generateTrackList(directory);

    srand(time(NULL));

    int randomTrackIndex = rand() % trackList.count;

    // Allocate memory for entire path including ".mp3" and "/"
    char *trackPath = (char*)malloc((strlen(directory) + strlen(trackList.array[randomTrackIndex]) + 6) * sizeof(char));

    // Prefix directory path to chosen .mp3 file
    sprintf(trackPath, "%s/%s.mp3", directory, trackList.array[randomTrackIndex]);

    Mix_Music* track = Mix_LoadMUS(trackPath);

    // Draw button hitboxes (debug)
    // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Set button color to green
    // SDL_RenderFillRect(renderer, &buttonTopLeft);
    // SDL_RenderFillRect(renderer, &buttonTopRight);
    // SDL_RenderFillRect(renderer, &buttonBotLeft);
    // SDL_RenderFillRect(renderer, &buttonBotRight);

    renderText(renderer, WINDOW_WIDTH * 3 / 16, WINDOW_HEIGHT * 71 / 120, "WWWWWWWWWWWWW", font); // TopLeft
    renderText(renderer, WINDOW_WIDTH * 41 / 64, WINDOW_HEIGHT * 71 / 120, "WWWWWWWWWWWWWW", font); // TopRight
    renderText(renderer, WINDOW_WIDTH * 3 / 16, WINDOW_HEIGHT * 121 / 160, "Obama Was HereX", font); // BotLeft
    renderText(renderer, WINDOW_WIDTH * 41 / 64, WINDOW_HEIGHT * 121 / 160, "Obama Was HereXX", font); // BotRight
    SDL_RenderPresent(renderer); // Update screen

    while (!quit) {
        Uint32 frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (isInsideButton(x, y, buttonTopLeft)) {
                    clickCountTopLeft++;
                    printf("ButtonTopLeft clicked! Click count: %d\n", clickCountTopLeft);
                }
                if (isInsideButton(x, y, buttonTopRight)) {
                    clickCountTopRight++;
                    printf("ButtonTopRight clicked! Resuming song. Click count: %d\n", clickCountTopRight);
                    Mix_ResumeMusic();
                }
                if (isInsideButton(x, y, buttonBotLeft)) {
                    clickCountBotLeft++;
                    printf("ButtonBotLeft clicked! Pausing song. Click count: %d\n", clickCountBotLeft);
                    Mix_PauseMusic();
                }
                if (isInsideButton(x, y, buttonBotRight)) {
                    clickCountBotRight++;
                    printf("ButtonBotRight clicked! Playing song. Click count: %d\n", clickCountBotRight);
                    Mix_PlayMusic(track, -1);
                }
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        clickCountTopLeft++;
                        printf("ButtonTopLeft clicked! Click count: %d\n", clickCountTopLeft);
                        break;
                    case SDLK_2:
                        clickCountTopRight++;
                        printf("ButtonTopRight clicked! Resuming song. Click count: %d\n", clickCountTopRight);
                        Mix_ResumeMusic();
                        break;
                    case SDLK_3:
                        clickCountBotLeft++;
                        printf("ButtonBotLeft clicked! Pausing song. Click count: %d\n", clickCountBotLeft);
                        Mix_PauseMusic();
                        break;
                    case SDLK_4:
                        clickCountBotRight++;
                        printf("ButtonBotRight clicked! Playing song. Click count: %d\n", clickCountBotRight);
                        Mix_PlayMusic(track, -1);
                        break;
                }
            }
        }
        // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Clear screen
        // SDL_RenderClear(renderer);

        // Calculate the time taken for rendering this frame
        Uint32 frameTime = SDL_GetTicks() - frameStart;

        // Delay to control the frame rate
        if (frameTime < 16) { // Targeting 60 FPS (1000ms / 60 = 16.666ms)
            SDL_Delay(16 - frameTime);
        }
    }

    SDL_DestroyTexture(backgroundTexture);
    IMG_Quit();
    SDL_Quit();

    TTF_CloseFont(font);
    TTF_Quit();

    Mix_FreeMusic(track);
    Mix_CloseAudio();
    Mix_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}