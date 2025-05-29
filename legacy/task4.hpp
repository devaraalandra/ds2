#ifndef GAMERESULTMANAGER_HPP
#define GAMERESULTMANAGER_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

// Structure to represent a match result
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
    MatchResult() : match_id(0), group_id(0), round(0), player1_id(0), 
                   player2_id(0), winner_id(0) {}
    
    // Parameterized constructor
    MatchResult(int mid, const std::string& st, int gid, int r, int p1, int p2,
                const std::string& sched_time, const std::string& stat, int winner, const std::string& sc)
        : match_id(mid), stage(st), group_id(gid), round(r), player1_id(p1), 
          player2_id(p2), scheduled_time(sched_time), status(stat), winner_id(winner), score(sc) {}
};

// Structure to represent player statistics
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
    PlayerStats() : player_id(0), wins(0), losses(0), total_matches(0), avg_score(0.0) {}
    
    // Parameterized constructor
    PlayerStats(int pid, const std::string& n, const std::string& r, const std::string& cont, 
                const std::string& reg_time)
        : player_id(pid), name(n), rank(r), contact(cont), registration_time(reg_time),
          wins(0), losses(0), total_matches(0), avg_score(0.0) {}
};

// Custom Stack implementation for storing recent match results
// Stack is chosen because we need quick access to the most recent matches (LIFO - Last In, First Out)
// This allows us to efficiently display the latest 5 matches without traversing the entire history
template<typename T>
class ResultStack {
private:
    static const int MAX_SIZE = 100;  // Maximum capacity for the stack
    T data[MAX_SIZE];                 // Array to store stack elements
    int top_index;                    // Index of the top element
    
public:
    // Constructor initializes empty stack
    ResultStack() : top_index(-1) {}
    
    // Check if stack is empty
    bool isEmpty() const {
        return top_index == -1;
    }
    
    // Check if stack is full
    bool isFull() const {
        return top_index == MAX_SIZE - 1;
    }
    
    // Push element onto stack (add to top)
    bool push(const T& item) {
        if (isFull()) {
            return false;  // Stack overflow
        }
        data[++top_index] = item;
        return true;
    }
    
    // Pop element from stack (remove from top)
    bool pop(T& item) {
        if (isEmpty()) {
            return false;  // Stack underflow
        }
        item = data[top_index--];
        return true;
    }
    
    // Peek at top element without removing it
    bool peek(T& item) const {
        if (isEmpty()) {
            return false;
        }
        item = data[top_index];
        return true;
    }
    
    // Get current size of stack
    int size() const {
        return top_index + 1;
    }
    
    // Get element at specific index from top
    bool getFromTop(int index, T& item) const {
        if (index < 0 || index > top_index) {
            return false;
        }
        item = data[top_index - index];
        return true;
    }
};

// Custom Queue implementation for maintaining chronological match history
// Queue is chosen because we need to maintain matches in chronological order (FIFO - First In, First Out)
// This preserves the timeline of matches for historical analysis and tournament progression tracking
template<typename T>
class HistoryQueue {
private:
    static const int MAX_SIZE = 1000;  // Maximum capacity for the queue
    T data[MAX_SIZE];                  // Array to store queue elements
    int front_index;                   // Index of the front element
    int rear_index;                    // Index of the rear element
    int count;                         // Number of elements in queue
    
public:
    // Constructor initializes empty queue
    HistoryQueue() : front_index(0), rear_index(-1), count(0) {}
    
    // Check if queue is empty
    bool isEmpty() const {
        return count == 0;
    }
    
    // Check if queue is full
    bool isFull() const {
        return count == MAX_SIZE;
    }
    
    // Enqueue element (add to rear)
    bool enqueue(const T& item) {
        if (isFull()) {
            return false;  // Queue overflow
        }
        rear_index = (rear_index + 1) % MAX_SIZE;  // Circular array implementation
        data[rear_index] = item;
        count++;
        return true;
    }
    
    // Dequeue element (remove from front)
    bool dequeue(T& item) {
        if (isEmpty()) {
            return false;  // Queue underflow
        }
        item = data[front_index];
        front_index = (front_index + 1) % MAX_SIZE;  // Circular array implementation
        count--;
        return true;
    }
    
    // Peek at front element without removing it
    bool front(T& item) const {
        if (isEmpty()) {
            return false;
        }
        item = data[front_index];
        return true;
    }
    
    // Get current size of queue
    int size() const {
        return count;
    }
    
    // Get element at specific index (0 = front)
    bool getAt(int index, T& item) const {
        if (index < 0 || index >= count) {
            return false;
        }
        int actual_index = (front_index + index) % MAX_SIZE;
        item = data[actual_index];
        return true;
    }
};

// Main class for managing game results and player performance
class GameResultManager {
private:
    ResultStack<MatchResult> recent_matches;    // Stack for quick access to recent matches
    HistoryQueue<MatchResult> match_history;    // Queue for chronological match history
    PlayerStats* player_stats;                  // Array-based structure for player statistics
    int max_players;                           // Maximum number of players
    int current_player_count;                  // Current number of players loaded
    
    // Helper function to split CSV line into tokens
    void splitCSVLine(const std::string& line, std::string tokens[], int max_tokens);
    
    // Helper function to find player by ID
    int findPlayerIndex(int player_id);
    
    // Helper function to parse score and extract numeric value
    double parseScore(const std::string& score_str);
    
    // Helper function to update player statistics based on match result
    void updatePlayerStats(const MatchResult& match);
    
public:
    // Constructor
    GameResultManager(int max_player_capacity = 500);
    
    // Destructor
    ~GameResultManager();
    
    // Load player data from players.csv
    bool loadPlayerData(const std::string& filename);
    
    // Load existing matches from matches.csv
    bool loadMatchHistory(const std::string& filename);
    
    // Log a new match result and append to matches.csv
    bool logMatchResult(const MatchResult& match, const std::string& filename);
    
    // Display the most recent N matches (default 5)
    void displayRecentMatches(int count = 5);
    
    // Display player performance statistics
    void displayPlayerStats(int player_id);
    
    // Display all player statistics
    void displayAllPlayerStats();
    
    // Query matches by player ID
    void queryMatchesByPlayer(int player_id);
    
    // Query matches by stage (using optional brackets.csv for filtering)
    void queryMatchesByStage(const std::string& stage);
    
    // Get total number of matches in history
    int getTotalMatches() const;
    
    // Get total number of players
    int getTotalPlayers() const;
    
    // Create sample CSV files for demonstration
    void createSampleFiles();
    
    // Function to demonstrate logging a new match
    void demonstrateMatchLogging();
    
    // Function to display menu options
    void displayMenu();
    
    // Main program execution
    void runProgram();
};

#endif // GAMERESULTMANAGER_HPP