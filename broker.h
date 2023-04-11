//
// Created by forem on 4/10/2023.
//

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

/*
typedef struct dictentry {
    std::string word;
    long count;
    struct dictentry *next;
    void findEndingNodeOfAStr(const char *string) {}
} dictentry;
*/
//struct ThreadData;
//struct dictentry; // forward declaration
//struct Dictionary;
typedef struct {
    RequestType requestType; // the request type for the thread (Bitcoin or Ethereum)
    int requestTime; // the time interval for generating trade requests (in milliseconds)
} ProducerData;

/*
typedef struct {
};ConsumerData;*/

typedef struct BROKER{
    RequestType requestType;
    ConsumerType consumerType;
    std::queue<RequestType> boundedBuffer; // Bounded buffer queue
    pthread_mutex_t bufferMutex; // Bounded buffer mutex

    sem_t full; // waits for buffer to be non-full, counts number of full slots

    sem_t empty; // waits for buffer to be non-empty, counts number of empty slots

    int count; // # of items in the buffer

    int numRequests;
    int X_ProcessingTime;
    int Y_ProcessingTime;
    int BTC_reqTime;
    int ETH_reqTime;

    int numBitcoinRequestsInQueue;
} BROKER;

#endif //A4_1_BROKER_H
