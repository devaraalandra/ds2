#ifndef TASK3_HPP
#define TASK3_HPP

#include <string>
using std::string;

// -------- Spectator Class --------
class Spectator {
public:
    string id, name, supportedPlayer, category, day;
    bool wantsLiveStream;
    int paymentAmount;
    bool active;
    Spectator* next;

    Spectator(string _id, string _name, bool _wantsLiveStream, string _supportedPlayer,
              string _category, string _day);

    void assignPaymentAmount();
};

// -------- SpectatorList --------
class SpectatorList {
    Spectator* head;
    int nextId;  // auto-increment ID counter
public:
    SpectatorList();
    ~SpectatorList(); // Destructor to clean up linked list

    Spectator* getHead() const;

    // Register spectator with auto-generated ID, returns generated ID
    string registerSpectator(const string& name, bool wantsLiveStream,
                             const string& supportedPlayer, const string& category, const string& day);

    // Load spectators from CSV file, sets nextId correctly
    void loadFromCSV(const char* filename);

    void displaySpectators() const;
    void displayActiveSpectators(const string& roundName) const;

    // Optional: Clear all spectators from list (useful in destructor)
    void clear();
};

// -------- Match Class --------
class Match {
public:
    int matchId;
    string team1, team2;
    string scheduledTime;
    string stage;
    int round;
    string status;
    string winnerId;
    Match* next;

    Match(int id, const string& t1, const string& t2, const string& sched, const string& stg,
          int rnd, const string& sts = "", const string& winner = "");
};

// -------- MatchList --------
class MatchList {
    Match* head;
public:
    MatchList();
    ~MatchList();
    Match* getHead() const;
    void loadFromCSV(const char* filename);
    void printGroupStageRound1Matches(const string& day) const;
};

// -------- StringSet --------
class StringSet {
    struct Node;
    Node* head;
public:
    StringSet();
    ~StringSet();
    bool contains(const string& s) const;
    void insert(const string& s);
};

// -------- PrioritySeatingQueue --------
class PrioritySeatingQueue {
    struct Node;
    Node* front;

    int getPriority(const string& category);
public:
    PrioritySeatingQueue();
    ~PrioritySeatingQueue();
    void enqueue(Spectator* sp);
    void displayQueue() const;
};

// -------- MultiLevelLiveStreamQueue --------
class MultiLevelLiveStreamQueue {
    struct Node;
    Node* influencerHead;
    Node* vipHead;
    Node* generalHead;
    int capacity;

    int countList(Node* head) const;
public:
    MultiLevelLiveStreamQueue(int cap);
    ~MultiLevelLiveStreamQueue();
    void enqueue(Spectator* sp);
    void displayQueue() const;
};

// -------- CircularStreamRotation --------
class CircularStreamRotation {
    struct Node;
    Node* tail;
    int size;
    int capacity;
public:
    CircularStreamRotation(int cap);
    ~CircularStreamRotation();
    bool isFull() const;
    bool isEmpty() const;
    void enqueue(Spectator* sp);
    Spectator* dequeue();
    void displayQueue() const;
};

// -------- WatchHistoryStack --------
class WatchHistoryStack {
    struct Node;
    Node* top;
public:
    WatchHistoryStack();
    ~WatchHistoryStack();
    void push(Spectator* sp);
    void display() const;
};

// -------- WaitingQueue --------
class WaitingQueue {
    struct Node;
    Node* front;
    Node* rear;
public:
    WaitingQueue();
    ~WaitingQueue();
    bool isEmpty() const;
    void enqueue(Spectator* sp);
    Spectator* dequeue();
};

// -------- Utility functions --------
void updateSpectatorsByMatchResults(SpectatorList& spectators, MatchList& matches,
                                    const string& stage, int round);

void simulateQueueManagement(SpectatorList& regList);

#endif // TASK3_HPP
