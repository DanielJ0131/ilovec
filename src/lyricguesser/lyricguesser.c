#include <stdio.h>

#define MAX_LENGTH 1000

int main() {
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
