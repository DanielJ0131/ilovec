#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void clearScreen() {
    // ANSI escape code clear the screen
    printf("\033[2J\033[H");
}

void printHeader() {
    // ANSI escape code set text color to cyan
    printf("\033[1;36m");

    // Print a stylized title
    printf("+-------------------------+\n");
    printf("|   Cool 3D Dice Menu    |\n");
    printf("+-------------------------+\n");

    // ANSI escape code reset text color
    printf("\033[0m");
}

void printMenu() {
    // ANSI escape code set text color to green
    printf("\033[1;32m");

    printf("1. Roll the dice\n");
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
            printf("Rolling the dice...\n");
            break;
        case '2':
            exit(0);
            break;
        default:
            printf("Invalid choice. Please try again.\n");
    }
}

int rollDice() {
    return (rand() % 6) + 1;
}

void animation() {
    struct timespec sleepTime;
    sleepTime.tv_sec = 0;
    sleepTime.tv_nsec = 250000000; // 250 milliseconds

    clearScreen();
    printf("Rolling the dice\n");
    nanosleep(&sleepTime, NULL);
    clearScreen();

    printf("Rolling the dice.\n");
    nanosleep(&sleepTime, NULL);
    clearScreen();

    printf("Rolling the dice..\n");
    nanosleep(&sleepTime, NULL);
    clearScreen();

    printf("Rolling the dice...\n");
    nanosleep(&sleepTime, NULL);
    clearScreen();

    printf("Rolling the dice....\n");
    nanosleep(&sleepTime, NULL);
    clearScreen();

}

void drawDice(int number) {
    printf("    .----------------.\n");
    
    switch(number) {
        case 1:
            printf("   /  o         o   / |\n");
            printf("  /       o        /  |\n");
            printf(" /  o         o   /  o|\n");
            printf(" --------------- '    |\n");
            printf("|                |  o |\n");
            printf("|                |    |\n");
            printf("|       o        | o /\n");
            printf("|                |  /\n");
            printf("|                | /\n");
            break;
        case 2:
            printf("   /            o   / |\n");
            printf("  /       o        /  |\n");
            printf(" /  o             /   |\n");
            printf(" --------------- '    |\n");
            printf("| o              |  o |\n");
            printf("|                |    |\n");
            printf("|                |   / \n");
            printf("|                |  /\n");
            printf("|             o  | /\n");
            break;
        case 3:
            printf("   /                / |\n");
            printf("  /       o        /  |\n");
            printf(" /                /  o|\n");
            printf(" --------------- '    |\n");
            printf("| o              |    |\n");
            printf("|                |    |\n");
            printf("|       o        | o / \n");
            printf("|                |  /\n");
            printf("|             o  | /\n");
            break;
        case 4:
            printf("   /  o         o   / |\n");
            printf("  /       o        /  |\n");
            printf(" /  o         o   /   |\n");
            printf(" --------------- '    |\n");
            printf("| o           o  |  o |\n");
            printf("|                |    |\n");
            printf("|                |   / \n");
            printf("|                |  /\n");
            printf("| o           o  | /\n");
            break;
        case 5:
            printf("   /                / |\n");
            printf("  /       o        /  |\n");
            printf(" /                /  o|\n");
            printf(" --------------- '    |\n");
            printf("| o           o  |  o |\n");
            printf("|                |    |\n");
            printf("|       o        | o / \n");
            printf("|                |  /\n");
            printf("| o           o  | /\n");
            break;
        case 6:
            printf("   /  o         o   / |\n");
            printf("  /       o        /  |\n");
            printf(" /  o         o   /  o|\n");
            printf(" --------------- '    |\n");
            printf("| o     o     o  |  o |\n");
            printf("|                |    |\n");
            printf("|                | o / \n");
            printf("|                |  /\n");
            printf("| o     o     o  | /\n");
            break;
        default:
            break;
    }
    
    printf("'----------------'\n");

}

int main(void) {
    menu();
    while (1) {
        animation();
        int result = rollDice();
        printf("The dice rolled: %d!\n", result);
        drawDice(result);
        sleep(2);
        clearScreen();
    }
}