#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 80 // 80 chars per line, per command, should be enough.
#define ARRAY_SIZE 1000 // pids and alias arrays

pid_t pids[ARRAY_SIZE];
int pidIndex = 0, aliasNum = 0;
char aliasArr[ARRAY_SIZE][2][MAX_LINE]; // [#][0] -> alias, [#][1] -> command

/*The setup function below will not return any value, but it will just: read
in the next command line; separate it into distinct arguments (using blanks as
delimiters), and set the args array entries to point to the beginning of what
will become null-terminated, C-style strings. */
void setup(char inputBuffer[], char *args[], int *background)
{
	int length, // # of characters in the command line
	i, // loop index for accessing inputBuffer array
	start, // index where beginning of next command parameter is
	ct; // index of where to place the next parameter into args[]

	ct = 0;

	// read what the user enters on the command line
	length = read(STDIN_FILENO, inputBuffer, MAX_LINE);

	/*0 is the system predefined file descriptor for stdin (standard input),
	   which is the user's screen in this case. inputBuffer by itself is the
	   same as &inputBuffer[0], i.e. the starting address of where to store
	   the command that is read, and length holds the number of characters
	   read in. inputBuffer is not a null terminated C-string. */

	start = -1;
	if (length == 0)
		exit(0); // ^d was entered, end of user command stream

	// the signal interrupted the read system call
	/*if the process is in the read() system call, read returns -1
	However, if this occurs, errno is set to EINTR. We can check this  value
	and disregard the -1 value */
	if ((length < 0) && (errno != EINTR))
	{
		perror("error reading the command");
		exit(-1); // terminate with error code of -1
	}

	// examine every character in the inputBuffer
	for (i = 0; i < length; i++)
	{

		switch (inputBuffer[i])
		{
			case ' ':
			case '\t':
				// argument separators
				if (start != -1)
				{
					args[ct] = &inputBuffer[start]; // set up pointer
					ct++;
				}

				inputBuffer[i] = '\0'; // add a null char; make a C string
				start = -1;
				break;

			case '\n':
				// should be the final char examined
				if (start != -1)
				{
					args[ct] = &inputBuffer[start];
					ct++;
				}

				inputBuffer[i] = '\0';
				args[ct] = NULL; // no more arguments to this command
				break;

			default:
				// some other character
				if (start == -1)
					start = i;
				if (inputBuffer[i] == '&')
				{
					*background = 1;
					inputBuffer[i - 1] = '\0';
					i++;
                    args[ct] = NULL;
				}
		}
	}
	args[ct] = NULL; // just in case the input line was > 80
}

int execute(int background, char *args[], char *envPath)
{
    int redirectNo = 0, i = 0, fd, fd1;

    while (args[i] != NULL) // checks ioRedirect commands
	{
        if (strcmp(">", args[i]) == 0) // > file.out
		{
			redirectNo = 1;
            break;
        }
		else if (strcmp(">>", args[i]) == 0) // >> file.out
		{
			redirectNo = 2;
			break;
		}
		else if (strcmp("<", args[i]) == 0 && strcmp(">", args[i+2]) == 0) // < file.in > file.out
		{
			redirectNo = 4;
			break;
		}
		else if (strcmp("<", args[i]) == 0) // < file.in
		{
			redirectNo = 3;
			break;
		}
        i++;
    }

    pid_t pid = fork();

    if (pid < 0)
        fprintf(stderr, "Fork failed!\n");

    if (pid == 0)
	{
		if (redirectNo != 0) // search ioRedirect if any
		{
			switch (redirectNo)
			{
				case 1: // > file.out
					args[i] = NULL;
					fd = open(args[i+1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
					dup2(fd, fileno(stdout));
					close(fd);
					break;
				case 2: // >> file.out
					args[i] = NULL;
					fd = open(args[i+1], O_WRONLY | O_APPEND | O_CREAT, 0644);
					dup2(fd, fileno(stdout));
					close(fd);
					break;
				case 3: // < file.in
					args[i] = NULL;
					fd = open(args[i+1], O_RDONLY, 0644);
					dup2(fd, fileno(stdin));
					close(fd);
					break;
				case 4: // < file.in > file.out
					args[i] = NULL;
					fd = open(args[i+1], O_RDONLY, 0644);
					fd1 = open(args[i+3], O_WRONLY | O_TRUNC | O_CREAT, 0644);
					dup2(fd, fileno(stdin));
					dup2(fd1, fileno(stdout));
					close(fd1);
					break;
			}
		}
		
		// search alias commands if any
		char t[50][80];
		int j = 1, a = 0, x = 0;;
		for (i = 0; i < aliasNum; i++)
		{
			if (strcmp(args[0], aliasArr[i][0]) == 0)
			{
				while (1)
				{
					t[a][x] = aliasArr[i][1][j];
					j++;
					x++;
					
					if (aliasArr[i][1][j] == ' ' || aliasArr[i][1][j] == '\0')
					{
						t[a][x] = '\0';
						args[a] = t[a];
						a++;
						j++;
						if (aliasArr[i][1][j] == '\0')
							break;
						x = 0;
					}
				}
				args[a] = NULL;
			}
		}
		
		// execute commands
		char *token = strtok(envPath, ":");
		char temp[80] = "";
		
		while(execv(temp, args) != 0)
		{
            strcpy(temp, token);
            strcat(temp, "/");
            strcat(temp, args[0]);
			token = strtok(NULL, ":");
		}
		
		fprintf(stdout, "Command %s not found!\n", args[0]);
        exit(0);
	}
	
	// check background
	if (background != 0)
	{
		pids[pidIndex] = pid;
		pidIndex++;
		
		printf("[%d] : %d\n", pidIndex, pid);
	}
	else
		waitpid(pid, NULL, 0);
}

void removeChar(char *str, char garbage)
{
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++)
	{
        *dst = *src;
        if (*dst != garbage)
			dst++;
    }
    *dst = '\0';
}

void alias(char *args[])
{
	int i = 0;
    char temp[80];
    int flag = 0;
	
	if (strcmp(args[1], "-l") == 0) // lists aliases
	{
        if (aliasNum == 0)
			printf("\tThere is nothing to list.\n");
		else
		{
			for (i = 0; i < aliasNum; i++)
				printf("\t%s \"%s\"\n", aliasArr[i][0], aliasArr[i][1]);
		}
        return;
    }

    while(args[i] != NULL)
    {
        if (args[i][0] == '\"')
        {
            if (args[i][strlen(args[i])-1] == '\"')
                flag = 1;
            removeChar(args[i], '\"');
            strcat(temp, args[i]);
            break;
        }
        i++;
    }

    i++;

    while(args[i] != NULL && flag == 0)
    {
        if(args[i][strlen(args[i])-1] == '\"')
        {
            strcat(temp, " ");
            removeChar(args[i], '\"');
            strcat(temp, args[i]);
            break;
        }
        strcat(temp, " ");
        strcat(temp, args[i]);
        i++;
    }

	// add the alias to alias list
    strcpy(aliasArr[aliasNum][0], args[i+1]);
	strcpy(aliasArr[aliasNum][1], temp);
	aliasNum++;
}

void unalias(char *args[])
{
	int i, j;
	for (i = 0; i < aliasNum; i++) {
		if (strcmp(args[1], aliasArr[i][0]) == 0)
		{
			for (j = i; j < (aliasNum - 1); j++)
			{
				strcpy(aliasArr[j][0], aliasArr[j+1][0]);
				strcpy(aliasArr[j][1], aliasArr[j+1][1]);
			}
			aliasNum--;
			break;
		}
	}
}

void runCommand(int background, char *args[], char *envPath)
{
	if (strcmp(args[0], "alias") == 0)
	{
		alias(args);
		return;
	}
	else if (strcmp(args[0], "unalias") == 0)
	{
		unalias(args);
		return;
	}
	else if (strcmp(args[0], "exit") == 0)
	{
		printf("Exited.\n");
		exit(0);
	}
	else
	{
		execute(background, args, envPath);
		return;
	}
}

int main(void)
{
	char inputBuffer[MAX_LINE]; // buffer to hold command entered
	int background; // equals 1 if a command is followed by '&'
	char *args[MAX_LINE / 2 + 1]; // command line arguments
	char *envPath = getenv("PATH");
	
	while (1)
	{
		background = 0;
		printf("myshell> ");
		// setup() calls exit() when Control-D is entered
		fflush(NULL);
		setup(inputBuffer, args, &background);
		
		if(args[0] == NULL)
			continue;
		
		runCommand(background, args, envPath);

		/**the steps are:
		(1) fork a child process using fork()
		(2) the child process will invoke execv()
		(3) if background == 0, the parent will wait,
		otherwise it will invoke the setup() function again. */
	}
}