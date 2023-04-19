//
// Created by forem on 4/10/2023.
//


#ifndef A4_1_PRODUCER_H
#define A4_1_PRODUCER_H

#include <iostream>
#include <string.h>
#include <cstdio>
#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>
#include "broker.h"

class PRODUCER{
public:
    BROKER* broker;
    RequestType requestType;
    unsigned int requestTime;
};

void* producer(void* ptr);
#endif
//A4_1_PRODUCER_H
