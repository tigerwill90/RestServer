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
  void (*callback)(char* query, char* body, char* bearer);
};

class RestServer {
public:
  RestServer(EthernetServer& client);

  void run();

  void addRoute(const char* method, const char* route, void (*f)(const char *, const char *, const char *));
  void onNotFound(void (*f)(char *));
  void sendResponse(const char* status, const char* cententType, uint8_t delayTime);

  void addData(char* name, String& value);
  void addData(char* name, uint16_t value);
  void addData(char* name, int value);
  void addData(char* name, float value);
  void addData(char* name, char* value);

private:
  Routes routes_[ROUTES_TOTAL];
  char jsonBuffer[OUTPUT_BUFFER_SIZE];
  uint8_t routesIndex_;
  uint16_t bufferIndex_;

  EthernetServer& server_;
  EthernetClient client_;

  void check();
  void reset();
  void (*notFoundCallback)(char* route);
  void addToBuffer(char* value);
};

#endif
