#ifndef restsettings_h
#define restsettings_h

#define ROUTES_TOTAL 1

#ifndef ROUTES_LENGHT
#define ROUTES_LENGHT 11
#endif

#ifndef QUERY_LENGTH
#define QUERY_LENGTH 1
#endif

#ifndef BODY_LENGTH
#define BODY_LENGTH 1
#endif

#ifndef HEADERS_LENGTH
#define HEADERS_LENGTH 180
#endif

#ifndef OUTPUT_BUFFER_SIZE
#define OUTPUT_BUFFER_SIZE 40
#endif

#define METHODS_LENGTH 7
#define ANY "*"
#define GET "GET"
#define PUT "PUT"
#define HEAD "HEAD"
#define POST "POST"
#define TRACE "TRACE"
#define DELETE "DELETE"
#define OPTIONS "OPTIONS"

#define OK "HTTP/1.1 200 OK\r\n"
//#define ACCEPTED "HTTP/1.1 202 Accepted\r\n"
//#define NO_CONTENT "HTTP/1.1 204 No Content\r\n"
#define UNAUTHORIZED "HTTP/1.1 401 Unauthorized\r\n"
#define NOT_FOUND "HTTP/1.1 404 Not Found\r\n"
#define SERVER_ERROR "HTTP/1.1 500 Internal Server Error\r\n"

#endif
