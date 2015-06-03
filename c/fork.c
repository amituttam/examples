#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

char **run_command_sync(const char **cmd)
{
	printf("run_command: Spawning process with command: %s\n", cmd[0]);
	pid_t pid = 0;
	int pipefd[2];
	FILE *output;
	char line[256];
	int status;
	int res;
	char **strout;

	res = pipe(pipefd);
	if (res == -1)
		printf("run_command: Failed to create pipe\n");

	/* Create the fork */
	pid = fork();

	/* in child */
	if (pid == 0)
	{
		/* close read end of pipe */
		close(pipefd[0]);

		/* redirect standard out/err to our pipe */
		dup2(pipefd[1], STDOUT_FILENO);
		dup2(pipefd[1], STDERR_FILENO);
		close(pipefd[1]);

		/* replace process with dpkg */
		execvp(cmd[0], (char **) cmd);
	}

	/* in parent, close the write end, and redirect read end */
	close(pipefd[1]);
	output = fdopen(pipefd[0], "r");
	if (output == NULL)
		printf("run_command: cmdfailed\n");

	/* Read output */
	int i = 0;
	strout = malloc(sizeof(char) * 50);
	while (fgets(line, sizeof(line), output) != NULL)
	{
		printf("output: '%s'\n", line);
		strout[i] = strdup(line);
		i++;
	}
	strout[i] = NULL;
	printf("output: DONE\n");

	res = waitpid(pid, &status, 0);
	if (!WIFEXITED(status) != 0)
		printf("xmlrpc_application_upgrade: Could not run dpkg, error: '%d'\n", res);
	else
		printf("xmlrpc_application_upgrade: dpkg completed successfully\n");

	fclose(output);
	return strout;
}

int main()
{
	const char *cmd[] = { "ls",
                          NULL };

	char **out;
	out = run_command_sync(cmd);
	int i;
	for (i = 0; out[i] != NULL; i++)
	{
		printf("OUT: %s", out[i]);
		free(out[i]);
		out[i] = NULL;
	}
	free(out);
	return 0;
}
