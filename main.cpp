#include "mbed.h"
#include "VarRecord.h"
#include "HttpControl.h"
#include "EthernetInterface.h"

Serial pc(USBTX, USBRX);
DigitalOut led1(LED1);
DigitalOut led2(LED2);

void accessLED1(const char* inStr, size_t inLen, const http_method & method, char* outStr, size_t& outLen, int& http_return_code)
{
  debug("accedLED1: Started");
  if (method == HTTP_POST)
  {
    outStr = NULL;
    outLen = 0;
    if(inLen == 1)
    {
      if (inStr[0] == '0') led1 = 0;
      else led1 = 1;
      http_return_code = 200;
      debug("accedLED1: returning post");
      return;
    }
    outLen = 0;
    http_return_code = 400;
    debug("accedLED1: returning post error");
    return;
  }
  // else GET
  outStr = new char[1];
  outLen = 1;
  if(led1.read() == 1) outStr[0] = 31;
  else outStr[0] = 30;
  http_return_code = 200;
  debug("accedLED1: returning get");
};

int main() {
    //pc.baud(115200);
    pc.printf("Embedd Web Server Starting Up!\n");

    // Connect to the network (see mbed_app.json for the connectivity method used)
    //NetworkInterface* network = NetworkInterface::get_default_instance();
    EthernetInterface* network = new EthernetInterface();
    network->set_dhcp(false);
    network->set_network("10.123.123.5","255.255.255.0","10.123.123.1");
    debug("About to connect to network\n");
    network->connect();
    const nsapi_connection_status_t & connection_status = network->get_connection_status();
    switch (connection_status)
    {
        case NSAPI_STATUS_LOCAL_UP:
          debug("Connection is \"local up\"\n");
          break;
        case NSAPI_STATUS_GLOBAL_UP:
          debug("Connection is \"global up\"\n");
          break;
        case NSAPI_STATUS_DISCONNECTED:
          debug("Connection is \"disconnected\"\n");
          return 1;
          break;
        case NSAPI_STATUS_CONNECTING:
          debug("Connection is \"connecting\"\n");
          return 1;
          break;
        case NSAPI_STATUS_ERROR_UNSUPPORTED:
          debug("Connection is \"Error: Networking Unsupported\"\n");
          return 1;
          break;
        default:
          debug("Connection is in Unknown Error state\n");
          return 1;
          break;
    } //switch

    ewc::HttpControl server(network);
    //server.addVar("LED1",&led1,ewc::DecInteger32,"Output Pin LED1",true);
    server.addVar("LED1",NULL,ewc::Boolean,"Output Pin LED1",true,accessLED1);
    server.addVar("LED2",&led2,ewc::DecInteger32,"Output Pin LED2",true);
    nsapi_error_t res = server.start(8080);

    if (res == NSAPI_ERROR_OK) {
        printf("Server is listening at http://%s:8080\n", network->get_ip_address());
    }
    else {
        printf("Server could not be started... %d\n", res);
    }

    wait(osWaitForever);
}
