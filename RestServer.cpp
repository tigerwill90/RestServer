#include "RestServer.h"

const String HTTP_COMMON_HEADER PROGMEM = "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: POST, GET, PUT, OPTIONS\r\nContent-Type: application/json\r\nConnection: close\r\n";

RestServer::RestServer(EthernetServer& server): server_(server), routesIndex_(0), bufferIndex_(1) {
}

void RestServer::run() {
  client_ = server_.available();
  if (client_) {
    LOG("Connexion start");
    // Check the received request and process it
    check();

    // Stop the client connection
    client_.stop();
    LOG("Connexion stop !");

    // Necessary resets
    reset();
  }
}

void RestServer::reset() {
  // Reset buffer
  memset(jsonBuffer, 0, sizeof(jsonBuffer));
  // Reset buffer index
  bufferIndex_ = 1;
}

void RestServer::addRoute(const char * method, const char * route, void (*f)(const char* query, const char* body, const char* bearer)) {
  routes_[routesIndex_].method   = (char *)method;
  routes_[routesIndex_].name     = (char *)route;
  routes_[routesIndex_].callback = (void(*)(char *, char *, char*))f;

  routesIndex_++;
}

void RestServer::onNotFound(void (*f)(const char* route)) {
  notFoundCallback = f;
}

void RestServer::addToBuffer(char * value) {
  for (int i = 0; i < strlen(value); i++){
    jsonBuffer[bufferIndex_] = value[i];
    bufferIndex_++;
  }
}

void RestServer::addData(const char* name, const char * value) {
  char bufferAux[OUTPUT_BUFFER_SIZE] = {0};
  uint16_t idx = 0;

  // Format the data as:
  // "name":"value",
  bufferAux[idx++] = '"';
  for (int i = 0; i < strlen(name); i++){
    bufferAux[idx++] = name[i];
  }
  bufferAux[idx++] = '"';

  bufferAux[idx++] = ':';
  bufferAux[idx++] = '"';
  for (int i = 0; i < strlen(value); i++){
    bufferAux[idx++] = value[i];
  }
  bufferAux[idx++] = '"';
  bufferAux[idx++] = ',';

  addToBuffer(bufferAux);
}

// Add to output buffer_
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(ESP8266) || defined(CORE_WILDFIRE)
void RestServer::addData(char* name, String& value){
  for (int i = 0; i < value.length(); i++){
    jsonBuffer[bufferIndex_] = value[i];
    bufferIndex_++;
  }
}
#endif

// Add to output buffer_
void RestServer::addData(char* name, uint16_t value){
  char number[10];
  itoa(value,number,10);

  addData(name, number);
}

// Add to output buffer_
void RestServer::addData(char* name, int value){
  char number[10];
  itoa(value,number,10);
  addData(name, number);
}

// Add to output buffer_ (Mega & ESP only)
// #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(ESP8266) || defined(CORE_WILDFIRE)
void RestServer::addData(char* name, float value){
  char number[10];
  dtostrf(value, 5, 2, number);

  addData(name, number);
}
// #endif

/**
 *
 **/
void RestServer::sendResponse(const char* status, uint8_t delayTime) {

  if (strlen(jsonBuffer) == 1) {
    jsonBuffer[strlen(jsonBuffer)] = '}';
  } else {
    jsonBuffer[strlen(jsonBuffer) - 1] = '}';
  }

  client_.print(status);
  client_.println(HTTP_COMMON_HEADER);

  for (uint8_t i = 0; i < strlen(jsonBuffer); i++) {
  client_.print(jsonBuffer[i]);
    delay(delayTime);
  }

}

// Extract information about the HTTP Header
void RestServer::check() {
  char route[ROUTES_LENGHT] = {0};
  bool routePrepare = false;
  bool routeCatchFinished = false;
  uint8_t r = 0;

  char query[QUERY_LENGTH] = {0};
  bool queryPrepare = false;
  bool queryCatchFinished = false;
  uint8_t q = 0;

  char body[BODY_LENGTH] = {0};
  uint8_t b = 0;

  char method[METHODS_LENGTH] = {0};
  bool methodCatchFinished = false;
  uint8_t m = 0;

  char bearer[HEADERS_LENGTH] = {'t','o','k','e','n','='};
  bool bearerMatch = false;
  uint8_t lineCount = 0;
  uint8_t baseLength = 6;
  uint8_t h = baseLength;

  bool currentLineIsBlank = true;
  char c;
  char cLast;
  while ( client_.connected() && client_.available() ) {
    c = client_.read();

    // Start end of line process ////////////////
    // if you've gotten to the end of the line (received a newline
    // character) and the line is blank, the http request header has ended,
    // so you can send a reply or check the body of the http header
    if (c == '\n' && currentLineIsBlank) {
      // Here is where the parameters of other HTTP Methods will be.
      while(client_.available() && client_.connected()) {
          body[b++] = (client_.read());
      }

      break; //Finish catching mod
    }

    if (c == '\n') {
      currentLineIsBlank = true; // you're starting a new line
      lineCount++;
    }

    else if (c != '\r')
      currentLineIsBlank = false; // you've gotten a character on the current line
    // End end of line process //////////////////

    // Start route catch process ////////////////
    if(c == '/' && !routePrepare)
      routePrepare = true;

    if((c == ' ' || c == '?') && routePrepare)
      routeCatchFinished = true;

    if(routePrepare && !routeCatchFinished)
      route[r++] = c;
    // End route catch process

    // Start query catch process
    if(c == ' ' && queryPrepare)
      queryCatchFinished = true;

    if(queryPrepare && !queryCatchFinished)
      query[q++] = c;

    if(c == '?' && !queryPrepare)
      queryPrepare = true;
    // End query catch process /////////////////

    // Start method catch process ///////////////
    if(c == ' ' && !methodCatchFinished)
      methodCatchFinished = true;

    if(!methodCatchFinished)
      method[m++] = c;
    // End method catch process /////////////////

    // Check for specific Bearer name header and extract it
    if (lineCount > 1) {
      // header value
      if (cLast != ':' && c != ' ') {
        bearer[h++] = c;
        // bearer matching
        if (!bearerMatch && strstr(bearer, "Bearer") != NULL) {
          h = baseLength;
          memset(&bearer[h], 0, sizeof(bearer) - baseLength);
          bearerMatch = true;
        }
        // the token is entierly catched, delete \n char
        if (bearerMatch && c == '\n') {
          bearer[h-1] = 0;
          break;
        }
        // the line is checked and no bearer found
        if (!bearerMatch && c == '\n') {
          h = baseLength;
          memset(&bearer[h], 0, sizeof(bearer) - baseLength);
        }
      }
    }

    cLast = c;
  }

  bool routeMatch = false;
  uint8_t routesIndex = -1;
  for(int i = 0; i < routesIndex_; i++) {
      // Check if the routes names matches
      if(strcmp(route, routes_[i].name) == 0) {
        // Check if the HTTP METHOD matters
        if(strcmp("*",routes_[i].method) == 0) {
          routeMatch = true;
          routesIndex = i;
          break;
          // If it matters, check if the methods matches
        } else if(strcmp(method, routes_[i].method) == 0) {
          routeMatch = true;
          routesIndex = i;
          break;
        }
      }
  }

  // If route match, execute callback
  jsonBuffer[0] = '{';
  if (routeMatch) {
    //remove empty char in end
    bearer[strlen(bearer) - 1] = 0;
    routes_[routesIndex].callback(query, body, bearer);
    //LOG("Route callback !");
  } else {
    notFoundCallback(route);
    //LOG("Not found callback !");
  }
}
