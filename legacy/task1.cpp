#include "task1.hpp"
using namespace std;
// Player class implementation
Player::Player(int _id, const char* _name, const char* _rank, const char* _registrationType, int _ranking,
               const char* _email, int _teamID, bool _checkInStatus)
    : id(_id), wins(0), losses(0), groupId(0), registered(true), checkedIn(_checkInStatus) {
    strncpy(name, _name, sizeof(name) - 1); name[sizeof(name) - 1] = '\0';
    strncpy(rank, _rank, sizeof(rank) - 1); rank[sizeof(rank) - 1] = '\0';
    strncpy(registrationType, _registrationType, sizeof(registrationType) - 1); registrationType[sizeof(registrationType) - 1] = '\0';
    strcpy(currentStage, "group");
    checkInTime[0] = '\0';
    // Optionally store email, teamID, ranking if you add them as members
}

int Player::getId() const { return id; }
const char* Player::getName() const { return name; }
const char* Player::getRank() const { return rank; }
const char* Player::getCurrentStage() const { return currentStage; }
int Player::getWins() const { return wins; }
int Player::getLosses() const { return losses; }
int Player::getGroupId() const { return groupId; }
bool Player::isRegistered() const { return registered; }
bool Player::isCheckedIn() const { return checkedIn; }
const char* Player::getCheckInTime() const { return checkInTime; }

void Player::setCurrentStage(const char* stage) { 
    strncpy(currentStage, stage, 19); currentStage[19] = '\0'; 
}

void Player::setGroupId(int id) { groupId = id; }

void Player::setCheckIn(bool status, const char* time) { 
    checkedIn = status; 
    if (time[0] != '\0') {
        strncpy(checkInTime, time, 19); checkInTime[19] = '\0';
    }
}

void Player::incrementWins() { wins++; }
void Player::incrementLosses() { losses++; }

void Player::advanceStage() {
    if (strcmp(currentStage, "group") == 0) {
        strcpy(currentStage, "knockout");
    } else if (strcmp(currentStage, "knockout") == 0) {
        strcpy(currentStage, "final");
    }
}

// MatchQueue class implementation
MatchQueue::Node::Node(Match* m) : match(m), next(nullptr) {}

MatchQueue::MatchQueue() : front(nullptr), rear(nullptr), size(0) {}

MatchQueue::~MatchQueue() {
    while (!isEmpty()) {
        dequeue();
    }
}

void MatchQueue::enqueue(Match* match) {
    Node* newNode = new Node(match);
    if (isEmpty()) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
    size++;
}

Match* MatchQueue::dequeue() {
    if (isEmpty()) return nullptr;
    Node* temp = front;
    Match* match = temp->match;
    front = front->next;
    if (front == nullptr) {
        rear = nullptr;
    }
    delete temp;
    size--;
    return match;
}

bool MatchQueue::isEmpty() const {
    return front == nullptr;
}

int MatchQueue::getSize() const {
    return size;
}

// PlayerPriorityQueue class implementation
PlayerPriorityQueue::Node::Node(Player* p) : player(p), next(nullptr) {}

PlayerPriorityQueue::PlayerPriorityQueue() : head(nullptr), size(0) {}

PlayerPriorityQueue::~PlayerPriorityQueue() {
    while (!isEmpty()) {
        dequeue();
    }
}

// Helper function to compare check-in times (format: YYYY-MM-DD HH:MM)
bool PlayerPriorityQueue::isEarlier(const char* time1, const char* time2) {
    // Simple comparison: earlier timestamp has higher priority
    return strcmp(time1, time2) < 0;
}

void PlayerPriorityQueue::enqueue(Player* player) {
    Node* newNode = new Node(player);
    if (isEmpty()) {
        head = newNode;
    } else if (isEarlier(player->getCheckInTime(), head->player->getCheckInTime())) {
        newNode->next = head;
        head = newNode;
    } else {
        Node* current = head;
        Node* prev = nullptr;
        while (current != nullptr && !isEarlier(player->getCheckInTime(), current->player->getCheckInTime())) {
            prev = current;
            current = current->next;
        }
        if (prev != nullptr) {
            prev->next = newNode;
            newNode->next = current;
        }
    }
    size++;
}

Player* PlayerPriorityQueue::dequeue() {
    if (isEmpty()) return nullptr;
    Node* temp = head;
    Player* player = temp->player;
    head = head->next;
    delete temp;
    size--;
    return player;
}

bool PlayerPriorityQueue::isEmpty() const {
    return head == nullptr;
}

int PlayerPriorityQueue::getSize() const {
    return size;
}

// Match class implementation
Match::Match(int _id, Player* p1, Player* p2, const char* _stage, int _groupId, int _round) 
    : id(_id), player1(p1), player2(p2), groupId(_groupId), round(_round), winner(nullptr) {
    strncpy(stage, _stage, 19); stage[19] = '\0';
    strcpy(status, "scheduled");
    strcpy(score, "0-0");
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[20];
    strftime(buffer, 20, "%Y-%m-%d %H:%M", ltm);
    strncpy(scheduledTime, buffer, 19); scheduledTime[19] = '\0';
}

int Match::getId() const { return id; }
Player* Match::getPlayer1() const { return player1; }
Player* Match::getPlayer2() const { return player2; }
const char* Match::getStage() const { return stage; }
int Match::getGroupId() const { return groupId; }
int Match::getRound() const { return round; }
const char* Match::getStatus() const { return status; }
Player* Match::getWinner() const { return winner; }
const char* Match::getScore() const { return score; }
const char* Match::getScheduledTime() const { return scheduledTime; }

void Match::setStatus(const char* _status) { 
    strncpy(status, _status, 19); status[19] = '\0'; 
}

void Match::setWinner(Player* _winner) { 
    winner = _winner; 
    strcpy(status, "completed");
    if (winner == player1) {
        player1->incrementWins();
        player2->incrementLosses();
        strcpy(score, "1-0");
    } else if (winner == player2) {
        player2->incrementWins();
        player1->incrementLosses();
        strcpy(score, "0-1");
    }
}

// Group class implementation
Group::Group(int _id, const char* _rankType, const char* _registrationType) : id(_id), playerCount(0), matchCount(0), 
       winner(nullptr), completed(false), semiFinalsCompleted(0) {
    strncpy(rankType, _rankType, 1); rankType[1] = '\0';
    strncpy(registrationType, _registrationType, sizeof(registrationType) - 1); registrationType[sizeof(registrationType) - 1] = '\0';
}

Group::~Group() {
    for (int i = 0; i < matchCount; i++) {
        delete matches[i];
    }
}

int Group::getId() const { return id; }
bool Group::isCompleted() const { return completed; }
Player* Group::getWinner() const { return winner; }
int Group::getSemiFinalsCompleted() const { return semiFinalsCompleted; }
const char* Group::getRankType() const { return rankType; }
int Group::getPlayerCount() const { return playerCount; }

void Group::addPlayer(Player* player) {
    if (playerCount < 4) {
        players[playerCount++] = player;
        player->setGroupId(id);
    }
}

void Group::createSemifinalsOnly(int& nextMatchId) {
    if (playerCount < 4) {
        cout << "Error: Group " << id << " does not have enough players." << endl;
        return;
    }
    matches[0] = new Match(nextMatchId++, players[0], players[3], "group", id, 1);
    matches[1] = new Match(nextMatchId++, players[1], players[2], "group", id, 1);
    matchCount = 2;
    cout << "Group " << id << " (Rank " << rankType << ") semifinal matches created." << endl;
}

void Group::createFinalMatch(int& nextMatchId, Player* semifinal1Winner, Player* semifinal2Winner) {
    matches[2] = new Match(nextMatchId++, semifinal1Winner, semifinal2Winner, "group", id, 2);
    matchCount = 3;
    cout << "Group " << id << " (Rank " << rankType << ") final match created: " 
         << semifinal1Winner->getName() << " vs " << semifinal2Winner->getName() << endl;
}

Match* Group::getMatch(int index) {
    if (index >= 0 && index < matchCount) {
        return matches[index];
    }
    return nullptr;
}

int Group::getMatchCount() const {
    return matchCount;
}

void Group::incrementSemiFinalsCompleted() {
    semiFinalsCompleted++;
}

bool Group::areSemifinalsComplete() {
    return semiFinalsCompleted >= 2;
}

void Group::setGroupWinner(Player* player) {
    winner = player;
    completed = true;
}

void Group::displayStatus() {
    cout << "\n----- GROUP " << id << " (RANK " << rankType << ") STATUS -----\n";
    cout << "Players:\n";
    for (int i = 0; i < playerCount; i++) {
        cout << players[i]->getName() << " (Rank: " << players[i]->getRank() 
             << ", Check-in: " << (players[i]->isCheckedIn() ? "YES" : "NO") << ")" << endl;
    }
    
    cout << "\nMatches:\n";
    for (int i = 0; i < matchCount; i++) {
        if (matches[i] != nullptr) {
            cout << "Match " << matches[i]->getId() << ": " 
                 << matches[i]->getPlayer1()->getName() << " (Rank: " << matches[i]->getPlayer1()->getRank() << ")"
                 << " vs " 
                 << matches[i]->getPlayer2()->getName() << " (Rank: " << matches[i]->getPlayer2()->getRank() << ")"
                 << " - Status: " << matches[i]->getStatus();
            if (strcmp(matches[i]->getStatus(), "completed") == 0) {
                cout << " - Winner: " << matches[i]->getWinner()->getName();
            }
            cout << endl;
        }
    }
    
    if (completed) {
        cout << "\nGroup Winner: " << winner->getName() << endl;
    }
}

// Tournament class implementation
Tournament::Tournament(int _maxPlayers, int _maxMatches, int _maxGroupWinners) 
    : playerCount(0), maxPlayers(_maxPlayers), matchCount(0), maxMatches(_maxMatches), 
      groupCount(0), groupWinnerCount(0), maxGroupWinners(_maxGroupWinners), nextMatchId(1), 
      totalMatchesPlayed(0), groupSemifinalsCreated(false), knockoutCreated(false), groupsCreated(false) {
    players = new Player*[maxPlayers];
    matches = new Match*[maxMatches];
    groups = new Group*[10];
    groupWinners = new Player*[maxGroupWinners];
}

Tournament::~Tournament() {
    for (int i = 0; i < playerCount; i++) {
        delete players[i];
    }
    delete[] players;
    for (int i = 0; i < matchCount; i++) {
        delete matches[i];
    }
    delete[] matches;
    for (int i = 0; i < groupCount; i++) {
        delete groups[i];
    }
    delete[] groups;
    delete[] groupWinners;
}

void Tournament::loadPlayersFromCSV(const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return;
    }
    char line[512];
    file.getline(line, 512); // Skip header
    while (file.getline(line, 512) && playerCount < maxPlayers) {
        if (strlen(line) == 0) continue;
        stringstream ss(line);
        char idStr[10], nameStr[100], regTypeStr[30], emailStr[100], rankStr[2], checkInStr[10];
        ss.getline(idStr, 10, ',');
        ss.getline(nameStr, 100, ',');
        ss.getline(regTypeStr, 30, ',');
        ss.getline(emailStr, 100, ',');
        ss.getline(rankStr, 2, ',');
        ss.getline(checkInStr, 10, ',');
        int id = 0;
        try {
            id = atoi(idStr);
        } catch (...) {
            cout << "Warning: Invalid ID '" << idStr << "', skipping line." << endl;
            continue;
        }
        if (strcmp(rankStr, "A") != 0 && strcmp(rankStr, "B") != 0 && 
            strcmp(rankStr, "C") != 0 && strcmp(rankStr, "D") != 0) {
            cout << "Warning: Invalid rank '" << rankStr << "', skipping player " << nameStr << endl;
            continue;
        }
        bool checkedIn = (strcmp(checkInStr, "YES") == 0);
        Player* p = new Player(id, nameStr, rankStr, regTypeStr, 0, emailStr, 0, checkedIn);
        players[playerCount++] = p;
        if (checkedIn) {
            playerCheckInQueue.enqueue(p);
        }
    }
    file.close();
    cout << "Loaded " << playerCount << " players from " << filename << endl;
}

void Tournament::groupPlayersByRank() {
    // Group by both rank and registration type
    struct PlayerGroup {
        Player* players[100];
        int count;
        const char* rank;
        const char* regType;
    };
    const char* ranks[] = {"A", "B", "C", "D"};
    const char* regTypes[] = {"Early-Bird", "Wildcard", "Standard", "Last-Minute"};
    PlayerGroup groupsArr[16];
    int groupArrCount = 0;
    for (int r = 0; r < 4; ++r) {
        for (int t = 0; t < 4; ++t) {
            groupsArr[groupArrCount].count = 0;
            groupsArr[groupArrCount].rank = ranks[r];
            groupsArr[groupArrCount].regType = regTypes[t];
            ++groupArrCount;
        }
    }
    while (!playerCheckInQueue.isEmpty()) {
        Player* player = playerCheckInQueue.dequeue();
        for (int i = 0; i < 16; ++i) {
            if (strcmp(player->getRank(), groupsArr[i].rank) == 0 &&
                strcmp(player->getRegistrationType(), groupsArr[i].regType) == 0) {
                groupsArr[i].players[groupsArr[i].count++] = player;
                break;
            }
        }
    }
    int groupIndex = 0;
    for (int i = 0; i < 16 && groupIndex < 10; ++i) {
        for (int j = 0; j + 3 < groupsArr[i].count && groupIndex < 10; j += 4) {
            groups[groupIndex] = new Group(groupIndex + 1, groupsArr[i].rank, groupsArr[i].regType); // pass regType
            for (int k = 0; k < 4; ++k) {
                groups[groupIndex]->addPlayer(groupsArr[i].players[j + k]);
            }
            groupIndex++;
        }
    }
    groupCount = groupIndex;
    groupsCreated = true;
    cout << "\nGroups created: " << groupCount << endl;
    for (int i = 0; i < groupCount; i++) {
        cout << "Group " << groups[i]->getId() << " (Rank " << groups[i]->getRankType() 
             << ", Registration Type: " << groups[i]->getRegistrationType() << ") - " << groups[i]->getPlayerCount() << " players" << endl;
    }
}

void Tournament::saveMatchesToCSV(const char* filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return;
    }
    
    file << "match_id,stage,group_id,round,player1_id,player2_id,scheduled_time,status,winner_id,score\n";
    for (int i = 0; i < matchCount; i++) {
        Match* match = matches[i];
        file << match->getId() << "," << match->getStage() << "," << match->getGroupId() << ","
             << match->getRound() << "," << match->getPlayer1()->getId() << ","
             << match->getPlayer2()->getId() << "," << match->getScheduledTime() << ","
             << match->getStatus() << ",";
        if (match->getWinner() != nullptr) {
            file << match->getWinner()->getId();
        }
        file << "," << match->getScore() << "\n";
    }
    file.close();
}

void Tournament::saveBracketsToCSV(const char* filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return;
    }
    
    file << "bracket_id,stage,player_id,group_id,rank,status\n";
    int bracketId = 1;
    for (int i = 0; i < playerCount; i++) {
        if (players[i]->isCheckedIn() && players[i]->getGroupId() > 0) {
            file << bracketId++ << ",group," << players[i]->getId() << "," 
                 << players[i]->getGroupId() << "," << players[i]->getRank() << ",active\n";
        }
    }
    for (int i = 0; i < groupWinnerCount; i++) {
        file << bracketId++ << ",knockout," << groupWinners[i]->getId() << "," 
             << groupWinners[i]->getGroupId() << "," << groupWinners[i]->getRank() << ",advanced\n";
    }
    file.close();
}

void Tournament::initialize(const char* playerFilename) {
    loadPlayersFromCSV(playerFilename);
    groupPlayersByRank();
    cout << "Tournament initialized with players and check-in data." << endl;
}

void Tournament::displayCheckInStatus() {
    cout << "\n===== CHECK-IN STATUS =====\n";
    int checkedInA = 0, checkedInB = 0, checkedInC = 0, checkedInD = 0;
    int totalA = 0, totalB = 0, totalC = 0, totalD = 0;
    
    for (int i = 0; i < playerCount; i++) {
        if (strcmp(players[i]->getRank(), "A") == 0) {
            totalA++;
            if (players[i]->isCheckedIn()) checkedInA++;
        } else if (strcmp(players[i]->getRank(), "B") == 0) {
            totalB++;
            if (players[i]->isCheckedIn()) checkedInB++;
        } else if (strcmp(players[i]->getRank(), "C") == 0) {
            totalC++;
            if (players[i]->isCheckedIn()) checkedInC++;
        } else if (strcmp(players[i]->getRank(), "D") == 0) {
            totalD++;
            if (players[i]->isCheckedIn()) checkedInD++;
        }
    }
    
    cout << "\nRANK A PLAYERS:\n";
    for (int i = 0; i < playerCount; i++) {
        if (strcmp(players[i]->getRank(), "A") == 0) {
            cout << players[i]->getName() << " - ";
            if (players[i]->isCheckedIn()) {
                cout << "CHECKED IN at " << players[i]->getCheckInTime();
            } else {
                cout << "NOT CHECKED IN";
            }
            cout << endl;
        }
    }
    
    cout << "\nRANK B PLAYERS:\n";
    for (int i = 0; i < playerCount; i++) {
        if (strcmp(players[i]->getRank(), "B") == 0) {
            cout << players[i]->getName() << " - ";
            if (players[i]->isCheckedIn()) {
                cout << "CHECKED IN at " << players[i]->getCheckInTime();
            } else {
                cout << "NOT CHECKED IN";
            }
            cout << endl;
        }
    }
    
    cout << "\nRANK C PLAYERS:\n";
    for (int i = 0; i < playerCount; i++) {
        if (strcmp(players[i]->getRank(), "C") == 0) {
            cout << players[i]->getName() << " - ";
            if (players[i]->isCheckedIn()) {
                cout << "CHECKED IN at " << players[i]->getCheckInTime();
            } else {
                cout << "NOT CHECKED IN";
            }
            cout << endl;
        }
    }
    
    cout << "\nRANK D PLAYERS:\n";
    for (int i = 0; i < playerCount; i++) {
        if (strcmp(players[i]->getRank(), "D") == 0) {
            cout << players[i]->getName() << " - ";
            if (players[i]->isCheckedIn()) {
                cout << "CHECKED IN at " << players[i]->getCheckInTime();
            } else {
                cout << "NOT CHECKED IN";
            }
            cout << endl;
        }
    }
    
    cout << "\nSUMMARY:\n";
    cout << "Rank A: " << checkedInA << "/" << totalA << " checked in" << endl;
    cout << "Rank B: " << checkedInB << "/" << totalB << " checked in" << endl;
    cout << "Rank C: " << checkedInC << "/" << totalC << " checked in" << endl;
    cout << "Rank D: " << checkedInD << "/" << totalD << " checked in" << endl;
}

void Tournament::createGroupSemifinals() {
    if (groupSemifinalsCreated) {
        cout << "Group semifinals already created." << endl;
        return;
    }
    
    if (!groupsCreated) {
        cout << "Error: Groups not created. Please initialize tournament first." << endl;
        return;
    }
    
    if (groupCount == 0) {
        cout << "Error: No groups available. Check if enough players checked in." << endl;
        return;
    }
    
    for (int i = 0; i < groupCount; i++) {
        groups[i]->createSemifinalsOnly(nextMatchId);
        for (int j = 0; j < 2; j++) {
            Match* match = groups[i]->getMatch(j);
            if (match != nullptr) {
                matches[matchCount++] = match;
                upcomingMatches.enqueue(match);
            }
        }
    }
    
    groupSemifinalsCreated = true;
    saveMatchesToCSV("matches.csv");
    saveBracketsToCSV("brackets.csv");
    cout << "Created group stage semifinal matches for " << groupCount << " groups." << endl;
}

Match* Tournament::getNextMatch() {
    if (upcomingMatches.isEmpty()) {
        return nullptr;
    }
    return upcomingMatches.dequeue();
}

void Tournament::updateMatchResult(Match* match, Player* winner) {
    if (match == nullptr || winner == nullptr) {
        return;
    }
    
    match->setWinner(winner);
    totalMatchesPlayed++;
    
    if (strcmp(match->getStage(), "group") == 0 && match->getRound() == 1) {
        int groupId = match->getGroupId();
        Group* group = groups[groupId - 1];
        group->incrementSemiFinalsCompleted();
        
        if (group->areSemifinalsComplete()) {
            Player* winner1 = group->getMatch(0)->getWinner();
            Player* winner2 = group->getMatch(1)->getWinner();
            group->createFinalMatch(nextMatchId, winner1, winner2);
            Match* finalMatch = group->getMatch(2);
            matches[matchCount++] = finalMatch;
            upcomingMatches.enqueue(finalMatch);
        }
    } else if (strcmp(match->getStage(), "group") == 0 && match->getRound() == 2) {
        int groupId = match->getGroupId();
        Group* group = groups[groupId - 1];
        group->setGroupWinner(winner);
        winner->advanceStage();
        if (groupWinnerCount < maxGroupWinners) {
            groupWinners[groupWinnerCount++] = winner;
        }
        cout << "Group " << groupId << " (Rank " << group->getRankType() 
             << ") completed! Winner: " << winner->getName() << endl;
        
        bool allGroupsComplete = true;
        for (int i = 0; i < groupCount; i++) {
            if (!groups[i]->isCompleted()) {
                allGroupsComplete = false;
                break;
            }
        }
        
        if (allGroupsComplete) {
            cout << "\nAll groups completed! Creating knockout stage matches..." << endl;
            createKnockoutMatches();
        }
    } else if (strcmp(match->getStage(), "knockout") == 0 && match->getRound() == 1) {
        bool bothSemifinalsCompleted = true;
        Player* semifinal1Winner = nullptr;
        Player* semifinal2Winner = nullptr;
        
        for (int i = 0; i < matchCount; i++) {
            if (strcmp(matches[i]->getStage(), "knockout") == 0 && matches[i]->getRound() == 1) {
                if (strcmp(matches[i]->getStatus(), "completed") != 0) {
                    bothSemifinalsCompleted = false;
                    break;
                }
                if (semifinal1Winner == nullptr) {
                    semifinal1Winner = matches[i]->getWinner();
                } else {
                    semifinal2Winner = matches[i]->getWinner();
                }
            }
        }
        
        if (bothSemifinalsCompleted && semifinal1Winner != nullptr && semifinal2Winner != nullptr) {
            createFinalMatch(semifinal1Winner, semifinal2Winner);
        }
    } else if (strcmp(match->getStage(), "knockout") == 0 && match->getRound() == 2) {
        cout << "\n*** TOURNAMENT CHAMPION: " << winner->getName() << " ***\n" << endl;
    }
    
    saveMatchesToCSV("matches.csv");
    saveBracketsToCSV("brackets.csv");
}

void Tournament::createKnockoutMatches() {
    if (knockoutCreated) {
        cout << "Knockout matches already created." << endl;
        return;
    }
    
    if (groupWinnerCount < 2) {
        cout << "Error: Not enough group winners for knockout stage." << endl;
        return;
    }
    
    if (groupWinnerCount == 2) {
        createFinalMatch(groupWinners[0], groupWinners[1]);
    } else if (groupWinnerCount >= 4) {
        Match* semi1 = new Match(nextMatchId++, groupWinners[0], groupWinners[1], "knockout", 0, 1);
        Match* semi2 = new Match(nextMatchId++, groupWinners[2], groupWinners[3], "knockout", 0, 1);
        matches[matchCount++] = semi1;
        matches[matchCount++] = semi2;
        upcomingMatches.enqueue(semi1);
        upcomingMatches.enqueue(semi2);
        cout << "Created knockout stage semifinal matches:\n";
        cout << "Semifinal 1: " << groupWinners[0]->getName() << " vs " << groupWinners[1]->getName() << endl;
        cout << "Semifinal 2: " << groupWinners[2]->getName() << " vs " << groupWinners[3]->getName() << endl;
    }
    
    knockoutCreated = true;
    saveMatchesToCSV("matches.csv");
}

void Tournament::createFinalMatch(Player* finalist1, Player* finalist2) {
    Match* finalMatch = new Match(nextMatchId++, finalist1, finalist2, "knockout", 0, 2);
    matches[matchCount++] = finalMatch;
    upcomingMatches.enqueue(finalMatch);
    saveMatchesToCSV("matches.csv");
    cout << "\nCreated final match: " << finalist1->getName() << " vs " << finalist2->getName() << endl;
}

void Tournament::displayStatus() {
    cout << "\n===== TOURNAMENT STATUS =====\n";
    cout << "Total Players: " << playerCount << endl;
    cout << "Active Groups: " << groupCount << endl;
    cout << "Matches Played: " << totalMatchesPlayed << endl;
    
    const char* currentStage;
    int totalExpectedMatches = groupCount * 3;
    if (totalMatchesPlayed < totalExpectedMatches) {
        currentStage = "Group Stage";
    } else if (totalMatchesPlayed < totalExpectedMatches + 2) {
        currentStage = "Knockout Semifinals";
    } else if (totalMatchesPlayed < totalExpectedMatches + 3) {
        currentStage = "Knockout Final";
    } else {
        currentStage = "Tournament Completed";
    }
    cout << "Current Stage: " << currentStage << endl;
    
    if (totalMatchesPlayed == totalExpectedMatches + 3) {
        for (int i = 0; i < matchCount; i++) {
            if (strcmp(matches[i]->getStage(), "knockout") == 0 && matches[i]->getRound() == 2 && 
                strcmp(matches[i]->getStatus(), "completed") == 0) {
                cout << "\n🏆 TOURNAMENT CHAMPION: " << matches[i]->getWinner()->getName() 
                     << " (Rank: " << matches[i]->getWinner()->getRank() << ") 🏆" << endl;
                cout << "Final Match Result: " << matches[i]->getPlayer1()->getName() 
                     << " vs " << matches[i]->getPlayer2()->getName() 
                     << " - Winner: " << matches[i]->getWinner()->getName() << endl;
                break;
            }
        }
    }
    
    for (int i = 0; i < groupCount; i++) {
        groups[i]->displayStatus();
    }
    
    bool allGroupsComplete = true;
    for (int i = 0; i < groupCount; i++) {
        if (!groups[i]->isCompleted()) {
            allGroupsComplete = false;
            break;
        }
    }
    
    if (allGroupsComplete && groupCount > 0) {
        cout << "\n----- GROUP WINNERS -----\n";
        for (int i = 0; i < groupCount; i++) {
            if (groups[i]->isCompleted()) {
                cout << "Group " << groups[i]->getId() << " (Rank " << groups[i]->getRankType() 
                     << ") Winner: " << groups[i]->getWinner()->getName() 
                     << " (Rank: " << groups[i]->getWinner()->getRank() << ")" << endl;
            }
        }
    }
    
    bool hasKnockoutResults = false;
    for (int i = 0; i < matchCount; i++) {
        if (strcmp(matches[i]->getStage(), "knockout") == 0 && strcmp(matches[i]->getStatus(), "completed") == 0) {
            if (!hasKnockoutResults) {
                cout << "\n----- KNOCKOUT RESULTS -----\n";
                hasKnockoutResults = true;
            }
            if (matches[i]->getRound() == 1) {
                cout << "Knockout Semifinal: " << matches[i]->getPlayer1()->getName() 
                     << " vs " << matches[i]->getPlayer2()->getName() 
                     << " - Winner: " << matches[i]->getWinner()->getName() << endl;
            } else if (matches[i]->getRound() == 2) {
                cout << "Knockout Final: " << matches[i]->getPlayer1()->getName() 
                     << " vs " << matches[i]->getPlayer2()->getName() 
                     << " - Winner: " << matches[i]->getWinner()->getName() << endl;
            }
        }
    }
    
    cout << "\n----- UPCOMING MATCHES -----\n";
    int upcomingCount = 0;
    MatchQueue tempQueue;
    while (!upcomingMatches.isEmpty()) {
        Match* match = upcomingMatches.dequeue();
        upcomingCount++;
        cout << "Match " << upcomingCount << ": " << match->getStage() << " ";
        if (strcmp(match->getStage(), "group") == 0) {
            cout << "Group " << match->getGroupId() << " ";
            if (match->getRound() == 1) {
                cout << "Semifinal";
            } else {
                cout << "Final";
            }
        } else {
            if (match->getRound() == 1) {
                cout << "Semifinal";
            } else {
                cout << "Final";
            }
        }
        cout << " - " << match->getPlayer1()->getName() << " vs " << match->getPlayer2()->getName() << endl;
        tempQueue.enqueue(match);
    }
    
    if (upcomingCount == 0) {
        int totalExpectedMatches = groupCount * 3;
        if (totalMatchesPlayed >= totalExpectedMatches + 3) {
            cout << "Tournament is complete! No more matches." << endl;
        } else {
            cout << "No upcoming matches scheduled yet." << endl;
        }
    }
    
    while (!tempQueue.isEmpty()) {
        upcomingMatches.enqueue(tempQueue.dequeue());
    }
}

void Tournament::runCLI_TASK1() {
    int choice;
    bool exit = false;
    while (!exit) {
        cout << "\n===== ASIA PACIFIC UNIVERSITY ESPORTS CHAMPIONSHIP =====\n";
        const char* currentStage;
        if (!groupsCreated) {
            currentStage = "Setup - Tournament not initialized";
        } else if (!groupSemifinalsCreated) {
            currentStage = "Setup - Create matches to begin";
        } else {
            int totalExpectedMatches = groupCount * 3;
            if (totalMatchesPlayed < totalExpectedMatches) {
                currentStage = "Group Stage";
            } else if (totalMatchesPlayed < totalExpectedMatches + 2) {
                currentStage = "Knockout Semifinals";
            } else if (totalMatchesPlayed < totalExpectedMatches + 3) {
                currentStage = "Knockout Final";
            } else {
                currentStage = "Tournament Completed";
            }
        }
        
        cout << "STAGE: " << currentStage << endl;
        cout << "Matches played: " << totalMatchesPlayed << endl;
        cout << "Active groups: " << groupCount << "\n\n";
        
        cout << "1. Display check-in status\n";
        cout << "2. Create group stage matches\n";
        cout << "3. Play next match\n";
        cout << "4. Display tournament status\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                if (!groupsCreated) {
                    cout << "Tournament not initialized yet." << endl;
                } else {
                    displayCheckInStatus();
                }
                break;
            case 2:
                createGroupSemifinals();
                break;
            case 3:
                if (!groupSemifinalsCreated) {
                    cout << "Please create group stage matches first." << endl;
                } else {
                    int totalExpectedMatches = groupCount * 3;
                    if (totalMatchesPlayed >= totalExpectedMatches + 3) {
                        cout << "Tournament is already complete!" << endl;
                    } else {
                        Match* match = getNextMatch();
                        if (match == nullptr) {
                            cout << "No more matches to play." << endl;
                        } else {
                            cout << "\nPlaying next match..." << endl;
                            if (strcmp(match->getStage(), "group") == 0) {
                                cout << "Group " << match->getGroupId() << " ";
                                if (match->getRound() == 1) {
                                    cout << "Semifinal";
                                } else {
                                    cout << "Final";
                                }
                            } else {
                                if (match->getRound() == 1) {
                                    cout << "Knockout Semifinal";
                                } else {
                                    cout << "Knockout Final";
                                }
                            }
                            cout << ": " << match->getPlayer1()->getName() << " (Rank: " << match->getPlayer1()->getRank() << ")" 
                                 << " vs " << match->getPlayer2()->getName() << " (Rank: " << match->getPlayer2()->getRank() << ")" << endl;
                            
                            int random = rand() % 2;
                            Player* winner = (random == 0) ? match->getPlayer1() : match->getPlayer2();
                            updateMatchResult(match, winner);
                            cout << "Match completed. Winner: " << winner->getName() << " (Rank: " << winner->getRank() << ")" << endl;
                            
                            bool allGroupsComplete = true;
                            for (int i = 0; i < groupCount; i++) {
                                if (!groups[i]->isCompleted()) {
                                    allGroupsComplete = false;
                                    break;
                                }
                            }
                            
                            if (allGroupsComplete && groupCount > 0 && totalMatchesPlayed == groupCount * 3) {
                                cout << "\n===================================================" << endl;
                                cout << "*** GROUP STAGE COMPLETE! MOVING TO SEMIFINALS ***" << endl;
                                cout << "===================================================" << endl;
                                displayStatus();
                            } else if (totalMatchesPlayed == groupCount * 3 + 2) {
                                cout << "\n===============================================" << endl;
                                cout << "*** SEMIFINALS COMPLETE! MOVING TO FINAL ***" << endl;
                                cout << "===============================================" << endl;
                                displayStatus();
                            } else if (totalMatchesPlayed == groupCount * 3 + 3) {
                                cout << "\n=====================================\n" << endl;
                                cout << "*** TOURNAMENT COMPLETE! ***" << endl;
                                cout << "=====================================\n" << endl;
                                displayStatus();
                            }
                        }
                    }
                }
                break;
            case 4:
                displayStatus();
                break;
            case 5:
                exit = true;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    }
}

int main() {
    srand(time(nullptr));
    Tournament tournament;
    tournament.initialize("Player_Registration.csv");
    tournament.runCLI_TASK1();
    cout << "\nTournament completed. Thank you!" << endl;
    return 0;
}