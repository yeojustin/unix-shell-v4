// command.h

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEPARATOR_CONCURRENT "&"
#define SEPARATOR_INPUT "<"
#define SEPARATOR_OUTPUT ">"
#define SEPARATOR_ERROR "2>"
#define SEPARATOR_PIPE "|"
#define SEPARATOR_SEQUENTIAL ";"
#define WILDCARD_ALL '*'
#define WILDCARD_EXACT '?'
#define MAX_COMMANDS 100
#define MAX_ARGUMENTS 1000

static const char whiteSpace[2] = {(char)0x20, (char)0x09};

typedef struct CommandStruct
{
  char *name;
  char *stdin;
  char *stdout;
  char *stderr;
  char *argv[MAX_ARGUMENTS];
  int argc;
  int background;
  int pipe;
  int redirection;
} Command;

Command *createCommand(char *input, int background, int pipe);
int getArguments(char *input, Command *command);
char *getSeparator(char *input);
void handleCommandLine(char *input, int background, int pipe, int iteration, Command **commands);