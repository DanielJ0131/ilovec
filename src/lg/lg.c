#include <stddef.h>
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

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
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
            clearInputBuffer();
            break;
        case '2':
            printf("Thanks for playing! Obama was here 2024\n");
            exit(0);
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            sleep(2);
            menu();
            break;
    }
}

// Structure to hold both array and number of elements
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
            // Allocate memory for each string without .txt
            lyricsList[index] = (char*)malloc((strlen(entry->d_name) - 3) * sizeof(char));
            if (!lyricsList[index]) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }

            // Construct the string without .txt
            size_t filenameLength = strlen(entry->d_name);
            entry->d_name[filenameLength - 4] = '\0';

            strcpy(lyricsList[index], entry->d_name);

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

    return result;
}

// Function to free the memory allocated for the list of strings
void freeLyricsStruct(char **array, int count) {
    for (int i = 0; i < count; i++) {
        free(array[i]);
    }
    free(array);
}

int hints() {
    clearScreen();
    printf("Hints available:\n");
    printf("1. See next line of lyrics\n");
    printf("2. Cancel\n");
    char choice;
    printf("Enter your choice: ");
    scanf(" %c", &choice);

    switch (choice) {
        case '1':
            clearInputBuffer();
            clearScreen();
            return 1;
        case '2':
            clearInputBuffer();
            clearScreen();
            return 0;
        default:
            printf("Invalid choice. Please try again.\n");
            sleep(2);
            clearInputBuffer();
            hints();
            return 0;
    }
    return 0;
}

// Function to parse user input for correct answer
int guesser(const char *correctAnswer) {
    char input[35];

    printf("Enter your answer: \n");
    printf("Alternatively, enter \"1\" for hints.\n");

    fgets(input, sizeof(input), stdin);
    size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
    
    if (input[0] == '1') {
        return 0;
    } else {
        int result = strcmp(input, correctAnswer);

        if (result == 0) {
            printf("Correct!\n");
            sleep(1);
        } else {
            printf("Incorrect. Try again.\n");
            sleep(1);
            return guesser(correctAnswer);
        }
    }
    return 1;
}

// Print lines based on hints used
void printLines(char **array, int count, int hintsUsed, int index) {
    printf("Which song is this?\n\n");

    printf("\033[1;32m");
    if (index + hintsUsed >= count) {

        // Print from first element
        for (int i = 0; i <= (index + hintsUsed - count); i++) {
            printf("%s\n", array[i]);
        }
        printf("\n");

        // Print until last element
        for (int i = 0; i <= (count - index - 1); i++) {
            printf("%s\n", array[(index + i)]);
        }
        printf("\n");
    } else {
            for (int i = 0; i <= hintsUsed; i++) {
            printf("%s\n", array[(index + i)]);
        }
        printf("\n"); 
    }
    printf("\033[0m");

}

int main() {
    while (1) {
        menu();
        clearScreen();

        // Define the paths to the lyrics
        const char *directory = "lyrics"; // Set directory path
        LyricsStruct lyricsList = generateLyricsList(directory);
    
        if (lyricsList.array == NULL || lyricsList.count == 0) {
            perror("Failed to generate or empty lyrics list.");
            return 1;
        }

        // FOR DEBUGGING
        // for (int i = 0; i <= lyricsList.count - 1; i++) {
        //     printf("%d: %s", (i + 1), lyricsList.array[i]);
        // }
        // return 0;

        srand(time(NULL));

        int randomIndex = rand() % lyricsList.count;

        // Allocate memory for entire path including ".txt" and "/"
        char *lyricsPath = (char*)malloc((strlen(directory) + strlen(lyricsList.array[randomIndex]) + 6) * sizeof(char));

        // Prefix directory path to chosen .txt file
        sprintf(lyricsPath, "%s/%s.txt", directory, lyricsList.array[randomIndex]);

        // Open the file unless it doesn't exist
        FILE *file = fopen(lyricsPath, "r");
        if (file == NULL) {
            printf("Failed to open the file.\n");
            sleep(2);
            main();
        }

        LyricsStruct lyrics = generateLyrics(file);

        // Close the file
        fclose(file);
    
        // Allocate memory and store answer before freeing lyricsList
        char *correctAnswer = (char*)malloc((strlen(lyricsList.array[randomIndex]) + 1) * sizeof(char));
        if (correctAnswer == NULL) {
            perror("Error allocating memory for correctAnswer");
        } else {
            strcpy(correctAnswer, lyricsList.array[randomIndex]); 
        }

        // Free the allocated memory from generateLyricsList()
        freeLyricsStruct(lyricsList.array, lyricsList.count);

        srand(time(NULL));

        // Generate a random index within the range of lyricsCount
        randomIndex = rand() % lyrics.count;

        int hintsUsed = 0;

        printLines(lyrics.array, lyrics.count, hintsUsed, randomIndex);

        // Loop until correct
        while (guesser(correctAnswer) == 0) {

            // Print out lines based on hints
            if (hints() != 0) {

                if ((randomIndex + hintsUsed) == (lyrics.count - 1)) {
                    printf("Last line reached. Printing first line instead.\n");
                }

                if (hintsUsed <= (lyrics.count - 1)) {
                    hintsUsed++;
                    printLines(lyrics.array, lyrics.count, hintsUsed, randomIndex);

                } else {

                    printf("Maximum number of hints reached. Please make a guess.\n");
                    printLines(lyrics.array, lyrics.count, hintsUsed, randomIndex);
                }

            } else {
                printLines(lyrics.array, lyrics.count, hintsUsed, randomIndex);
            }
        }

        free(correctAnswer);
        freeLyricsStruct(lyrics.array, lyrics.count);
    }

    return 0;
}