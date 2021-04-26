// Implement the client which allows a single user to communicate with the server in this file.
// The client must have multiple threads so you will need to implement some worker functions
// as thread entry points here.

#include <stdio.h>
#include <unistd.h>
#include "blather.h"


simpio_t simpio_actual;
simpio_t *simpio = &simpio_actual;

int server_fd;
client_t client_actual;
client_t *client = &client_actual;

pthread_t user_thread;
pthread_t server_thread;
simpio_t simpio_actual;
char pid[100]; // process id, used to name file

// The user thread performs an input loop until the user has completed a line.
// It then writes message data into the to-server FIFO to get it to the server
// and goes back to reading user input.
void *user_worker(void *arg) {
    while (1) {
        simpio_reset(simpio);
        iprintf(simpio, "");                                          // print prompt
        while (!simpio->line_ready && !simpio->end_of_input) {          // read until line is complete
            simpio_get_char(simpio);
        }
        mesg_t mesg;
        memset(&mesg, 0, sizeof(mesg));
        strcpy(mesg.name, client->name);
        strcpy(mesg.body, simpio_actual.buf); // fill mesg body with what user just input

        // if end_of_input, the client leave
        if (simpio_actual.end_of_input) {
            mesg.kind = BL_DEPARTED;
        } else {
            mesg.kind = BL_MESG;
        }

        // sent to the server
        long n_write = write(client->to_server_fd, &mesg, sizeof(mesg_t));
        check_fail(n_write == -1, 1, "write to fd %d error.\n", client->to_server_fd);

        // break the loop at the end of input
        if (simpio_actual.end_of_input) {
            break;
        }
        simpio_reset(&simpio_actual);
    }

    pthread_cancel(server_thread);

    return NULL;
}

// The server thread reads data from the to-client FIFO and prints to the screen
// as data is read.
void *server_worker(void *arg) {
    while (1) {
        mesg_t mesg;
        memset(&mesg, 0, sizeof(mesg));
        struct pollfd poll_fds[1];
        poll_fds[0].fd = client->to_client_fd;
        poll_fds[0].events |= POLLIN;
        int num = poll(poll_fds, 1, -1);
        if (num > 0) {
            if (poll_fds[0].revents & POLLIN) {
                read(client->to_client_fd, &mesg, sizeof(mesg_t));
                switch (mesg.kind) {
                    case BL_MESG:
                        iprintf(simpio, "[%s] : %s\n", mesg.name, mesg.body);
                        break;
                    case BL_JOINED:
                        iprintf(simpio, "-- %s JOINED --\n", mesg.name);
                        break;
                    case BL_DEPARTED: // actually won't happen here
                        iprintf(simpio, "-- %s DEPARTED --\n", mesg.name);
                        break;
                    case BL_SHUTDOWN:
                        iprintf(simpio, "!!! server is shutting down !!!\n");
                        pthread_cancel(user_thread);
                        break;
                    case BL_DISCONNECTED: // TODO ADVANCED
                        iprintf(simpio, "-- %s DISCONNECTED --\n", mesg.name);
                        break;
                    case BL_PING:
                        memset(&mesg, 0, sizeof(mesg));
                        strcpy(mesg.name, client->name);
                        // response to the server
                        long n_write = write(client->to_server_fd, &mesg, sizeof(mesg_t));
                        check_fail(n_write == -1, 1, "write to fd %d error.\n", client->to_server_fd);
                        break;
                }
            }
        }
    }
    return NULL;
}

void grace_leave(int sig) {
    mesg_t mesg;
    memset(&mesg, 0, sizeof(mesg));
    strcpy(mesg.name, client->name);
    mesg.kind = BL_DEPARTED;
    // sent to the server, tell other client about the leave
    long n_write = write(client->to_server_fd, &mesg, sizeof(mesg_t));
    check_fail(n_write == -1, 1, "write to fd %d error.\n", client->to_server_fd);

    close(server_fd);
    close(client->to_server_fd);
    close(client->to_client_fd);
    exit(0);
}

void init_simpio(char *name) {
    // simpio related
    char prompt[MAXNAME + 5];
    sprintf(prompt, "%s>> ", name); // create a prompt string
    simpio_set_prompt(simpio, prompt);  // set the prompt
    simpio_reset(simpio); // initialize io
    simpio_noncanonical_terminal_mode();
}

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        log_printf("Please specify the server name and user name.\n");
        return 0;
    }

    // The client should also handle SIGTERM and SIGINT by shutting down gracefully.
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = grace_leave;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    init_simpio(argv[2]);

    sprintf(pid, "%d", getpid());
    dbg_printf("server_name: %s    client_name: %s \n", argv[1], argv[2]); // server_name and client_name

    char server_fifo[MAXNAME];
    strcpy(server_fifo, argv[1]);
    strcat(server_fifo, ".fifo"); // server filename filled

    strcpy(client->name, argv[2]); // client name filled

    strcpy(client->to_server_fname, pid);
    strcat(client->to_server_fname, ".server.fifo"); // to_server_fname filled

    strcpy(client->to_client_fname, pid);
    strcat(client->to_client_fname, ".client.fifo"); // to_client_fname filled


    // create fifo files
    mkfifo(client->to_server_fname, DEFAULT_PERMS);
    mkfifo(client->to_client_fname, DEFAULT_PERMS);

    // open fifo files
    server_fd = open(server_fifo, O_RDWR);
    check_fail(server_fd == -1, 1, "open server fifo error\n");

    client->to_server_fd = open(client->to_server_fname, O_RDWR);
    check_fail(client->to_server_fd == -1, 1, "open to_server fifo error\n");

    client->to_client_fd = open(client->to_client_fname, O_RDWR);
    check_fail(client->to_client_fd == -1, 1, "open to_client fifo error\n");


    // fill join info
    join_t join;
    memset(&join, 0, sizeof(join_t));
    strcpy(join.name, argv[2]);
    strcpy(join.to_client_fname, client->to_client_fname);
    strcpy(join.to_server_fname, client->to_server_fname);
    long n_write = write(server_fd, &join, sizeof(join_t)); // tell server the client is joining
    check_fail(n_write == -1, 1, "write to %d error.\n", server_fd);

    // create pthreads
    int user_thread_id = pthread_create(&user_thread,
                                        NULL, user_worker, NULL);
    check_fail(user_thread_id != 0, 1, "create the user thread error.\n");
    int server_thread_id = pthread_create(&server_thread,
                                          NULL, server_worker, NULL);
    check_fail(server_thread_id != 0, 1, "create the server thread error.\n");

    // waiting for threads end
    pthread_join(user_thread, NULL);
    pthread_join(server_thread, NULL);

    close(server_fd);
    close(client->to_server_fd);
    close(client->to_client_fd);

    return 0;
}
