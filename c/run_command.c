/*
 * run_command.c - Handles the execution of commands on the local
 * filesystem.
 *
 * Copyright (C) 2012 Logicube, Inc.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define LOG_DEBUG(format, ...)          printf("%s==>%d " format "\n", __FILE__, __LINE__, ## __VA_ARGS__ )
#define LOG_INFO(format, ...)           printf("%s==>%d " format "\n", __FILE__, __LINE__, ## __VA_ARGS__ )
#define LOG_WARNING(format, ...)        printf("%s==>%d " format "\n", __FILE__, __LINE__, ## __VA_ARGS__ )
#define LOG_ERROR(format, ...)          printf("%s==>%d " format "\n", __FILE__, __LINE__, ## __VA_ARGS__ )

/**
  * free2 - helper function to free an array of strings
  *
  * @v: an array of strings
  *
  * Usually used to free the output from run_command_sync. This is just
  * a convenience function so that multiple calls to free are not
  * necessary.
  *
  */
void free2(char **v)
{
	int i;
	for (i = 0; v[i] != NULL; i++)
	{
		free(v[i]);
		v[i] = NULL;
	}
	if (v != NULL)
		free(v);
}

/**
  * run_command_sync_env - run a command synchronously with the
  * specified enviroment settings
  *
  * @cmd: a const array of the command and its arguments.
  * @out: the location to save the output, can be NULL.
  * @lines: the number of lines of the output to save, can be 0.
  * @spid: return value of the pid of the spawned process, can be NULL.
  * @envp: NULL terminated array of enviroment settings
  *
  * @Returns: -1 on error, otherwise the exit status of the child
  */
int run_command_sync_env(const char **cmd, char ***out, unsigned int lines,
		pid_t *spid, char *const envp[])
{
	pid_t pid = 0;
	int pipefd[2];
	int status;
	int save;
	int res;
	int i = 0;
	char line[256];
	FILE *output;

	/* Print out enviroment settings */
	if (envp != NULL)
	{
		while(envp[i])
		{
			LOG_INFO("run_command_sync_env: envp[%d]=%s", i, envp[i]);
			i++;
		}
	}

	/* Create the pipe */
	res = pipe(pipefd);
	if (res == -1)
		LOG_ERROR("run_command_sync_env: Failed to create pipe");

	/* Print out all the arguments */
	LOG_INFO("run_command_sync_env: Spawning process with command: %s", cmd[0]);
	for (i = 1; cmd[i] != NULL; i++)
		LOG_INFO("run_command_sync_env: arg[%d]: %s", i, cmd[i]);

	/* Create the fork */
	pid = fork();

	/* in child */
	if (pid == 0)
	{
		/* close read end of pipe */
		close(pipefd[0]);

		/* redirect standard out/err to our pipe */
		LOG_INFO("STDOUT_FILENO=%d STDERR_FILENO=%d", STDOUT_FILENO,
				STDERR_FILENO);
		dup2(pipefd[1], STDOUT_FILENO);
		dup2(pipefd[1], STDERR_FILENO);
		close(pipefd[1]);

		/* replace process with cmd */
		if (envp != NULL)
			execvpe(cmd[0], (char **) cmd, envp);
		else
			execvp(cmd[0], (char **) cmd);
	}

	/* in parent, save the child's process id */
	if (spid != NULL)
	{
		LOG_DEBUG("run_command_sync_env: %s pid = %d", cmd[0], pid);
		*spid = pid;
	}

	/* in parent, close the write end, and read the read end */
	close(pipefd[1]);
	output = fdopen(pipefd[0], "r");
	if (output == NULL)
	{
		int err = errno;
		LOG_ERROR("run_command_sync_env: Failed to open read pipe, err= %d - '%s'",
				err, strerror(err));
	}

	/* Are we going to save the output */
	save = (lines && out != NULL) ? 1 : 0;
	if (save)
		*out = malloc(sizeof(char *) * lines);

	/* Read output */
	for (i = 0; fgets(line, sizeof(line), output) != NULL; i++)
	{
		/* Remove trailing newline */
		size_t ln = strlen(line) - 1;
		line[ln] = '\0';
		LOG_DEBUG("output: '%s'", line);

		/* Only save output if there is space */
		if (i < lines)
			if (save)
				(*out)[i] = strdup(line);
	}

	/* Make sure last index is the max number of lines */
	if (i >= lines && lines > 0)
	{
		LOG_DEBUG("run_command_sync_env: Output lines (%d) exceeded max lines (%d)",
				i, lines);
		i = lines - 1;
		LOG_DEBUG("run_command_sync_env: Adjusted index to: %d", i);
	}

	if (save)
		(*out)[i] = NULL;

	/* Wait for child to terminate */
	res = waitpid(pid, &status, 0);

	/* Check if successfull */
	if (res == -1)
	{
		LOG_ERROR("run_command_sync_env: waitpid(%d) was not successfull", pid);
		return -1;
	}

	/* Check exit status */
	if (WIFEXITED(status) == 0)
		LOG_ERROR("run_command_sync_env: '%s' exited abnormally", cmd[0]);
	else
	{
		int exit_status = WEXITSTATUS(status);
		LOG_INFO("run_command_sync_env: '%s' exited normally with return value: %d",
				cmd[0], exit_status);
		return exit_status;
	}

	return -1;
}

/**
  * run_command_sync - run a command synchronously
  *
  * @cmd: a const array of the command and its arguments.
  * @out: the location to save the output, can be NULL.
  * @lines: the number of lines of the output to save, can be 0.
  * @spid: return value of the pid of the spawned process, can be NULL.
  *
  * A wrapper around run_command_sync_env().
  *
  * @Returns: -1 on error, otherwise the exit status of the child
  */
int run_command_sync(const char **cmd, char ***out, unsigned int lines,
		pid_t *spid)
{
	return run_command_sync_env(cmd, out, lines, spid, NULL);
}

int main()
{
	int res;
	const char *newdbentry[] = { "/home/amit/src/open-iscsi/usr/iscsiadm",
	                             "--mode",
								 "discoverydb",
	                             "--type",
								 "sendtargets",
	                             "--portal",
								 "192.168.1.140",
	                             "--op=new",
	                             NULL };

#if 0
	const char *newdbentry[] = { "ls",
	                             "-l",
								 "/home/amit",
	                             NULL };
#endif

#if 0
	/* Insert new DB */
	FILE *out = popen("/home/amit/src/open-iscsi/usr/iscsiadm --mode discoverydb "
			"--type sendtargets --portal 192.168.1.140 --op=new", "r");
	if (out == NULL)
	{
		int err = errno;
		LOG_ERROR("main: Failed to open read pipe, err= %d - '%s'",
				err, strerror(err));
	}

	pclose(out);
#endif

	res = run_command_sync(newdbentry, NULL, 0, NULL);
	if (res != 0)
	{
		LOG_WARNING("iscsi_discover: Failed to insert new entry in database for portal");
	}

	return 0;
}
