#ifndef ESPORTSCHAMPIONSHIP_HPP
#define ESPORTSCHAMPIONSHIP_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits> // For std::numeric_limits, often used in main input handling

// Forward declarations if any class needs to know about another before full definition
class Spectator; // Used by several Task 3 queue/stack classes

// ---- Task 3 Class Definitions ----

// -------- Spectator Class (from Task3.hpp) --------
class Spectator {
public:
    std::string id, name, supportedPlayer, category, day;
    bool wantsLiveStream;
    int paymentAmount;
    bool active; // active in tournament or left
    Spectator* next;

    Spectator(std::string _id, std::string _name, bool _wantsLiveStream, std::string _supportedPlayer,
              std::string _category, std::string _day);

    void assignPaymentAmount();
};

// -------- SpectatorList (Linked List - from Task3.hpp) --------
class SpectatorList {
    Spectator* head;
public:
    SpectatorList();
    Spectator* getHead() const;
    void registerSpectator(const std::string& id, const std::string& name, bool wantsLiveStream,
                           const std::string& supportedPlayer, const std::string& category, const std::string& day);
    void loadFromCSV(const char* filename);
    void displaySpectators() const;
    void displayActiveSpectators(const std::string& roundName) const;
    // Helper to save to CSV, can be called from main
    void saveToCSV(const char* filename) const;
};

// -------- Task3Match Class (Renamed from Match - from Task3.hpp) --------
class Task3Match {
public:
    int matchId;
    std::string team1, team2; // Player IDs
    std::string scheduledTime;
    std::string stage;
    int round;
    std::string status;
    std::string winnerId; // Player ID
    Task3Match* next;

    Task3Match(int id, const std::string& t1, const std::string& t2, const std::string& sched, const std::string& stg,
               int rnd, const std::string& sts = "", const std::string& winner = "");
};

// -------- Task3MatchList (Linked List - Renamed from MatchList - from Task3.hpp) --------
class Task3MatchList {
    Task3Match* head;
public:
    Task3MatchList();
    ~Task3MatchList();
    Task3Match* getHead() const;
    void loadFromCSV(const char* filename);
    void printGroupStageRound1Matches(const std::string& day) const; // Used during spectator registration
};

// -------- StringSet (linked list based set - from Task3.hpp) --------
class StringSet {
    struct Node {
        std::string val;
        Node* next;
        Node(const std::string& v) : val(v), next(nullptr) {}
    };
    Node* head;
public:
    StringSet();
    ~StringSet();
    bool contains(const std::string& s) const;
    void insert(const std::string& s);
};

// -------- Priority Seating Queue (linked list - from Task3.hpp) --------
class PrioritySeatingQueue {
    struct Node {
        Spectator* data;
        Node* next;
        Node(Spectator* sp) : data(sp), next(nullptr) {}
    };
    Node* front;

    int getPriority(const std::string& category); // Made private as it's an internal helper
public:
    PrioritySeatingQueue();
    // Proper destructor for linked list
    ~PrioritySeatingQueue();
    void enqueue(Spectator* sp);
    void displayQueue() const;
};

// -------- MultiLevelLiveStreamQueue (linked list - from Task3.hpp) --------
class MultiLevelLiveStreamQueue {
    struct Node {
        Spectator* data;
        Node* next;
        Node(Spectator* sp) : data(sp), next(nullptr) {}
    };
    Node* influencerHead;
    Node* vipHead;
    Node* generalHead;
    int capacity;

    int countList(Node* head) const;
    // Helper for destructor
    void clearList(Node*& head_ptr);
    // Helper for display
    int displayGroup(const std::string& label, Node* head, int count) const;
    int getTotalCount() const; // New helper to get total count

public:
    MultiLevelLiveStreamQueue(int cap);
    // Proper destructor
    ~MultiLevelLiveStreamQueue();
    void enqueue(Spectator* sp);
    void displayQueue() const;
};

// -------- CircularStreamRotation (linked list - from Task3.hpp) --------
class CircularStreamRotation {
    struct Node {
        Spectator* data;
        Node* next;
        Node(Spectator* sp) : data(sp), next(nullptr) {}
    };
    Node* tail;  // tail points to last node, tail->next is front
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

// -------- WatchHistoryStack (linked list stack - from Task3.hpp) --------
class WatchHistoryStack {
    struct Node {
        Spectator* data;
        Node* next;
        Node(Spectator* sp) : data(sp), next(nullptr) {}
    };
    Node* top;

public:
    WatchHistoryStack();
    // Proper destructor
    ~WatchHistoryStack();
    void push(Spectator* sp);
    void display() const;
};

// -------- WaitingQueue (linked list queue - from Task3.hpp) --------
class WaitingQueue {
    struct Node {
        Spectator* data;
        Node* next;
        Node(Spectator* sp) : data(sp), next(nullptr) {}
    };
    Node* front;
    Node* rear;
public:
    WaitingQueue();
    // Proper destructor
    ~WaitingQueue();
    bool isEmpty() const;
    void enqueue(Spectator* sp);
    Spectator* dequeue();
    // Added display for debugging/verification if needed, though not in original
    // void displayQueue() const; 
};


// ---- Task 4 Data Structures and Class Definitions ----

// Structure to represent a match result (from task4.hpp)
struct MatchResult {
    int match_id;
    std::string stage;
    int group_id;
    int round;
    int player1_id;
    int player2_id;
    std::string scheduled_time;
    std::string status;
    int winner_id;
    std::string score;
    
    // Default constructor
    MatchResult();
    
    // Parameterized constructor
    MatchResult(int mid, const std::string& st, int gid, int r, int p1, int p2,
                const std::string& sched_time, const std::string& stat, int winner, const std::string& sc);
};

// Structure to represent player statistics (from task4.hpp)
struct PlayerStats {
    int player_id;
    std::string name;
    std::string rank;
    std::string contact;
    std::string registration_time;
    int wins;
    int losses;
    int total_matches;
    double avg_score;
    
    // Default constructor
    PlayerStats();
    
    // Parameterized constructor
    PlayerStats(int pid, const std::string& n, const std::string& r, const std::string& cont, 
                const std::string& reg_time);
};

// Custom Stack implementation for storing recent match results (from task4.hpp)
template<typename T>
class ResultStack {
private:
    static const int MAX_SIZE = 100;
    T data[MAX_SIZE];
    int top_index;
    
public:
    ResultStack();
    bool isEmpty() const;
    bool isFull() const;
    bool push(const T& item);
    bool pop(T& item);
    bool peek(T& item) const;
    int size() const;
    bool getFromTop(int index, T& item) const; // Get element at specific index from top
};

// Custom Queue implementation for maintaining chronological match history (from task4.hpp)
template<typename T>
class HistoryQueue {
private:
    static const int MAX_SIZE = 1000;
    T data[MAX_SIZE];
    int front_index;
    int rear_index;
    int count;
    
public:
    HistoryQueue();
    bool isEmpty() const;
    bool isFull() const;
    bool enqueue(const T& item);
    bool dequeue(T& item);
    bool front(T& item) const; // Peek at front
    int size() const;
    bool getAt(int index, T& item) const; // Get element at specific index (0 = front)
};

// Main class for managing game results and player performance (from task4.hpp)
class GameResultManager {
private:
    ResultStack<MatchResult> recent_matches;
    HistoryQueue<MatchResult> match_history;
    PlayerStats* player_stats;
    int max_players;
    int current_player_count;
    
    void splitCSVLine(const std::string& line, std::string tokens[], int max_tokens);
    int findPlayerIndex(int player_id);
    double parseScore(const std::string& score_str);
    void updatePlayerStats(const MatchResult& match);
    
public:
    GameResultManager(int max_player_capacity = 500);
    ~GameResultManager();
    
    bool loadPlayerData(const std::string& filename);
    bool loadMatchHistory(const std::string& filename);
    bool logMatchResult(const MatchResult& match, const std::string& filename);
    void displayRecentMatches(int count = 5);
    void displayPlayerStats(int player_id);
    void displayAllPlayerStats();
    void queryMatchesByPlayer(int player_id);
    void queryMatchesByStage(const std::string& stage);
    int getTotalMatches() const;
    int getTotalPlayers() const;
    void createSampleFiles();
    void demonstrateMatchLogging(); // Kept for potential direct call
    // displayMenu() and runProgram() from original task4.hpp are handled by the integrated main
};


// ---- Task 3 Utility Function Prototypes (from Task3.hpp) ----
void updateSpectatorsByMatchResults(SpectatorList& spectators, Task3MatchList& matches,
                                    const std::string& stage, int round);

void simulateQueueManagement(SpectatorList& regList);


// Template Implementations for ResultStack and HistoryQueue (must be in header or included .tpp)
// Since they are used by GameResultManager which is not templated, it's fine to put implementations in .cpp if GameResultManager is the only user.
// However, typical template usage puts impl in .hpp or .tpp. The original task4.hpp had them directly in the class.
// For simplicity and adherence to original structure, keeping template implementations here.

// ---- ResultStack Implementation ----
template<typename T>
ResultStack<T>::ResultStack() : top_index(-1) {}

template<typename T>
bool ResultStack<T>::isEmpty() const {
    return top_index == -1;
}

template<typename T>
bool ResultStack<T>::isFull() const {
    return top_index == MAX_SIZE - 1;
}

template<typename T>
bool ResultStack<T>::push(const T& item) {
    if (isFull()) {
        return false; // Stack overflow
    }
    data[++top_index] = item;
    return true;
}

template<typename T>
bool ResultStack<T>::pop(T& item) {
    if (isEmpty()) {
        return false; // Stack underflow
    }
    item = data[top_index--];
    return true;
}

template<typename T>
bool ResultStack<T>::peek(T& item) const {
    if (isEmpty()) {
        return false;
    }
    item = data[top_index];
    return true;
}

template<typename T>
int ResultStack<T>::size() const {
    return top_index + 1;
}

template<typename T>
bool ResultStack<T>::getFromTop(int index, T& item) const {
    if (index < 0 || index > top_index) {
        return false;
    }
    item = data[top_index - index];
    return true;
}

// ---- HistoryQueue Implementation ----
template<typename T>
HistoryQueue<T>::HistoryQueue() : front_index(0), rear_index(-1), count(0) {}

template<typename T>
bool HistoryQueue<T>::isEmpty() const {
    return count == 0;
}

template<typename T>
bool HistoryQueue<T>::isFull() const {
    return count == MAX_SIZE;
}

template<typename T>
bool HistoryQueue<T>::enqueue(const T& item) {
    if (isFull()) {
        return false; // Queue overflow
    }
    rear_index = (rear_index + 1) % MAX_SIZE;
    data[rear_index] = item;
    count++;
    return true;
}

template<typename T>
bool HistoryQueue<T>::dequeue(T& item) {
    if (isEmpty()) {
        return false; // Queue underflow
    }
    item = data[front_index];
    front_index = (front_index + 1) % MAX_SIZE;
    count--;
    return true;
}

template<typename T>
bool HistoryQueue<T>::front(T& item) const {
    if (isEmpty()) {
        return false;
    }
    item = data[front_index];
    return true;
}

template<typename T>
int HistoryQueue<T>::size() const {
    return count;
}

template<typename T>
bool HistoryQueue<T>::getAt(int index, T& item) const {
    if (index < 0 || index >= count) {
        return false;
    }
    int actual_index = (front_index + index) % MAX_SIZE;
    item = data[actual_index];
    return true;
}


#endif // ESPORTSCHAMPIONSHIP_HPP