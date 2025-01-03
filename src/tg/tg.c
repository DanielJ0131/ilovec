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

bool isInsideButton(int x, int y, SDL_Rect button) {
    return (x >= button.x && x <= button.x + button.w &&
            y >= button.y && y <= button.y + button.h);
}

int menu(SDL_Event event, bool quit, SDL_Rect buttons[4]) {

    while (!quit) {
        Uint32 frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (isInsideButton(x, y, buttons[0])) {
                    return 0;
                }
                if (isInsideButton(x, y, buttons[1])) {
                    quit = true;
                    return 1;
                }
                if (isInsideButton(x, y, buttons[2])) {
                    quit = true;
                    return 1;
                }
                if (isInsideButton(x, y, buttons[3])) {
                }
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        return 0;
                        break;
                    case SDLK_2:
                        quit = true;
                        return 1;
                        break;
                    case SDLK_3:
                        quit = true;
                        return 1;
                        break;
                    case SDLK_4:
                        break;
                    case SDLK_5:
                        quit = true;
                        return 1;
                        break;
                }
            }
        }

        // Calculate the time taken for rendering this frame
        Uint32 frameTime = SDL_GetTicks() - frameStart;

        // Delay to control the frame rate
        if (frameTime < 16) {
            SDL_Delay(16 - frameTime);
        }
    }
    return 1;

}

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

// Function to free the memory allocated for the list of strings
void freeLyricsStruct(char **array, int count) {
    for (int i = 0; i < count; i++) {
        free(array[i]);
    }
    free(array);
}

char** createSubstrings(const char *str, int maxLength) {

    int length = strlen(str);
    int numSubstrings = length / maxLength; // Integer division to calculate the number of full substrings
    if (length % maxLength != 0) {
        numSubstrings++; // Increment the number of substrings if there's a remainder
    }
    char **substrings = malloc((numSubstrings + 2) * sizeof(char*));
    if (substrings == NULL) {
        fprintf(stderr, "Memory allocation failed for substrings\n");
        exit(1); // Exit the program with an error code
    }

    if (numSubstrings > 1) {

        int start = 0; // Starting index of current substring
        int end = 0;   // Ending index of current substring
        int substringIndex = 0;

        while (start < length) {
            // Move end to last possible character or end of string, whichever comes first
            end = start + maxLength - 1;
            if (end >= length - 1) {
                end = length - 1;
            } else {
                // Move end backward until a space is found
                end++; // Check after last character, if end is at the end of a word
                while (end > start && str[end] != ' ') {
                    end--;
                }
                end--; // Place end on last character of the word
            }

            // Allocate memory for the substring
            substrings[substringIndex] = malloc((end - start + 2) * sizeof(char)); // +2 for null terminator and 0th index
            if (substrings[substringIndex] == NULL) {
                fprintf(stderr, "Memory allocation failed for substring %d\n", substringIndex);
                exit(1); // Exit the program with an error code
            }
        
            // Copy the substring into the allocated memory
            strncpy(substrings[substringIndex], str + start, end - start + 1);
            substrings[substringIndex][end - start + 1] = '\0'; // Null-terminate the substring

            // Print debugging information
            printf("Substring %d: \"%s\"\n", substringIndex+1, substrings[substringIndex]);

            // Move start to the next non-space character after end
            start = end + 1;
            while (start < length && str[start] == ' ') {
                start++;
            }

            substringIndex++; // Move to the next index in the substrings array
        }
        substrings[substringIndex] = NULL;
    
    } else {
        substrings[0] = malloc((length + 1) * sizeof(char));
        if (substrings[0] == NULL) {
            fprintf(stderr, "Memory allocation failed for substring 0\n");
            exit(1); // Exit the program with an error code
        }
        strncpy(substrings[0], str, length + 1);
        substrings[0][length] = '\0'; // Null-terminate the substring
        substrings[1] = NULL;
    }
    return substrings;

}

void renderText(SDL_Renderer* renderer, int x, int y, const char *str, TTF_Font* font, int maxLength) {

    char **substrings = createSubstrings(str, maxLength);

    for (int i = 0; substrings[i] != NULL; i++) {

        SDL_Color color = {0, 0, 0}; // Black color
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, substrings[i], color);
        if (!textSurface) {
            perror("Error rendering surface");
        }

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface); // Free the surface after creating the texture

        // Get the dimensions of the rendered text
        int textWidth, textHeight;
        SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

        // Create a destination rectangle for rendering the text texture scaling with substrings rendered
        int scaledY = y + (textHeight * i);
        SDL_Rect textRect = {x, scaledY, textWidth, textHeight};

        // Render the text texture onto the screen
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_DestroyTexture(textTexture);

        // Free the memory allocated for each substring
        free(substrings[i]);
    }
    free(substrings);
    printf("String complete.\n");

}

int* generateUniqueIndices(int trackAmount, int correctIndex, int *array, int correctChoice) {

    int optionsAmount = 4;
    int i, j, isUnique;

    // Set correct answer
    array[correctChoice] = correctIndex;
    printf("correctindex = %d\n", correctIndex);

    // Choose unique indices
    for (i = 0; i < (optionsAmount); i++) {
        if (i == correctChoice) continue;

        do {
            array[i] = rand() % trackAmount;
            isUnique = 1; // Assume number is unique

            if (array[i] == correctIndex) {
                isUnique = 0;

            } else {
                // Check if number is unique
                for (j = 0; j < i; j++) {
                    if (array[i] == array[j]) {
                        isUnique = 0;
                        break;
                    }
                }
            }

        } while (!isUnique); // Repeat if number is not unique
    }

    return array;
}

void playMusic(Mix_Music *track) {

    double duration = Mix_MusicDuration(track);

    // Generate a random time point within the duration
    printf("%lf\n", duration);
    double randomTimePoint = ((double)rand() / (double)RAND_MAX) * duration;
    printf("%lf\n", randomTimePoint);
    
    // Ensure snippet is at least 50s long
    if (randomTimePoint > (duration - 50)) {
        randomTimePoint = duration - 50;
    }

    // Play the music
    Mix_PlayMusic(track, 0);

    // Set the music position to the random time point
    Mix_SetMusicPosition(randomTimePoint);

}

// void renderProgressBar(SDL_Renderer *renderer, int time) {
// 
//     SDL_Rect buttonTopRight = { WINDOW_WIDTH * 41/80, WINDOW_HEIGHT * 9/16, WINDOW_WIDTH * 71/160, WINDOW_HEIGHT * 11/80 };
// 
//     SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Set button color to green
//     SDL_RenderFillRect(renderer, &progressBar);
// }

int main() {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* backgroundTexture = NULL;
    SDL_Surface* backgroundSurface = NULL;
    SDL_Event event;
    bool wrong = false;
    bool quit = false;
    bool quitProgram = false;
    bool correct = false;
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

    TTF_Font* optionFont = TTF_OpenFont("fonts/PublicPixel-z84yD.ttf", 12);
    if (!optionFont) {
        perror("Error loading optionFont");
    }

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    backgroundSurface = IMG_Load("img/backgroundv3.jpg");
    if (!backgroundSurface) {
        printf("Error loading background image: %s\n", IMG_GetError());
        return 1;
    }

    // Convert surface to texture
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);

    // Render background texture
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    SDL_Rect progressBarBackground = { WINDOW_WIDTH * 21/80, WINDOW_HEIGHT * 71/240, WINDOW_WIDTH * 161/325, WINDOW_HEIGHT * 5/48 };
    
    // TopLeft, TopRight, BotLeft, BotRight
    SDL_Rect buttons[4] = {
        { WINDOW_WIDTH * 9/160, WINDOW_HEIGHT * 9/16, WINDOW_WIDTH * 71/160, WINDOW_HEIGHT * 11/80 },
        { WINDOW_WIDTH * 41/80, WINDOW_HEIGHT * 9/16, WINDOW_WIDTH * 71/160, WINDOW_HEIGHT * 11/80 },
        { WINDOW_WIDTH * 3/80, WINDOW_HEIGHT * 35/48, WINDOW_WIDTH * 71/160, WINDOW_HEIGHT * 11/80 },
        { WINDOW_WIDTH * 1/2, WINDOW_HEIGHT * 35/48, WINDOW_WIDTH * 71/160, WINDOW_HEIGHT * 11/80 }
    };

    renderText(renderer, WINDOW_WIDTH * 3 / 16, WINDOW_HEIGHT * 71 / 120, "Play", optionFont, 15); // TopLeft
    renderText(renderer, WINDOW_WIDTH * 3 / 16, WINDOW_HEIGHT * 121 / 160, "Quit", optionFont, 15); // BotLeft
    SDL_RenderPresent(renderer); // Update screen

    int seed = time(NULL);
    srand(seed);
    printf("Seed: \"%d\"\n", seed);

    // Define the paths to the lyrics
    const char *directory = "tracks"; // Set directory path
    LyricsStruct trackList = generateTrackList(directory);

    while (menu(event, quit, buttons) == 0 ) {

        // Clear text by rendering backgroundTexture again
	SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

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

        // Print indices for tracks (debug)
        // for (int i = 0; i < trackList.count; i++) {
        //     printf("%d = %s\n", i, trackList.array[i]);
        // }
        int uniqueIndices[4];
        int correctChoice = rand() % 4;
        generateUniqueIndices(trackList.count, randomTrackIndex, uniqueIndices, correctChoice);

	// Debug
        printf("correctIndexChoice = %d\n", correctChoice);
        for (int i = 0; i < 4; i++) {
            printf("uniqueIndices[%d] = %d\n", i, uniqueIndices[i]);
        }

        SDL_SetRenderDrawColor(renderer, 92, 117, 0, 255);
        SDL_RenderFillRect(renderer, &progressBarBackground);
        renderText(renderer, WINDOW_WIDTH * 3 / 16, WINDOW_HEIGHT * 71 / 120, trackList.array[uniqueIndices[0]], optionFont, 15); // TopLeft
        renderText(renderer, WINDOW_WIDTH * 41 / 64, WINDOW_HEIGHT * 71 / 120, trackList.array[uniqueIndices[1]], optionFont, 15); // TopRight
        renderText(renderer, WINDOW_WIDTH * 3 / 16, WINDOW_HEIGHT * 121 / 160, trackList.array[uniqueIndices[2]], optionFont, 15); // BotLeft
        renderText(renderer, WINDOW_WIDTH * 41 / 64, WINDOW_HEIGHT * 121 / 160, trackList.array[uniqueIndices[3]], optionFont, 15); // BotRight
        SDL_RenderPresent(renderer); // Update screen

        playMusic(track);
        while (!quit) {
            Uint32 frameStart = SDL_GetTicks();

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quitProgram = true;
                    quit = true;
                } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    if (isInsideButton(x, y, buttons[0])) {
                        clickCountTopLeft++;
                        printf("ButtonTopLeft clicked! Click count: %d\n", clickCountTopLeft);
                            if (correctChoice == 0) {
                                printf("Correct! Well done.\n");
                                correct = true;
                            } else {
                                wrong = true;
                            }

                        printf("Note: To quit, press 5.\n");
                    }
                    if (isInsideButton(x, y, buttons[1])) {
                        clickCountTopRight++;
                        printf("ButtonTopRight clicked! Resuming song. Click count: %d\n", clickCountTopRight);
                            if (correctChoice == 1) {
                                printf("Correct! Well done.\n");
                                correct = true;
                            } else {
                                wrong = true;
                            }

                        printf("Note: To quit, press 5.\n");
                    }
                    if (isInsideButton(x, y, buttons[2])) {
                        clickCountBotLeft++;
                        printf("ButtonBotLeft clicked! Pausing song. Click count: %d\n", clickCountBotLeft);
                            if (correctChoice == 2) {
                                printf("Correct! Well done.\n");
                                correct = true;
                            } else {
                                wrong = true;
                            }

                        printf("Note: To quit, press 5.\n");
                    }
                    if (isInsideButton(x, y, buttons[3])) {
                        clickCountBotRight++;
                        printf("ButtonBotRight clicked! Playing song. Click count: %d\n", clickCountBotRight);
                            if (correctChoice == 3) {
                                printf("Correct! Well done.\n");
                                correct = true;
                            } else {
                                wrong = true;
                            }

                        printf("Note: To quit, press 5.\n");
                    }
                } else if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_1:
                            clickCountTopLeft++;
                            printf("ButtonTopLeft clicked! Click count: %d\n", clickCountTopLeft);
                            if (correctChoice == 0) {
                                printf("Correct! Well done.\n");
                                correct = true;
                            } else {
                                wrong = true;
                            }

                            printf("Note: To quit, press 5.\n");
                            break;
                        case SDLK_2:
                            clickCountTopRight++;
                            printf("ButtonTopRight clicked! Resuming song. Click count: %d\n", clickCountTopRight);
                            // Mix_ResumeMusic();
                            if (correctChoice == 1) {
                                printf("Correct! Well done.\n");
                                correct = true;
                            } else {
                                wrong = true;
                            }

                            printf("Note: To quit, press 5.\n");
                            break;
                        case SDLK_3:
                            clickCountBotLeft++;
                            printf("ButtonBotLeft clicked! Pausing song. Click count: %d\n", clickCountBotLeft);
                            // Mix_PauseMusic();
                            if (correctChoice == 2) {
                                printf("Correct! Well done.\n");
                                correct = true;
                            } else {
                                wrong = true;
                            }

                            printf("Note: To quit, press 5.\n");
                            break;
                        case SDLK_4:
                            clickCountBotRight++;
                            printf("ButtonBotRight clicked! Playing song. Click count: %d\n", clickCountBotRight);
                            // Mix_PlayMusic(track, -1);
                            if (correctChoice == 3) {
                                printf("Correct! Well done.\n");
                                correct = true;
                            } else {
                                wrong = true;
                            }

                            printf("Note: To quit, press 5.\n");
                            break;
                        case SDLK_5:
                            quit = true;
                            break;
                        case SDLK_6:
                            Mix_PlayMusic(track, -1);
                            break;
                    }
                }
                if (correct == true) {
                    SDL_RenderFillRect(renderer, &progressBarBackground);
                    renderText(renderer, progressBarBackground.x, progressBarBackground.y, "Correct! Well done! Press 5 to return to menu.", optionFont, 26);
                    correct = false;
                    SDL_RenderPresent(renderer);
                } else if (wrong == true) {
                    renderText(renderer, progressBarBackground.x, progressBarBackground.y, "Wrong. Try again.", optionFont, 26);
                    wrong = false;
                    SDL_RenderPresent(renderer);
                }


            }
            // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Clear screen
            // SDL_RenderClear(renderer);

            // renderProgressBar();

            // Calculate the time taken for rendering this frame
            Uint32 frameTime = SDL_GetTicks() - frameStart;

            // Delay to control the frame rate
            if (frameTime < 16) { // Targeting 60 FPS (1000ms / 60 = 16.666ms)
                SDL_Delay(16 - frameTime);
            }
        }

        free(trackPath);
        Mix_FreeMusic(track);
        quit = false;
        
        if (quitProgram == true) {
            break;
        }

        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        renderText(renderer, WINDOW_WIDTH * 3 / 16, WINDOW_HEIGHT * 71 / 120, "Play", optionFont, 15);
        renderText(renderer, WINDOW_WIDTH * 3 / 16, WINDOW_HEIGHT * 121 / 160, "Quit", optionFont, 15);
        SDL_RenderPresent(renderer);
    }

    printf("Thanks for playing! Obama was here 2024\n");

    freeLyricsStruct(trackList.array, trackList.count);

    SDL_DestroyTexture(backgroundTexture);
    IMG_Quit();
    SDL_Quit();

    TTF_CloseFont(optionFont);
    TTF_Quit();

    Mix_CloseAudio();
    Mix_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
