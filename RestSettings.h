#ifndef restsettings_h
#define restsettings_h

#ifndef ROUTES_TOTAL
#define ROUTES_TOTAL 3
#endif

#ifndef HEADER_TOTAL
#define HEADER_TOTAL 3
#endif

#ifndef ROUTES_LENGHT
#define ROUTES_LENGHT 30
#endif

#ifndef QUERY_LENGTH
#define QUERY_LENGTH 30
#endif

#ifndef BODY_LENGTH
#define BODY_LENGTH 100
#endif

#ifndef HEADERS_LENGTH
#define HEADERS_LENGTH 250
#endif

#ifndef HTTP_COMMON_HEADER
//#define HTTP_COMMON_HEADER "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: POST, GET, PUT, OPTIONS\r\nContent-Type: text/plain\r\nConnection: close\r\n"
#define HTTP_COMMON_HEADER "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: POST, GET, PUT, OPTIONS\r\nContent-Type: application/json\r\nConnection: close\r\n"
#endif

#ifndef OUTPUT_BUFFER_SIZE
#define OUTPUT_BUFFER_SIZE 200
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
#define CREATED "HTTP/1.1 201 Created\r\n"
#define ACCEPTED "HTTP/1.1 202 Accepted\r\n"
#define NO_CONTENT "HTTP/1.1 204 No Content\r\n"
#define BAD_REQUEST "HTTP/1.1 400 Bad Request\r\n"
#define UNAUTHORIZED "HTTP/1.1 401 Unauthorized\r\n"
#define FORBIDDEN "HTTP/1.1 403 Forbidden\r\n"
#define NOT_FOUND "HTTP/1.1 404 Not Found\r\n"
#define METHOD_NOT_ALLOWED "HTTP/1.1 405 Method Not Allowed\r\n"
#define INTERNAL_SERVER_ERROR "HTTP/1.1 500 Internal Server Error\r\n"

#endif
