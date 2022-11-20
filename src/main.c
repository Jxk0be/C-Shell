/*	Jake Shoffner
	lab8 (JSH part 3 | main.c)
	20 November 2022
	In this part of the lab, we were to finish out the JSH program by adding part 3. Part 3 basically adds pipes to the shell, and it must interact with parts 1 and 2 as well.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "fields.h"

/**
* @name Print_Prompt.
* @brief Prints the user given prompt to stdout.
* @param[in] shell The name of the prompt to be printed.
* @return nothing, void function.
*/
void Print_Prompt(char *shell) {
	printf("%s", shell);
}

int main(int argc, char** argv) {
    char sh_name[200], **newargv, **pipeCom;
	int status, fd1, fd2, d1, id1, id, amp = 0, counter = 0, pipefd[2];

	/* Command line arguments check, and update shell name */
	if (argc > 2) {
		printf("Bad number of arguments -- 1 or 2\n"); exit(1);
	}
	if (argc == 1) strcpy(sh_name, "jsh1: \0");
	else if (argc == 2) {
		if (strcmp(argv[1], "-\0") == 0) strcpy(sh_name, "");
		else strcpy(sh_name, argv[1]);
	}
	
	Print_Prompt(sh_name);
	IS input = new_inputstruct(NULL);

	/* This section is looping through each command and doing the relative forking and exec calls */
	while (get_line(input) >= 0) {
        newargv = (char **)malloc(sizeof(char *) * (input->NF + 1));
        pipeCom = (char **)malloc(sizeof(char *) * (input->NF + 1));

        if (strcmp(input->fields[0], "exit\0") == 0) break;
		
		/* Resetting the counter and the ampersand flag for each line of commands */
		counter = 0;
		amp = 0;
		
		/* Filling up the newargv array and making last index NULL */
        for (unsigned int i = 0; i < input->NF; ++i) {
			if (strcmp(input->fields[i], "&\0") == 0) amp = 1;
			newargv[i] = input->fields[i]; 
		}
        newargv[input->NF] = NULL;
	
		/* Same fork() from parts 1 and 2 */
        int frk = fork();
        if (frk == 0){
            for (unsigned int i = 0; i < input->NF; i++) {
                pipeCom[counter] = newargv[i];
				
				/* Checking for file redirection and pipes */
                if (strcmp(newargv[i], "<\0") == 0) {
                    fd1 = open(newargv[i + 1], O_RDONLY);
                    if (fd1 < 0) exit(1);
                    if (dup2(fd1, 0) != 0) exit(1);

                    close(fd1);
                    newargv[i] = NULL;
                    pipeCom[counter] = NULL;
                }
                else if (strcmp(newargv[i], ">\0") == 0) {
                    fd2 = open(newargv[i + 1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
                    if (fd2 < 0) exit(2);
                    if (dup2(fd2, 1) != 1) exit(1);

                    close(fd2);
                    newargv[i] = NULL;
                    pipeCom[counter] = NULL;
                }
                else if (strcmp(newargv[i], ">>\0") == 0) {
                    fd2 = open(newargv[i + 1], O_WRONLY | O_APPEND | O_CREAT, 0644);
                    if (fd2 < 0) exit(2);
                    if (dup2(fd2, 1) != 1) exit(1);

                    close(fd2);	
                    newargv[i] = NULL;
                    pipeCom[counter] = NULL;
                }
                else if (strcmp(newargv[i], "|\0") == 0) {
                    int p = pipe(pipefd);
                    if (p < 0) exit(1);

                    /* NULL out the "|"  from both arrays */
                    pipeCom[counter] = NULL;
                    pipeCom[i] = NULL;
					
					/* fork() for the pipe(). Thanks to Dr. James Plank for the structure in his notes
					   web.eecs.utk.edu/~jplank/plank/classes/cs360/360/notes/Pipe/src/headsort.c */
                    int frk1 = fork();
                    if (frk1 == 0) {
                        if (dup2(pipefd[1], 1) != 1) exit(1);
                        close(pipefd[0]);
                        close(pipefd[1]);

                        /* Basically, we need to call exec() on everything to the left of the "|" */
                        execvp(pipeCom[0], pipeCom);
                        perror(pipeCom[0]);
                        exit(1);
                    }
                    else {
						/* We need to wait if there's an ampersand present */
                        if (amp == 0) {
                            while(1) {
                                id1 = wait(&d1);
                                if (id1 == frk1) break;
                            }
                        }
						
						/* Finish with the dup call for the pipefd[0] */
						if (dup2(pipefd[0], 0) != 0) exit(1);
                        close(pipefd[0]);
                        close(pipefd[1]);
                        
						/* Reset the countre variable and skip the increment */
						counter = 0;
						continue;
                    }
                }
                counter++;
            }
			
			/* If there's an ampersand, then we need to NULL it out */
            if (amp == 1) {    
                pipeCom[counter - 1] = NULL;
                newargv[input->NF - 1] = NULL;
            }
			
			/* Same as before from parts 1 and 2 */
            execvp(pipeCom[0], pipeCom);
            perror(pipeCom[0]);
            exit(1);
        }
        else {
            if (amp == 0) {
                while(1) {
                    id = wait(&status);
                    if (id == frk) break;
                }
            }
        }
    }
}
