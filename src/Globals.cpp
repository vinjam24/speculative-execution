#include "Globals.h"
#include <map>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <iostream>


Speculator* speculator = nullptr;

// Function to create or get shared memory segment
int createSharedMemory() {
    int shmid = shmget(IPC_PRIVATE, sizeof(Speculator), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

// Function to initialize the global shared object
void initializeGlobalSharedObject() {
    // Create or get shared memory segment
    int shmid = createSharedMemory();

    // Attach to the shared memory segment
    speculator = (Speculator*)shmat(shmid, NULL, 0);
    if (speculator == (Speculator*)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
}

void cleanupSharedMemory() {
    // Detach from the shared memory segment
    shmdt(speculator);

    // Remove the shared memory segment (optional)
    // shmctl(shmid, IPC_RMID, NULL);
}

std::map<int, char*> cache; // Declare the global variable
