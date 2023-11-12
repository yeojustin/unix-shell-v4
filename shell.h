// shell.h

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wordexp.h>

#include "command.h"

#define BUF_SIZE 256
#define CHANGE_DIR "cd"
#define PRINT_DIR "pwd"
#define PROMPT "prompt"
#define EXIT "exit"

extern char *promptName;
extern Command *commandArray[MAX_COMMANDS];
extern char *input;

int redirection(Command *command);
void collectChildren();
void handleSignals();
int setupSignals();
int toggleSignalBlock(int flag, int signalNumber);
void freeCommands(Command **commands);
void executeCommands(Command **commands);
void createPipedProcesses(Command **pipedCommands, int count);
void createProcess(Command *command);