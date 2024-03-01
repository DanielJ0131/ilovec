#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#define MAX_LENGTH 300

// Taken from dice/dice.c
void clearScreen() {
    // ANSI escape code clear the screen
    printf("\033[2J\033[H");
}

void printHeader() {
    // ANSI escape code set text color to cyan
    printf("\033[1;36m");

    // Print a stylized title
    printf("+-------------------------+\n");
    printf("|      Lyric Guesser      |\n");
    printf("+-------------------------+\n");

    // ANSI escape code reset text color
    printf("\033[0m");
}

void printMenu() {
    // ANSI escape code set text color to green
    printf("\033[1;32m");

    printf("1. Play\n");
    printf("2. Exit\n");

    printf("\033[0m");
}

void menu() {
    clearScreen();
    printHeader();
    printMenu();

    char choice;
    printf("Enter your choice: ");
    scanf(" %c", &choice);  // The space removes whitespace it's not a typo

    switch (choice) {
        case '1':
            break;
        case '2':
            exit(0);
            break;
        default:
            perror("Invalid choice. Please try again.\n");
            sleep(2);
            menu();
            break;
    }
}

// Structure to hold both the array and number of elements
typedef struct {
    char **array;
    int count;
} LyricsStruct;

// Function to generate a list and amount of entries in a directory
LyricsStruct generateLyricsList(const char *directory) {
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
            // Allocate memory for each string
            lyricsList[index] = (char*)malloc((strlen(directory) + strlen(entry->d_name) + 2) * sizeof(char));
            if (!lyricsList[index]) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }

            // Construct the string in the desired format
            sprintf(lyricsList[index], "%s/%s", directory, entry->d_name);

            index++;
        }
    }

    // Close the directory
    closedir(dir);

    LyricsStruct result;
    result.array = lyricsList;
    result.count = fileCount;

    return result;
}

// Function to generate lyrics from a text file
LyricsStruct generateLyrics(FILE *file) {
    LyricsStruct result;
    result.array = malloc(MAX_LENGTH * sizeof(char *));
    result.count = 0;
    
    if (result.array == NULL) {
        perror("Error allocating memory for lines array");
        return result;
    }

    char buffer[100];  // Adjust the buffer size based on your needs

    // Read each line from the file
    while (fgets(buffer, sizeof(buffer), file) != NULL && result.count < MAX_LENGTH) {
        // Remove the newline character at the end
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // Allocate memory for the line and store it in the array
        result.array[result.count] = strdup(buffer);
        if (result.array[result.count] == NULL) {
            perror("Error allocating memory");

            // Free allocated memory before returning
            for (int i = 0; i < result.count; i++) {
                free(result.array[i]);
            }
            free(result.array);

            return result;
        }

       result.count++;
    }

    // Close the file
    fclose(file);

    return result;
}

// Function to free the memory allocated for the list of strings
void freeLyricsList(char **lyricsList, int size) {
    for (int i = 0; i < size; i++) {
        free(lyricsList[i]);
    }
    free(lyricsList);
}

int main() {
    menu();

    // Define the paths to the lyrics
    const char *directory = "lyrics"; // Set directory path
    LyricsStruct lyricsList = generateLyricsList(directory);
    
    if (lyricsList.array == NULL || lyricsList.count == 0) {
        perror("Failed to generate or empty lyrics list.\n");
        return 1;
    }

    srand(time(NULL));

    int randomIndex = rand() % lyricsList.count;

    char *randomLyrics = lyricsList.array[randomIndex];

    // Open the file unless it doesn't exist
    FILE *file = fopen(randomLyrics, "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        sleep(2);
        main();
    }

    LyricsStruct lyrics = generateLyrics(file);

    // Free the allocated memory from generateLyricsList()
    freeLyricsList(lyricsList.array, lyricsList.count);

    srand(time(NULL));

    // Generate a random index within the range of lyricsCount
    randomIndex = rand() % lyrics.count;

    // Print out the randomly selected line of lyric
    printf("%s\n", lyrics.array[randomIndex]);

    printf("hello obama\n");
    return 0;
}