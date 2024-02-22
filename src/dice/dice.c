#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void menu() {
    printf("1. Roll the dice\n");
    printf("2. Exit\n");
    if (getchar() == '2') {
        exit(0);
    }
}

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
    printf("Rolling the dice\n");
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
    printf(".----------------.\n");
    
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

void main() {
    menu();
    while (1) {
        animation();
        int result = rollDice();
        printf("The dice rolled: %d!\n", result);
        drawDice(result);
        sleep(1);
        clearTerminal();
    }
}