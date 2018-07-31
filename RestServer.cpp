#include "RestServer.h"

RestServer::RestServer(EthernetServer& server): server_(server), routesIndex_(0), bufferIndex_(0), dataIndex_(0) {
}

void RestServer::run() {
  client_ = server_.available();
  if (client_) {
    JSON_START();
    // Check the received request and process it
    int routeIndex = check();
    bufferIndex_--;
    JSON_CLOSE();

    // Send data for the client
    send(8, 0);

    // Stop the client connection
    client_.stop();

    // Necessary resets
    reset();
  }
}

void RestServer::reset() {
  // Reset buffer
  memset(&buffer_[0], 0, sizeof(buffer_));

  // Reset buffer index
  bufferIndex_ = 0;
}

void RestServer::addRoute(char * method, char * route, void (*f)(char* query, char* body)) {
  // memcpy(routes_[routesIndex_].name, route, strlen(route)+1);
  routes_[routesIndex_].method   = method;
  routes_[routesIndex_].name     = route;
  routes_[routesIndex_].callback = f;

  routesIndex_++;
}

void RestServer::onNotFound(void (*f)(char* route)) {
  notFoundCallback = f;
}

void RestServer::addToBuffer(char * value) {
  for (int i = 0; i < strlen(value); i++){
    buffer_[bufferIndex_+i] = value[i];
  }
  bufferIndex_ = bufferIndex_ + strlen(value);
}

void RestServer::addToBufferTest(char * value, int routeIndex) {
  for (int i = 0; i < strlen(value); i++){
    payload_[routeIndex].jsonBuffer[i] = value[i];
  }
}

void RestServer::addData(char* name, char * value) {
  char bufferAux[255] = {0};
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
  //addToBufferTest(bufferAux, dataIndex_);
  dataIndex_++;
}

// Add to output buffer_
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(ESP8266) || defined(CORE_WILDFIRE)
void RestServer::addData(char* name, String& value){
  for (int i = 0; i < value.length(); i++){
    buffer_[bufferIndex_+i] = value[i];
  }
  bufferIndex_ = bufferIndex_ + value.length();
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

// Send the HTTP response for the client
void RestServer::send(uint8_t chunkSize, uint8_t delayTime) {
  // First, send the HTTP Common Header
  client_.println(HTTP_COMMON_HEADER);

  // Send all of it
  if (chunkSize == 0)
    client_.print(buffer_);

  // Send chunk by chunk #####################################

  // Max iterations
  uint8_t max = (int)(bufferIndex_/chunkSize) + 1;

  // Send data
  for (uint8_t i = 0; i < max; i++) {
    char bufferAux[chunkSize+1];
    memcpy(bufferAux, buffer_ + i*chunkSize, chunkSize);
    bufferAux[chunkSize] = '\0';

    client_.print(bufferAux);

    // Wait for client_ to get data
    delay(delayTime);
  }

  client_.stop();
}

// Extract information about the HTTP Header
int RestServer::check() {
  char route[ROUTES_LENGHT] = {0};
  bool routePrepare = false;
  bool routeCatchFinished = false;
  uint8_t r = 0;

  char query[QUERY_LENGTH] = {0};
  bool queryPrepare = false;
  bool queryCatchFinished = false;
  uint8_t q = 0;

  char body[BODY_LENGTH] = {0};
  //bool bodyPrepare = false;
  //bool bodyCatchFinished = false;
  uint8_t b = 0;

  char method[METHODS_LENGTH] = {0};
  bool methodCatchFinished = false;
  uint8_t m = 0;

  bool currentLineIsBlank = true;
  char c;
  while ( client_.connected() && client_.available() ) {
    c = client_.read();
    // DLOGChar(c);

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

    if (c == '\n')
      currentLineIsBlank = true; // you're starting a new line
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

  }

  bool routeMatch = false;
  int routeMatchIndex = -1;
  for(int i = 0; i < routesIndex_; i++) {
      // Check if the routes names matches
      if(strcmp(route, routes_[i].name) == 0) {
        // Check if the HTTP METHOD matters
        if(strcmp(routes_[i].method,"*") == 0) {
          routeMatch = true;
          routeMatchIndex = i;
          break;
          // If it matters, check if the methods matches
        } else if(strcmp(method, routes_[i].method) == 0) {
          routeMatch = true;
          routeMatchIndex = i;
          break;
        }
      }
  }

  // If route match, execute callback
  if (routeMatch) {
    routes_[routeMatchIndex].callback(query, body);
    LOG("Route callback!");
  } else {
    notFoundCallback(route);
    LOG("Not found callback!");
  }

  return routeMatchIndex;
}
