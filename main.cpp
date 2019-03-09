#include "mbed.h"
#include "HttpControl.h"

Serial pc(USBTX, USBRX);
DigitalOut led(LED1);

int main() {
    pc.baud(115200);

    // Connect to the network (see mbed_app.json for the connectivity method used)
    NetworkInterface* network = NetworkInterface::get_default_instance();
    if (!network) {
        printf("Cannot connect to the network, see serial output\n");
        return 1;
    }

    ewc::HttpControl server(network);
    nsapi_error_t res = server.start(8080);

    if (res == NSAPI_ERROR_OK) {
        printf("Server is listening at http://%s:8080\n", network->get_ip_address());
    }
    else {
        printf("Server could not be started... %d\n", res);
    }

    wait(osWaitForever);
}
