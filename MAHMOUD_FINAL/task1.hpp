#ifndef TOURNAMENT_HPP
#define TOURNAMENT_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstring>

using namespace std;

// Forward declarations
class Match;
class Group;
class Tournament;

// Player class to store player information
class Player {
private:
    int id;
    char name[100];
    char rank[2]; // "A", "B", "C", or "D"
    char registrationType[30]; // <-- add this
    char currentStage[20];
    int wins;
    int losses;
    int groupId;
    bool registered;
    bool checkedIn;
    char checkInTime[20];
    
public:
    Player(int _id, const char* _name, const char* _rank, const char* _registrationType = "", int _ranking = 0,
           const char* _email = "",
           int _teamID = 0, bool _checkInStatus = false);
    
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
    const char* getRegistrationType() const { return registrationType; }
    
    void setCurrentStage(const char* stage);
    void setGroupId(int id);
    void setCheckIn(bool status, const char* time = "");
    
    void incrementWins();
    void incrementLosses();
    
    void advanceStage();
};

// Custom Queue implementation for scheduling matches
class MatchQueue {
private:
    struct Node {
        Match* match;
        Node* next;
        Node(Match* m);
    };
    
    Node* front;
    Node* rear;
    int size;
    
public:
    MatchQueue();
    ~MatchQueue();
    
    void enqueue(Match* match);
    Match* dequeue();
    bool isEmpty() const;
    int getSize() const;
};

// Custom Priority Queue for player check-in prioritization
class PlayerPriorityQueue {
private:
    struct Node {
        Player* player;
        Node* next;
        Node(Player* p);
    };
    
    Node* head;
    int size;
    
    // Helper function to compare check-in times (format: YYYY-MM-DD HH:MM)
    bool isEarlier(const char* time1, const char* time2);
    
public:
    PlayerPriorityQueue();
    ~PlayerPriorityQueue();
    
    void enqueue(Player* player);
    Player* dequeue();
    bool isEmpty() const;
    int getSize() const;
};

// Match class to store match information
class Match {
private:
    int id;
    Player* player1;
    Player* player2;
    char stage[20];
    int groupId;
    int round;
    char status[20];
    Player* winner;
    char score[10];
    char scheduledTime[20];
    
public:
    Match(int _id, Player* p1, Player* p2, const char* _stage, int _groupId = 0, int _round = 1);
    
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
};

// Group class to manage group stage
class Group {
private:
    int id;
    Player* players[4];
    int playerCount;
    Match* matches[3];
    int matchCount;
    Player* winner;
    bool completed;
    int semiFinalsCompleted;
    char rankType[2];
    char registrationType[30]; // <-- add this
    
public:
    Group(int _id, const char* _rankType, const char* _registrationType); // update constructor
    ~Group();
    
    int getId() const;
    bool isCompleted() const;
    Player* getWinner() const;
    int getSemiFinalsCompleted() const;
    const char* getRankType() const;
    int getPlayerCount() const;
    const char* getRegistrationType() const { return registrationType; } // getter
    
    void addPlayer(Player* player);
    void createSemifinalsOnly(int& nextMatchId);
    void createFinalMatch(int& nextMatchId, Player* semifinal1Winner, Player* semifinal2Winner);
    Match* getMatch(int index);
    int getMatchCount() const;
    void incrementSemiFinalsCompleted();
    bool areSemifinalsComplete();
    void setGroupWinner(Player* player);
    void displayStatus();
};

// Tournament class to manage the tournament
class Tournament {
private:
    MatchQueue upcomingMatches;
    PlayerPriorityQueue playerCheckInQueue;
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
    int nextMatchId;
    int totalMatchesPlayed;
    bool groupSemifinalsCreated;
    bool knockoutCreated;
    bool groupsCreated;
    
    void loadPlayersFromCSV(const char* filename);
    void groupPlayersByRank();
    void saveMatchesToCSV(const char* filename);
    void saveBracketsToCSV(const char* filename);
    void createKnockoutMatches();
    void createFinalMatch(Player* finalist1, Player* finalist2);
    
public:
    Tournament(int _maxPlayers = 100, int _maxMatches = 200, int _maxGroupWinners = 10);
    ~Tournament();
    
    void initialize(const char* playerFilename);
    void displayCheckInStatus();
    void createGroupSemifinals();
    Match* getNextMatch();
    void updateMatchResult(Match* match, Player* winner);
    void displayStatus();
    void runCLI_TASK1();
};

//task2





#endif // TOURNAMENT_HPP