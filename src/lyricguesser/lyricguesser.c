#include <stdio.h>

#define MAX_LENGTH 1000


void menu() {
    printf("1. Play\n");
    printf("2. Exit\n");
    printf("Enter your choice: ");
    char choice = getchar();

    switch (choice) {
        case '1':
            printf("Playing the game...\n");
            break;
        case '2':
            printf("Exiting the game...\n");
            break;
        default:
            printf("Invalid choice.\n");
            break;
    }
}


int main() {
    menu();
    FILE *file = fopen("lyrics.txt", "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    char lyrics[MAX_LENGTH];
    while (fgets(lyrics, MAX_LENGTH, file) != NULL) {
        printf("%s", lyrics);
    }

    fclose(file);
    return 0;
}
