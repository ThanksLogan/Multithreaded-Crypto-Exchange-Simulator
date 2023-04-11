/*
 * created on 4/10/2023
 * AUTHORS:
 * Logan Foreman REDID: 825056655
 * Shane Wechsler REDID:
 */
#ifndef A4_1_BROKER_H
#define A4_1_BROKER_H

#include <string>
#include <queue>
#include <pthread.h>
#include <semaphore.h>

#include "producer.h"
#include "consumer.h"
#include "log.h"
#include "cryptoexchange.h"

#define BUFFER_SIZE 16
#define MAX_BTC_REQUESTS 5

typedef struct BROKER{
    RequestType requestType;
    ConsumerType consumerType;
    std::queue<RequestType> boundedBuffer; // Bounded buffer queue
    pthread_mutex_t bufferMutex; // Bounded buffer mutex

    sem_t full; // waits for buffer to be non-full, counts number of full slots

    sem_t empty; // waits for buffer to be non-empty, counts number of empty slots

    int count; // # of items in the buffer

    unsigned int numRequests;
    unsigned int maxRequests;
    unsigned int BTC_reqTime;
    unsigned int ETH_reqTime;
    unsigned int X_ProcessingTime;
    unsigned int Y_ProcessingTime;

    unsigned int produced[RequestTypeN] = {};
    unsigned int consumed[ConsumerTypeN] = {};

    unsigned int inRequestQueue[RequestTypeN] = {};


    int numBitcoinRequestsInQueue;
    int numEthereumRequestsInQueue;
} BROKER;

#endif //A4_1_BROKER_H
