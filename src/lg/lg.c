#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LENGTH 100

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
    scanf(" %c", &choice);

    switch (choice) {
        case '1':
            break;
        case '2':
            exit(0);
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            sleep(2);
            menu();
            break;
    }
}

int main() {
    menu();

    // Open the file unless it doesn't exist
    FILE *file = fopen("lyrics/cherrypop.txt", "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        sleep(2);
        main();
    }

    // Create a 2D array to store the lyrics
    char lyrics[MAX_LENGTH][MAX_LENGTH];
    int lyricsCount = 0;
    char lyricsString[MAX_LENGTH];
    while (fgets(lyricsString, MAX_LENGTH, file) != NULL) {
        // Copy the lyrics to the list
        if (lyrics[lyricsCount] != NULL) {
            strcpy(lyrics[lyricsCount], lyricsString);
            lyricsCount++;
        }
    }

    // Seed the random number generator
    srand(time(NULL));

    // Generate a random index within the range of lyricsCount
    int randomIndex = rand() % lyricsCount;

    // Print out the randomly selected lyric
    printf("%s", lyrics[randomIndex]);

    fclose(file);
    return 0;
}
