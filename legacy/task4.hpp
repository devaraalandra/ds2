#ifndef TASK4_HPP
#define TASK4_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>

/**
 * Structure to store player statistics and information
 * Contains all relevant player data including performance metrics
 */
struct PlayerStats {
    int player_id;              // Unique player identifier
    std::string name;           // Player's full name
    std::string rank;           // Player's current rank/level
    std::string contact;        // Contact information (email/phone)
    std::string registration_time;  // When player registered
    int total_matches;          // Total number of matches played
    int wins;                   // Number of wins
    int losses;                 // Number of losses
    double avg_score;           // Average score across all matches
    
    // Default constructor initializes all fields with appropriate default values
    PlayerStats() : player_id(0), name(""), rank(""), contact(""), 
                   registration_time(""), total_matches(0), wins(0), 
                   losses(0), avg_score(0.0) {}
    
    // Parameterized constructor for easy initialization
    PlayerStats(int id, const std::string& n, const std::string& r, 
               const std::string& c, const std::string& reg_time)
        : player_id(id), name(n), rank(r), contact(c), 
          registration_time(reg_time), total_matches(0), wins(0), 
          losses(0), avg_score(0.0) {}
};

/**
 * Structure to store match result information
 * Contains all details about a single match
 */
struct MatchResult {
    int match_id;               // Unique match identifier
    std::string stage;          // Tournament stage (group, knockout, etc.)
    int group_id;              // Group identifier (0 if not applicable)
    int round;                 // Round number within stage
    int player1_id;            // First player's ID
    int player2_id;            // Second player's ID
    std::string scheduled_time; // Match scheduled time (YYYY-MM-DD HH:MM:SS)
    std::string status;         // Match status (completed, pending, etc.)
    int winner_id;             // Winner's player ID (0 for draw/no winner)
    std::string score;         // Match score in format "X-Y"
    
    // Default constructor
    MatchResult() : match_id(0), stage(""), group_id(0), round(0),
                   player1_id(0), player2_id(0), scheduled_time(""),
                   status(""), winner_id(0), score("") {}
    
    // Parameterized constructor for easy match creation
    MatchResult(int m_id, const std::string& st, int g_id, int rd,
               int p1_id, int p2_id, const std::string& sched_time,
               const std::string& stat, int win_id, const std::string& sc)
        : match_id(m_id), stage(st), group_id(g_id), round(rd),
          player1_id(p1_id), player2_id(p2_id), scheduled_time(sched_time),
          status(stat), winner_id(win_id), score(sc) {}
};

/**
 * Stack class for storing recent match results (LIFO - Last In, First Out)
 * Provides quick access to the most recent matches for immediate review
 */
class Stack {
private:
    static const int MAX_CAPACITY = 1000;  // Maximum stack capacity
    MatchResult data[MAX_CAPACITY];        // Fixed-size array for storage
    int top_index;                         // Index of top element (-1 when empty)

public:
    // Constructor initializes empty stack
    Stack() : top_index(-1) {}
    
    /**
     * Add a match result to the top of the stack
     * @param match The match result to add
     * @return true if successful, false if stack is full
     */
    bool push(const MatchResult& match);
    
    /**
     * Retrieve the i-th match from the top without removing it
     * @param index 0-based index from top (0 = topmost element)
     * @param out Reference to store the retrieved match
     * @return true if successful, false if index is invalid
     */
    bool getFromTop(int index, MatchResult& out) const;
    
    /**
     * Get the current number of matches in the stack
     * @return Number of matches stored
     */
    int size() const { return top_index + 1; }
    
    /**
     * Check if the stack is empty
     * @return true if empty, false otherwise
     */
    bool isEmpty() const { return top_index == -1; }
};

/**
 * Queue class for storing match history (FIFO - First In, First Out)
 * Maintains chronological order of all matches for historical analysis
 */
class Queue {
private:
    static const int MAX_CAPACITY = 1000;  // Maximum queue capacity
    MatchResult data[MAX_CAPACITY];        // Fixed-size array for storage
    int front_index;                       // Index of front element
    int rear_index;                        // Index of rear element
    int current_size;                      // Current number of elements

public:
    // Constructor initializes empty queue
    Queue() : front_index(0), rear_index(-1), current_size(0) {}
    
    /**
     * Add a match result to the rear of the queue
     * @param match The match result to add
     * @return true if successful, false if queue is full
     */
    bool enqueue(const MatchResult& match);
    
    /**
     * Retrieve the i-th match without removing it
     * @param index 0-based index from front (0 = oldest element)
     * @param out Reference to store the retrieved match
     * @return true if successful, false if index is invalid
     */
    bool getAt(int index, MatchResult& out) const;
    
    /**
     * Get the current number of matches in the queue
     * @return Number of matches stored
     */
    int size() const { return current_size; }
    
    /**
     * Check if the queue is empty
     * @return true if empty, false otherwise
     */
    bool isEmpty() const { return current_size == 0; }
};

/**
 * Main class for managing game results and player performance
 * Integrates all data structures and provides comprehensive functionality
 */
class GameResultManager {
private:
    PlayerStats* player_stats;      // Dynamic array of player statistics
    int max_players;               // Maximum number of players allowed
    int current_player_count;      // Current number of players loaded
    int next_match_id;             // Next available match ID for auto-assignment
    Stack recent_matches;          // Stack for recent match results
    Queue match_history;           // Queue for complete match history

public:
    /**
     * Constructor initializes the manager with specified capacity
     * @param max_players Maximum number of players to support
     */
    GameResultManager(int max_players);
    
    /**
     * Destructor deallocates dynamic memory
     */
    ~GameResultManager();
    
    /**
     * Load player data from CSV file
     * @param filename Path to the players CSV file
     * @return true if file opened successfully, false otherwise
     */
    bool loadPlayerData(const std::string& filename);
    
    /**
     * Load match history from CSV file
     * @param filename Path to the matches CSV file
     * @return true if file opened successfully, false otherwise
     */
    bool loadMatchHistory(const std::string& filename);
    
    /**
     * Log a new match result to memory and file
     * @param match The match result to log
     * @param filename Optional specific filename (uses date-based if empty)
     * @return true if logged successfully, false on critical failure
     */
    bool logMatchResult(const MatchResult& match, const std::string& filename = "");
    
    /**
     * Display the most recent matches
     * @param count Number of recent matches to display (default 5)
     */
    void displayRecentMatches(int count = 5);
    
    /**
     * Display statistics for a specific player
     * @param player_id ID of the player to display
     */
    void displayPlayerStats(int player_id);
    
    /**
     * Display statistics for all players
     */
    void displayAllPlayerStats();
    
    /**
     * Query and display all matches involving a specific player
     * @param player_id ID of the player to query
     */
    void queryMatchesByPlayer(int player_id);
    
    /**
     * Query and display all matches in a specific tournament stage
     * @param stage Name of the tournament stage
     */
    void queryMatchesByStage(const std::string& stage);
    
    /**
     * Display the main program menu
     */
    void displayMenu();
    
    /**
     * Run the main program loop with user interaction
     */
    void runProgram();

private:
    /**
     * Helper function to extract date from scheduled time string
     * @param scheduled_time Time string in format "YYYY-MM-DD HH:MM:SS"
     * @return Date string in format "YYYY-MM-DD" or empty string if invalid
     */
    std::string extractDateFromScheduledTime(const std::string& scheduled_time);
    
    /**
     * Helper function to parse CSV line into tokens
     * @param line CSV line to parse
     * @param tokens Array to store parsed tokens
     * @param max_tokens Maximum number of tokens to parse
     */
    void splitCSVLine(const std::string& line, std::string tokens[], int max_tokens);
    
    /**
     * Helper function to find player index by ID
     * @param player_id Player ID to search for
     * @return Index in player_stats array, or -1 if not found
     */
    int findPlayerIndex(int player_id);
    
    /**
     * Helper function to parse score from score string
     * @param score_str Score string in format "X-Y"
     * @return First score value as double, or 0.0 on error
     */
    double parseScore(const std::string& score_str);
    
    /**
     * Helper function to update player statistics after a match
     * @param player_id ID of player to update
     * @param is_winner Whether the player won the match
     * @param score Player's score in the match
     */
    void updatePlayerStats(int player_id, bool is_winner, double score);
};

#endif // TASK4_HPP