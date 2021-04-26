# include "blather.h"

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("Please specify the log file name.\n");
    }
    FILE *log = fopen(argv[1], "r");
    check_fail(log == NULL, 1, "open log file %d error.\n", argv[1]);
    who_t who;
    fread(&who, sizeof(who_t), 1, log);
    printf("%d CLIENTS\n", who.n_clients);
    for (int i = 0; i < who.n_clients; ++i) {
        printf("%d: %s\n", i, who.names[i]);
    }

    printf("MESSAGES\n");
    mesg_t mesg;
    while (fread(&mesg, sizeof(mesg_t), 1, log)) {
        switch (mesg.kind) {
            case BL_MESG:
                printf("[%s] : %s\n", mesg.name, mesg.body);
                break;
            case BL_JOINED:
                printf("-- %s JOINED --\n", mesg.name);
                break;
            case BL_DEPARTED: // actually won't happen here
                printf("-- %s DEPARTED --\n", mesg.name);
                break;
            case BL_SHUTDOWN:
                printf("!!! server is shutting down !!!\n");
                break;
            case BL_DISCONNECTED: // TODO ADVANCED
                printf("-- %s DISCONNECTED --\n", mesg.name);
                break;
            case BL_PING:
                break;
        }
    }

    fclose(log);
    return 0;
}
