
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define READFD 0
#define WRITEFD 1
#define BUFSIZE 255

void check_failure(int res, char *msg) {
    if (res == -1) {
        perror(msg);
        /* rydde? */
        exit(EXIT_FAILURE);
    }
}

// child process from fork : reads
void child_main(int readfd) {

    char buf[BUFSIZE] = { 0 };
    int rc;

    while (strcmp(buf, "q"))
    {
        rc = read(readfd, buf, BUFSIZE - 1);
        check_failure(rc, "read");
        buf[rc] = '\0';
        printf("Child: %d | msg: %s\n", getpid(), buf);
    }

    close(readfd);
}

// parent process from fork : writes
void parent_main(int writefd) {

    int wc;
    char msg[BUFSIZE] = { 0 }; //  fills with 0's

    while (strcmp(msg, "q"))
    {
        fgets(msg, BUFSIZE, stdin);
        msg[strlen(msg) - 1] = 0; // removes newline

        wc = write(writefd, msg, strlen(msg));
        check_failure(wc, "write");
    }

    printf("Parent: %d\n", getpid());

    close(writefd);
}

int main(void) {

    pid_t is_parent; // process-id

    int fds[2];
    check_failure(pipe(fds), "pip");

    is_parent = fork();

    // parent
    if (is_parent) {
        check_failure(is_parent, "fork");

        close(fds[READFD]);
        parent_main(fds[WRITEFD]);
    }
    // child
    else {
        // child
        close(fds[WRITEFD]);
        child_main(fds[READFD]);
    }

    return 0;
}