#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <sys/types.h>
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "parson.h"

#define BUFLEN 4096
#define LINELEN 1000
#define DEFAULT_IP "185.118.200.35"
#define DEFAULT_PORT 8081

/* functii tasks.c */

void start_tasks(char *response);
void task_2(char *prev_response, char *response);
void task_3(char *prev_response, char *response);
void task_4(char *prev_response, char *response, const char *JWT_token);
void task_5(char *prev_response, char *response, const char *JWT_token);

/* functii requests.c */

// construieste un request de tip GET
void compute_get_request(char *message, 
                    const char *url,
                    const char *url_params,
                    const char *IP, 
                    const char *JWT_token,
                    const char *cookies);

// construieste un request de tip POST
void compute_post_request(char *message, 
                    const char *url,
                    const char *IP, 
                    const char *JWT_token,
                    const char *cookies,
                    const char *data_type,
                    const char *data);

// trimite weather request-ul utilizand informatiile existente in json_obj si retine raspunsul primit
void get_weather(JSON_Object *json_obj, char *response);

/* functii helpers.c */

void error(const char *msg);
void compute_message(char *message, const char *line);
int open_connection(const char *IP, short PORT);
void close_connection(int sockfd);
void send_to_server(int sockfd, char *message);
void receive_from_server(int sockfd, char *res);
void get_cookies(char *cookies, char *response); // parseaza cookie-urile dintr-un raspuns primit de la server
const char *get_ip(const char* name); // afla IP-ul url-ului dat ca parametru
