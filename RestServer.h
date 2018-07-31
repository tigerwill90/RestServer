#ifndef restserver_h
#define restserver_h

// Include Arduino header
#include "Arduino.h"
#include "Log.h"
#include <Ethernet.h>
#include "RestSettings.h"

struct Routes {
  char* method;
  char* name;
  void (*callback)(char* query, char* body);
};

struct Payload {
  int routeIndex;
  char jsonBuffer[50];
};

class RestServer {
public:
  RestServer(EthernetServer& client);

  void run();

  void addRoute(char* method, char* route, void (*f)(char *, char *));
  void onNotFound(void (*f)(char *));

  void addData(char* name, String& value);
  void addData(char* name, uint16_t value);
  void addData(char* name, int value);
  void addData(char* name, float value);
  void addData(char* name, char* value);

private:
  Routes routes_[ROUTES_TOTAL];
  Payload payload_[ROUTES_TOTAL];
  uint8_t routesIndex_;
  uint8_t dataIndex_;
  char buffer_[OUTPUT_BUFFER_SIZE];
  uint16_t bufferIndex_;

  EthernetServer& server_;
  EthernetClient client_;

  int check();
  void reset();
  void (*notFoundCallback)(char* route);
  void addToBuffer(char* value);
  void addToBufferTest(char* value, int routeIndex);
  void send(uint8_t chunkSize, uint8_t delayTime);
};

#endif