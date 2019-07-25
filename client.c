#include "headers.h"

int main(int argc, char *argv[]) {

    char *response = calloc(BUFLEN, sizeof(char));

    start_tasks(response);
    printf("%s\n", response);
    free(response);

    return 0;
}