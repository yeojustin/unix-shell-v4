// command.c

#include "command.h"

// Function to extract arguments from the input and populate the Command structure
int getArguments(char *input, Command *command)
{
  char *token = NULL;
  int argumentCount = 0;

  // Tokenize the input using whiteSpace as the delimiter
  char *saveptr;
  token = strtok_r(input, whiteSpace, &saveptr);

  // Process each token
  while (token != NULL)
  {
    // Check for wildcard characters and expand them using glob if found
    if (index(token, WILDCARD_ALL) != NULL || index(token, WILDCARD_EXACT) != NULL)
    {
      glob_t result;
      glob(token, 0, NULL, &result);

      // Copy each expanded path to the command arguments
      for (int i = 0; i < result.gl_pathc; i++)
      {
        command->argv[argumentCount] = strdup(result.gl_pathv[i]);
        argumentCount++;
      }

      globfree(&result);
    }
    else
    {
      // Copy regular token to the command arguments
      command->argv[argumentCount] = strdup(token);
      argumentCount++;
    }

    // Move to the next token
    token = strtok_r(NULL, whiteSpace, &saveptr);
  }

  return argumentCount;
}

// Function to determine the separator type in the input (sequential, concurrent, or pipe)
char *getSeparator(char *input)
{
  char *separator = NULL;

  // Check for each type of separator and return the appropriate one
  if ((separator = index(input, *SEPARATOR_SEQUENTIAL)) != NULL)
  {
    separator = SEPARATOR_SEQUENTIAL;
  }
  else if ((separator = index(input, *SEPARATOR_CONCURRENT)) != NULL)
  {
    separator = SEPARATOR_CONCURRENT;
  }
  else if ((separator = index(input, *SEPARATOR_PIPE)) != NULL)
  {
    separator = SEPARATOR_PIPE;
  }

  return separator;
}

// Function to create a Command structure from the input, handling redirection and other properties
Command *createCommand(char *input, int background, int pipe)
{
  char *token = NULL;
  char *stdinResult = index(input, *SEPARATOR_INPUT);
  char *stdoutResult = index(input, *SEPARATOR_OUTPUT);
  char *stderrResult = index(input, *SEPARATOR_ERROR);
  Command *command = calloc(1, sizeof(Command));

  // Check for output redirection
  if (stdoutResult != NULL)
  {
    token = strtok(input, SEPARATOR_OUTPUT);
    token = strtok(NULL, " ");
    command->redirection = 1;
    command->stdout = strdup(token);
  }
  // Check for input redirection
  else if (stdinResult != NULL)
  {
    token = strtok(input, SEPARATOR_INPUT);
    token = strtok(NULL, " ");
    command->redirection = 0;
    command->stdin = strdup(token);
  }
  // Check for standard error redirection
  else if (stderrResult != NULL)
  {
    token = strtok(input, SEPARATOR_ERROR);
    token = strtok(NULL, " ");
    command->redirection = 2; // Set redirection to standard error
    command->stderr = strdup(token);
  }

  // Get arguments and populate the command structure
  command->argc = getArguments(input, command);
  command->name = command->argv[0];
  command->background = background;
  command->pipe = pipe;

  // Check if the command has valid arguments
  if (command->argc > 0)
  {
    return command;
  }
  else
  {
    free(command);
    return NULL;
  }
}

void handleCommandLine(char *input, int background, int pipe, int iteration, Command **commands)
{
  int isBackground = 0;
  int isPipe = 0;
  char *separator = NULL;
  char *token = NULL;
  static int commandCount;

  if (iteration == 0)
  {
    commandCount = 0;
  }

  if ((separator = getSeparator(input)) != NULL)
  {
    token = strtok(input, separator);
    token = strtok(NULL, "");

    if (strcmp(separator, SEPARATOR_CONCURRENT) == 0)
    {
      isBackground = 1;
    }

    if (strcmp(separator, SEPARATOR_PIPE) == 0)
    {
      isPipe = commandCount + 1;
    }

    handleCommandLine(input, isBackground, isPipe, 1, commands);
  }
  else
  {
    commands[commandCount] = createCommand(input, background, pipe);
    commandCount++;
  }

  if (token != NULL)
  {
    handleCommandLine(token, background, pipe, 1, commands);
  }
}




/*
// Function to extract arguments from the input and populate the Command structure
int getArguments(char *input, Command *command)
{
  char *token = NULL;
  int argumentCount = 0;

  // Tokenize the input using whiteSpace as the delimiter
  token = strtok(input, whiteSpace);

  // Process each token
  while (token != NULL)
  {
    // Check for wildcard characters and expand them using glob if found
    if (index(token, WILDCARD_ALL) != NULL || index(token, WILDCARD_EXACT) != NULL)
    {
      glob_t result;
      glob(token, 0, NULL, &result);

      // Copy each expanded path to the command arguments
      for (int i = 0; i < result.gl_pathc; i++)
      {
        command->argv[argumentCount] = strdup(result.gl_pathv[i]);
        argumentCount++;
      }

      globfree(&result);
    }
    else
    {
      // Copy regular token to the command arguments
      command->argv[argumentCount] = strdup(token);
      argumentCount++;
    }

    // Move to the next token
    token = strtok(NULL, whiteSpace);
  }

  return argumentCount;
}
*/
