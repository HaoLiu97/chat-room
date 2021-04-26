// Implement the server which manages the interactions between clients in this file making use
// of the service functions in server_funcs.c to get the job done.

#include "blather.h"

server_t server_actual;
server_t * server = &server_actual;
int DO_ADVANCED;

// shutting down gracefully.
void grace_shutdown(int sig) {
    dbg_printf("shutdown gracefully.\n");
    server_shutdown(server);
    exit(0);
}

void ping_clients(int sig) {
    dbg_printf("ping clients\n");
    alarm(1);
    server_tick(server);
    dbg_printf("server has ran for %d second.\n", server->time_sec);
    server_ping_clients(server);
}

int main(int argc, char *argv[]) {
    if(argc <= 1) {
        log_printf("Please specify the server name.\n");
        return 0;
    }

    if(getenv("BL_ADVANCED")){
        DO_ADVANCED=1;
    }

    // The server should handle SIGTERM and SIGINT by shutting down gracefully.
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = grace_shutdown;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT,  &sa, NULL);

    // ping
    struct sigaction sa_ping;
    sigemptyset(&sa_ping.sa_mask);
    sa_ping.sa_handler = ping_clients;
    sa_ping.sa_flags = SA_RESTART; // restart poll
    sigaction(SIGALRM, &sa_ping, NULL);
    if(DO_ADVANCED) {
        alarm(1);
    }

    // start server
    server_start(server, argv[1], DEFAULT_PERMS);

    // infinite loop, quit by handle signal
    while(1) {
        dbg_printf("checking source.\n");
        server_check_sources(server);
        dbg_printf("check source done.\n");

        // handle join request
        if (server_join_ready(server)) {
            server_handle_join(server);
            dbg_printf("new client join server.\n");
        }

        // handle data sent from each client
        for (int i = 0; i < server->n_clients; i++) {
            if (server_client_ready(server, i)) {
                server_handle_client(server, i);
            }
        }
    }
    return 0;
}
