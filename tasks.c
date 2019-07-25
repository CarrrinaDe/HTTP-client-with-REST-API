#include "headers.h"

void start_tasks(char *response) {

	char *message = calloc(BUFLEN, sizeof(char));

	compute_get_request(message, 
                        "/task1/start", 
                        NULL,
                        DEFAULT_IP,
                        NULL,
                        NULL); // construiesc request-ul pe care il voi trimite

    int sockfd = open_connection(DEFAULT_IP, DEFAULT_PORT); // deschid un socket
    send_to_server(sockfd, message); // trimit mesajul
    free(message);
    receive_from_server(sockfd, response); // primesc raspunsul in response
    close_connection(sockfd); // inchid conexiunea

    char *prev_response = calloc(BUFLEN, sizeof(char));
    strcpy(prev_response, response); // salvez raspunsul primit
    memset(response, 0, BUFLEN); // reinitializez variabila in care voi retine raspunsul de la urmatorul task
    task_2(prev_response, response); // trec la urmatorul task
    free(prev_response);
}

void task_2(char *prev_response, char *response) {

	char *cookies = calloc(BUFLEN, sizeof(char));
	char *form_data = calloc(BUFLEN, sizeof(char));
	char *message = calloc(BUFLEN, sizeof(char));
    int i;
    JSON_Object *response_obj, *data_obj; 

    get_cookies(cookies, prev_response); // extrag toate cookie-urile in cookies 

    response_obj = json_object(json_parse_string(strstr(prev_response, "\r\n\r\n") + strlen("\r\n\r\n"))); // structura de tip JSON din body-ul raspunsului
    data_obj = json_value_get_object(json_object_get_value(response_obj, "data")); // obiectul JSON din campul data din response_obj
    strcpy(form_data, json_object_get_name(data_obj, 0));   // field 1 (username)
    strcat(form_data, "=");
    strcat(form_data, json_value_get_string(json_object_get_value_at(data_obj, 0))); // value 1
    for (i = 1; i < json_object_get_count(data_obj); i++) {     // concatenez urmatoarele perechi de field=value
        strcat(form_data, "&");
        strcat(form_data, json_object_get_name(data_obj, i));
        strcat(form_data, "=");
        strcat(form_data, json_value_get_string(json_object_get_value_at(data_obj, i)));
    } 

    if (strncmp(json_object_get_string(response_obj, "method"), "GET", 3) == 0) {
        compute_get_request(message, 
                            json_object_get_string(response_obj, "url"), 
                            form_data, 
                            DEFAULT_IP,
                            NULL, 
                            cookies);
    }
    else {
        compute_post_request(message, 
                            json_object_get_string(response_obj, "url"), 
                            DEFAULT_IP,
                            NULL, 
                            cookies, 
                            json_object_get_string(response_obj, "type"), 
                            form_data);
    }
    free(cookies);
    free(form_data);

    int sockfd = open_connection(DEFAULT_IP, DEFAULT_PORT);
    send_to_server(sockfd, message);
    free(message);
    receive_from_server(sockfd, response);
    close_connection(sockfd);

	memset(prev_response, 0, BUFLEN);
    strcpy(prev_response, response);
    memset(response, 0, BUFLEN);
    task_3(prev_response, response);

}

void task_3(char *prev_response, char *response) {

	char *cookies = calloc(BUFLEN, sizeof(char));
	char *form_data = calloc(BUFLEN, sizeof(char));
	char *message = calloc(BUFLEN, sizeof(char));
	const char *JWT_token = NULL;
    int i;
    JSON_Object *response_obj, *data_obj, *query_params; 
	
	get_cookies(cookies, prev_response);
    response_obj = json_object(json_parse_string(strstr(prev_response, "\r\n\r\n") + strlen("\r\n\r\n")));
    data_obj = json_value_get_object(json_object_get_value(response_obj, "data"));

    JWT_token = json_object_get_string(data_obj, "token");
    strcpy(form_data, "raspuns1=omul&raspuns2=numele");
    
    // parsez queryParams
    query_params = json_value_get_object(json_object_get_value(data_obj, "queryParams"));
    for (i = 0; i < json_object_get_count(query_params); i++) { // concatenez parametri de tip field=value la form_data
        strcat(form_data, "&");
        strcat(form_data, json_object_get_name(query_params, i));
        strcat(form_data, "=");
        strcat(form_data, json_value_get_string(json_object_get_value_at(query_params, i)));
    } 
    if (strncmp(json_object_get_string(response_obj, "method"), "GET", 3) == 0) {
        compute_get_request(message, 
                            json_object_get_string(response_obj, "url"), 
                            form_data, 
                            DEFAULT_IP,
                            JWT_token,
                            cookies);
    }
    else {
        compute_post_request(message, 
                            json_object_get_string(response_obj, "url"), 
                            DEFAULT_IP,
                            JWT_token, 
                            cookies, 
                            json_object_get_string(response_obj, "type"), 
                            form_data);
    }
    free(cookies);
    free(form_data);

    int sockfd = open_connection(DEFAULT_IP, DEFAULT_PORT);
    send_to_server(sockfd, message);
    free(message);
    receive_from_server(sockfd, response);
    close_connection(sockfd);

    memset(prev_response, 0, BUFLEN);
    strcpy(prev_response, response);
    memset(response, 0, BUFLEN);
    task_4(prev_response, response, JWT_token);
}

void task_4(char *prev_response, char *response, const char *JWT_token) {

	char *cookies = calloc(BUFLEN, sizeof(char));
	char *form_data = calloc(BUFLEN, sizeof(char));
	char *message = calloc(BUFLEN, sizeof(char));
    JSON_Object *response_obj; 

    get_cookies(cookies, prev_response);
    response_obj = json_object(json_parse_string(strstr(prev_response, "\r\n\r\n") + strlen("\r\n\r\n")));

    if (strncmp(json_object_get_string(response_obj, "method"), "GET", 3) == 0) {
        compute_get_request(message, 
                            json_object_get_string(response_obj, "url"), 
                            form_data, 
                            DEFAULT_IP,
                            JWT_token, 
                            cookies);
    }
    else {
        compute_post_request(message, 
                            json_object_get_string(response_obj, "url"), 
                            DEFAULT_IP,
                            JWT_token, 
                            cookies, 
                            json_object_get_string(response_obj, "type"), 
                            form_data);
    }
    free(cookies);

    int sockfd = open_connection(DEFAULT_IP, DEFAULT_PORT);
    send_to_server(sockfd, message);
    free(message);
    receive_from_server(sockfd, response);
    close_connection(sockfd);

    memset(prev_response, 0, BUFLEN);
    strcpy(prev_response, response);
    memset(response, 0, BUFLEN);
    task_5(prev_response, response, JWT_token);
	
}

void task_5(char *prev_response, char *response, const char *JWT_token) {

	char *cookies = calloc(BUFLEN, sizeof(char));
	char *message = calloc(BUFLEN, sizeof(char));
    char *weather_response = calloc(BUFLEN, sizeof(char));
    JSON_Object *response_obj; 

	get_cookies(cookies, prev_response);
    response_obj = json_object(json_parse_string(strstr(prev_response, "\r\n\r\n") + strlen("\r\n\r\n")));
    get_weather(json_value_get_object(json_object_get_value(response_obj, "data")), weather_response); // fac weather request-ul si retin raspunsul in response
    if (strncmp(json_object_get_string(response_obj, "method"), "GET", 3) == 0) {
        compute_get_request(message, 
                            json_object_get_string(response_obj, "url"), 
                            NULL, 
                            DEFAULT_IP,
                            JWT_token, 
                            cookies);
    }
    else {
        compute_post_request(message, 
                            json_object_get_string(response_obj, "url"), 
                            DEFAULT_IP,
                            JWT_token, 
                            cookies, 
                            json_object_get_string(response_obj, "type"), 
                            strstr(weather_response, "\r\n\r\n") + strlen("\r\n\r\n"));
    }
    free(cookies);

    int sockfd = open_connection(DEFAULT_IP, DEFAULT_PORT);
    send_to_server(sockfd, message);
    receive_from_server(sockfd, response);
    close_connection(sockfd);
	
}