#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


void close_pipe(int *pipe)
{
    close(pipe[0]);
    close(pipe[1]);
}

int main()
{
    pid_t pid, pid2, pid3, pid4;
    int fd1[2], fd2[2], fd3[2], fd4[2];

    char foo[4096] = {0};

    if (pipe(fd1) || pipe(fd2) || pipe(fd3) || pipe(fd4))
    {
        // Even if one pipe() fails, no point in continuing further, exit early
        fprintf(stderr, "One or more pipe() failed.\n");
        return EXIT_FAILURE;
    }

    pid = fork();

    if (pid == (pid_t) -1) {

        fprintf(stderr, "For failed in Parent process.\n");
        return EXIT_FAILURE;
    }


    if (pid == (pid_t) 0)
    {

        /* Child1 process */

        pid2 = fork();

        if (pid2 == (pid_t) -1) {

            fprintf(stderr, "For failed in Child1 process.\n");
            return EXIT_FAILURE;
        }


        if (pid2 == (pid_t) 0) {

            /* Child2 process */

            pid3 = fork();

            if (pid3 == (pid_t) -1) {

                fprintf(stderr, "For failed in Child2 process.\n");
                return EXIT_FAILURE;
            }

            if (pid3 == (pid_t) 0) {

                /* Child3 process */

                pid4 = fork();

                if (pid4 == (pid_t) -1) {

                    fprintf(stderr, "For failed in Child3 process.\n");
                    return EXIT_FAILURE;
                }

                if (pid4 == (pid_t) 0) {

                    /* Child4 process */

                    close_pipe(fd2);
                    close_pipe(fd3);

                    // Read data from Child3
                    close(fd4[1]);
                    dup2 (fd4[0], STDIN_FILENO);
                    close(fd4[0]);

                    // Send data to main parent
                    close(fd1[0]);
                    dup2 (fd1[1], STDOUT_FILENO);
                    close(fd1[1]);

                    execl("/bin/head", "head", "-n", "1", (char *)0);
                    exit(EXIT_SUCCESS);

                }


                close_pipe(fd1);
                close_pipe(fd2);

                // Read data from Child2
                close(fd3[1]);
                dup2 (fd3[0], STDIN_FILENO);
                close(fd3[0]);

                // Send data to Child4
                close(fd4[0]);
                dup2 (fd4[1], STDOUT_FILENO);
                close(fd4[1]);

                execl("/bin/awk", "awk", "{print $9}", (char *)0);
                exit(EXIT_SUCCESS);
            }

            close_pipe(fd4);
            close_pipe(fd1);

            // Read data from Child1
            close(fd2[1]);
            dup2 (fd2[0], STDIN_FILENO);
            close(fd2[0]);

            // Send data to Child3
            close(fd3[0]);
            dup2 (fd3[1], STDOUT_FILENO);
            close(fd3[1]);

            execl("/bin/grep", "grep", "ex", (char *)0);
            exit(EXIT_SUCCESS);

        }

        close_pipe(fd1);
        close_pipe(fd3);
        close_pipe(fd4);

        // Send data to Child2
        dup2 (fd2[1], STDOUT_FILENO);
        close(fd2[0]);
        close(fd2[1]);

        execl("/bin/ls", "ls", "-lha", (char *)0);
        exit(EXIT_SUCCESS);

    }

    close_pipe(fd3);
    close_pipe(fd4);
    close_pipe(fd2);

    close(fd1[1]);

    // Read data from Child4
    int nbytes = read(fd1[0], foo, sizeof(foo));
    printf("%s", foo);
    wait(NULL);

    return EXIT_SUCCESS;
}
