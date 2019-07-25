#include "headers.h"

void compute_get_request(char *message, 
                    const char *url,
                    const char *url_params,
                    const char *IP, 
                    const char *JWT_token,
                    const char *cookies) {

    char *line = calloc(LINELEN, sizeof(char));

    if (url_params != NULL)
        sprintf(line, "GET %s?%s HTTP/1.1", url, url_params);
    else 
        sprintf(line, "GET %s HTTP/1.1", url);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    sprintf(line, "Host: %s", IP);
    compute_message(message, line); 
    memset(line, 0, LINELEN);

    if (JWT_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", JWT_token);
        compute_message(message, line);
        memset(line, 0, LINELEN);
    }

    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
        memset(line, 0, LINELEN);
    }

    strcat(message, "\r\n");
    free(line);

}

void compute_post_request(char *message, 
                    const char *url,
                    const char *IP, 
                    const char *JWT_token,
                    const char *cookies,
                    const char *data_type,
                    const char *data) {

    char *line = calloc(LINELEN, sizeof(char));
  
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    sprintf(line, "Host: %s", IP);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    if (JWT_token != NULL) {
        sprintf(line, "Authorization: Bearer %s", JWT_token);
        compute_message(message, line);
        memset(line, 0, LINELEN);
    }

    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
        memset(line, 0, LINELEN);
    }

    sprintf(line, "Content-Type: %s", data_type);
    compute_message(message, line);
    memset(line, 0, LINELEN);

    sprintf(line, "Content-Length: %ld", strlen(data));
    compute_message(message, line);
    free(line);

    strcat(message, "\r\n");

    compute_message(message, data);

}

void get_weather(JSON_Object *json_obj, char *response) {

    const char *full_url = NULL, *IP = NULL;
    char *form_data = calloc(BUFLEN, sizeof(char)), *name = calloc(BUFLEN, sizeof(char)), *message = calloc(BUFLEN, sizeof(char));
    int i;
    JSON_Object *query_params;

    // parsez queryParams
    query_params = json_value_get_object(json_object_get_value(json_obj, "queryParams"));
    strcpy(form_data, json_object_get_name(query_params, 0)); // extrag field1
    strcat(form_data, "=");
    strcat(form_data, json_value_get_string(json_object_get_value_at(query_params, 0))); // extrag value1
    for (i = 1; i < json_object_get_count(query_params); i++) { // continui sa concatenez perechi de tip field=value din queryParams
        strcat(form_data, "&");
        strcat(form_data, json_object_get_name(query_params, i));
        strcat(form_data, "=");
        strcat(form_data, json_value_get_string(json_object_get_value_at(query_params, i)));
    } 

    full_url = json_object_get_string(json_obj, "url");
    strncpy(name, full_url, strstr(full_url, "/") - full_url); // copiez in name doar ce se afla pana la '/' din url-ul intreg
    IP = get_ip(name); // aflu IP-ul url-ului
    free(name);
    if (strncmp(json_object_get_string(json_obj, "method"), "GET", 3) == 0) {
        compute_get_request(message,
                            strstr(json_object_get_string(json_obj, "url"), "/"), // transmit ce se afla dupa '/' din url-ul intreg
                            form_data,
                            IP, 
                            NULL,
                            NULL);
    }
    else {
        compute_post_request(message, 
                            strstr(json_object_get_string(json_obj, "url"), "/"),
                            IP,
                            NULL, 
                            NULL, 
                            json_object_get_string(json_obj, "type"), 
                            form_data);
    }
    free(form_data);

    int sockfd = open_connection(IP, 80); // port 80 pentru Web
    send_to_server(sockfd, message);
    free(message);
    receive_from_server(sockfd, response);
    close_connection(sockfd);
}