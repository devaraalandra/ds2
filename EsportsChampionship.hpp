#ifndef ESPORTSCHAMPIONSHIP_HPP
#define ESPORTSCHAMPIONSHIP_HPP

// Common Standard Includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <limits>
#include <algorithm> // For std::min (used in Task 4)
// #include <stdexcept> // For std::stoi, std::stod exceptions (used in Task 4)

// Forward declarations
class Player;
class Match;
class Group;
class MatchQueue;
class PlayerPriorityQueue;
class Tournament;

struct Task4_MatchResult;
class Task4_Stack;
class Task4_Queue;
struct Task4_PlayerStats;
class Task4_GameResultManager;


// Task 1: Match Scheduling
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
    char rank[2];
    char registrationType[30];
    char currentStage[20];
    int wins;
    int losses;
    int groupId;
    bool registered;
    bool checkedIn;
    char checkInTime[20];
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
    bool isEarlier(const char* time1, const char* time2);
};

class Match {
public:
    Match(int _id, Player* p1, Player* p2, const char* _stage, int _groupId, int _round);
    int getId() const;
    Player* getPlayer1() const;
    Player* getPlayer2() const;
    const char* getStage() const;
    int getGroupId() const;
    int getRound() const;
    const char* getStatus() const;
    Player* getWinner() const;
    const char* getScore() const;
    const char* getScheduledTime() const;

    void setStatus(const char* _status);
    void setWinner(Player* _winner);
private:
    int id;
    Player *player1, *player2;
    char stage[20];
    int groupId;
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
    Match* getMatch(int index);
    int getMatchCount() const;
    void incrementSemiFinalsCompleted();
    bool areSemifinalsComplete();
    void setGroupWinner(Player* player);
    void displayStatus();

private:
    int id;
    char rankType[2];
    char registrationType[30];
    Player* players[4]; // Max 4 players per group
    int playerCount;
    Match* matches[3]; // 2 semifinals, 1 final
    int matchCount;
    Player* winner;
    bool completed;
    int semiFinalsCompleted;
};

class Tournament {
public:
    Tournament(int _maxPlayers = 100, int _maxMatches = 200, int _maxGroupWinners = 20);
    ~Tournament();

    void initialize(const char* playerFilename);
    void displayCheckInStatus();
    void createGroupSemifinals();
    Match* getNextMatch();
    void updateMatchResult(Match* match, Player* winner);
    void displayStatus();
    void runCLI_TASK1();
    bool areGroupsCreated() const;

private:
    void loadPlayersFromCSV(const char* filename);
    void groupPlayersByRank();
    void saveMatchesToCSV(const char* filename);
    void saveBracketsToCSV(const char* filename);
    void createKnockoutMatches();
    void createFinalMatch(Player* finalist1, Player* finalist2);

    Player** players;
    int playerCount;
    int maxPlayers;
    Match** matches;
    int matchCount;
    int maxMatches;
    Group** groups;
    int groupCount;
    Player** groupWinners;
    int groupWinnerCount;
    int maxGroupWinners;

    MatchQueue upcomingMatches;
    PlayerPriorityQueue playerCheckInQueue;

    int nextMatchId;
    int totalMatchesPlayed;
    bool groupSemifinalsCreated;
    bool knockoutCreated;
    bool groupsCreated;
};


// Task 4: Result Logging
const int TASK4_MAX_CAPACITY = 100;

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

    Task4_MatchResult();
    Task4_MatchResult(int mid, const std::string& st, int gid, int r, int p1, int p2,
                      const std::string& sch_time, const std::string& stat, int wid, const std::string& scr);
};

class Task4_Stack {
public:
    Task4_Stack();
    bool push(const Task4_MatchResult& match);
    bool pop(Task4_MatchResult& out);
    bool peek(Task4_MatchResult& out) const;
    bool getFromTop(int index, Task4_MatchResult& out) const;
    bool isEmpty() const;
    int size() const;

private:
    Task4_MatchResult data[TASK4_MAX_CAPACITY];
    int top_index;
};

class Task4_Queue {
public:
    Task4_Queue();
    bool enqueue(const Task4_MatchResult& match);
    bool dequeue(Task4_MatchResult& out);
    bool peek(Task4_MatchResult& out) const;
    bool getAt(int index, Task4_MatchResult& out) const;
    bool isEmpty() const;
    int size() const;

private:
    Task4_MatchResult data[TASK4_MAX_CAPACITY];
    int front_index;
    int rear_index;
    int current_size;
};

struct Task4_PlayerStats {
    int player_id;
    std::string name;
    std::string rank;
    std::string contact;
    std::string registration_time;
    int total_matches;
    int wins;
    int losses;
    double avg_score;

    Task4_PlayerStats();
    Task4_PlayerStats(int pid, const std::string& n, const std::string& r, const std::string& c, const std::string& reg_time);
};

class Task4_GameResultManager {
public:
    Task4_GameResultManager(int max_players = 100);
    ~Task4_GameResultManager();

    bool loadPlayerData(const std::string& filename);
    bool loadMatchHistory(const std::string& filename);
    void displayRecentMatches(int count = 5);
    void displayPlayerStats(int player_id);
    void displayAllPlayerStats();
    void queryMatchesByPlayer(int player_id);
    void queryMatchesByStage(const std::string& stage);
    void runProgram();

private:
    std::string extractDateFromScheduledTime(const std::string& scheduled_time);
    void splitCSVLine(const std::string& line, std::string tokens[], int max_tokens);
    int findPlayerIndex(int player_id);
    double parseScore(const std::string& score_str);
    void updatePlayerStats(int player_id, bool is_winner, double score);
    void displayMenu_Task4();

    Task4_PlayerStats* player_stats;
    int task4_max_players;
    int current_player_count;

    Task4_Stack recent_matches;
    Task4_Queue match_history;

    int next_match_id;
};

#endif // ESPORTSCHAMPIONSHIP_HPP