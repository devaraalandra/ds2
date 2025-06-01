// EsportsChampionship.cpp
#include "EsportsChampionship.hpp"


using namespace std;


Player::Player(int _id, const char* _name, const char* _rank, const char* _registrationType, int _ranking,
               const char* _email, int _teamID, bool _checkInStatus)
    : id(_id), wins(0), losses(0), groupId(0), registered(true), checkedIn(_checkInStatus) {
    strncpy(name, _name, sizeof(name) - 1); name[sizeof(name) - 1] = '\0';
    strncpy(rank, _rank, sizeof(rank) - 1); rank[sizeof(rank) - 1] = '\0';
    strncpy(registrationType, _registrationType, sizeof(registrationType) - 1); registrationType[sizeof(registrationType) - 1] = '\0';
    strcpy(currentStage, "group");
    checkInTime[0] = '\0';
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
const char* Player::getRegistrationType() const { return registrationType; }

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

Match* MatchQueue::peek() const {
    if (isEmpty()) return nullptr;
    return front->match;
}

bool MatchQueue::isEmpty() const { return front == nullptr; }
int MatchQueue::getSize() const { return size; }

PlayerPriorityQueue::Node::Node(Player* p) : player(p), next(nullptr) {}
PlayerPriorityQueue::PlayerPriorityQueue() : head(nullptr), size(0) {}
PlayerPriorityQueue::~PlayerPriorityQueue() {
    while (!isEmpty()) {
        dequeue();
    }
}

bool PlayerPriorityQueue::isEarlier(const char* time1, const char* time2) {
    return strcmp(time1, time2) < 0;
}

void PlayerPriorityQueue::enqueue(Player* player) {
    Node* newNode = new Node(player);
    if (isEmpty() || isEarlier(player->getCheckInTime(), head->player->getCheckInTime())) {
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
        } else { 
            head = newNode;
        }
        newNode->next = current;
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

bool PlayerPriorityQueue::isEmpty() const { return head == nullptr; }
int PlayerPriorityQueue::getSize() const { return size; }

Match::Match(int _id, Player* p1, Player* p2, const char* _stage, int _groupId, int _round)
    : id(_id), player1(p1), player2(p2), groupId(_groupId), round(_round), winner(nullptr) {
    strncpy(stage, _stage, 19); stage[19] = '\0';
    strcpy(status, "scheduled");
    strcpy(score, "0-0");
    time_t now_val = time(0);
    tm* ltm_val = localtime(&now_val);
    char buffer[20];
    strftime(buffer, 20, "%Y-%m-%d %H:%M", ltm_val);
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

Group::Group(int _id, const char* _rankType, const char* _registrationType) : id(_id), playerCount(0), matchCount(0),
    winner(nullptr), completed(false), semiFinalsCompleted(0) {
    strncpy(rankType, _rankType, sizeof(rankType) - 1); rankType[sizeof(rankType) - 1] = '\0';
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
const char* Group::getRegistrationType() const { return registrationType; }
int Group::getPlayerCount() const { return playerCount; }
Player* Group::getPlayer(int index) const { if(index >= 0 && index < playerCount) return players[index]; return nullptr;}


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

int Group::getMatchCount() const { return matchCount; }
void Group::incrementSemiFinalsCompleted() { semiFinalsCompleted++; }
bool Group::areSemifinalsComplete() { return semiFinalsCompleted >= 2; }
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

bool Tournament::areGroupsCreated() const { return groupsCreated; }

void Tournament::loadPlayersFromCSV(const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return;
    }
    char line[512];
    file.getline(line, 512); 
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
        if (idStr[0] == '\0') { cout << "Warning: Empty ID, skipping line." << endl; continue; }
        try { id = atoi(idStr); }
        catch (...) { cout << "Warning: Invalid ID '" << idStr << "', skipping line." << endl; continue; }
        if (id == 0 && idStr[0] != '0') { cout << "Warning: Invalid ID conversion for '" << idStr << "', skipping line." << endl; continue; }

        if (strcmp(rankStr, "A") != 0 && strcmp(rankStr, "B") != 0 &&
            strcmp(rankStr, "C") != 0 && strcmp(rankStr, "D") != 0) {
            cout << "Warning: Invalid rank '" << rankStr << "', skipping player " << nameStr << endl;
            continue;
        }
        bool checkedIn = (strcmp(checkInStr, "YES") == 0);
        Player* p = new Player(id, nameStr, rankStr, regTypeStr, 0, emailStr, 0, checkedIn);
        players[playerCount++] = p;
        if (checkedIn) {
            if (p->getCheckInTime()[0] == '\0') { 
                time_t now_ct = time(0);
                tm* ltm_ct = localtime(&now_ct);
                char buffer_ct[20];
                strftime(buffer_ct, 20, "%Y-%m-%d %H:%M", ltm_ct);
                p->setCheckIn(true, buffer_ct);
            }
            playerCheckInQueue.enqueue(p);
        }
    }
    file.close();
    cout << "Loaded " << playerCount << " players from " << filename << endl;
}

void Tournament::groupPlayersByRank() {
    struct PlayerGroupTemp {
        Player* players[100];
        int count;
        const char* rank;
        const char* regType;
    };
    const char* ranks[] = {"A", "B", "C", "D"};
    const char* regTypes[] = {"Early-Bird", "Wildcard", "Standard", "Last-Minute"}; 
    PlayerGroupTemp groupsArr[16]; 
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
        bool assigned = false;
        for (int i = 0; i < 16; ++i) {
            if (strcmp(player->getRank(), groupsArr[i].rank) == 0 &&
                strcmp(player->getRegistrationType(), groupsArr[i].regType) == 0) {
                if(groupsArr[i].count < 100) {
                    groupsArr[i].players[groupsArr[i].count++] = player;
                }
                assigned = true;
                break;
            }
        }
        if (!assigned) {  }
    }

    int groupIndex = 0;
    for (int i = 0; i < 16 && groupIndex < 10; ++i) { 
        for (int j = 0; (j + 3) < groupsArr[i].count && groupIndex < 10; j += 4) {
            groups[groupIndex] = new Group(groupIndex + 1, groupsArr[i].rank, groupsArr[i].regType);
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
    if (!file.is_open()) { cout << "Error: Could not open file " << filename << endl; return; }

    file << "match_id,stage,group_id,round,player1_id,player2_id,scheduled_time,status,winner_id,score\n";
    for (int i = 0; i < matchCount; i++) {
        Match* match = matches[i];
        if (!match) continue;
        file << match->getId() << "," << match->getStage() << "," << match->getGroupId() << ","
             << match->getRound() << "," << match->getPlayer1()->getId() << ","
             << match->getPlayer2()->getId() << "," << match->getScheduledTime() << ","
             << match->getStatus() << ",";
        if (match->getWinner() != nullptr) { file << match->getWinner()->getId(); }
        file << "," << match->getScore() << "\n";
    }
    file.close();
}

void Tournament::saveBracketsToCSV(const char* filename) {
    ofstream file(filename);
    if (!file.is_open()) { cout << "Error: Could not open file " << filename << endl; return; }

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
    int checkedInRanks[4] = {0}; int totalRanks[4] = {0};
    const char* rankChars[] = {"A", "B", "C", "D"};

    for (int i = 0; i < playerCount; i++) {
        for(int r=0; r<4; ++r) {
            if (strcmp(players[i]->getRank(), rankChars[r]) == 0) {
                totalRanks[r]++;
                if (players[i]->isCheckedIn()) checkedInRanks[r]++;
                break;
            }
        }
    }

    for(int r=0; r<4; ++r) {
        cout << "\nRANK " << rankChars[r] << " PLAYERS:\n";
        for (int i = 0; i < playerCount; i++) {
            if (strcmp(players[i]->getRank(), rankChars[r]) == 0) {
                cout << players[i]->getName() << " - ";
                if (players[i]->isCheckedIn()) {
                    cout << "CHECKED IN at " << players[i]->getCheckInTime();
                } else {
                    cout << "NOT CHECKED IN";
                }
                cout << endl;
            }
        }
    }

    cout << "\nSUMMARY:\n";
    for(int r=0; r<4; ++r) {
        cout << "Rank " << rankChars[r] << ": " << checkedInRanks[r] << "/" << totalRanks[r] << " checked in" << endl;
    }
}

void Tournament::createGroupSemifinals() {
    if (groupSemifinalsCreated) { cout << "Group semifinals already created." << endl; return; }
    if (!groupsCreated) { cout << "Error: Groups not created. Please initialize tournament first." << endl; return; }
    if (groupCount == 0) { cout << "Error: No groups available. Check if enough players checked in." << endl; return; }

    for (int i = 0; i < groupCount; i++) {
        groups[i]->createSemifinalsOnly(nextMatchId);
        for (int j = 0; j < 2; j++) { 
            Match* match = groups[i]->getMatch(j);
            if (match != nullptr && matchCount < maxMatches) {
                matches[matchCount++] = match; 
                upcomingMatches.enqueue(match);
            } else if (matchCount >= maxMatches) {
                cout << "Warning: Max matches limit reached, cannot add more group semifinal matches." << endl;
            }
        }
    }
    groupSemifinalsCreated = true;
    saveMatchesToCSV("matches.csv");
    saveBracketsToCSV("brackets.csv");
    cout << "Created group stage semifinal matches for " << groupCount << " groups." << endl;
}

Match* Tournament::getNextMatch() {
    if (upcomingMatches.isEmpty()) { return nullptr; }
    return upcomingMatches.dequeue();
}

void Tournament::updateMatchResult(Match* match, Player* winner_player) { 
    if (match == nullptr || winner_player == nullptr) { return; }

    match->setWinner(winner_player);
    totalMatchesPlayed++;

    if (strcmp(match->getStage(), "group") == 0 && match->getRound() == 1) { 
        int groupIdVal = match->getGroupId(); 
        Group* groupPtr = groups[groupIdVal - 1]; 
        groupPtr->incrementSemiFinalsCompleted();

        if (groupPtr->areSemifinalsComplete()) {
            Player* winner1 = groupPtr->getMatch(0)->getWinner();
            Player* winner2 = groupPtr->getMatch(1)->getWinner();
            if (winner1 && winner2) {
                groupPtr->createFinalMatch(nextMatchId, winner1, winner2);
                Match* finalMatch = groupPtr->getMatch(2);
                if (finalMatch != nullptr && matchCount < maxMatches) {
                    matches[matchCount++] = finalMatch;
                    upcomingMatches.enqueue(finalMatch);
                } else if (matchCount >= maxMatches) {
                     cout << "Warning: Max matches limit reached, cannot add group final." << endl;
                }
            }
        }
    } else if (strcmp(match->getStage(), "group") == 0 && match->getRound() == 2) { 
        int groupIdVal = match->getGroupId();
        Group* groupPtr = groups[groupIdVal - 1];
        groupPtr->setGroupWinner(winner_player);
        winner_player->advanceStage();
        if (groupWinnerCount < maxGroupWinners) {
            groupWinners[groupWinnerCount++] = winner_player;
        } else {
             cout << "Warning: Max group winners limit reached." << endl;
        }
        cout << "Group " << groupIdVal << " (Rank " << groupPtr->getRankType()
             << ") completed! Winner: " << winner_player->getName() << endl;

        bool allGroupsComplete = true;
        for (int i = 0; i < groupCount; i++) {
            if (!groups[i]->isCompleted()) { allGroupsComplete = false; break; }
        }

        if (allGroupsComplete && groupCount > 0) {
            cout << "\nAll groups completed! Creating knockout stage matches..." << endl;
            createKnockoutMatches();
        }
    } else if (strcmp(match->getStage(), "knockout") == 0 && match->getRound() == 1) { 
        bool allKnockoutSemifinalsCompleted = true;
        Player* semifinalWinners[2] = {nullptr, nullptr};
        int semifinalWinnerCount = 0;

        for (int i = 0; i < matchCount; i++) {
            if (matches[i] && strcmp(matches[i]->getStage(), "knockout") == 0 && matches[i]->getRound() == 1) {
                if (strcmp(matches[i]->getStatus(), "completed") != 0) {
                    allKnockoutSemifinalsCompleted = false; break;
                } else {
                    if (semifinalWinnerCount < 2) {
                        semifinalWinners[semifinalWinnerCount++] = matches[i]->getWinner();
                    }
                }
            }
        }

        if (allKnockoutSemifinalsCompleted && semifinalWinnerCount == 2) {
             if(semifinalWinners[0] && semifinalWinners[1]) {
                createFinalMatch(semifinalWinners[0], semifinalWinners[1]);
             }
        }
    } else if (strcmp(match->getStage(), "knockout") == 0 && match->getRound() == 2) { 
        cout << "\n*** TOURNAMENT CHAMPION: " << winner_player->getName() << " ***\n" << endl;
    }

    saveMatchesToCSV("matches.csv");
    saveBracketsToCSV("brackets.csv");
}

void Tournament::createKnockoutMatches() {
    if (knockoutCreated) { cout << "Knockout matches already created." << endl; return; }
    if (groupWinnerCount < 2) {
        cout << "Error: Not enough group winners for knockout stage (" << groupWinnerCount << " found)." << endl;
        if (groupWinnerCount == 1) {
             cout << "\n*** TOURNAMENT CHAMPION (by default): " << groupWinners[0]->getName() << " ***\n" << endl;
        }
        return;
    }

    if (groupWinnerCount == 2) {
        createFinalMatch(groupWinners[0], groupWinners[1]);
    } else if (groupWinnerCount == 3) {
        cout << "Knockout for 3 players: " << groupWinners[0]->getName() << " vs " << groupWinners[1]->getName() << ". Winner plays " << groupWinners[2]->getName() << endl;
        Match* semi1 = new Match(nextMatchId++, groupWinners[0], groupWinners[1], "knockout", 0, 1);
        if (matchCount < maxMatches) matches[matchCount++] = semi1; else cout << "Max matches reached.\n";
        upcomingMatches.enqueue(semi1);
        
    } else if (groupWinnerCount >= 4) {
        Match* semi1 = new Match(nextMatchId++, groupWinners[0], groupWinners[1], "knockout", 0, 1);
        Match* semi2 = new Match(nextMatchId++, groupWinners[2], groupWinners[3], "knockout", 0, 1);
        if (matchCount < maxMatches -1 ) {
             matches[matchCount++] = semi1;
             matches[matchCount++] = semi2;
        } else {
             cout << "Warning: Max matches limit reached, cannot create all knockout semifinals." << endl;
        }
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
    if (matchCount < maxMatches) {
        matches[matchCount++] = finalMatch;
    } else {
        cout << "Warning: Max matches limit reached, cannot create final match." << endl;
        delete finalMatch;
        return;
    }
    upcomingMatches.enqueue(finalMatch);
    saveMatchesToCSV("matches.csv");
    cout << "\nCreated final match: " << finalist1->getName() << " vs " << finalist2->getName() << endl;
}

void Tournament::displayStatus() {
    cout << "\n===== TOURNAMENT STATUS =====\n";
    cout << "Total Players: " << playerCount << endl;
    cout << "Active Groups: " << groupCount << endl;
    cout << "Matches Played: " << totalMatchesPlayed << endl;

    const char* currentStageStrVal; 
    int expectedGroupMatches = groupCount * 3;
    int expectedKnockoutMatches = 0;
    if (groupWinnerCount >= 4) expectedKnockoutMatches = 3;
    else if (groupWinnerCount >= 2) expectedKnockoutMatches = 1;

    if (!groupsCreated) currentStageStrVal = "Not Initialized";
    else if (totalMatchesPlayed < expectedGroupMatches) currentStageStrVal = "Group Stage";
    else if (knockoutCreated && expectedKnockoutMatches == 3 && totalMatchesPlayed < expectedGroupMatches + 2) currentStageStrVal = "Knockout Semifinals";
    else if (knockoutCreated && totalMatchesPlayed < expectedGroupMatches + expectedKnockoutMatches) currentStageStrVal = "Knockout Final";
    else if (totalMatchesPlayed >= expectedGroupMatches + expectedKnockoutMatches && knockoutCreated) currentStageStrVal = "Tournament Completed";
    else currentStageStrVal = "Awaiting Next Stage";
    cout << "Current Stage: " << currentStageStrVal << endl;

    if (strcmp(currentStageStrVal, "Tournament Completed") == 0) {
        for (int i = 0; i < matchCount; i++) {
            if (matches[i] && strcmp(matches[i]->getStage(), "knockout") == 0 && matches[i]->getRound() == 2 &&
                strcmp(matches[i]->getStatus(), "completed") == 0) {
                cout << "\n🏆 TOURNAMENT CHAMPION: " << matches[i]->getWinner()->getName()
                     << " (Rank: " << matches[i]->getWinner()->getRank() << ") 🏆" << endl;
                break;
            }
        }
    }

    for (int i = 0; i < groupCount; i++) { groups[i]->displayStatus(); }

    bool allGroupsAreDone = true; 
    if (groupCount == 0 && !groupsCreated) allGroupsAreDone = false;
    else if (groupCount > 0) {
        for (int i = 0; i < groupCount; i++) {
            if (!groups[i]->isCompleted()) { allGroupsAreDone = false; break; }
        }
    }


    if (allGroupsAreDone && groupCount > 0) {
        cout << "\n----- GROUP WINNERS -----\n";
        for (int i = 0; i < groupWinnerCount; i++) {
             if (groupWinners[i]) {
                cout << "Group " << groupWinners[i]->getGroupId()
                     << " Winner: " << groupWinners[i]->getName()
                     << " (Rank: " << groupWinners[i]->getRank() << ")" << endl;
            }
        }
    }

    bool hasKnockoutResults = false;
    for (int i = 0; i < matchCount; i++) {
        if (matches[i] && strcmp(matches[i]->getStage(), "knockout") == 0 && strcmp(matches[i]->getStatus(), "completed") == 0) {
            if (!hasKnockoutResults) {
                cout << "\n----- KNOCKOUT RESULTS -----\n"; hasKnockoutResults = true;
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
    if (upcomingMatches.isEmpty()) {
         bool champFound = false;
         for (int i = 0; i < matchCount; i++) {
             if (matches[i] && strcmp(matches[i]->getStage(), "knockout") == 0 && matches[i]->getRound() == 2 && strcmp(matches[i]->getStatus(), "completed") == 0) {
                 champFound = true; break;
             }
         }
         if (champFound || (totalMatchesPlayed >= expectedGroupMatches + expectedKnockoutMatches && knockoutCreated && expectedKnockoutMatches > 0) || (expectedGroupMatches == 0 && expectedKnockoutMatches == 0 && groupsCreated) ){
            cout << "Tournament is complete! No more matches." << endl;
         } else {
            cout << "No upcoming matches scheduled yet, or waiting for prior matches to complete." << endl;
         }
    } else {
        MatchQueue tempQueue;
        int upcomingCount = 0;
        while (!upcomingMatches.isEmpty()) {
            Match* match = upcomingMatches.dequeue();
            upcomingCount++;
            cout << "Match " << upcomingCount << ": " << match->getStage() << " ";
            if (strcmp(match->getStage(), "group") == 0) {
                cout << "Group " << match->getGroupId() << " ";
                if (match->getRound() == 1) cout << "Semifinal"; else cout << "Final";
            } else {
                if (match->getRound() == 1) cout << "Semifinal"; else cout << "Final";
            }
            cout << " - " << match->getPlayer1()->getName() << " vs " << match->getPlayer2()->getName() << endl;
            tempQueue.enqueue(match);
        }
        while (!tempQueue.isEmpty()) { upcomingMatches.enqueue(tempQueue.dequeue()); }
    }
}

void Tournament::runCLI_TASK1() {
    int choice_val; 
    bool exitCLI = false;
    while (!exitCLI) {
        cout << "\n===== ASIA PACIFIC UNIVERSITY ESPORTS CHAMPIONSHIP (TASK 1) =====\n";
        const char* currentStageDisplay;
        if (!groupsCreated) currentStageDisplay = "Setup - Tournament not initialized";
        else if (!groupSemifinalsCreated) currentStageDisplay = "Setup - Create group matches";
        else {
            int expGroupMatches = groupCount * 3;
            int expKnockoutMatches = 0;
            if (groupWinnerCount >= 4) expKnockoutMatches = 3;
            else if (groupWinnerCount >= 2) expKnockoutMatches = 1;

            if (totalMatchesPlayed < expGroupMatches) currentStageDisplay = "Group Stage";
            else if (totalMatchesPlayed < expGroupMatches + (expKnockoutMatches > 0 ? expKnockoutMatches -1 : 0) && knockoutCreated ) currentStageDisplay = "Knockout Semifinals";
            else if (totalMatchesPlayed < expGroupMatches + expKnockoutMatches && knockoutCreated) currentStageDisplay = "Knockout Final";
            else if (totalMatchesPlayed >= expGroupMatches + expKnockoutMatches && knockoutCreated) currentStageDisplay = "Tournament Completed";
            else currentStageDisplay = "Transitioning or Awaiting Matches";
        }
        cout << "CURRENT STAGE: " << currentStageDisplay << endl;
        cout << "Matches played: " << totalMatchesPlayed << endl;
        cout << "Active groups: " << groupCount << "\n\n";

        cout << "Tournament Management Menu:\n";
        cout << "1. Display check-in status\n";
        cout << "2. Create group stage semifinal matches\n";
        cout << "3. Play next match (simulated)\n";
        cout << "4. Display tournament status & brackets\n";
        cout << "5. Return to Main Menu\n";
        cout << "Enter your choice: ";

        cin >> choice_val;
        if (cin.fail()) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number." << endl; choice_val = 99;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (choice_val) {
            case 1:
                if (!groupsCreated) { cout << "Tournament not initialized yet." << endl; }
                else { displayCheckInStatus(); }
                break;
            case 2:
                if (!groupsCreated) { cout << "Tournament not initialized yet." << endl; }
                else { createGroupSemifinals(); }
                break;
            case 3:
                if (!groupsCreated) { cout << "Tournament not initialized yet." << endl; }
                else if (!groupSemifinalsCreated) { cout << "Please create group stage matches first (Option 2)." << endl; }
                else {
                    int expected_total_matches = (groupCount * 3);
                    if (groupWinnerCount >= 4) expected_total_matches += 3;
                    else if (groupWinnerCount >= 2) expected_total_matches +=1;

                    if (totalMatchesPlayed >= expected_total_matches && knockoutCreated && expected_total_matches > 0) {
                        cout << "Tournament is already complete!" << endl; displayStatus();
                    } else {
                        Match* match = getNextMatch();
                        if (match == nullptr) {
                            cout << "No more matches to play, or waiting for prior results." << endl;
                        } else {
                            cout << "\nPlaying next match..." << endl;
                            cout << "Match ID: " << match->getId() << " Stage: " << match->getStage();
                            if (strcmp(match->getStage(), "group") == 0) {
                                cout << " Group " << match->getGroupId();
                                if (match->getRound() == 1) cout << " Semifinal"; else cout << " Final";
                            } else {
                                if (match->getRound() == 1) cout << " Semifinal"; else cout << " Final";
                            }
                            cout << ": " << match->getPlayer1()->getName() << " (Rank: " << match->getPlayer1()->getRank() << ")"
                                 << " vs " << match->getPlayer2()->getName() << " (Rank: " << match->getPlayer2()->getRank() << ")" << endl;

                            Player* winner = (rand() % 2 == 0) ? match->getPlayer1() : match->getPlayer2();
                            updateMatchResult(match, winner);
                            cout << "Match completed. Winner: " << winner->getName() << " (Rank: " << winner->getRank() << ")" << endl;

                            
                            bool allGrpsComplete = true;
                            if(groupCount == 0) allGrpsComplete = false;
                            for (int i = 0; i < groupCount; i++) if (!groups[i]->isCompleted()) allGrpsComplete = false;

                            int expGroupMatches_cli = groupCount * 3;
                            int expKnockoutMatches_cli = 0;
                            if(groupWinnerCount >=4) expKnockoutMatches_cli = 3;
                            else if (groupWinnerCount >=2) expKnockoutMatches_cli =1;

                            if (allGrpsComplete && groupCount > 0 && totalMatchesPlayed == expGroupMatches_cli) {
                                cout << "\n===================================================\n"
                                     << "*** GROUP STAGE COMPLETE! MOVING TO KNOCKOUTS ***\n"
                                     << "===================================================" << endl;
                                displayStatus();
                            } else if (knockoutCreated && totalMatchesPlayed == expGroupMatches_cli + (expKnockoutMatches_cli > 1 ? expKnockoutMatches_cli -1: 0) && expKnockoutMatches_cli > 1) {
                                cout << "\n===============================================\n"
                                     << "*** KNOCKOUT SEMIFINALS COMPLETE! MOVING TO FINAL ***\n"
                                     << "===============================================" << endl;
                                displayStatus();
                            } else if (knockoutCreated && totalMatchesPlayed == expGroupMatches_cli + expKnockoutMatches_cli && expKnockoutMatches_cli > 0) {
                                cout << "\n=====================================\n"
                                     << "*** TOURNAMENT COMPLETE! ***\n"
                                     << "=====================================\n" << endl;
                                displayStatus();
                            }
                        }
                    }
                }
                break;
            case 4:
                if (!groupsCreated) { cout << "Tournament not initialized yet." << endl; }
                else { displayStatus(); }
                break;
            case 5: exitCLI = true; break;
            default: cout << "Invalid choice. Please try again." << endl; break;
        }
        if (!exitCLI && choice_val != 99) {
             cout << "\n(Task 1: Press Enter to continue...)";
             
        }
    }
}


static int isYesResponse_Task2(const char* input) {
    char lower[10];
    int i;
    for (i = 0; input[i] && i < 9; i++) {
        lower[i] = tolower(input[i]);
    }
    lower[i] = '\0';
    return strcmp(lower, "yes") == 0 || strcmp(lower, "y") == 0;
}

static int fileExists_Task2(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

static int isNumeric_Task2(const char* str) {
    if (!str || !str[0]) return 0;
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}


static int getLastPlayerID_Task2(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return 1000; 
    char line[256];
    int maxID = 1000; 
    fgets(line, 256, file); 
    while (fgets(line, 256, file)) {
        if (line[0] == '\0' || line[0] == '\n') continue;
        char* token = strtok(line, ",");
        if (token && isNumeric_Task2(token)) {
            int id = atoi(token);
            if (id > maxID) maxID = id;
        }
    }
    fclose(file);
    return maxID;
}

static void sanitizeForCSV_Task2(char* dest, const char* src, int maxLen) {
    int i, j = 0;
    for (i = 0; src[i] && j < maxLen - 1; i++) {
        if (src[i] >= 32 && src[i] <= 126) { 
            dest[j++] = (src[i] == ',') ? ';' : src[i]; 
        }
    }
    dest[j] = '\0';
}

static void normalizeRegistrationType_Task2(char* dest, const char* src, int maxLen) {
    int i = 0, j = 0;
    while (src[i] == ' ' || src[i] == '\t') i++; 
    int firstWord = 1;
    while (src[i] && j < maxLen - 1) {
        if (src[i] == ' ' || src[i] == '-') {
            dest[j++] = '-'; 
            firstWord = 1;
            i++;
            while (src[i] == ' ' || src[i] == '-') i++; 
            continue;
        }
        if (firstWord) {
            dest[j++] = toupper(src[i++]);
            firstWord = 0;
        } else {
            dest[j++] = tolower(src[i++]);
        }
    }
    while (j > 0 && (dest[j-1] == ' ' || dest[j-1] == '-')) j--; 
    dest[j] = '\0';
}

static void clearInputBuffer_Task2() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void waitForEnter_Task2() {
    printf("\nPress Enter to return to the Task 2 menu...");
    clearInputBuffer_Task2(); 
}


void Task2_RegistrationQueue::init() {
    front = NULL;
    rear = NULL;
    size = 0;
}

int Task2_RegistrationQueue::isEmpty() {
    return front == NULL;
}

void Task2_RegistrationQueue::enqueue(struct Task2_PlayerNode* player) {
    player->next = NULL;
    if (isEmpty()) {
        front = rear = player;
    } else {
        rear->next = player;
        rear = player;
    }
    size++;
}

struct Task2_PlayerNode* Task2_RegistrationQueue::dequeue() {
    if (isEmpty()) return NULL;
    struct Task2_PlayerNode* temp = front;
    front = front->next;
    if (front == NULL) rear = NULL;
    size--;
    return temp;
}

struct Task2_PlayerNode* Task2_RegistrationQueue::peek() {
    return front;
}

int Task2_RegistrationQueue::getSize() {
    return size;
}

void Task2_RegistrationQueue::destroy() {
    while (!isEmpty()) {
        struct Task2_PlayerNode* temp = dequeue();
        free(temp);
    }
}


void Task2_RegistrationPriorityQueue::init() {
    earlyBirdQueue.init();
    wildcardQueue.init();
    standardQueue.init();
    lastMinuteQueue.init();
    waitlistQueue.init();
    size = 0;
}

void Task2_RegistrationPriorityQueue::enqueue(struct Task2_PlayerNode* player) {
    if (strcmp(player->registrationType, "Early-Bird") == 0) {
        earlyBirdQueue.enqueue(player);
    } else if (strcmp(player->registrationType, "Wildcard") == 0) {
        wildcardQueue.enqueue(player);
    } else if (strcmp(player->registrationType, "Standard") == 0) {
        standardQueue.enqueue(player);
    } else if (strcmp(player->registrationType, "Last-Minute") == 0) {
        lastMinuteQueue.enqueue(player);
    } else {
        standardQueue.enqueue(player); 
    }
    size++;
}

struct Task2_PlayerNode* Task2_RegistrationPriorityQueue::dequeue() {
    struct Task2_PlayerNode* player = NULL;
    if (!earlyBirdQueue.isEmpty()) {
        player = earlyBirdQueue.dequeue();
    } else if (!wildcardQueue.isEmpty()) {
        player = wildcardQueue.dequeue();
    } else if (!standardQueue.isEmpty()) {
        player = standardQueue.dequeue();
    } else if (!lastMinuteQueue.isEmpty()) {
        player = lastMinuteQueue.dequeue();
    }
    if (player != NULL) size--;
    return player;
}

int Task2_RegistrationPriorityQueue::isEmpty() {
    return earlyBirdQueue.isEmpty() && wildcardQueue.isEmpty() &&
           standardQueue.isEmpty() && lastMinuteQueue.isEmpty();
}

int Task2_RegistrationPriorityQueue::getSize() {
    return size;
}

struct Task2_RegistrationQueue* Task2_RegistrationPriorityQueue::getWaitlistQueue() {
    return &waitlistQueue;
}

void Task2_RegistrationPriorityQueue::setWaitlistQueue(struct Task2_RegistrationQueue* q) {
    waitlistQueue.destroy(); 
    waitlistQueue.init();
    struct Task2_PlayerNode* current = q->front;
    while (current) {
        struct Task2_PlayerNode* newPlayer = (struct Task2_PlayerNode*)malloc(sizeof(struct Task2_PlayerNode));
        memcpy(newPlayer, current, sizeof(struct Task2_PlayerNode));
        newPlayer->next = NULL; 
        waitlistQueue.enqueue(newPlayer);
        current = current->next;
    }
    
    while (!q->isEmpty()) {
        struct Task2_PlayerNode* temp = q->dequeue();
        free(temp);
    }
}

void readPlayersFromCSV_Task2(struct Task2_RegistrationPriorityQueue* pq, const char* filename) {
    if (!fileExists_Task2(filename)) {
        printf("Task 2: CSV file '%s' not found. Starting with empty registration list.\n", filename);
        return;
    }
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Task 2: Error opening CSV file '%s'.\n", filename);
        return;
    }

    char line[256];
    if (!fgets(line, 256, file)) { 
        printf("Task 2: Warning: CSV file '%s' is empty or header missing.\n", filename);
        fclose(file);
        return;
    }

    while (fgets(line, 256, file)) {
        if (line[0] == '\n' || line[0] == '\0') continue; 
        
        struct Task2_PlayerNode* player = (struct Task2_PlayerNode*)malloc(sizeof(struct Task2_PlayerNode));
        
        player->ranking = 0; 
        player->teamID = 0;
        player->checkInStatus = 0;
        player->group = 0;
        player->next = NULL;
        strcpy(player->checkInTime, "N/A");
        player->rank[0] = '\0'; player->rank[1] = '\0';
        player->playerID[0] = '\0';
        player->playerName[0] = '\0';
        player->registrationType[0] = '\0';
        player->email[0] = '\0';
        strcpy(player->status, "MAIN"); 
        
        char* token = strtok(line, ",");
        int field = 0;
        while (token && field < 7) { 
            int len = strlen(token);
            if (len > 0 && token[len-1] == '\n') token[len-1] = '\0'; 

            switch (field) {
                case 0: strncpy(player->playerID, token, 9); player->playerID[9] = '\0'; break;
                case 1: strncpy(player->playerName, token, 49); player->playerName[49] = '\0'; break;
                case 2: strncpy(player->registrationType, token, 19); player->registrationType[19] = '\0'; break;
                case 3: strncpy(player->email, token, 99); player->email[99] = '\0'; break;
                case 4: if (token[0]) player->rank[0] = token[0]; player->rank[1] = '\0'; break;
                case 5: player->checkInStatus = (strcmp(token, "YES") == 0); break;
                case 6: 
                        if (token[0]) {
                            strncpy(player->checkInTime, token, 49); 
                            player->checkInTime[49] = '\0';
                        }
                        break;
            }
            token = strtok(NULL, ",");
            field++;
        }
        if (token && token[0] != '\n' && token[0] != '\0') { 
            int len = strlen(token);
            if (len > 0 && token[len-1] == '\n') token[len-1] = '\0';
            strncpy(player->status, token, 9); player->status[9] = '\0';
        }


        int valid = 1;
        if (!isNumeric_Task2(player->playerID) || atoi(player->playerID) < 1000) valid = 0;
        if (!player->playerName[0]) valid = 0;
        if (!player->registrationType[0]) valid = 0;
        if (!player->email[0]) valid = 0;

        if (player->rank[0] == '\0') { 
            if (strcmp(player->registrationType, "Early-Bird") == 0) player->rank[0] = 'A';
            else if (strcmp(player->registrationType, "Wildcard") == 0) player->rank[0] = 'B';
            else if (strcmp(player->registrationType, "Standard") == 0) player->rank[0] = 'C';
            else if (strcmp(player->registrationType, "Last-Minute") == 0) player->rank[0] = 'D';
            player->rank[1] = '\0';
        }

        if (valid) {
            if (strcmp(player->status, "WAITLIST") == 0) {
                pq->getWaitlistQueue()->enqueue(player);
            } else {
                pq->enqueue(player);
            }
        } else {
            printf("Task 2: Invalid player data in CSV: ID %s, Name %s. Skipping.\n", player->playerID, player->playerName);
            free(player);
        }
    }
    fclose(file);
    printf("Task 2: Player data loaded from %s. Main queue: %d, Waitlist: %d\n",
           filename, pq->getSize(), pq->getWaitlistQueue()->getSize());
}

void writePlayersToCSV_Task2(struct Task2_RegistrationPriorityQueue* pq, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Task 2: Error opening CSV file '%s' for writing!\n", filename);
        return;
    }

    fprintf(file, "Player ID,Player Name,Registration Type,Email,Rank,Check-In,Time of Check-In,Status\n");
    
    Task2_RegistrationPriorityQueue temp_pq;
    temp_pq.init();
    char sanitized_buffer[100]; 
    
    while (!pq->isEmpty()) {
        struct Task2_PlayerNode* player = pq->dequeue();
        if (!player->playerID[0] || !player->playerName[0]) { 
            free(player); 
            continue;
        }
        sanitizeForCSV_Task2(sanitized_buffer, player->playerID, 10);
        fprintf(file, "%s,", sanitized_buffer);
        sanitizeForCSV_Task2(sanitized_buffer, player->playerName, 50);
        fprintf(file, "%s,", sanitized_buffer);
        sanitizeForCSV_Task2(sanitized_buffer, player->registrationType, 20);
        fprintf(file, "%s,", sanitized_buffer);
        sanitizeForCSV_Task2(sanitized_buffer, player->email, 100);
        fprintf(file, "%s,", sanitized_buffer);
        fprintf(file, "%c,%s,", player->rank[0], player->checkInStatus ? "YES" : "NO");
        sanitizeForCSV_Task2(sanitized_buffer, player->checkInTime, 50);
        fprintf(file, "%s,", sanitized_buffer);
        sanitizeForCSV_Task2(sanitized_buffer, player->status, 10);
        fprintf(file, "%s\n", sanitized_buffer);
        temp_pq.enqueue(player); 
    }
    
    while (!temp_pq.isEmpty()) {
        pq->enqueue(temp_pq.dequeue());
    }
    
    struct Task2_RegistrationQueue* waitlist = pq->getWaitlistQueue();
    struct Task2_RegistrationQueue temp_waitlist;
    temp_waitlist.init();
    
    while (!waitlist->isEmpty()) {
        struct Task2_PlayerNode* player = waitlist->dequeue();
        if (!player->playerID[0] || !player->playerName[0]) {
            free(player);
            continue;
        }
        sanitizeForCSV_Task2(sanitized_buffer, player->playerID, 10);
        fprintf(file, "%s,", sanitized_buffer);
        sanitizeForCSV_Task2(sanitized_buffer, player->playerName, 50);
        fprintf(file, "%s,", sanitized_buffer);
        sanitizeForCSV_Task2(sanitized_buffer, player->registrationType, 20);
        fprintf(file, "%s,", sanitized_buffer);
        sanitizeForCSV_Task2(sanitized_buffer, player->email, 100);
        fprintf(file, "%s,", sanitized_buffer);
        fprintf(file, "%c,%s,", player->rank[0], player->checkInStatus ? "YES" : "NO");
        sanitizeForCSV_Task2(sanitized_buffer, player->checkInTime, 50);
        fprintf(file, "%s,", sanitized_buffer);
        sanitizeForCSV_Task2(sanitized_buffer, player->status, 10);
        fprintf(file, "%s\n", sanitized_buffer);
        temp_waitlist.enqueue(player);
    }
    
    pq->setWaitlistQueue(&temp_waitlist); 
    fclose(file);
}

void registerPlayer_Task2(struct Task2_RegistrationPriorityQueue* pq, const char* filename) {
    struct Task2_PlayerNode* player = (struct Task2_PlayerNode*)malloc(sizeof(struct Task2_PlayerNode));
    player->ranking = 0;
    player->teamID = 0;
    player->checkInStatus = 0;
    player->group = 0;
    player->next = NULL;
    strcpy(player->checkInTime, "N/A");
    
    int lastID = getLastPlayerID_Task2(filename);
    sprintf(player->playerID, "%d", lastID + 1);
    
    printf("\n+------------------------------+\n");
    printf("|      PLAYER REGISTRATION     |\n");
    printf("+------------------------------+\n");
    printf("Player ID: %s\n", player->playerID);
    
    printf("Enter Player Name: ");
    fgets(player->playerName, 50, stdin);
    player->playerName[strcspn(player->playerName, "\n")] = '\0';
    
    printf("Enter Registration Type (Early-Bird/Standard/Wildcard/Last-Minute): ");
    char inputType[20];
    fgets(inputType, 20, stdin);
    inputType[strcspn(inputType, "\n")] = '\0';
    
    normalizeRegistrationType_Task2(player->registrationType, inputType, 20);
    
    if (strcmp(player->registrationType, "Early-Bird") == 0) {
        player->rank[0] = 'A';
    } else if (strcmp(player->registrationType, "Wildcard") == 0) {
        player->rank[0] = 'B';
    } else if (strcmp(player->registrationType, "Standard") == 0) {
        player->rank[0] = 'C';
    } else if (strcmp(player->registrationType, "Last-Minute") == 0) {
        player->rank[0] = 'D';
    } else {
        printf("Invalid registration type '%s', defaulting to 'Standard'.\n", player->registrationType);
        player->rank[0] = 'C'; 
        strcpy(player->registrationType, "Standard");
    }
    player->rank[1] = '\0';
    
    printf("Enter Email: ");
    fgets(player->email, 100, stdin);
    player->email[strcspn(player->email, "\n")] = '\0';

    int mainQueueSize = pq->getSize();
    if (mainQueueSize < 16) { 
        strcpy(player->status, "MAIN");
        pq->enqueue(player);
        printf("\nPlayer registered successfully! Rank assigned: %c. Total main queue: %d\n",
               player->rank[0], pq->getSize());
    } else {
        strcpy(player->status, "WAITLIST");
        pq->getWaitlistQueue()->enqueue(player);
        printf("\nMain queue full. Player added to waitlist! Rank assigned: %c. Total waitlist: %d\n",
               player->rank[0], pq->getWaitlistQueue()->getSize());
    }
    
    writePlayersToCSV_Task2(pq, filename);
    waitForEnter_Task2();
}

void checkInPlayer_Task2(struct Task2_RegistrationPriorityQueue* pq, const char* filename) {
    char playerID_input[10], continueCheckIn_input[10];
    int found_flag;
    struct Task2_PlayerNode* foundPlayer_ptr;

    printf("\n+------------------------------+\n");
    printf("|       PLAYER CHECK-IN         |\n");
    printf("+------------------------------+\n");
    
    while (1) {
        printf("Enter Player ID to check-in: ");
        fgets(playerID_input, 10, stdin);
        playerID_input[strcspn(playerID_input, "\n")] = '\0';
        
        while (!playerID_input[0]) {
            printf("Player ID cannot be empty. Please enter Player ID: ");
            fgets(playerID_input, 10, stdin);
            playerID_input[strcspn(playerID_input, "\n")] = '\0';
        }

        Task2_RegistrationPriorityQueue temp_pq_checkin;
        temp_pq_checkin.init();
        found_flag = 0;
        foundPlayer_ptr = NULL;

        while (!pq->isEmpty()) {
            struct Task2_PlayerNode* player = pq->dequeue();
            if (strcmp(playerID_input, player->playerID) == 0) {
                found_flag = 1;
                foundPlayer_ptr = player;
                if (!player->checkInStatus) {
                    player->checkInStatus = 1;
                    time_t now_time = time(NULL);
                    struct tm* timeinfo_val = localtime(&now_time);
                    char time_buffer[50];
                    strftime(time_buffer, 50, "%Y-%m-%d %H:%M:%S", timeinfo_val);
                    strncpy(player->checkInTime, time_buffer, 49);
                    player->checkInTime[49] = '\0';
                }
            }
            temp_pq_checkin.enqueue(player);
        }
        
        while (!temp_pq_checkin.isEmpty()) {
            pq->enqueue(temp_pq_checkin.dequeue());
        }

        if (found_flag) {
            printf("Player %s (%s) checked in successfully at %s!\n",
                   playerID_input, foundPlayer_ptr->playerName, foundPlayer_ptr->checkInTime);
            writePlayersToCSV_Task2(pq, filename);
        } else {
            printf("Player %s not found in main queue!\n", playerID_input);
        }

        printf("\nDo you have more players to check in? (yes/no): ");
        fgets(continueCheckIn_input, 10, stdin);
        continueCheckIn_input[strcspn(continueCheckIn_input, "\n")] = '\0';
        if (!isYesResponse_Task2(continueCheckIn_input)) break;
    }

    waitForEnter_Task2();
}

static void withdrawPlayerLogic_Task2(struct Task2_RegistrationPriorityQueue* pq, const char* playerID_to_withdraw, const char* filename) {
    Task2_RegistrationPriorityQueue temp_pq_withdraw;
    temp_pq_withdraw.init();
    int found_in_main = 0;
    char withdrawnRank_val[2] = ""; 
    char withdrawnName_val[50] = "";

    while (!pq->isEmpty()) {
        struct Task2_PlayerNode* player = pq->dequeue();
        if (strcmp(player->playerID, playerID_to_withdraw) == 0) {
            found_in_main = 1;
            strcpy(withdrawnRank_val, player->rank);
            strcpy(withdrawnName_val, player->playerName);
            free(player); 
        } else {
            temp_pq_withdraw.enqueue(player);
        }
    }
    
    while (!temp_pq_withdraw.isEmpty()) {
        pq->enqueue(temp_pq_withdraw.dequeue());
    }

    if (found_in_main) {
        printf("Player %s (%s) withdrawn successfully from main queue.\n", playerID_to_withdraw, withdrawnName_val);
        
        struct Task2_RegistrationQueue* waitlist_ptr = pq->getWaitlistQueue();
        struct Task2_RegistrationQueue temp_waitlist_processing;
        temp_waitlist_processing.init();
        int promoted_flag = 0;
        struct Task2_PlayerNode* promotedPlayer_ptr = NULL;
        
        
        struct Task2_PlayerNode* current_waitlist_player = waitlist_ptr->front;
        struct Task2_PlayerNode* best_match_for_promotion = NULL;

        
        while(current_waitlist_player != NULL){
            if(strcmp(current_waitlist_player->rank, withdrawnRank_val) == 0){
                if(best_match_for_promotion == NULL ){ 
                     best_match_for_promotion = current_waitlist_player;
                }
            }
            current_waitlist_player = current_waitlist_player->next;
        }


        current_waitlist_player = waitlist_ptr->dequeue(); 
        while(current_waitlist_player != NULL){
            if(!promoted_flag && best_match_for_promotion != NULL && strcmp(current_waitlist_player->playerID, best_match_for_promotion->playerID) == 0){
                promotedPlayer_ptr = current_waitlist_player;
                strcpy(promotedPlayer_ptr->status, "MAIN");
                pq->enqueue(promotedPlayer_ptr); 
                promoted_flag = 1;
                printf("Player %s (%s) promoted from waitlist to main queue.\n",
                       promotedPlayer_ptr->playerID, promotedPlayer_ptr->playerName);
            } else {
                temp_waitlist_processing.enqueue(current_waitlist_player);
            }
            current_waitlist_player = waitlist_ptr->dequeue();
        }
        
        pq->setWaitlistQueue(&temp_waitlist_processing); 
        
        if (!promoted_flag) {
            printf("No suitable player (matching rank %s) found on waitlist for promotion, or waitlist is empty.\n", withdrawnRank_val);
        }
        
        writePlayersToCSV_Task2(pq, filename);
    } else {
        printf("Player %s not found in main queue. No withdrawal action taken.\n", playerID_to_withdraw);
    }
}


void handleWithdrawPlayer_Task2(struct Task2_RegistrationPriorityQueue* pq, const char* filename) {
    char playerID_input_withdraw[10];
    
    printf("\n+------------------------------+\n");
    printf("|      PLAYER WITHDRAWAL       |\n");
    printf("+------------------------------+\n");
    printf("Enter Player ID to withdraw: ");
    fgets(playerID_input_withdraw, 10, stdin);
    playerID_input_withdraw[strcspn(playerID_input_withdraw, "\n")] = '\0';
    
    withdrawPlayerLogic_Task2(pq, playerID_input_withdraw, filename);
    waitForEnter_Task2();
}

void displayWaitlist_Task2(struct Task2_RegistrationPriorityQueue* pq) {
    printf("\n+------------------------------+\n");
    printf("|        WAITLIST PLAYERS      |\n");
    printf("+------------------------------+\n");
    
    struct Task2_RegistrationQueue* waitlist_display = pq->getWaitlistQueue();
    if (waitlist_display->isEmpty()) {
        printf("Waitlist is empty.\n");
        waitForEnter_Task2();
        return;
    }
    
    printf("%-10s %-20s %-15s %-30s %-5s\n",
           "Player ID", "Player Name", "Type", "Email", "Rank");
    printf("--------------------------------------------------------------------------\n");
    
    struct Task2_RegistrationQueue temp_waitlist_display;
    temp_waitlist_display.init();
    
    struct Task2_PlayerNode* current_node = waitlist_display->front;
    while(current_node != NULL) {
        printf("%-10s %-20s %-15s %-30s %-5c\n",
               current_node->playerID, current_node->playerName, current_node->registrationType,
               current_node->email, current_node->rank[0]);
        
        struct Task2_PlayerNode* new_copy_node = (struct Task2_PlayerNode*)malloc(sizeof(struct Task2_PlayerNode));
        memcpy(new_copy_node, current_node, sizeof(struct Task2_PlayerNode));
        new_copy_node->next = NULL;
        temp_waitlist_display.enqueue(new_copy_node);
        current_node = current_node->next;
    }
    
    
    pq->getWaitlistQueue()->destroy(); 
    *(pq->getWaitlistQueue()) = temp_waitlist_display; 


    waitForEnter_Task2();
}


void runCLI_TASK2() {
    struct Task2_RegistrationPriorityQueue task2_pq;
    task2_pq.init();
    const char* task2_csv_filename = "Player_Registration.csv";
    
    printf("\n\n--- Accessing: Player Registration & Queueing (Task 2) ---\n");
    printf("+----------------------------------------------------+\n");
    printf("|      APUEC Player Registration System (Task 2)     |\n");
    printf("+----------------------------------------------------+\n");
    printf("Loading player data for Task 2 from '%s'...\n", task2_csv_filename);
    readPlayersFromCSV_Task2(&task2_pq, task2_csv_filename);
    printf("Current number of registered players in main queue (Task 2): %d\n", task2_pq.getSize());
    printf("Current number of players in waitlist (Task 2): %d\n", task2_pq.getWaitlistQueue()->getSize());


    int task2_choice;
    do {
        printf("\n+------------------------------------------+\n");
        printf("|           Task 2: Main Menu              |\n");
        printf("+------------------------------------------+\n");
        printf("| 1. Register Player                       |\n");
        printf("| 2. Check-In Player                       |\n");
        printf("| 3. Withdraw Player                       |\n");
        printf("| 4. View Waitlist                         |\n");
        printf("| 5. Return to APUEC Main Menu             |\n");
        printf("+------------------------------------------+\n");
        printf("Enter choice for Task 2 (1-5): ");
        
        if (scanf("%d", &task2_choice) != 1) {
            clearInputBuffer_Task2(); 
            printf("Invalid input! Please enter a number between 1 and 5 for Task 2.\n");
            continue;
        }
        clearInputBuffer_Task2(); 

        switch (task2_choice) {
            case 1: registerPlayer_Task2(&task2_pq, task2_csv_filename); break;
            case 2: checkInPlayer_Task2(&task2_pq, task2_csv_filename); break;
            case 3: handleWithdrawPlayer_Task2(&task2_pq, task2_csv_filename); break;
            case 4: displayWaitlist_Task2(&task2_pq); break;
            case 5: printf("\nReturning to APUEC Main Menu from Task 2...\n"); break;
            default: printf("Invalid choice for Task 2! Please enter a number between 1 and 5.\n"); break;
        }
    } while (task2_choice != 5);

    
    task2_pq.earlyBirdQueue.destroy();
    task2_pq.wildcardQueue.destroy();
    task2_pq.standardQueue.destroy();
    task2_pq.lastMinuteQueue.destroy();
    task2_pq.waitlistQueue.destroy();
}


void runTask3_SpectatorManagement() {
    cout << "\n--- Spectator Management (Task 3) ---" << endl;
    cout << "This functionality is a placeholder." << endl;
    cout << "Returning to main menu." << endl;
}



Task4_MatchResult::Task4_MatchResult() : match_id(0), group_id(0), round(0), player1_id(0), player2_id(0), winner_id(0) {}

Task4_MatchResult::Task4_MatchResult(int mid, const std::string& st, int gid, int r, int p1, int p2,
                                   const std::string& sch_time, const std::string& stat, int wid, const std::string& scr)
    : match_id(mid), stage(st), group_id(gid), round(r), player1_id(p1), player2_id(p2),
      scheduled_time(sch_time), status(stat), winner_id(wid), score(scr) {}

Task4_Stack::Task4_Stack() : top_index(-1) {}

bool Task4_Stack::push(const Task4_MatchResult& match) {
    if (top_index >= TASK4_MAX_CAPACITY - 1) {
        std::cerr << "Warning: Task4_Stack is full\n"; return false;
    }
    data[++top_index] = match; return true;
}

bool Task4_Stack::pop(Task4_MatchResult& out) {
    if (isEmpty()) return false;
    out = data[top_index--]; return true;
}

bool Task4_Stack::peek(Task4_MatchResult& out) const {
    if (isEmpty()) return false;
    out = data[top_index]; return true;
}

bool Task4_Stack::getFromTop(int index, Task4_MatchResult& out) const {
    if (index < 0 || index > top_index) return false;
    out = data[top_index - index]; return true;
}

bool Task4_Stack::isEmpty() const { return top_index == -1; }
int Task4_Stack::size() const { return top_index + 1; }

Task4_Queue::Task4_Queue() : front_index(0), rear_index(-1), current_size(0) {}

bool Task4_Queue::enqueue(const Task4_MatchResult& match) {
    if (current_size >= TASK4_MAX_CAPACITY) {
        std::cerr << "Warning: Task4_Queue is full\n"; return false;
    }
    rear_index = (rear_index + 1) % TASK4_MAX_CAPACITY;
    data[rear_index] = match;
    current_size++; return true;
}

bool Task4_Queue::dequeue(Task4_MatchResult& out) {
    if (isEmpty()) return false;
    out = data[front_index];
    front_index = (front_index + 1) % TASK4_MAX_CAPACITY;
    current_size--; return true;
}

bool Task4_Queue::peek(Task4_MatchResult& out) const {
   if (isEmpty()) return false;
   out = data[front_index]; return true;
}

bool Task4_Queue::getAt(int index, Task4_MatchResult& out) const {
    if (index < 0 || index >= current_size) return false;
    int actual_index = (front_index + index) % TASK4_MAX_CAPACITY;
    out = data[actual_index]; return true;
}

bool Task4_Queue::isEmpty() const { return current_size == 0; }
int Task4_Queue::size() const { return current_size; }

Task4_PlayerStats::Task4_PlayerStats() : player_id(0), total_matches(0), wins(0), losses(0), avg_score(0.0) {}

Task4_PlayerStats::Task4_PlayerStats(int pid, const std::string& n, const std::string& r_val, const std::string& c, const std::string& reg_time)
    : player_id(pid), name(n), rank(r_val), contact(c), registration_time(reg_time),
      total_matches(0), wins(0), losses(0), avg_score(0.0) {} 

Task4_GameResultManager::Task4_GameResultManager(int mp)
    : task4_max_players(mp), current_player_count(0), next_match_id(1) {
    player_stats = new Task4_PlayerStats[task4_max_players];
}

Task4_GameResultManager::~Task4_GameResultManager() {
    delete[] player_stats;
}

bool Task4_GameResultManager::loadPlayerData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Cannot open players file '" << filename << "' for Task 4\n"; return false;
    }
    std::string line_str; 
    int loaded_count = 0; bool header_skipped = false;
    while (std::getline(file, line_str) && current_player_count < task4_max_players) {
        if (line_str.empty() || line_str.find_first_not_of(" \t\r\n") == std::string::npos) continue;
        if (!header_skipped) { header_skipped = true; continue; }

        std::string tokens[6]; 
        splitCSVLine(line_str, tokens, 6);

        if (tokens[0].empty()) { std::cerr << "Warning (Task 4): Empty player ID in line: " << line_str << "\n"; continue; }
        int player_id_val; 
        try { player_id_val = std::stoi(tokens[0]); }
        catch (const std::exception& e) { std::cerr << "Warning (Task 4): Invalid player ID '" << tokens[0] << "'. Error: " << e.what() << "\n"; continue; }

        if (findPlayerIndex(player_id_val) != -1) { std::cerr << "Warning (Task 4): Duplicate player ID " << player_id_val << "\n"; continue; }

        player_stats[current_player_count] = Task4_PlayerStats(player_id_val, tokens[1], tokens[4], tokens[3], tokens[2]);
        current_player_count++; loaded_count++;
    }
    file.close();
    std::cout << "Task 4: Loaded " << loaded_count << " players from " << filename << "\n";
    return true;
}

bool Task4_GameResultManager::loadMatchHistory(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Cannot open matches file '" << filename << "' for Task 4\n"; return false;
    }
    std::string line_str; int loaded_count = 0; bool header_skipped = false;
    while (std::getline(file, line_str)) {
        if (line_str.empty() || line_str.find_first_not_of(" \t\r\n") == std::string::npos) continue;
        if (!header_skipped) { header_skipped = true; continue; }

        std::string tokens[10]; 
        splitCSVLine(line_str, tokens, 10);

        if (tokens[0].empty() || tokens[4].empty() || tokens[5].empty()) { std::cerr << "Warning (Task 4): Missing required fields in match line: " << line_str << "\n"; continue; }

        int match_id_val, group_id_val = 0, round_val = 0, player1_id_val, player2_id_val, winner_id_val = 0;
        try {
            match_id_val = std::stoi(tokens[0]);
            if (!tokens[2].empty()) group_id_val = std::stoi(tokens[2]);
            if (!tokens[3].empty()) round_val = std::stoi(tokens[3]);
            player1_id_val = std::stoi(tokens[4]);
            player2_id_val = std::stoi(tokens[5]);
            if (!tokens[8].empty()) winner_id_val = std::stoi(tokens[8]);
            if (match_id_val >= next_match_id) next_match_id = match_id_val + 1;
        } catch (const std::exception& e) { std::cerr << "Warning (Task 4): Invalid numeric data in match line: " << line_str << ". Error: " << e.what() << "\n"; continue; }

        if (findPlayerIndex(player1_id_val) == -1 || findPlayerIndex(player2_id_val) == -1) { std::cerr << "Warning (Task 4): Player ID not found, skipping match\n"; continue; }

        Task4_MatchResult match_res(match_id_val, tokens[1], group_id_val, round_val, player1_id_val,
                                   player2_id_val, tokens[6], tokens[7], winner_id_val, tokens[9]);
        match_history.enqueue(match_res);
        recent_matches.push(match_res);

        if (winner_id_val != 0) { 
             
            double p1_score_from_string = 0.0;
            double p2_score_from_string = 0.0;
            size_t dash_pos = tokens[9].find('-');
            if (dash_pos != std::string::npos) {
                try { p1_score_from_string = std::stod(tokens[9].substr(0, dash_pos)); } catch(...) {}
                try { p2_score_from_string = std::stod(tokens[9].substr(dash_pos + 1)); } catch(...) {}
            }

            updatePlayerStats(player1_id_val, (winner_id_val == player1_id_val), p1_score_from_string);
            updatePlayerStats(player2_id_val, (winner_id_val == player2_id_val), p2_score_from_string);
        }
        loaded_count++;
    }
    file.close();
    std::cout << "Task 4: Loaded " << loaded_count << " matches from " << filename << "\n";
    return true;
}


void Task4_GameResultManager::displayRecentMatches(int count) {
    std::cout << "\n" << std::string(100, '=') << "\n"
              << "                                  TASK 4: RECENT MATCHES\n"
              << std::string(100, '=') << "\n";
    if (recent_matches.isEmpty()) {
        std::cout << "│" << std::left << std::setw(98) << " No recent matches available." << "│\n"
                  << std::string(100, '=') << "\n"; return;
    }
    int display_count = std::min(count, recent_matches.size());
    std::cout << "Showing last " << display_count << " matches:\n" << std::string(100, '-') << "\n";
    std::cout << "│" << std::left << std::setw(8) << " Match" << "│" << std::setw(12) << " Stage" << "│"
              << std::setw(25) << " Players" << "│" << std::setw(15) << " Winner" << "│"
              << std::setw(12) << " Score" << "│" << std::setw(12) << " Date" << "│"
              << std::setw(12) << " Status" << "│\n";
    std::cout << "│" << std::string(7, '-') << "│" << std::string(11, '-') << "│" << std::string(24, '-') << "│"
              << std::string(14, '-') << "│" << std::string(11, '-') << "│" << std::string(11, '-') << "│"
              << std::string(11, '-') << "│\n";

    for (int i = 0; i < display_count; i++) {
        Task4_MatchResult match_res;
        if (recent_matches.getFromTop(i, match_res)) {
            int p1_index = findPlayerIndex(match_res.player1_id);
            int p2_index = findPlayerIndex(match_res.player2_id);
            std::string p1_name = (p1_index != -1) ? player_stats[p1_index].name : "ID:" + std::to_string(match_res.player1_id);
            std::string p2_name = (p2_index != -1) ? player_stats[p2_index].name : "ID:" + std::to_string(match_res.player2_id);
            if (p1_name.length() > 10) p1_name = p1_name.substr(0, 9) + ".";
            if (p2_name.length() > 10) p2_name = p2_name.substr(0, 9) + ".";
            std::string players_str = p1_name + " vs " + p2_name;
            if (players_str.length() > 24) players_str = players_str.substr(0, 21) + "...";

            std::string winner_name_str = "Draw/None";
            if (match_res.winner_id != 0) {
                int winner_index = findPlayerIndex(match_res.winner_id);
                winner_name_str = (winner_index != -1) ? player_stats[winner_index].name : "ID:" + std::to_string(match_res.winner_id);
                if (winner_name_str.length() > 14) winner_name_str = winner_name_str.substr(0, 11) + "...";
            }
            std::string date_str = extractDateFromScheduledTime(match_res.scheduled_time);
            if (date_str.empty()) date_str = "Unknown";

            std::cout << "│" << std::left << std::setw(8) << (" " + std::to_string(match_res.match_id)) << "│"
                      << std::setw(12) << (" " + (match_res.stage.length() > 11 ? match_res.stage.substr(0,8)+"..." : match_res.stage)) << "│"
                      << std::setw(25) << (" " + players_str) << "│"
                      << std::setw(15) << (" " + winner_name_str) << "│"
                      << std::setw(12) << (" " + (match_res.score.length() > 11 ? match_res.score.substr(0,8)+"..." : match_res.score)) << "│"
                      << std::setw(12) << (" " + date_str) << "│"
                      << std::setw(12) << (" " + (match_res.status.length() > 11 ? match_res.status.substr(0,8)+"..." : match_res.status)) << "│\n";
        }
    }
    std::cout << std::string(100, '=') << "\n";
}

void Task4_GameResultManager::displayPlayerStats(int player_id_val) { 
    int index = findPlayerIndex(player_id_val);
    if (index == -1) {
        std::cout << "\nPlayer not found (Task 4).\n"; return;
    }
    const Task4_PlayerStats& player_stat = player_stats[index];
    std::cout << "\nPLAYER STATISTICS (Task 4)\nID: " << player_stat.player_id << "\nName: " << player_stat.name
              << "\nRank: " << player_stat.rank << "\nContact: " << player_stat.contact
              << "\nReg. Type: " << player_stat.registration_time << "\nMatches: " << player_stat.total_matches
              << "\nWins: " << player_stat.wins << "\nLosses: " << player_stat.losses;
    if (player_stat.total_matches > 0) {
        double win_rate = (static_cast<double>(player_stat.wins) / player_stat.total_matches) * 100.0;
        std::cout << "\nWin Rate: " << std::fixed << std::setprecision(1) << win_rate << "%";
    } else { std::cout << "\nWin Rate: N/A"; }
    std::cout << "\nAverage Score: " << std::fixed << std::setprecision(2) << player_stat.avg_score << std::endl;
}

void Task4_GameResultManager::displayAllPlayerStats() {
    std::cout << "\nTASK 4: ALL PLAYER STATISTICS\n";
    if (current_player_count == 0) { std::cout << "No players loaded.\n"; return; }
    std::cout << std::left << std::setw(5) << "ID" << std::setw(19) << "Name" << std::setw(13) << "Rank"
              << std::setw(11) << "Matches" << std::setw(9) << "Wins" << std::setw(11) << "Losses"
              << std::setw(13) << "Win Rate" << std::setw(13) << "Avg Score" << "\n"
              << std::string(95, '-') << "\n";
    for (int i = 0; i < current_player_count; i++) {
        const Task4_PlayerStats& ps = player_stats[i]; 
        std::string win_rate_str = "N/A";
        if (ps.total_matches > 0) {
            double wr = (static_cast<double>(ps.wins) / ps.total_matches) * 100.0; 
            std::ostringstream oss; oss << std::fixed << std::setprecision(1) << wr << "%";
            win_rate_str = oss.str();
        }
        std::ostringstream as_oss; as_oss << std::fixed << std::setprecision(2) << ps.avg_score; 
        std::cout << std::left << std::setw(5) << ps.player_id
                  << std::setw(19) << (ps.name.length()>17?ps.name.substr(0,14)+"...":ps.name)
                  << std::setw(13) << (ps.rank.length()>11?ps.rank.substr(0,8)+"...":ps.rank)
                  << std::setw(11) << ps.total_matches << std::setw(9) << ps.wins
                  << std::setw(11) << ps.losses << std::setw(13) << win_rate_str
                  << std::setw(13) << as_oss.str() << "\n";
    }
    std::cout << std::string(95, '-') << "\n";
}

void Task4_GameResultManager::queryMatchesByPlayer(int player_id_val) { 
    int player_idx = findPlayerIndex(player_id_val);
    if (player_idx == -1) { std::cout << "\nPlayer not found (Task 4).\n"; return; }
    std::cout << "\nTASK 4: MATCHES FOR PLAYER " << player_id_val << " (" << player_stats[player_idx].name << ")\n";
    bool found = false; 
    std::cout << std::left << std::setw(9) << "MatchID" << std::setw(19) << "Opponent" << std::setw(13) << "Stage"
              << std::setw(11) << "Result" << std::setw(13) << "Score" << std::setw(13) << "Date"
              << std::setw(13) << "Round" << "\n" << std::string(90, '-') << "\n";
    for (int i = 0; i < match_history.size(); i++) {
        Task4_MatchResult mi; if (match_history.getAt(i, mi)) { 
            if (mi.player1_id == player_id_val || mi.player2_id == player_id_val) {
                found = true;
                int opp_id = (mi.player1_id == player_id_val) ? mi.player2_id : mi.player1_id; 
                int opp_idx = findPlayerIndex(opp_id); 
                std::string opp_name = (opp_idx != -1) ? player_stats[opp_idx].name : "ID:" + std::to_string(opp_id); 
                if(opp_name.length() > 17) opp_name = opp_name.substr(0,14) + "...";
                std::string res_str = "DRAW"; 
                if (mi.winner_id == player_id_val) res_str = "WIN";
                else if (mi.winner_id != 0) res_str = "LOSS";
                std::string date_val = extractDateFromScheduledTime(mi.scheduled_time);
                if(date_val.empty()) date_val = "Unknown";
                std::cout << std::left << std::setw(9) << mi.match_id
                          << std::setw(19) << opp_name
                          << std::setw(13) << (mi.stage.length()>11?mi.stage.substr(0,8)+"...":mi.stage)
                          << std::setw(11) << res_str
                          << std::setw(13) << (mi.score.length()>11?mi.score.substr(0,8)+"...":mi.score)
                          << std::setw(13) << date_val
                          << std::setw(13) << mi.round << "\n";
            }
        }
    }
    if (!found) { std::cout << "No matches found for this player.\n"; }
    std::cout << std::string(90, '-') << "\n";
}

void Task4_GameResultManager::queryMatchesByStage(const std::string& stage_query) {
    std::cout << "\nTASK 4: MATCHES IN STAGE: " << stage_query << "\n";
    bool found = false; 
    std::cout << std::left << std::setw(9) << "MatchID" << std::setw(26) << "Players" << std::setw(16) << "Winner"
              << std::setw(13) << "Score" << std::setw(13) << "Round" << std::setw(13) << "Date" << "\n"
              << std::string(90, '-') << "\n";
    for (int i = 0; i < match_history.size(); i++) {
        Task4_MatchResult mr; if (match_history.getAt(i, mr)) { 
            if (mr.stage == stage_query) {
                found = true;
                int p1_idx = findPlayerIndex(mr.player1_id); 
                int p2_idx = findPlayerIndex(mr.player2_id); 
                std::string p1n = (p1_idx != -1) ? player_stats[p1_idx].name : "ID:" + std::to_string(mr.player1_id); 
                std::string p2n = (p2_idx != -1) ? player_stats[p2_idx].name : "ID:" + std::to_string(mr.player2_id); 
                if(p1n.length()>10) p1n = p1n.substr(0,9)+"."; if(p2n.length()>10) p2n = p2n.substr(0,9)+".";
                std::string players_s = p1n + " vs " + p2n; 
                if(players_s.length() > 24) players_s = players_s.substr(0,21)+"...";

                std::string win_name = "Draw/None"; 
                if (mr.winner_id != 0) {
                    int win_idx = findPlayerIndex(mr.winner_id); 
                    win_name = (win_idx != -1) ? player_stats[win_idx].name : "ID:" + std::to_string(mr.winner_id);
                    if(win_name.length() > 14) win_name = win_name.substr(0,11)+"...";
                }
                std::string date_s = extractDateFromScheduledTime(mr.scheduled_time); 
                if(date_s.empty()) date_s = "Unknown";
                std::cout << std::left << std::setw(9) << mr.match_id
                          << std::setw(26) << players_s
                          << std::setw(16) << win_name
                          << std::setw(13) << (mr.score.length()>11?mr.score.substr(0,8)+"...":mr.score)
                          << std::setw(13) << mr.round
                          << std::setw(13) << date_s << "\n";
            }
        }
    }
    if (!found) { std::cout << "No matches found for this stage.\n"; }
    std::cout << std::string(90, '-') << "\n";
}

void Task4_GameResultManager::displayMenu_Task4() {
    std::cout << "\nAPUEC MANAGEMENT SYSTEM (TASK 4)\nGame Result Logging & Performance History\n"
              << "1. Display Recent Matches (Last 5)\n"
              << "2. Display All Player Statistics\n"
              << "3. Display Specific Player Statistics\n"
              << "4. Query Matches by Player\n"
              << "5. Query Matches by Stage\n"
              << "6. Add New Match Result\n"
              << "0. Return to Main Menu\n"
              << "Enter your choice (Task 4): ";
}

void Task4_GameResultManager::addMatchResult(int match_id, const char* stage, int group_id, int round, 
                                            int player1_id, int player2_id, const char* scheduled_time, 
                                            const char* status, int winner_id, const char* score) {
    
    int p1_index = findPlayerIndex(player1_id);
    int p2_index = findPlayerIndex(player2_id);
    if (p1_index == -1 || p2_index == -1) {
        std::cerr << "Error (Task 4): Invalid player ID(s) - Player1: " << player1_id 
                  << ", Player2: " << player2_id << "\n";
        return;
    }

    
    Task4_MatchResult match;
    match.match_id = match_id;
    match.stage = stage; 
    match.group_id = group_id;
    match.round = round;
    match.player1_id = player1_id;
    match.player2_id = player2_id;
    match.scheduled_time = scheduled_time; 
    match.status = status; 
    match.winner_id = winner_id;
    match.score = score; 

    
    if (!recent_matches.push(match)) {
        std::cerr << "Error (Task 4): Failed to add match " << match_id << " to recent_matches Stack\n";
    }
    if (!match_history.enqueue(match)) {
        std::cerr << "Error (Task 4): Failed to add match " << match_id << " to match_history Queue\n";
    }

    
    if (match_id >= next_match_id) {
        next_match_id = match_id + 1;
    }

    
    if (winner_id != 0 && strcmp(status, "completed") == 0) {
        double p1_score = 0.0, p2_score = 0.0;
        
        std::string score_str(score);
        size_t dash_pos = score_str.find('-');
        if (dash_pos != std::string::npos) {
            try { 
                p1_score = std::stod(score_str.substr(0, dash_pos)); 
            } catch (...) {}
            try { 
                p2_score = std::stod(score_str.substr(dash_pos + 1)); 
            } catch (...) {}
        }
        updatePlayerStats(player1_id, (winner_id == player1_id), p1_score);
        updatePlayerStats(player2_id, (winner_id == player2_id), p2_score);
    }

    std::cout << "Task 4: Added match " << match_id << " (" << player_stats[p1_index].name 
              << " vs " << player_stats[p2_index].name << ") to results\n";
}

void Task4_GameResultManager::runProgram() {
    std::cout << "=== Task 4: Game Result System Starting ===\n";
    current_player_count = 0; 
    while(!match_history.isEmpty()) { Task4_MatchResult temp; match_history.dequeue(temp); } 
    while(!recent_matches.isEmpty()) { Task4_MatchResult temp; recent_matches.pop(temp); } 

    std::cout << "Task 4: Loading player data (players.csv)...\n";
    loadPlayerData("Player_Registration.csv"); 
    std::cout << "Task 4: Loading match history (matches.csv)...\n";
    loadMatchHistory("matches.csv");
    std::cout << "Task 4 System ready!\n";

    int choice_val; bool exit_cli = false; 
    while (!exit_cli) {
        displayMenu_Task4();
        if (!(std::cin >> choice_val)) {
            std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input (Task 4).\n"; choice_val = -1;
        } else { std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }

        switch (choice_val) {
            case 1: displayRecentMatches(); break;
            case 2: displayAllPlayerStats(); break;
            case 3: {
                std::cout << "Enter player ID (Task 4): "; int pid;
                if (std::cin >> pid) { std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); displayPlayerStats(pid); }
                else { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cout << "Invalid player ID format.\n"; }
                break;
            }
            case 4: {
                std::cout << "Enter player ID (Task 4): "; int pid;
                if (std::cin >> pid) { std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); queryMatchesByPlayer(pid); }
                else { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); std::cout << "Invalid player ID format.\n"; }
                break;
            }
            case 5: {
                std::cout << "Enter stage name (Task 4): "; std::string s_name; 
                std::getline(std::cin, s_name); queryMatchesByStage(s_name);
                break;
            }
            case 6: {
                std::cout << "Enter match details (Task 4):\n";
                int match_id, group_id, round, player1_id, player2_id, winner_id;
                char stage[20], scheduled_time[20], status[20], score[20];

                std::cout << "Match ID: "; std::cin >> match_id;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Stage (e.g., group, knockout): "; std::cin.getline(stage, sizeof(stage));
                std::cout << "Group ID (0 if none): "; std::cin >> group_id;
                std::cout << "Round: "; std::cin >> round;
                std::cout << "Player 1 ID: "; std::cin >> player1_id;
                std::cout << "Player 2 ID: "; std::cin >> player2_id;
                std::cout << "Winner ID (0 for none/draw): "; std::cin >> winner_id;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Scheduled Time (YYYY-MM-DD HH:MM): "; std::cin.getline(scheduled_time, sizeof(scheduled_time));
                std::cout << "Status (e.g., scheduled, completed): "; std::cin.getline(status, sizeof(status));
                std::cout << "Score (e.g., 1-0): "; std::cin.getline(score, sizeof(score));

                addMatchResult(match_id, stage, group_id, round, player1_id, player2_id, scheduled_time, status, winner_id, score);
                break;
            }
            case 0: exit_cli = true; std::cout << "Returning to main APUEC menu from Task 4.\n"; break;
            default: if (choice_val != -1) std::cout << "Invalid choice (Task 4).\n"; break;
        }
        if (!exit_cli && choice_val != -1) {
            std::cout << "\n(Task 4: Press Enter to return to Task 4 menu...)";
            
        }
    }
}

std::string Task4_GameResultManager::extractDateFromScheduledTime(const std::string& scheduled_time) {
    if (scheduled_time.length() < 10) return "";
    std::string date_part = scheduled_time.substr(0, 10);
    if (date_part.length() == 10 && date_part[4] == '-' && date_part[7] == '-') {
        try {
            int y = std::stoi(date_part.substr(0, 4)); int m = std::stoi(date_part.substr(5, 2)); int d = std::stoi(date_part.substr(8, 2));
            if (y >= 2000 && y <= 2100 && m >= 1 && m <= 12 && d >= 1 && d <= 31) return date_part;
        } catch (const std::exception&) { return ""; }
    }
    return "";
}

void Task4_GameResultManager::splitCSVLine(const std::string& line_str, std::string tokens[], int max_tokens) { 
    std::istringstream stream(line_str); std::string token; int count = 0;
    while (std::getline(stream, token, ',') && count < max_tokens) {
        size_t start = token.find_first_not_of(" \t\r\n"); size_t end = token.find_last_not_of(" \t\r\n");
        tokens[count++] = (start == std::string::npos) ? "" : token.substr(start, end - start + 1);
    }
    while (count < max_tokens) tokens[count++] = "";
}

int Task4_GameResultManager::findPlayerIndex(int player_id_val) { 
    for (int i = 0; i < current_player_count; i++) {
        if (player_stats[i].player_id == player_id_val) return i;
    }
    return -1;
}

double Task4_GameResultManager::parseScore(const std::string& score_str) { 
    size_t dash_pos = score_str.find('-');
    if (dash_pos == std::string::npos) return 0.0;
    std::string first_score = score_str.substr(0, dash_pos); 
    size_t start = first_score.find_first_not_of(" \t"); size_t end = first_score.find_last_not_of(" \t");
    if (start != std::string::npos) first_score = first_score.substr(start, end - start + 1); else first_score = "";
    try { return std::stod(first_score); }
    catch (const std::exception&) { return 0.0; }
}

void Task4_GameResultManager::updatePlayerStats(int player_id_val, bool is_winner, double score_val) { 
    int index = findPlayerIndex(player_id_val); if (index == -1) return;
    Task4_PlayerStats& ps_ref = player_stats[index]; 
    ps_ref.total_matches++;
    if (is_winner) ps_ref.wins++; else ps_ref.losses++;
    if (ps_ref.total_matches == 1) ps_ref.avg_score = score_val;
    else ps_ref.avg_score = ((ps_ref.avg_score * (ps_ref.total_matches - 1)) + score_val) / ps_ref.total_matches;
}



void displayIntegratedMainMenu() {
    cout << "\n\n####################################################\n"
         << "### ASIA PACIFIC UNIVERSITY ESPORTS CHAMPIONSHIP ###\n"
         << "###           INTEGRATED MANAGEMENT SYSTEM       ###\n"
         << "####################################################\n"
         << "Please select a system module:\n"
         << "1. Tournament Management & Match Scheduling (Task 1)\n"
         << "2. Player Registration & Queueing (Task 2)\n"
         << "3. Spectator Management (Task 3 - Placeholder)\n"
         << "4. Result Logging & Performance History (Task 4)\n"
         << "0. Exit Application\n"
         << "Enter your choice: ";
}

int main() {
    srand(time(nullptr));

    Tournament task1_tournamentManager;
    Task4_GameResultManager task4_gameResultManager(100); 

    bool task1Initialized = false;
    int choice_val; 
    bool exitApplication = false;

    while (!exitApplication) {
        displayIntegratedMainMenu();
        cin >> choice_val;

        if (cin.fail()) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number." << endl; choice_val = -1;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        }

        switch (choice_val) {
            case 1:
                cout << "\n--- Accessing: Tournament Management & Match Scheduling (Task 1) ---\n";
                if (!task1Initialized) {
                    cout << "Initializing Task 1: Tournament System (requires Player_Registration.csv)..." << endl;
                    task1_tournamentManager.initialize("Player_Registration.csv");
                    task1Initialized = true;
                } else {
                    cout << "Task 1: Tournament System already initialized." << endl;
                }
                task1_tournamentManager.runCLI_TASK1();
                cout << "\n--- Returned from Task 1 ---\n";
                break;
            case 2: 
                runCLI_TASK2(); 
                break;
            case 3: runTask3_SpectatorManagement(); break;
            case 4:
                cout << "\n--- Accessing: Result Logging & Performance History (Task 4) ---\n";
                task4_gameResultManager.runProgram();
                cout << "\n--- Returned from Task 4 ---\n";
                break;
            case 0:
                exitApplication = true;
                cout << "\nExiting APUEC Integrated Management System. Goodbye!\n";
                break;
            default:
                if (choice_val != -1) { cout << "Invalid choice. Please try again." << endl; }
                break;
        }
        if (!exitApplication && choice_val != -1) {
            cout << "\n(Main Menu: Press Enter to continue or if stuck...)";
            
        }
    }
    return 0;
}
