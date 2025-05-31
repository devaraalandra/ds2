// APUEC_System.h
#ifndef APUEC_SYSTEM_H
#define APUEC_SYSTEM_H

// Common Standard Includes (No STL Containers like <vector>, <list>, <queue>, <stack>)
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>    // For std::string (used in Task 4, and by sstream)
#include <cstring>   // For C-style string functions (e.g., strcpy, strcmp)
#include <cstdlib>   // For general utilities (e.g., atoi, rand, srand, malloc, free)
#include <ctime>     // For time functions (e.g., time, localtime, strftime)
#include <iomanip>   // For I/O manipulators (e.g., setw, setprecision)
#include <limits>    // For numeric_limits
#include <algorithm> // For std::min, std::max (if needed, ensure not for containers)
#include <cstdio>    // For C-style I/O (e.g., printf, scanf, FILE ops from Task 2)
#include <cctype>    // For character functions (e.g., tolower, isdigit from Task 2)

// Forward declarations if strictly necessary, but full declarations preferred in H for this structure.

// Common Constants
const int TASK4_MAX_CAPACITY = 100; // Used by Task 4

// Task 1: Match Scheduling - Class Declarations
class Player; // Forward declaration for Match, Group, PlayerPriorityQueue
class Match;  // Forward declaration for MatchQueue, Group, Tournament
class Group;  // Forward declaration for Tournament
class Tournament;
class MatchQueue;
class PlayerPriorityQueue;


class Player {
public:
    Player(int _id, const char* _name, const char* _rank, const char* _registrationType, int _ranking,
           const char* _email, int _teamID, bool _checkInStatus);
    int getId() const;
    const char* getName() const;
    const char* getRank() const;
    const char* getCurrentStage() const;
    int getWins() const;
    int getLosses() const;
    int getGroupId() const;
    bool isRegistered() const;
    bool isCheckedIn() const;
    const char* getCheckInTime() const;
    const char* getRegistrationType() const;

    void setCurrentStage(const char* stage);
    void setGroupId(int id);
    void setCheckIn(bool status, const char* time);
    void incrementWins();
    void incrementLosses();
    void advanceStage();

private:
    int id;
    char name[100];
    char rank[2]; // e.g. "A", "B"
    char registrationType[30];
    char currentStage[20];
    int wins;
    int losses;
    int groupId;
    bool registered;
    bool checkedIn;
    char checkInTime[20]; // Format: YYYY-MM-DD HH:MM
};

class MatchQueue {
public:
    MatchQueue();
    ~MatchQueue();
    void enqueue(Match* match);
    Match* dequeue();
    bool isEmpty() const;
    int getSize() const;
private:
    struct Node {
        Match* match;
        Node* next;
        Node(Match* m);
    };
    Node *front, *rear;
    int size;
};

class PlayerPriorityQueue {
public:
    PlayerPriorityQueue();
    ~PlayerPriorityQueue();
    void enqueue(Player* player);
    Player* dequeue();
    bool isEmpty() const;
    int getSize() const;
private:
    struct Node {
        Player* player;
        Node* next;
        Node(Player* p);
    };
    Node* head;
    int size;
    bool isEarlier(const char* time1, const char* time2); // Compares "YYYY-MM-DD HH:MM"
};

class Match {
public:
    Match(int _id, Player* p1, Player* p2, const char* _stage, int _groupId, int _round);
    int getId() const;
    Player* getPlayer1() const;
    Player* getPlayer2() const;
    const char* getStage() const; // "group", "knockout"
    int getGroupId() const;
    int getRound() const; // 1 for semi, 2 for final in group/knockout
    const char* getStatus() const; // "scheduled", "completed"
    Player* getWinner() const;
    const char* getScore() const; // e.g. "1-0"
    const char* getScheduledTime() const; // Format: YYYY-MM-DD HH:MM

    void setStatus(const char* _status);
    void setWinner(Player* _winner);
private:
    int id;
    Player *player1, *player2;
    char stage[20];
    int groupId; // 0 if not a group match (e.g. overall knockout)
    int round;
    char status[20];
    Player* winner;
    char score[10];
    char scheduledTime[20];
};

class Group {
public:
    Group(int _id, const char* _rankType, const char* _registrationType);
    ~Group();
    int getId() const;
    bool isCompleted() const;
    Player* getWinner() const;
    int getSemiFinalsCompleted() const;
    const char* getRankType() const;
    const char* getRegistrationType() const;
    int getPlayerCount() const;
    Player* getPlayer(int index) const;


    void addPlayer(Player* player);
    void createSemifinalsOnly(int& nextMatchId);
    void createFinalMatch(int& nextMatchId, Player* semifinal1Winner, Player* semifinal2Winner);
    Match* getMatch(int index); // 0,1 for semis, 2 for final
    int getMatchCount() const;
    void incrementSemiFinalsCompleted();
    bool areSemifinalsComplete();
    void setGroupWinner(Player* player);
    void displayStatus();

private:
    int id;
    char rankType[2]; // "A", "B", etc. This group is for players of this rank.
    char registrationType[30]; // For this group, if specific.
    Player* players[4]; // Max 4 players per group for semifinal -> final structure
    int playerCount;
    Match* matches[3]; // 2 semifinals, 1 final
    int matchCount;
    Player* winner;
    bool completed;
    int semiFinalsCompleted; // Counter for completed semifinal matches
};

class Tournament {
public:
    Tournament(int _maxPlayers = 100, int _maxMatches = 200, int _maxGroupWinners = 20);
    ~Tournament();

    void initialize(const char* playerFilename); // Loads players, groups them by rank from check-in queue
    void displayCheckInStatus();
    void createGroupSemifinals(); // Creates semifinal matches for all valid groups
    Match* getNextMatch(); // Gets next match from upcomingMatches queue
    void updateMatchResult(Match* match, Player* winner); // Updates match, advances stages
    void displayStatus(); // Displays overall tournament status, group status, upcoming matches
    void runCLI_TASK1(); // Runs the command-line interface for Task 1
    bool areGroupsCreated() const;


private:
    void loadPlayersFromCSV(const char* filename);
    void groupPlayersByRank(); // Groups checked-in players by rank and registration type
    void saveMatchesToCSV(const char* filename); // Saves all tournament matches
    void saveBracketsToCSV(const char* filename); // Saves player progression (basic bracket info)
    void createKnockoutMatches(); // Creates knockout matches from group winners
    void createFinalMatch(Player* finalist1, Player* finalist2); // Creates the grand final match

    Player** players; // Array of all player objects
    int playerCount;
    int maxPlayers;
    Match** matches; // Array of all match objects created
    int matchCount;
    int maxMatches;
    Group** groups; // Array of group objects
    int groupCount;
    Player** groupWinners; // Array of players who won their groups
    int groupWinnerCount;
    int maxGroupWinners;

    MatchQueue upcomingMatches; // Queue for matches yet to be played
    PlayerPriorityQueue playerCheckInQueue; // Priority queue for checked-in players (by check-in time)

    int nextMatchId; // For generating unique match IDs
    int totalMatchesPlayed;
    bool groupSemifinalsCreated;
    bool knockoutCreated;
    bool groupsCreated; // Flag to check if initial grouping is done
};


// Task 2: Player Registration - Struct and Function Declarations

// Forward declaration for Task2_Queue and Task2_PriorityQueue
struct Task2_Player;

struct Task2_Queue {
    struct Task2_Player* front;
    struct Task2_Player* rear;
    int size;

    void init();
    int isEmpty();
    void enqueue(struct Task2_Player* player);
    struct Task2_Player* dequeue();
    struct Task2_Player* peek();
    int getSize();
    void destroy();
};

struct Task2_Player {
    char playerID[10];
    char playerName[50];
    int ranking; // Original field, seems unused in task2 logic, defaulted to 0
    char registrationType[20]; // Early-Bird, Standard, Wildcard, Last-Minute
    char email[100];
    int teamID; // Original field, seems unused in task2 logic, defaulted to 0
    int checkInStatus; // 1 for YES, 0 for NO
    int group; // Original field, seems unused in task2 logic, defaulted to 0
    char rank[2]; // A, B, C, D - derived from registrationType
    char checkInTime[50]; // YYYY-MM-DD HH:MM:SS or "N/A"
    char status[10]; // MAIN or WAITLIST
    struct Task2_Player* next; // For linking in the queue
};

struct Task2_PriorityQueue {
    struct Task2_Queue earlyBirdQueue;
    struct Task2_Queue wildcardQueue;
    struct Task2_Queue standardQueue;
    struct Task2_Queue lastMinuteQueue;
    struct Task2_Queue waitlistQueue;
    int size; // Total in main queues (not waitlist)

    void init();
    void enqueue(struct Task2_Player* player);
    struct Task2_Player* dequeue();
    int isEmpty();
    int getSize();
    struct Task2_Queue* getWaitlistQueue();
    void setWaitlistQueue(struct Task2_Queue* q); // Used for transferring ownership after processing waitlist
};

// Task 2 Function Prototypes (to be called from the integrated system)
void Task2_runPlayerRegistrationSystem(); // Main loop for Task 2
// Internal Task 2 functions (will be defined in .cpp, not necessarily needed in .h if static or only called by Task2_runPlayerRegistrationSystem)
// void Task2_registerPlayer(Task2_PriorityQueue* pq, const char* filename);
// void Task2_checkInPlayer(Task2_PriorityQueue* pq, const char* filename);
// void Task2_handleWithdrawPlayer(Task2_PriorityQueue* pq, const char* filename); // Renamed from handleWithdrawPlayer
// void Task2_displayWaitlist(Task2_PriorityQueue* pq);
// void Task2_readPlayersFromCSV(Task2_PriorityQueue* pq, const char* filename);
// void Task2_writePlayersToCSV(Task2_PriorityQueue* pq, const char* filename);


// Task 3: Spectator Management (Placeholder - No specific structs/classes defined for it yet)
void runTask3_SpectatorManagement();


// Task 4: Result Logging - Struct and Class Declarations
struct Task4_MatchResult {
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

    Task4_MatchResult(); // Default constructor
    Task4_MatchResult(int mid, const std::string& st, int gid, int r, int p1, int p2,
                      const std::string& sch_time, const std::string& stat, int wid, const std::string& scr);
};

class Task4_Stack {
public:
    Task4_Stack();
    bool push(const Task4_MatchResult& match);
    bool pop(Task4_MatchResult& out);
    bool peek(Task4_MatchResult& out) const;
    bool getFromTop(int index, Task4_MatchResult& out) const; // Get element by index from top (0 is top)
    bool isEmpty() const;
    int size() const;

private:
    Task4_MatchResult data[TASK4_MAX_CAPACITY];
    int top_index; // -1 for empty stack
};

class Task4_Queue { // Circular Queue
public:
    Task4_Queue();
    bool enqueue(const Task4_MatchResult& match);
    bool dequeue(Task4_MatchResult& out);
    bool peek(Task4_MatchResult& out) const;
    bool getAt(int index, Task4_MatchResult& out) const; // Get element by logical index (0 is front)
    bool isEmpty() const;
    int size() const;

private:
    Task4_MatchResult data[TASK4_MAX_CAPACITY];
    int front_index;
    int rear_index; // Points to the last element
    int current_size;
};

struct Task4_PlayerStats {
    int player_id;
    std::string name;
    std::string rank;
    std::string contact; // email
    std::string registration_time; // registration type
    int total_matches;
    int wins;
    int losses;
    double avg_score;

    Task4_PlayerStats(); // Default constructor
    Task4_PlayerStats(int pid, const std::string& n, const std::string& r, const std::string& c, const std::string& reg_time);
};

class Task4_GameResultManager {
public:
    Task4_GameResultManager(int max_players = 100);
    ~Task4_GameResultManager();

    // Public methods for loading data and running the system
    bool loadPlayerData(const std::string& filename); // Loads from "players.csv" typically
    bool loadMatchHistory(const std::string& filename); // Loads from "matches.csv" typically
    void displayRecentMatches(int count = 5);
    void displayPlayerStats(int player_id);
    void displayAllPlayerStats();
    void queryMatchesByPlayer(int player_id);
    void queryMatchesByStage(const std::string& stage);
    void runProgram(); // Main operational loop for Task 4

private:
    // Helper methods
    std::string extractDateFromScheduledTime(const std::string& scheduled_time);
    void splitCSVLine(const std::string& line, std::string tokens[], int max_tokens);
    int findPlayerIndex(int player_id); // Finds index in player_stats array
    double parseScore(const std::string& score_str); // Parses score like "X-Y", returns X
    void updatePlayerStats(int player_id, bool is_winner, double score);
    void displayMenu_Task4(); // Displays Task 4 specific menu

    Task4_PlayerStats* player_stats; // Dynamically allocated array of player statistics
    int task4_max_players;
    int current_player_count;

    Task4_Stack recent_matches; // Stores recent matches (e.g., last N matches)
    Task4_Queue match_history;  // Stores all matches loaded or processed

    int next_match_id; // Potentially used if new matches were to be logged by this system
};


#endif // APUEC_SYSTEM_H