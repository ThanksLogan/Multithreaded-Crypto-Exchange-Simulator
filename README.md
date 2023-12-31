# Crypto Exchange Simulation

## Overview
This C++ project simulates a crypto exchange using multithreading, semaphores, and mutexes. It includes producers for Bitcoin and Ethereum trade requests, and consumers using different blockchains (X and Y) to process these requests.

## Getting Started

### Prerequisites
- C++ compiler (e.g., g++)
- pthread library for threading
- sem library for semaphores
- Standard C++ libraries

### Installation and Compilation
1. Clone or download the project repository.
2. Navigate to the project directory.
3. Use the provided Makefile to compile the project:
```
make
```

This will create the executable cryptoexc.

## Running the Program
Execute the program with the following syntax:

```
./cryptoexc [options]
```
Options include:

+ -r N Total number of trade requests.

+ -x N Processing time in milliseconds for Blockchain X.

+ -y N Processing time in milliseconds for Blockchain Y.

+ -b N Time in milliseconds to produce a Bitcoin request.

+ -e N Time in milliseconds to produce an Ethereum request.

## Code Structure

### Key Components
- **PRODUCER**: Manages the production of cryptocurrency trade requests.
- **CONSUMER**: Handles the consumption and processing of requests using blockchains.
- **BROKER**: Acts as a mediator that maintains a queue of requests.
- **LOG**: Provides logging functionalities to track request handling.

### Source Files and Their Roles
- `producer.cpp` & `producer.h`: Implements the PRODUCER class for generating requests.
- `consumer.cpp` & `consumer.h`: Defines the CONSUMER class for processing requests.
- `broker.cpp` & `broker.h`: Contains the BROKER class to manage the request queue.
- `log.cpp` & `log.h`: Provides functions for logging the simulation activities.
- `main.cpp`: The main driver program.

### Threading and Synchronization
- The simulation uses `pthread` for threading.
- Mutual exclusion is handled using `pthread_mutex_t`.
- Synchronization between producer and consumer threads is achieved using semaphores (`sem_t`).

### Makefile Targets
- `$(PROGRAM)`: Compiles the main program.
- `clean`: Cleans up compiled objects and executable.

### Testing
- The program can be tested by specifying different command-line options to simulate various scenarios.
- Logs provide insights into the behavior of producers and consumers in the simulation.

