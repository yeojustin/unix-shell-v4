// main.c

#include <stdio.h>

#include "shell.h"

int main()
{
    char input[BUF_SIZE];
    char *inputPointer = NULL;

    setupSignals();

    while (1)
    {
        printf("%s ", promptName);
        inputPointer = fgets(input, BUF_SIZE, stdin);

        while (inputPointer == NULL && errno == EINTR)
        {
            inputPointer = fgets(input, BUF_SIZE, stdin);
        }

        input[strlen(input) - 1] = '\0';

        if (strlen(input) > 0)
        {
            toggleSignalBlock(SIG_BLOCK, SIGCHLD);

            handleCommandLine(input, 0, 0, 0, commandArray);
            executeCommands(commandArray);
            freeCommands(commandArray);

            toggleSignalBlock(SIG_UNBLOCK, SIGCHLD);
        }
    }

    exit(0);
}