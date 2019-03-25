#include "mbed.h"
#include "HttpControl.h"
#include "EthernetInterface.h"
#include "FuncRecord.h"

Serial pc(USBTX, USBRX);
DigitalOut led1(LED1);
DigitalOut led2(LED2);

class FuncToggleDigitalOut: ewc::FuncRecord
{
  private:
    DigitalOut _d;
  public:
    FuncToggleDigitalOut(char* nm, char* desc, bool atsl, PinName pin): 
        ewc::FuncRecord(nm,desc,atsl), _d(pin)
    {
      _d.write(0);
    }; // constructor
    void operator()(ParsedHttpRequest* request, TCPSocket* socket)
    {
      debug("FuncToggleDigitalOut::operator() called\n");
      debug("DigitalOut past value: %d\n",_d.read());
      int val = _d.read();
      if(val == 0) val = 1;
      else val = 0;
      _d.write(val);
      debug("DigitalOut future value: %d\n",_d.read());
      HttpResponseBuilder builder(200); // not found
      builder.send(socket, NULL, 0);
    };
}; // FuncToggleDigitalOut

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
    server.addVar("LED1",&led1,ewc::DecInteger32,"Output Pin LED1",true);
    server.addVar("LED2",&led2,ewc::DecInteger32,"Output Pin LED2",true);
    server.addFunc(new ewc::FuncRecord("dummy","dummy",true));
    server.addFunc((ewc::FuncRecord*) new FuncToggleDigitalOut("led1","Output Pin LED1",true,LED1));
    server.addFunc((ewc::FuncRecord*) new FuncToggleDigitalOut("led2","Output Pin LED2",true,LED2));
    server.addFunc((ewc::FuncRecord*) new FuncToggleDigitalOut("led3","Output Pin LED3",true,LED3));
    nsapi_error_t res = server.start(8080);

    if (res == NSAPI_ERROR_OK) {
        printf("Server is listening at http://%s:8080\n", network->get_ip_address());
    }
    else {
        printf("Server could not be started... %d\n", res);
    }

    wait(osWaitForever);
}
