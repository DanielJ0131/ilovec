#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int rollDice() {
    return (rand() % 6) + 1;
}

void clearTerminal() {
    system("clear"); // Use "cls" for Windows
}

void animation() {
    struct timespec sleepTime;
    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = 250000000; // 250 milliseconds

    clearTerminal();
    printf("Good luck Obama\n");
    nanosleep(&sleepTime, NULL);
    clearTerminal();

    printf("Rolling the dice.\n");
    nanosleep(&sleepTime, NULL);
    clearTerminal();

    printf("Rolling the dice..\n");
    nanosleep(&sleepTime, NULL);
    clearTerminal();

    printf("Rolling the dice...\n");
    nanosleep(&sleepTime, NULL);
    clearTerminal();

    printf("Rolling the dice....\n");
    nanosleep(&sleepTime, NULL);
    clearTerminal();

}

void drawDice(int number) {
    printf(" ________\n");
    printf("|        |\n");
    
    switch(number) {
        case 1:
            printf("|   *    |\n");
            printf("|        |\n");
            break;
        case 2:
            printf("| *   *  |\n");
            printf("|        |\n");
            break;
        case 3:
            printf("| * * *  |\n");
            printf("|        |\n");
            break;
        case 4:
            printf("| *   *  |\n");
            printf("| *   *  |\n");
            break;
        case 5:
            printf("| * * *  |\n");
            printf("| *   *  |\n");
            break;
        case 6:
            printf("| * * *  |\n");
            printf("| * * *  |\n");
            break;
        default:
            break;
    }
    
    printf(" --------\n");

}

void main() {
    while (1) {
        animation();
        int result = rollDice();
        printf("The dice rolled: %d!\n", result);
        drawDice(result);
        sleep(1);
        clearTerminal();
    }
}