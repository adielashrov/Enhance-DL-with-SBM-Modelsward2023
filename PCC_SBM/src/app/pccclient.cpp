#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <iostream>
#include "../core/udt.h"
#include "../core/options.h"
#include <signal.h>
#include "BProgram.h"
#include "../pcc/rate_control/SensorBThread.h"
// #define DATA_BATCH_SIZE 1000000
#define DATA_BATCH_SIZE 100

using namespace std;

void* monitor(void*);

UDTSOCKET client;
bool stop = false;

void intHandler(int dummy) {
    stop = true;
}

void prusage() {
    cout << "usage: .../pccclient <send|recv> server_ip server_port [OPTIONS]" << endl;
    exit(-1);
}

int main(int argc, char* argv[]) {
    
    cout << "Enter pccclient main method..." << endl;

    if ((argc < 4) || (0 == atoi(argv[3]))) {
        prusage();
    }
    const char* send_str = argv[1];
    const char* ip_str   = argv[2];
    const char* port_str = argv[3];
    Options::Parse(argc, argv);

    bool should_send = !strcmp(send_str, "send");
    
	signal(SIGINT, intHandler);
   
    // use this function to initialize the UDT library
    UDT::startup();

    struct addrinfo hints, *local, *peer;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (0 != getaddrinfo(NULL, "9000", &hints, &local)) {
        cout << "invalid network address.\n" << endl;
        return 0;
    }

    client = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);

    freeaddrinfo(local);

    if (0 != getaddrinfo(ip_str, port_str, &hints, &peer)) {
        cout << "incorrect server/peer address. " << ip_str << ":" << port_str << endl;
        return 0;
    }

    // connect to the server, implict bind
    if (UDT::ERROR == UDT::connect(client, peer->ai_addr, peer->ai_addrlen)) {
        cout << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
        return 0;
    }
    freeaddrinfo(peer);

    pthread_create(new pthread_t, NULL, monitor, &client);


    int batch_size = DATA_BATCH_SIZE;
    char* data = new char[batch_size];
    bzero(data, batch_size);

    try {

        /*
        cout << "Going to create a bProgram..." << endl;

        // start the BPProgram in the pccclient
        BProgram bProgram;
        
        // cout << "Before create sensorBThread..." << endl; 
        SensorBThread sensorBThread;
        bProgram.addThread(sensorBThread);

        cout << "Before run engine..." << endl;
        bProgram.runEngine();
        cout << "After run engine..." << endl;
        
        bProgram.runProgram();
        cout << "After run engine..." << endl;        
        */

        while (!stop) {
            int cur_size = 0;
            int this_call_size = 0;
            while (!stop && cur_size < batch_size) {
                
                if (should_send) {
                    this_call_size = UDT::send(client, data + cur_size, batch_size - cur_size, 0);
                } else {
                    this_call_size = UDT::recv(client, data + cur_size, batch_size - cur_size, 0);
                }
                
                if (this_call_size == UDT::ERROR) {
                    cout << "send/recv: " << UDT::getlasterror().getErrorMessage() << std::endl;
                    break;
                }
                
                cur_size += this_call_size;
            }
        }
    }
    catch (const std::exception& ex) {
        std::cout << ex.what() << std::endl;
        fflush( 0 );
    }
    catch(...) {
        printf( "Caught General exception - use current_exception" );
        fflush( 0 );   
    }

    UDT::close(client);

    delete [] data;

    // use this function to release the UDT library
    UDT::cleanup();

    return 0;
}

void* monitor(void* s) {

    UDTSOCKET u = *(UDTSOCKET*)s;
    UDT::TRACEINFO perf;

    cout << "\tRate (Mbps)\tRTT (ms)\tSent\tLost" << endl;
    int i = 0;
    while (true) {
        sleep(1);
        i++;
        if (UDT::ERROR == UDT::perfmon(u, &perf)) {
            cout << "perfmon: " << UDT::getlasterror().getErrorMessage() << endl;
            break;
        }
        cout   << i <<"\t"
               << perf.mbpsSendRate    << "\t"
               << perf.msRTT           << "\t"
               << perf.pktSentTotal    << "\t"
               << perf.pktSndLossTotal << endl;
    }
    return NULL;
}
