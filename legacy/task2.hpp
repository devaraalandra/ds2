#ifndef REGISTRATION_H
#define REGISTRATION_H

// Player structure: Holds registration details for each player
struct Player {
    char playerID[10];          // Unique ID for the player
    char playerName[50];        // Player's name
    int ranking;                // Player's ranking (not used)
    char registrationType[20];  // Type: Early-Bird, Standard, Wildcard, Last-Minute
    char email[100];            // Player's email address
    int teamID;                 // Team ID (not used)
    int checkInStatus;          // Check-in status (1 = checked in, 0 = not checked in)
    int group;                  // Group assignment (not used)
    char rank[2];               // Rank: A, B, C, or D
    char checkInTime[50];       // Time of check-in
    char status[10];            // Status: MAIN or WAITLIST
    struct Player* next;        // Pointer to next player in queue
};

// Queue class: Manages a basic queue for player registrations
struct Queue {
    struct Player* front;       // Front of the queue
    struct Player* rear;        // Rear of the queue
    int size;                   // Current number of players in queue

    void init();                // Initialize empty queue
    int isEmpty();              // Check if queue is empty
    void enqueue(struct Player* player); // Add player to rear
    struct Player* dequeue();   // Remove and return player from front
    struct Player* peek();      // Return front player without removing
    int getSize();              // Return current size of queue
    void destroy();             // Clean up all players
};

// PriorityQueue class: Manages prioritized registration with multiple queues
struct PriorityQueue {
    struct Queue earlyBirdQueue;   // Queue for Early-Bird (Rank A)
    struct Queue wildcardQueue;    // Queue for Wildcard (Rank B)
    struct Queue standardQueue;    // Queue for Standard (Rank C)
    struct Queue lastMinuteQueue;  // Queue for Last-Minute (Rank D)
    struct Queue waitlistQueue;    // Queue for waitlisted players
    int size;                      // Total number of registered players

    void init();                   // Initialize empty queues
    void enqueue(struct Player* player); // Add player to appropriate queue
    struct Player* dequeue();      // Remove player based on priority
    int isEmpty();                 // Check if all queues are empty
    int getSize();                 // Return total number of registered players
    struct Queue* getWaitlistQueue(); // Access waitlist queue
    void setWaitlistQueue(struct Queue* q); // Set waitlist queue
};

// Function prototypes
void registerPlayer(struct PriorityQueue* pq, const char* filename);
void checkInPlayer(struct PriorityQueue* pq, const char* filename);
void withdrawPlayer(struct PriorityQueue* pq, const char* playerID, const char* filename);
void readPlayersFromCSV(struct PriorityQueue* pq, const char* filename);
void writePlayersToCSV(struct PriorityQueue* pq, const char* filename);
void displayWaitlist(struct PriorityQueue* pq);

#endif