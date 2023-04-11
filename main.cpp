/*
 * created on 4/10/2023
 * AUTHORS:
 * Logan Foreman REDID: 825056655
 * Shane Wechsler REDID:
 */
#include <iostream>
#include <string.h>
#include <cstdio>
#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>

#include "log.h"
#include "producer.h"
#include "consumer.h"
#include "broker.h"


#define PRODUCTION_LIMIT 100
#define DEFAULT_DELAY 0
#define BADFLAG 2


int main(int argc, char **argv) {

    int count = 0; /* TODO: find out where this is gonna be used (if at all...) */
    unsigned int numRequests = 0; // Counter for current number of trade requests
    unsigned int maxRequests = PRODUCTION_LIMIT; // Max number of requests given by command line argument
    unsigned int X_ProcessingTime = DEFAULT_DELAY; // MS of time for blockchain X to process a trade request
    unsigned int Y_ProcessingTime = DEFAULT_DELAY; // MS of time for blockchain Y to process a trade request
    unsigned int BTC_reqTime = DEFAULT_DELAY; // MS of timme required to produce and publish a BTC request
    unsigned int ETH_reqTime = DEFAULT_DELAY; // MS of timme required to produce and publish a ETH request
    /*
     * Optional arguments:
    -r N Total number of trade requests (production limit). Default is 100 if
    not specified.
    -x N Specifies the number of milliseconds N that the consumer using
    Blockchain X requires for processing a trade request and
    completing its transaction. You would simulate this time to consume
    a request by putting the consumer thread to sleep for N
    milliseconds. Other consumer and producer threads (consuming
    over Blockchain Y, producing Bitcoin request, and producing
    Ethereum request) are handled similarly.
    -y N Similar argument for consuming over Blockchain Y.
    -b N Specifies the number of milliseconds required to produce and
    publish a Bitcoin request.
    -e N Specifies the number of milliseconds required to produce and
    publish an Ethereum request.
    Important: If an argument is not given for any one of the threads, that thread
    should incur no delay, i.e., the defaults for -x, -y, -b, -e above should be 0.
     */
    int option;
    while ( (option = getopt(argc, argv, ":r:x:y:b:e:")) != -1) {

        switch (option) {
            case 'r': {
                maxRequests = atoi(optarg);
                if(maxRequests < 0 || maxRequests > 1000){ // TODO: find out what max number of requests can be.
                    std::cout << "Number of trade requests must be a number within [0,1000]" << std::endl;
                    exit(BADFLAG);
                }
                break; }
            case 'x': {
                X_ProcessingTime = atoi(optarg);
                if(X_ProcessingTime < 0){
                    std::cout << "time for blockchain X must be greater than 0" << std::endl;
                    exit(BADFLAG);
                }
                break; }
            case 'y': {
                Y_ProcessingTime = atoi(optarg);
                if(Y_ProcessingTime < 0){
                    std::cout << "time for blockchain Y must be greater than 0" << std::endl;
                    exit(BADFLAG);
                }
                break; }
            case 'b': {
                BTC_reqTime = atoi(optarg);
                if(BTC_reqTime < 0){
                    std::cout << "time for Bitcoin must be greater than 0" << std::endl;
                    exit(BADFLAG);
                }
                break; }
            case 'e': {
                ETH_reqTime = atoi(optarg);
                if(ETH_reqTime < 0){
                    std::cout << "time for Ethereum must be greater than 0" << std::endl;
                    exit(BADFLAG);
                }
                break; }
            default:
                std::cout << "flag unrecognized, exiting..." << std::endl;
                exit(BADFLAG);
        }
    }


    /*__________INITIALIZE SHARED DATA_______________________*/
    BROKER broker;

    broker.boundedBuffer = std::queue<RequestType>();

    pthread_mutex_init(&broker.bufferMutex, NULL);

    broker.count = count;
    broker.produced[RequestTypeN] = {};
    broker.consumed[ConsumerTypeN] = {};
    broker.inRequestQueue[RequestTypeN] = {};
    broker.numBitcoinRequestsInQueue = 0;
    broker.numEthereumRequestsInQueue = 0;


    broker.maxRequests = maxRequests;
    broker.numRequests = numRequests;
    broker.BTC_reqTime = BTC_reqTime;
    broker.ETH_reqTime = ETH_reqTime;
    broker.X_ProcessingTime = X_ProcessingTime;
    broker.Y_ProcessingTime = Y_ProcessingTime;

    /* -- TESTING DIFFERENT REQUEST AMOUNTS & TIMES -- */
        broker.maxRequests = 100;
        broker.BTC_reqTime = 120;
        broker.ETH_reqTime = 1;
        broker.X_ProcessingTime = 5;
        broker.Y_ProcessingTime = 1;
    /* ----------------------------------------------- */

    sem_init(&(broker.empty), 0, BUFFER_SIZE);
    sem_init(&(broker.full),0,0);
    /*----------------------------------------------------------*/


    /*-------------- Create 2 Producer and 2 Consumer Threads -----------------------*/
    pthread_t btcProducerThread;
    pthread_t ethProducerThread;
    pthread_t xConsumerThread;
    pthread_t yConsumerThread;
    /* BTC producer */
    if (pthread_create(&btcProducerThread, NULL, &producer, &broker)) {
        std::cerr << "Error: failed to create thread for producer (btc)" << std::endl;
        exit(-1);
    }
    /* ETH producer */
    if (pthread_create(&ethProducerThread, NULL, &producer, &broker)) {
        std::cerr << "Error: failed to create thread for producer (eth)" << std::endl;
        exit(-1);
    }
    /* Blockchain X producer*/
    if (pthread_create(&xConsumerThread, NULL, &consumer, &broker)) {
        std::cerr << "Error: failed to create thread for consumer (X)" << std::endl;
        exit(-1);
    }
    /* Blockchain Y producer */
    if (pthread_create(&yConsumerThread, NULL, &consumer, &broker)) {
        std::cerr << "Error: failed to create thread for consumer (Y)" << std::endl;
        exit(-1);
    }

    /*------------------------------------------------------------------------------------------*/


    /* TODO: see if we need these
    // destroy semaphores
    sem_destroy(&(shared_data->wait));
    sem_destroy(&(shared_data->full));
     */


    /* OLD STRATEGIES FOR THREAD CALLING WITH WORKAROUND 'PARAMETERS':
    RequestType requestType;
    ConsumerType consumerType;
    struct ThreadArgs {
        void* arg1;
        void* arg2;
    };

     * //requestType = Bitcoin;
    //ThreadArgs btcArgs = { (void*) &broker, (void*) &requestType };
    //broker.requestType = Bitcoin;
    //broker.requestType = Ethereum;
    //ThreadArgs ethArgs = { (void*) &broker, (void*) &requestType };
     *
     *
     * //broker.consumerType = BlockchainX;
    //ThreadArgs xArgs = { (void*) &broker, (void*) &consumerType };
     * //broker.consumerType = BlockchainY;
    //ThreadArgs yArgs = { (void*) &broker, (void*) &consumerType };
     */
    while(true){
        std::cout << "";
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

