#include "headers.h"

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line) {
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(const char *IP, short PORT) {
    struct sockaddr_in serv_addr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    if (PORT != -1)
        serv_addr.sin_port = htons(PORT);
    else
        serv_addr.sin_port = htons(DEFAULT_PORT);
    if (IP != NULL)
        inet_aton(IP, &serv_addr.sin_addr);
    else
        inet_aton(DEFAULT_IP, &serv_addr.sin_addr);
    if (sockfd < 0)
        error("ERROR opening socket");

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting socket");

    return sockfd;
}

void close_connection(int sockfd) {
    close(sockfd);
}

void send_to_server(int sockfd, char *message) {
    int bytes, sent = 0;
    int total = strlen(message);
    do {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0)
            error("ERROR writing message to socket");
        if (bytes == 0)
            break;
        sent += bytes;
    } while (sent < total);
}

void receive_from_server(int sockfd, char *res) {
    char *response = calloc(BUFLEN, sizeof(char));
    int total = BUFLEN;
    int received = 0;
    do {
        int bytes = read(sockfd, response + received, total - received);
        if (bytes < 0)
            error("ERROR reading response from socket");
        if (bytes == 0) 
            break;
        received += bytes;
    } while (received < total);

    if (received == total)
        error("ERROR storing complete response from socket");

    strcpy(res, response);
    free(response);
}

void get_cookies(char *cookies, char *response) {
    char *tmp1, *tmp2;

    tmp1 = strstr(response, "Set-Cookie:") + strlen("Set-Cookie:"); // adresa primului rand ce contine cookie
    if (tmp1 == NULL)
        return ;
    tmp2 = strstr(tmp1, ";"); // adresa unde se termina string-ul efectiv cu cookie
    strncpy(cookies, tmp1, tmp2 - tmp1); // retin cookie efectiv 
    while (strstr(tmp2, "Set-Cookie:") != NULL) { // continui pentru a concatena restul cookie-urilor
        strcat(cookies, "; ");
        tmp1 = strstr(tmp2, "Set-Cookie:") + strlen("Set-Cookie:");
        tmp2 = strstr(tmp1, ";");
        strncat(cookies, tmp1, tmp2 - tmp1);
    }
}

const char* get_ip(const char* name) {
    int ret;
    struct addrinfo hints, *result, *p;
    char *res;

    result = calloc(1, sizeof(struct addrinfo));
    memset((char *) &hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;

    ret = getaddrinfo(name, NULL, &hints, &result); 
    if (ret < 0)
        gai_strerror(ret);

    for (p = result; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *addr = (struct sockaddr_in *)p->ai_addr;
            res = calloc(80, sizeof(char));
            inet_ntop(AF_INET, (struct in_addr *)&addr->sin_addr, res, 80);
            freeaddrinfo(result);
            return res;
        }
        else if (p->ai_family == AF_INET6) { // IPv6
            struct sockaddr_in6 *addr = (struct sockaddr_in6 *)p->ai_addr;
            res = calloc(80, sizeof(char));
            inet_ntop(AF_INET6, (struct in6_addr *)&addr->sin6_addr, res, 80);
            freeaddrinfo(result);
            return res;
        }
    }

    freeaddrinfo(result);
    return NULL;
}
