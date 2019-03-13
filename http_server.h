/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _HTTP_SERVER_
#define _HTTP_SERVER_

#include "mbed.h"
#include "http_request_parser.h"
#include "http_response.h"
#include "http_response_builder.h"

#ifndef HTTP_SERVER_MAX_CONCURRENT
#define HTTP_SERVER_MAX_CONCURRENT      5
#endif
// not needed except by mbed-simulator
#ifndef HTTP_RECEIVE_BUFFER_SIZE
#define HTTP_RECEIVE_BUFFER_SIZE 8 * 1024
#endif


typedef HttpResponse ParsedHttpRequest;

/**
 * \brief HttpServer implements the logic for setting up an HTTP server.
 */
class HttpServer {
public:
    /**
     * HttpRequest Constructor
     *
     * @param[in] network The network interface
    */
    HttpServer(NetworkInterface* network) {
        _network = network;
    }

    ~HttpServer() {
    }

    /**
     * Start running the server (it will run on it's own thread)
     */
    nsapi_error_t start(uint16_t port, Callback<void(ParsedHttpRequest* request, TCPSocket* socket)> a_handler) {
        server = new TCPSocket();

        nsapi_error_t ret;

        ret = server->open(_network);
        if (ret != NSAPI_ERROR_OK) {
            return ret;
        }

        ret = server->bind(port);
        if (ret != NSAPI_ERROR_OK) {
            return ret;
        }

        server->listen(HTTP_SERVER_MAX_CONCURRENT); // max. concurrent connections...

        handler = a_handler;

        main();

        return NSAPI_ERROR_OK;
    }

private:

    void receive_data(TCPSocket* socket) {

        // needs to keep running until the socket gets closed
        while (1) {

            ParsedHttpRequest* response = new ParsedHttpRequest();
            HttpParser* parser = new HttpParser(response, HTTP_REQUEST);

            // Set up a receive buffer (on the heap)
            uint8_t* recv_buffer = (uint8_t*)malloc(HTTP_RECEIVE_BUFFER_SIZE);

            // TCPSocket::recv is called until we don't have any data anymore
            nsapi_size_or_error_t recv_ret;
            while ((recv_ret = socket->recv(recv_buffer, HTTP_RECEIVE_BUFFER_SIZE)) > 0) {
                // Pass the chunk into the http_parser
                size_t nparsed = parser->execute((const char*)recv_buffer, (uint32_t) recv_ret);
                if (nparsed != (uint32_t) recv_ret) {
                    printf("Parsing failed... parsed %u bytes, received %d bytes\n", nparsed, recv_ret);
                    recv_ret = -2101;
                    break;
                }

                if (response->is_message_complete()) {
                    break;
                }
            }
            // error?
            if (recv_ret <= 0) {
                if (recv_ret < 0) {
                    printf("Error reading from socket %d\n", recv_ret);
                }

                // error = recv_ret;
                delete response;
                delete parser;
                free(recv_buffer);

                // q; should we always break out of the thread or only if NO_SOCKET ?
                // should we delete socket here? the socket seems already gone...
                if (recv_ret < -3000 || recv_ret == 0) {
                    return;
                }
                else {
                    continue;
                }
            }

            // When done, call parser.finish()
            parser->finish();

            // Free the receive buffer
            free(recv_buffer);

            // Let user application handle the request, if user needs a handle to response they need to memcpy themselves
            if (recv_ret > 0) {
                handler(response, socket);
            }

            // Free the response and parser
            delete response;
            delete parser;
        }
    }

    void main() {
        while (1) {
            nsapi_error_t accept_status = 0;
            TCPSocket* clt_sock = server->accept(&accept_status);
            if (accept_status == NSAPI_ERROR_OK) {
                receive_data(clt_sock);
                delete clt_sock;
            }
            else {
                delete clt_sock;
            }
        }
    }

    TCPSocket* server;
    NetworkInterface* _network;
    Callback<void(ParsedHttpRequest* request, TCPSocket* socket)> handler;
};

#endif // _HTTP_SERVER
