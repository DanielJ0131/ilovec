#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

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
            sleep(1);
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

int chooseMode() {
    char choice;
    printf("Please choose a mode.\n");
    printf("\n");
    printf("1. Normal        2. Multiple Choice\n");
    printf("\n");
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
            sleep(1);
            clearInputBuffer();
            clearScreen();
            chooseMode();
            return 0;
    }
    return 0;
}

// Prompt user to set difficulty
int setDifficulty() {
    char choice;
    printf("Please choose a difficulty.\n");
    printf("\n");
    printf("1. Easy        2. Medium\n");
    printf("3. Hard        4. Master\n");
    printf("\n");
    printf("Enter your choice: ");
    scanf(" %c", &choice);

    // Returns amount of lines shown
    switch (choice) {
        case '1':
            clearInputBuffer();
            clearScreen();
            return 8;
        case '2':
            clearInputBuffer();
            clearScreen();
            return 4;
        case '3':
            clearInputBuffer();
            clearScreen();
            return 2;
        case '4':
            clearInputBuffer();
            clearScreen();
            return 1;
        default:
            printf("Invalid choice. Please try again.\n");
            sleep(1);
            clearInputBuffer();
            clearScreen();
            setDifficulty();
            return 0;
    }
    return 0;
}

int hints(const char *songName) {
    clearScreen();
    printf("Hints available:\n");
    printf("1. See next line of lyrics\n");
    printf("2. Show release date\n");
    printf("3. Give up (show song title)\n");
    printf("4. Cancel\n");
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
            return 2;
        case '3':
            clearInputBuffer();
            clearScreen();
            printf("%s\n", songName);
            return 0;
        case '4':
            clearInputBuffer();
            clearScreen();
            return 0;
        default:
            printf("Invalid choice. Please try again.\n");
            sleep(2);
            clearInputBuffer();
            hints(songName);
            return 0;
    }
    return 0;
}

// Parse user input in Normal mode
int guesser(const char *songName) {
    char input[35];

    printf("Enter your answer: \n");
    printf("Alternatively, enter \"h\" for hints.\n");

    fgets(input, sizeof(input), stdin);
    size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

    // Process the input string
    for (int i = 0; input[i] != '\0'; i++) {
        // Convert uppercase characters to lowercase
        if (isupper(input[i])) {
            input[i] = tolower(input[i]);
        }
        // Remove spaces and single quotes by shifting characters
        if (input[i] == ' ' || input[i] == '\'') {
            for (int j = i; input[j] != '\0'; j++) {
                input[j] = input[j + 1];
            }
            i--; // Move back one position to recheck the current character
        }
    } 

    if (input[0] == 'h' && input[1] == '\0') {
        return 0;
    } else {
        int result = strcmp(input, songName);

        if (result == 0) {
            printf("Correct!\n");
            sleep(1);
        } else {
            printf("Incorrect. Try again.\n");
            sleep(1);
            return guesser(songName);
        }
    }
    return 1;
}


// Parse user input in Multiple Choice mode
int guesserMultipleChoice(LyricsStruct lyrics, int correctIndex, int randomValue) {

    int optionsAmount = 4;
    int numbers[optionsAmount];
    int i, j, isUnique;
    char choice;
    srand(randomValue);

    // Set correct answer
    int correctChoice = rand() % optionsAmount;
    numbers[correctChoice] = correctIndex;

    // Choose unique indices
    for (i = 0; i < (optionsAmount); i++) {
        if (i == correctChoice) continue;

        do {
           numbers[i] = rand() % lyrics.count;
           isUnique = 1; // Assume number is unique

           // Check if number is unique
           for (j = 0; j < i; j++) {
                if (numbers[i] == numbers[j] || numbers[i] == correctIndex) {
                    isUnique = 0;
                    break;
                }
           }
        } while (!isUnique); // Repeat if number is not unique
    }

    printf("1. %s        ", lyrics.array[numbers[0]]);
    printf("2. %s\n", lyrics.array[numbers[1]]);
    printf("3. %s        ", lyrics.array[numbers[2]]);
    printf("4. %s\n", lyrics.array[numbers[3]]);
    printf("Enter your choice: \n");
    printf("Alternatively, enter \"h\" for hints.\n");
    scanf(" %c", &choice);


    if (choice == 'h') {
        return 0;
    } else {

        // Subtract ASCII value from choice to get integer value
        if (choice - '0' - 1 == correctChoice) {
            printf("Correct!\n");
            sleep(1);
            clearInputBuffer();
        } else if (0 < choice && choice <= optionsAmount) {
            printf("Incorrect. Please try again.\n");
            sleep(1);
            clearInputBuffer();
            return guesserMultipleChoice(lyrics, correctIndex, randomValue);
        } else {
            printf("Invalid choice. Please try again.\n");
            sleep(1);
            clearInputBuffer();
            return guesserMultipleChoice(lyrics, correctIndex, randomValue);
        }

    }
    
    return 1;
}

// Print lines based on hints used
  void printLines(LyricsStruct lyrics, int hintsUsed, int index) {
    int i;

    printf("Which song is this?\n\n");

    if ((index + hintsUsed) == (lyrics.count - 1)) {
      printf("Last line reached. Printing from first line.\n");
    }

    if (hintsUsed >= (lyrics.count - 1)) {
      printf("Maximum number of lines printed. Please make a guess.\n");
    }

    printf("\033[1;32m"); // Green color
    if (index + hintsUsed >= lyrics.count) {

      // Print from first element
      for (i = 0; i <= (index + hintsUsed - lyrics.count); i++) {
        printf("%s\n", lyrics.array[i]);
      }
      printf("\n");

      // Print until last element
      for (i = 0; i <= (lyrics.count - index - 1); i++) {
        printf("%s\n", lyrics.array[(index + i)]);
      }
      printf("\n");
    } else {
      for (i = 0; i <= hintsUsed; i++) {
        printf("%s\n", lyrics.array[(index + i)]);
      }
      printf("\n");
    }
    printf("\033[0m");
  }

void printReleaseDate(const char *songName) {
    printf("Not yet implemented.\n");
}

int main() {
    while (1) {
        menu();
        clearScreen();

        // Define the paths to the lyrics
        const char *directory = "lyrics/imaginedragons"; // Set directory path
        LyricsStruct lyricsList = generateLyricsList(directory);
    
        if (lyricsList.array == NULL || lyricsList.count == 0) {
            perror("Failed to generate or empty lyrics list.");
            return 1;
        }

        // FOR DEBUGGING
        // for (int i = 0; i <= lyricsList.count - 1; i++) {
        //     printf("%d: %s\n", i, lyricsList.array[i]);
        // }
        // return 0;

        srand(time(NULL));

        int randomFileIndex = rand() % lyricsList.count;

        // Allocate memory for entire path including ".txt" and "/"
        char *lyricsPath = (char*)malloc((strlen(directory) + strlen(lyricsList.array[randomFileIndex]) + 6) * sizeof(char));

        // Prefix directory path to chosen .txt file
        sprintf(lyricsPath, "%s/%s.txt", directory, lyricsList.array[randomFileIndex]);

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
        char *correctAnswer = (char*)malloc((strlen(lyricsList.array[randomFileIndex]) + 1) * sizeof(char));
        if (correctAnswer == NULL) {
            perror("Error allocating memory for correctAnswer");
        } else {
            strcpy(correctAnswer, lyricsList.array[randomFileIndex]); 
        }

          int randomLineIndex = rand() % lyrics.count;

          // Choose mode and change hintsUsed based on difficulty chosen
          if (chooseMode() == 0) {

            int hintsUsed = (setDifficulty() - 1);

            printLines(lyrics, hintsUsed, randomLineIndex);

            // Set constant randomValue for while loop
            int randomValue = rand();

            while (guesserMultipleChoice(lyricsList, randomFileIndex, randomValue) == 0) {

                // Print out lines based on hints
                int hintValue = hints(correctAnswer);

                if (hintValue == 1) {

                    hintsUsed++;
                    printLines(lyrics, hintsUsed, randomLineIndex);

                } else if (hintValue == 2) {

                    printReleaseDate(correctAnswer);
                    printLines(lyrics, hintsUsed, randomLineIndex);

                } else {

                    printLines(lyrics, hintsUsed, randomLineIndex);
                }
            }

            freeLyricsStruct(lyricsList.array, lyricsList.count);

        } else {

            int hintsUsed = (setDifficulty() - 1);

            freeLyricsStruct(lyricsList.array, lyricsList.count);

            printLines(lyrics, hintsUsed, randomLineIndex);

            // Loop until correct (normal mode)
            while (guesser(correctAnswer) == 0) {

                // Print out lines based on hints
                int hintValue = hints(correctAnswer);

                if (hintValue == 1) {
                    hintsUsed++;
                } else if (hintValue == 2) {
                    printReleaseDate(correctAnswer);
                }
                printLines(lyrics, hintsUsed, randomLineIndex);
            }
        }
        free(correctAnswer);
        freeLyricsStruct(lyrics.array, lyrics.count);
    }
    return 0;
}