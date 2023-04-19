//
// Created by shane on 4/14/2023.
//

#ifndef A4_CONSUMER_H
#define A4_CONSUMER_H

#include <iostream>
#include <string.h>
#include <cstdio>
#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>
#include "producer.h"
#include "broker.h"

class BROKER;
class CONSUMER{
public:
    BROKER* broker;
    ConsumerType consumerType;
    unsigned int consumerTime;
};

void* consumer(void* ptr);
#endif //A4_CONSUMER_H
