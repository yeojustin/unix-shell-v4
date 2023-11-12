// shell.c

#include "shell.h"

// Define the global variables
char *promptName = "%";
Command *commandArray[MAX_COMMANDS];
char *input;


int setRedirection(Command *command)
{
    char *fileName = NULL;
    int descriptor = command->redirection;
    int file;

    if (descriptor == 0)
    {
        fileName = command->stdin;
        file = open(fileName, O_RDONLY);
    }
    else if (descriptor == 1)
    {
        fileName = command->stdout;
        file = open(fileName, O_WRONLY | O_CREAT, 0766);
    }
    else if (descriptor == 2)
    {
        fileName = command->stderr;
        file = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0766);
    }
    else
    {
        return 1;
    }

    // Check if file open successfully
    if (file < 0)
    {
        perror("Error opening file");
        exit(1);
    }
    else
    {
        dup2(file, descriptor);
        close(file);
    }

    if (fileName != NULL)
    {
        free(fileName);
    }

    return 0;
}

void collectChildren()
{
    int collect = 1;
    int status;
    pid_t pid;

    while (collect)
    {
        pid = waitpid(-1, &status, WNOHANG);

        if (pid <= 0)
        {
            collect = 0;
        }
    }
}

void handleSignals(int signalNumber)
{
    if (signalNumber == SIGCHLD)
    {
        collectChildren();
    }
}

int setupSignals()
{
    struct sigaction act;
    sigset_t signalSet;

    act.sa_flags = 0;
    act.sa_handler = handleSignals;

    if (sigaction(SIGCHLD, &act, NULL) != 0)
    {
        perror("sigaction");
        exit(1);
    }

    sigemptyset(&signalSet);
    sigaddset(&signalSet, SIGINT);
    sigaddset(&signalSet, SIGQUIT);
    sigaddset(&signalSet, SIGTSTP);

    if (sigprocmask(SIG_SETMASK, &signalSet, NULL) < 0)
    {
        perror("Error with SEG_SETMASK");
        return -1;
    }

    return 0;
}

int toggleSignalBlock(int flag, int signalNumber)
{
    sigset_t signalSet;
    sigemptyset(&signalSet);
    sigaddset(&signalSet, signalNumber);

    if (sigprocmask(flag, &signalSet, NULL) < 0)
    {
        perror("Error trying to (un)block signal");
        return -1;
    }
    else
    {
        return 0;
    }
}

void freeCommands(Command **commands)
{
    int index = 0;
    int argvIndex = 0;

    while (commands[index] != NULL)
    {
        while (commands[index]->argv[argvIndex] != NULL)
        {
            commands[index]->argv[argvIndex] = NULL;
            argvIndex++;
        }

        if (commands[index]->name != NULL)
        {
            free(commands[index]->name);
        }

        if (commands[index]->stdin != NULL)
        {
            free(commands[index]->stdin);
        }

        if (commands[index]->stdout != NULL)
        {
            free(commands[index]->stdout);
        }

        free(commands[index]);
        commands[index] = NULL;

        index++;
    }
}

void executeCommands(Command **commands)
{
    int index = 0;
    int pipeCount = 0;
    Command *command;
    Command *pipedCommands[MAX_COMMANDS];

    while ((command = commands[index++]) != NULL)
    {
        if (strcmp(command->name, CHANGE_DIR) == 0)
        {
            input = command->argv[1];
            if (input != NULL)
            {
                wordexp_t wordExpansion;
                wordexp(input, &wordExpansion, 0);

                if (chdir(wordExpansion.we_wordv[0]) < 0)
                {
                    printf("cd: %s: Not a directory.\n", wordExpansion.we_wordv[0]);
                }

                wordfree(&wordExpansion);
            }
            else
            {
                input = getenv("HOME");
                chdir(input);
            }
        }
        else if (strcmp(command->name, EXIT) == 0)
        {
            exit(0);
        }
        else if (strcmp(command->name, PRINT_DIR) == 0)
        {
            char currentDir[1024];
            getcwd(currentDir, 1024);
            printf("%s\n", currentDir);
        }
        else if (strcmp(command->name, PROMPT) == 0)
        {
            input = command->argv[1];
            if (input != NULL && strlen(input) > 0)
            {
                promptName = strdup(input);
            }
        }
        else
        {
            if (command->pipe > 0)
            {
                pipedCommands[pipeCount] = command;
                pipeCount++;
            }
            else
            {
                if (pipeCount > 0)
                {
                    pipedCommands[pipeCount++] = command;
                    createPipedProcesses(pipedCommands, pipeCount);

                    for (int i = 0; i < pipeCount; i++)
                    {
                        pipedCommands[i] = NULL;
                    }

                    pipeCount = 0;
                }
                else
                {
                    createProcess(command);
                }
            }
        }
    }

    free(command);
}

void createPipedProcesses(Command **pipedCommands, int count)
{
    pid_t pid;
    int i, j;
    int status;
    int pids[count];

    int isBackground = 0;
    int numOfPipes = count - 1;
    int pipes[numOfPipes][2];
    Command *command = NULL;

    for (i = 0; i < numOfPipes; i++)
    {
        if (pipe(pipes[i]) < 0)
        {
            perror("Error in pipe call");
            exit(1);
        }
    }

    for (i = 0; i < count; i++)
    {
        command = pipedCommands[i];
        pid = fork();

        if (command->background == 1)
        {
            isBackground = 1;
        }

        if (pid == 0)
        {
            if (i == 0)
            {
                dup2(pipes[i][1], 1);
            }
            else if (i == numOfPipes)
            {
                dup2(pipes[i - 1][0], 0);
            }
            else
            {
                dup2(pipes[i - 1][0], 0);
                dup2(pipes[i][1], 1);
            }

            if (command->stdin != NULL || command->stdout != NULL)
            {
                setRedirection(command);
            }

            for (j = 0; j < numOfPipes; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            if (execvp(command->name, command->argv) < 0)
            {
                fprintf(stderr, "%s: %s\n", command->name, strerror(errno));
                exit(1);
            }
        }

        pids[i] = pid;
    }

    for (i = 0; i < numOfPipes; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    if (isBackground == 0)
    {
        for (i = 0; i < count; i++)
        {
            waitpid(pids[i], &status, 0);
        }
    }
}

void createProcess(Command *command)
{
    pid_t pid;
    int status;

    pid = fork();

    if (pid == 0)
    {
        if (command->stdin != NULL || command->stdout != NULL)
        {
            setRedirection(command);
        }

        if (execvp(command->name, command->argv) < 0)
        {
            fprintf(stderr, "%s: %s\n", command->name, strerror(errno));
            exit(1);
        }
    }
    else if (pid < 0)
    {
        perror("Error forking");
        exit(1);
    }

    if (command->background == 0)
    {
        waitpid(pid, &status, 0);
    }
}


/*
int redirection(Command *command)
{
    char *stdinFile = command->stdin;
    char *stdoutFile = command->stdout;
    char *stderrFile = command->stderr;

    if (stdinFile != NULL)
    {
        int stdinFd = open(stdinFile, O_RDONLY);
        if (stdinFd < 0)
        {
            perror("Error opening input file");
            exit(1);
        }
        dup2(stdinFd, STDIN_FILENO);
        close(stdinFd);
    }

    if (stdoutFile != NULL)
    {
        int stdoutFd = open(stdoutFile, O_WRONLY | O_CREAT | O_TRUNC, 0766);
        if (stdoutFd < 0)
        {
            perror("Error opening output file");
            exit(1);
        }
        dup2(stdoutFd, STDOUT_FILENO);
        close(stdoutFd);
    }

    if (stderrFile != NULL)
    {
        int stderrFd = open(stderrFile, O_WRONLY | O_CREAT | O_TRUNC, 0766);
        if (stderrFd < 0)
        {
            perror("Error opening error file");
            exit(1);
        }
        dup2(stderrFd, STDERR_FILENO);
        close(stderrFd);
    }

    return 0;
}
*/