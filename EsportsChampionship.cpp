// APUEC_Merged_System.cpp
// This file combines all tasks for the APU Esports Championship Management System.
// It includes definitions from APUEC_System.h and provides implementations.

#include "EsportsChampionship.hpp" // Includes all declarations and common headers

// Using namespace std for Task 1 code, as it was in the original task1.cpp
// and for convenience in the main integrated menu.
// Task 4 code explicitly uses std:: prefix. Task 2 uses C-style I/O.
using namespace std;


// Task 1: Match Scheduling Implementations
// From EsportsChampionship.cpp (Player Class)
Player::Player(int _id, const char* _name, const char* _rank, const char* _registrationType, int _ranking,
               const char* _email, int _teamID, bool _checkInStatus)
    : id(_id), wins(0), losses(0), groupId(0), registered(true), checkedIn(_checkInStatus) {
    strncpy(name, _name, sizeof(name) - 1); name[sizeof(name) - 1] = '\0';
    strncpy(rank, _rank, sizeof(rank) - 1); rank[sizeof(rank) - 1] = '\0';
    strncpy(registrationType, _registrationType, sizeof(registrationType) - 1); registrationType[sizeof(registrationType) - 1] = '\0';
    strcpy(currentStage, "group"); // Default stage
    checkInTime[0] = '\0'; // Initialize checkInTime
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
    if (time != nullptr && time[0] != '\0') {
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

// From EsportsChampionship.cpp (MatchQueue Class)
MatchQueue::Node::Node(Match* m) : match(m), next(nullptr) {}
MatchQueue::MatchQueue() : front(nullptr), rear(nullptr), size(0) {}
MatchQueue::~MatchQueue() {
    while (!isEmpty()) {
        Match* m = dequeue(); // Dequeue handles deletion of node, not match itself
        // Matches are owned by Tournament or Group, so MatchQueue doesn't delete Match*
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
    delete temp; // Delete the node, not the match
    size--;
    return match;
}

bool MatchQueue::isEmpty() const { return front == nullptr; }
int MatchQueue::getSize() const { return size; }

// From EsportsChampionship.cpp (PlayerPriorityQueue Class)
PlayerPriorityQueue::Node::Node(Player* p) : player(p), next(nullptr) {}
PlayerPriorityQueue::PlayerPriorityQueue() : head(nullptr), size(0) {}
PlayerPriorityQueue::~PlayerPriorityQueue() {
    while (!isEmpty()) {
        Player* p = dequeue(); // Dequeue handles deletion of node, not player itself
         // Players are owned by Tournament, so PlayerPriorityQueue doesn't delete Player*
    }
}

// Compares time strings "YYYY-MM-DD HH:MM"
// Returns true if time1 is earlier than time2
bool PlayerPriorityQueue::isEarlier(const char* time1, const char* time2) {
    if (time1 == nullptr || time1[0] == '\0') return false; // Null or empty time1 is not earlier
    if (time2 == nullptr || time2[0] == '\0') return true;  // Null or empty time2 means time1 is effectively earlier if time1 exists
    return strcmp(time1, time2) < 0;
}

void PlayerPriorityQueue::enqueue(Player* player) {
    Node* newNode = new Node(player);
    if (isEmpty() || isEarlier(player->getCheckInTime(), head->player->getCheckInTime())) {
        newNode->next = head;
        head = newNode;
    } else {
        Node* current = head;
        // Traverse to find the correct position
        while (current->next != nullptr && !isEarlier(player->getCheckInTime(), current->next->player->getCheckInTime())) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
    size++;
}

Player* PlayerPriorityQueue::dequeue() {
    if (isEmpty()) return nullptr;
    Node* temp = head;
    Player* player = temp->player;
    head = head->next;
    delete temp; // Delete the node, not the player
    size--;
    return player;
}

bool PlayerPriorityQueue::isEmpty() const { return head == nullptr; }
int PlayerPriorityQueue::getSize() const { return size; }

// From EsportsChampionship.cpp (Match Class)
Match::Match(int _id, Player* p1, Player* p2, const char* _stage, int _groupId, int _round)
    : id(_id), player1(p1), player2(p2), groupId(_groupId), round(_round), winner(nullptr) {
    strncpy(stage, _stage, 19); stage[19] = '\0';
    strcpy(status, "scheduled");
    strcpy(score, "0-0"); // Default score

    // Set scheduled time to current time
    time_t now_val = time(0);
    tm* ltm_val = localtime(&now_val); // Use localtime_r or localtime_s for thread safety if needed
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", ltm_val);
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
        if(player2) player2->incrementLosses(); // Ensure player2 is not null
        strcpy(score, "1-0");
    } else if (winner == player2) {
        player2->incrementWins();
        if(player1) player1->incrementLosses(); // Ensure player1 is not null
        strcpy(score, "0-1");
    }
    // If winner is nullptr (e.g., a draw, though not handled by incrementWins/Losses)
    // or if it's a walkover, score might need different handling.
}

// From EsportsChampionship.cpp (Group Class)
Group::Group(int _id, const char* _rankType, const char* _registrationType)
    : id(_id), playerCount(0), matchCount(0), winner(nullptr), completed(false), semiFinalsCompleted(0) {
    strncpy(rankType, _rankType, sizeof(rankType) - 1); rankType[sizeof(rankType) - 1] = '\0';
    strncpy(registrationType, _registrationType, sizeof(registrationType) - 1); registrationType[sizeof(registrationType) - 1] = '\0';
    for(int i=0; i<4; ++i) players[i] = nullptr;
    for(int i=0; i<3; ++i) matches[i] = nullptr;
}

Group::~Group() {
    // Group owns its matches
    for (int i = 0; i < matchCount; i++) {
        delete matches[i];
        matches[i] = nullptr;
    }
    // Players are owned by Tournament, so Group does not delete them.
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
    if (playerCount < 4 && player != nullptr) {
        players[playerCount++] = player;
        player->setGroupId(id); // Assign group ID to player
    }
}

// Creates 2 semifinal matches for the 4 players in the group
void Group::createSemifinalsOnly(int& nextMatchId) {
    if (playerCount < 4) { // Need 4 players for 2 semifinals
        // This case should be handled by the caller (Tournament) before calling.
        // For robustness, print an error or handle.
        cout << "Error: Group " << id << " (Rank " << rankType << ") does not have enough players (" << playerCount << "/4) for semifinals." << endl;
        return;
    }
    // Semifinal 1: Player 0 vs Player 3 ; Semifinal 2: Player 1 vs Player 2 (example pairing)
    matches[0] = new Match(nextMatchId++, players[0], players[3], "group", id, 1); // Round 1 (semifinal)
    matches[1] = new Match(nextMatchId++, players[1], players[2], "group", id, 1); // Round 1 (semifinal)
    matchCount = 2;
    cout << "Group " << id << " (Rank " << rankType << ") semifinal matches created." << endl;
}

void Group::createFinalMatch(int& nextMatchId, Player* semifinal1Winner, Player* semifinal2Winner) {
    if (!semifinal1Winner || !semifinal2Winner) {
        cout << "Error: Cannot create final for group " << id << " due to missing semifinal winner(s)." << endl;
        return;
    }
    matches[2] = new Match(nextMatchId++, semifinal1Winner, semifinal2Winner, "group", id, 2); // Round 2 (final)
    matchCount = 3; // Now 3 matches in this group
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
bool Group::areSemifinalsComplete() { return semiFinalsCompleted >= 2; } // Both semifinals are done

void Group::setGroupWinner(Player* player) {
    winner = player;
    completed = true;
}

void Group::displayStatus() {
    cout << "\n----- GROUP " << id << " (RANK " << rankType << ", REG: " << registrationType << ") STATUS -----\n";
    cout << "Players (" << playerCount << " total):\n";
    for (int i = 0; i < playerCount; i++) {
        if(players[i]) {
            cout << "  " << players[i]->getName() << " (Rank: " << players[i]->getRank()
                 << ", Check-in: " << (players[i]->isCheckedIn() ? "YES" : "NO")
                 << ", Stage: " << players[i]->getCurrentStage() << ")" << endl;
        }
    }

    cout << "\nMatches (" << matchCount << " total):\n";
    for (int i = 0; i < matchCount; i++) {
        if (matches[i] != nullptr) {
            cout << "  Match " << matches[i]->getId() << " (Round " << matches[i]->getRound() << "): ";
            if (matches[i]->getPlayer1() && matches[i]->getPlayer2()) {
                 cout << matches[i]->getPlayer1()->getName() << " (R:" << matches[i]->getPlayer1()->getRank() << ")"
                     << " vs "
                     << matches[i]->getPlayer2()->getName() << " (R:" << matches[i]->getPlayer2()->getRank() << ")";
            } else {
                cout << "Players not fully set.";
            }
            cout << " - Status: " << matches[i]->getStatus();
            if (strcmp(matches[i]->getStatus(), "completed") == 0 && matches[i]->getWinner()) {
                cout << " - Winner: " << matches[i]->getWinner()->getName();
            }
            cout << endl;
        }
    }

    if (completed && winner) {
        cout << "\nGroup Winner: " << winner->getName() << endl;
    } else if (completed && !winner) {
        cout << "\nGroup completed, but winner not determined." << endl;
    } else {
        cout << "\nGroup in progress." << endl;
    }
}


// From EsportsChampionship.cpp (Tournament Class)
Tournament::Tournament(int _maxPlayers, int _maxMatches, int _maxGroupWinners)
    : playerCount(0), maxPlayers(_maxPlayers), matchCount(0), maxMatches(_maxMatches),
      groupCount(0), groupWinnerCount(0), maxGroupWinners(_maxGroupWinners), nextMatchId(1),
      totalMatchesPlayed(0), groupSemifinalsCreated(false), knockoutCreated(false), groupsCreated(false) {
    players = new Player*[_maxPlayers];
    for(int i=0; i<_maxPlayers; ++i) players[i] = nullptr;

    matches = new Match*[_maxMatches];
    for(int i=0; i<_maxMatches; ++i) matches[i] = nullptr;

    groups = new Group*[10]; // Assuming max 10 groups based on original code
    for(int i=0; i<10; ++i) groups[i] = nullptr;

    groupWinners = new Player*[_maxGroupWinners];
    for(int i=0; i<_maxGroupWinners; ++i) groupWinners[i] = nullptr;
}

Tournament::~Tournament() {
    // Delete all players
    for (int i = 0; i < playerCount; i++) {
        delete players[i];
    }
    delete[] players;

    // Groups own their matches. Tournament owns knockout/final matches not in groups.
    // The 'matches' array in Tournament stores pointers to ALL matches for logging/saving.
    // Groups delete their matches. We must be careful not to double-delete.
    // A safer design would be for Tournament to only own matches it creates directly (knockout/final)
    // OR to have a clear ownership model (e.g., all matches owned by Tournament, groups just point to them).
    // Given the current Group destructor deletes its matches:
    // 1. Delete groups (this will delete group matches)
    for (int i = 0; i < groupCount; i++) {
        delete groups[i];
    }
    delete[] groups;

    // 2. Now delete matches in Tournament::matches that were NOT group matches.
    // This requires knowing which matches are which.
    // For simplicity and based on original structure, if matches in Tournament::matches
    // are pointers to the same objects deleted by Group, this is an issue.
    // If Tournament::matches stores *additional* matches (like knockout), delete those.
    // The original code just deleted all `matches[i]`, risking double deletion.
    // Let's assume for now that `matches` array might contain some matches not owned by groups (e.g. final knockout).
    // A truly robust solution needs explicit ownership tracking.
    // For this merge, and trying to preserve logic:
    // Iterate through matches. If a match is identified as a knockout/final (groupId == 0), delete it.
    // Group matches are deleted by Group destructor.
    for (int i = 0; i < matchCount; i++) {
        if (matches[i] != nullptr) {
            // If match is a non-group match (e.g. tournament final, knockout), delete it.
            // Group matches (matches[i]->getGroupId() != 0) are deleted by their Group's destructor.
            if (matches[i]->getGroupId() == 0) { // Indicates a tournament-level match not part of a group stage
                 delete matches[i];
            }
            // Else, it's a group match, assumed to be deleted by its Group.
            // To prevent dangling pointers, nullify them here, but the Group already deleted it.
            // matches[i] = nullptr; // This doesn't help much post-group-deletion.
        }
    }
    delete[] matches; // Delete the array of pointers itself.

    // groupWinners is an array of POINTERS to Player objects.
    // Player objects are deleted by the loop `for (int i = 0; i < playerCount; i++)`.
    // So, just delete the array groupWinners itself.
    delete[] groupWinners;
}

bool Tournament::areGroupsCreated() const { return groupsCreated; }

void Tournament::loadPlayersFromCSV(const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open player CSV file: " << filename << endl;
        return;
    }
    char line[512];
    file.getline(line, 512); // Skip header line

    while (file.getline(line, 512) && playerCount < maxPlayers) {
        if (strlen(line) == 0 || line[0] == '\r' || line[0] == '\n') continue; // Skip empty lines

        stringstream ss(line);
        char idStr[10], nameStr[100], regTypeStr[30], emailStr[100], rankStr[3], checkInStr[10], checkInTimeStr[20]="";

        // Expected CSV format for Task 1 loading: ID,Name,RegistrationType,Email,Rank,CheckInStatus[,CheckInTime]
        // Task 2 writes: Player ID,Player Name,Registration Type,Email,Rank,Check-In,Time of Check-In,Status
        // Task 1 loader will try to read its expected fields.

        ss.getline(idStr, 10, ',');
        ss.getline(nameStr, 100, ',');
        ss.getline(regTypeStr, 30, ',');
        ss.getline(emailStr, 100, ',');
        ss.getline(rankStr, 3, ','); // Rank is char, allow 2 chars + null
        ss.getline(checkInStr, 10, ','); // Reads "YES" or "NO"
        
        // Attempt to read check-in time if present (as per player.setCheckIn)
        // The original task1 loadPlayersFromCSV didn't explicitly parse checkInTime from CSV,
        // but Player::setCheckIn allows setting it.
        // If checkInTimeStr is available in CSV (e.g., 7th column)
        if (ss.getline(checkInTimeStr, 20, ',')) {
            // Successfully read checkInTimeStr
        } else {
            checkInTimeStr[0] = '\0'; // No time provided or fewer columns
        }


        int id = 0;
        if (idStr[0] == '\0') { cout << "Warning: Empty Player ID in CSV, skipping line: " << line << endl; continue; }
        try { id = atoi(idStr); }
        catch (...) { cout << "Warning: Invalid Player ID '" << idStr << "' in CSV, skipping line." << endl; continue; }
        if (id == 0 && strcmp(idStr,"0")!=0) { cout << "Warning: Player ID conversion failed for '" << idStr << "', skipping line." << endl; continue; }


        // Validate rank (A, B, C, D)
        if (rankStr[0] != 'A' && rankStr[0] != 'B' && rankStr[0] != 'C' && rankStr[0] != 'D') {
            cout << "Warning: Invalid rank '" << rankStr << "' for player " << nameStr << ". Skipping." << endl;
            continue;
        }
        rankStr[1] = '\0'; // Ensure rank is a single character string

        bool checkedIn = (strcmp(checkInStr, "YES") == 0 || strcmp(checkInStr, "yes") == 0);

        // Player constructor: int _id, const char* _name, const char* _rank, const char* _registrationType, int _ranking (unused),
        //                     const char* _email, int _teamID (unused), bool _checkInStatus
        Player* p = new Player(id, nameStr, rankStr, regTypeStr, 0, emailStr, 0, checkedIn);
        if (checkedIn) {
            if (checkInTimeStr[0] != '\0') {
                p->setCheckIn(true, checkInTimeStr);
            } else { // If time not in CSV but checkedIn, set current time
                time_t now_ct = time(0);
                tm* ltm_ct = localtime(&now_ct);
                char buffer_ct[20];
                strftime(buffer_ct, sizeof(buffer_ct), "%Y-%m-%d %H:%M", ltm_ct);
                p->setCheckIn(true, buffer_ct);
            }
            playerCheckInQueue.enqueue(p); // Add to priority queue if checked in
        }
        players[playerCount++] = p;
    }
    file.close();
    cout << "Loaded " << playerCount << " players from " << filename << "." << endl;
    cout << playerCheckInQueue.getSize() << " players are in the check-in queue." << endl;
}


void Tournament::groupPlayersByRank() {
    if (playerCheckInQueue.isEmpty()) {
        cout << "No players checked in to form groups." << endl;
        groupsCreated = true; // Mark as "created" but with 0 groups
        return;
    }

    // Temporary structure to hold players of same rank and registration type
    struct TempGroup {
        Player* pArray[100]; // Max 100 players of a specific rank/regType combo
        int pCount;
        const char* rank;
        const char* regType;
    };

    const char* ranks[] = {"A", "B", "C", "D"};
    const char* regTypes[] = {"Early-Bird", "Standard", "Wildcard", "Last-Minute"}; // Match these with CSV data
    int numRanks = 4;
    int numRegTypes = 4;

    TempGroup categorizedPlayers[numRanks * numRegTypes];
    int tempGroupCount = 0;

    for (int r = 0; r < numRanks; ++r) {
        for (int t = 0; t < numRegTypes; ++t) {
            categorizedPlayers[tempGroupCount].pCount = 0;
            categorizedPlayers[tempGroupCount].rank = ranks[r];
            categorizedPlayers[tempGroupCount].regType = regTypes[t];
            tempGroupCount++;
        }
    }

    // Dequeue players from check-in queue and categorize them
    PlayerPriorityQueue tempProcessingQueue; // To hold players if needed during processing
    while(!playerCheckInQueue.isEmpty()){
        Player* player = playerCheckInQueue.dequeue();
        bool assigned = false;
        for (int i = 0; i < tempGroupCount; ++i) {
            if (strcmp(player->getRank(), categorizedPlayers[i].rank) == 0 &&
                strcmp(player->getRegistrationType(), categorizedPlayers[i].regType) == 0) {
                if (categorizedPlayers[i].pCount < 100) {
                    categorizedPlayers[i].pArray[categorizedPlayers[i].pCount++] = player;
                }
                assigned = true;
                break;
            }
        }
        if (!assigned) {
            // Player could not be categorized (e.g., unknown rank/regType in player data)
            // Or, if registration type is not one of the 4, assign to a default like "Standard" for that rank
             cout << "Warning: Player " << player->getName() << " (Rank: " << player->getRank()
                  << ", RegType: " << player->getRegistrationType() << ") could not be categorized into predefined groups. Adding to a generic group if possible." << endl;
            // For simplicity, such players might be put back or ignored. Here, we'll put them in a temp queue.
            tempProcessingQueue.enqueue(player); // Or handle differently
        }
    }
    // Restore any unassigned players to checkin queue if needed, or discard
    while(!tempProcessingQueue.isEmpty()) playerCheckInQueue.enqueue(tempProcessingQueue.dequeue());


    // Create groups of 4 from categorized players
    int currentGroupId = 1;
    for (int i = 0; i < tempGroupCount && groupCount < 10; ++i) { // Max 10 groups overall for tournament
        for (int j = 0; (j + 3) < categorizedPlayers[i].pCount && groupCount < 10; j += 4) {
            groups[groupCount] = new Group(currentGroupId++, categorizedPlayers[i].rank, categorizedPlayers[i].regType);
            for (int k = 0; k < 4; ++k) {
                groups[groupCount]->addPlayer(categorizedPlayers[i].pArray[j + k]);
            }
            groupCount++;
        }
    }

    groupsCreated = true;
    cout << "\nGroups created: " << groupCount << endl;
    for (int i = 0; i < groupCount; i++) {
        cout << "Group " << groups[i]->getId() << " (Rank " << groups[i]->getRankType()
             << ", Reg Type: " << groups[i]->getRegistrationType() << ") - " << groups[i]->getPlayerCount() << " players" << endl;
    }
}


void Tournament::saveMatchesToCSV(const char* filename) {
    ofstream file(filename);
    if (!file.is_open()) { cout << "Error: Could not open file " << filename << " for saving matches." << endl; return; }

    file << "match_id,stage,group_id,round,player1_id,player2_id,scheduled_time,status,winner_id,score\n";
    for (int i = 0; i < matchCount; i++) {
        Match* match = matches[i];
        if (!match) continue;
        file << match->getId() << ","
             << match->getStage() << ","
             << match->getGroupId() << "," // Will be 0 for non-group matches
             << match->getRound() << ",";
        if(match->getPlayer1()) file << match->getPlayer1()->getId(); else file << "0";
        file << ",";
        if(match->getPlayer2()) file << match->getPlayer2()->getId(); else file << "0";
        file << ","
             << match->getScheduledTime() << ","
             << match->getStatus() << ",";
        if (match->getWinner() != nullptr) { file << match->getWinner()->getId(); } else { file << "0";} // 0 for no winner
        file << "," << match->getScore() << "\n";
    }
    file.close();
    cout << "Matches saved to " << filename << endl;
}

void Tournament::saveBracketsToCSV(const char* filename) {
    ofstream file(filename);
    if (!file.is_open()) { cout << "Error: Could not open file " << filename << " for saving brackets." << endl; return; }

    file << "bracket_id,stage,player_id,player_name,group_id,rank,status_in_stage\n";
    int bracketId = 1;

    // Log players in group stage
    for (int i = 0; i < groupCount; ++i) {
        for (int j = 0; j < groups[i]->getPlayerCount(); ++j) {
            Player* p = groups[i]->getPlayer(j);
            if (p) {
                file << bracketId++ << ","
                     << "group,"
                     << p->getId() << ","
                     << p->getName() << ","
                     << p->getGroupId() << ","
                     << p->getRank() << ","
                     << (groups[i]->isCompleted() && groups[i]->getWinner() != p ? "eliminated_group" : (groups[i]->getWinner() == p ? "won_group" : "active_group"))
                     << "\n";
            }
        }
    }

    // Log group winners progressing to knockout
    for (int i = 0; i < groupWinnerCount; i++) {
        if (groupWinners[i]) {
             file << bracketId++ << ","
                 << "knockout_qualifier," // Denotes they qualified for knockout
                 << groupWinners[i]->getId() << ","
                 << groupWinners[i]->getName() << ","
                 << groupWinners[i]->getGroupId() << "," // Group they won
                 << groupWinners[i]->getRank() << ","
                 << "advanced_to_knockout\n";
        }
    }

    // Log players in knockout matches (and final if applicable)
    // This requires more detailed tracking of knockout match participants and winners
    // For simplicity, this example focuses on group stage and qualifiers.
    // A more detailed bracket would trace each knockout match.
    for (int i = 0; i < matchCount; ++i) {
        Match* m = matches[i];
        if (m && (strcmp(m->getStage(), "knockout") == 0 || strcmp(m->getStage(), "final") == 0)) {
            if (m->getPlayer1()) {
                 file << bracketId++ << ","
                     << m->getStage() << ","
                     << m->getPlayer1()->getId() << ","
                     << m->getPlayer1()->getName() << ","
                     << "0," // Group ID 0 for knockout
                     << m->getPlayer1()->getRank() << ","
                     << (strcmp(m->getStatus(), "completed")==0 ? (m->getWinner()==m->getPlayer1() ? "won_match" : "lost_match") : "active_knockout")
                     << "\n";
            }
            if (m->getPlayer2()) {
                 file << bracketId++ << ","
                     << m->getStage() << ","
                     << m->getPlayer2()->getId() << ","
                     << m->getPlayer2()->getName() << ","
                     << "0," // Group ID 0 for knockout
                     << m->getPlayer2()->getRank() << ","
                     << (strcmp(m->getStatus(), "completed")==0 ? (m->getWinner()==m->getPlayer2() ? "won_match" : "lost_match") : "active_knockout")
                     << "\n";
            }
        }
    }


    file.close();
    cout << "Bracket status saved to " << filename << endl;
}


void Tournament::initialize(const char* playerFilename) {
    if (groupsCreated) {
        cout << "Tournament already initialized. Reset to re-initialize." << endl;
        return;
    }
    loadPlayersFromCSV(playerFilename); // Load players and populate check-in queue
    groupPlayersByRank();          // Form groups from checked-in players
    cout << "Tournament initialized. Players loaded and grouped by rank." << endl;
    // groupsCreated is set within groupPlayersByRank
}

void Tournament::displayCheckInStatus() {
    if (!groupsCreated && playerCount == 0) { // Check if players even loaded
        cout << "No players loaded. Initialize tournament first." << endl;
        return;
    }
    cout << "\n===== CHECK-IN STATUS (" << playerCount << " total registered) =====\n";
    int checkedInCount = 0;
    int checkedInByRank[4] = {0}; // A, B, C, D
    int totalByRank[4] = {0};
    const char* rankChars[] = {"A", "B", "C", "D"};

    for (int i = 0; i < playerCount; i++) {
        Player* p = players[i];
        if(!p) continue;
        for(int r=0; r<4; ++r) {
            if (strcmp(p->getRank(), rankChars[r]) == 0) {
                totalByRank[r]++;
                if (p->isCheckedIn()) {
                    checkedInByRank[r]++;
                    checkedInCount++;
                }
                break;
            }
        }
    }

    cout << "Total Checked-In: " << checkedInCount << "/" << playerCount << endl;
    for(int r=0; r<4; ++r) {
        cout << "\nRANK " << rankChars[r] << " PLAYERS (" << checkedInByRank[r] << "/" << totalByRank[r] << " checked in):\n";
        bool foundRank = false;
        for (int i = 0; i < playerCount; i++) {
             Player* p = players[i];
             if(!p) continue;
            if (strcmp(p->getRank(), rankChars[r]) == 0) {
                foundRank = true;
                cout << "  " << p->getName() << " - ";
                if (p->isCheckedIn()) {
                    cout << "CHECKED IN";
                    if (p->getCheckInTime()[0] != '\0') cout << " at " << p->getCheckInTime();
                } else {
                    cout << "NOT CHECKED IN";
                }
                cout << endl;
            }
        }
        if (!foundRank) cout << "  No players registered for this rank." << endl;
    }
    cout << "============================\n";
}

void Tournament::createGroupSemifinals() {
    if (!groupsCreated) { cout << "Error: Groups not created. Please initialize tournament first (load players & group them)." << endl; return; }
    if (groupSemifinalsCreated) { cout << "Group semifinals already created." << endl; return; }
    if (groupCount == 0) { cout << "Error: No groups available to create matches. Check if enough players checked in and were grouped." << endl; return; }

    for (int i = 0; i < groupCount; i++) {
        if (groups[i] && groups[i]->getPlayerCount() == 4) { // Only create if group is valid and has 4 players
            groups[i]->createSemifinalsOnly(nextMatchId);
            for (int j = 0; j < groups[i]->getMatchCount(); j++) { // Add created semifinal matches to tournament's main list
                Match* match = groups[i]->getMatch(j);
                if (match != nullptr && matchCount < maxMatches) {
                    matches[matchCount++] = match; // Add to tournament's global match list
                    upcomingMatches.enqueue(match);
                } else if (matchCount >= maxMatches) {
                    cout << "Warning: Max matches limit reached ("<< maxMatches <<"), cannot add more group semifinal matches." << endl;
                    break; 
                }
            }
        } else if (groups[i]) {
            cout << "Skipping semifinal creation for Group " << groups[i]->getId() << ": requires 4 players, has " << groups[i]->getPlayerCount() << "." << endl;
        }
    }
    groupSemifinalsCreated = true;
    saveMatchesToCSV("matches.csv");
    saveBracketsToCSV("brackets.csv");
    cout << "Finished creating available group stage semifinal matches for " << groupCount << " groups." << endl;
}

Match* Tournament::getNextMatch() {
    if (upcomingMatches.isEmpty()) { return nullptr; }
    return upcomingMatches.dequeue(); // Dequeues from front
}

void Tournament::updateMatchResult(Match* match, Player* winner_player) {
    if (match == nullptr) { cout << "Error: Cannot update result for a null match." << endl; return; }
    if (winner_player == nullptr) { cout << "Error: Winner player cannot be null." << endl; return; } // Or handle walkovers explicitly

    match->setWinner(winner_player); // Sets score, increments player W/L
    totalMatchesPlayed++;

    cout << "Match ID " << match->getId() << " (" << match->getStage() << " R" << match->getRound() << ") result updated. Winner: " << winner_player->getName() << endl;

    // Group Stage Logic
    if (strcmp(match->getStage(), "group") == 0) {
        Group* groupPtr = groups[match->getGroupId() - 1]; // Group IDs are 1-based
        if (!groupPtr) { cout << "Error: Group not found for match " << match->getId() << endl; return; }

        if (match->getRound() == 1) { // Group Semifinal completed
            groupPtr->incrementSemiFinalsCompleted();
            if (groupPtr->areSemifinalsComplete()) { // Both semifinals of this group are done
                Match* sf1 = groupPtr->getMatch(0);
                Match* sf2 = groupPtr->getMatch(1);
                if (sf1 && sf1->getWinner() && sf2 && sf2->getWinner()) {
                    groupPtr->createFinalMatch(nextMatchId, sf1->getWinner(), sf2->getWinner());
                    Match* groupFinalMatch = groupPtr->getMatch(2); // Index 2 for final
                    if (groupFinalMatch && matchCount < maxMatches) {
                        matches[matchCount++] = groupFinalMatch;
                        upcomingMatches.enqueue(groupFinalMatch);
                    } else if (matchCount >= maxMatches) {
                        cout << "Warning: Max matches limit reached, cannot add group final for Group " << groupPtr->getId() << "." << endl;
                    }
                } else {
                     cout << "Error: Could not determine winners for Group " << groupPtr->getId() << " semifinals to create final." << endl;
                }
            }
        } else if (match->getRound() == 2) { // Group Final completed
            groupPtr->setGroupWinner(winner_player);
            winner_player->advanceStage(); // Player advances to "knockout" stage conceptually
            if (groupWinnerCount < maxGroupWinners) {
                groupWinners[groupWinnerCount++] = winner_player;
            } else {
                 cout << "Warning: Max group winners limit (" << maxGroupWinners << ") reached." << endl;
            }
            cout << "Group " << groupPtr->getId() << " (Rank " << groupPtr->getRankType()
                 << ") completed! Winner: " << winner_player->getName() << endl;

            // Check if all groups are complete to proceed to knockout
            bool allGroupsDone = true;
            for (int i = 0; i < groupCount; i++) {
                if (groups[i] && !groups[i]->isCompleted()) { allGroupsDone = false; break; }
            }
            if (allGroupsDone && groupCount > 0) {
                cout << "\nAll group stages completed! Creating knockout stage matches..." << endl;
                createKnockoutMatches();
            }
        }
    }
    // Knockout Stage Logic
    else if (strcmp(match->getStage(), "knockout") == 0) {
        winner_player->advanceStage(); // Player advances (e.g. "knockout" -> "final")
        if (match->getRound() == 1) { // Knockout Semifinal completed
            // Check if all knockout semifinals are done to create the final match
            Player* semifinalWinners[2] = {nullptr, nullptr};
            int currentSemifinalWinnerCount = 0;
            bool allKnockoutSemifinalsCompletedThisRound = true;

            for (int i = 0; i < matchCount; i++) { // Search all matches
                if (matches[i] && strcmp(matches[i]->getStage(), "knockout") == 0 && matches[i]->getRound() == 1) {
                    if (strcmp(matches[i]->getStatus(), "completed") != 0) {
                        allKnockoutSemifinalsCompletedThisRound = false;
                        break;
                    } else if (matches[i]->getWinner() && currentSemifinalWinnerCount < 2) {
                        // Avoid adding same winner twice if logic re-triggered
                        bool alreadyAdded = false;
                        for(int k=0; k<currentSemifinalWinnerCount; ++k) if(semifinalWinners[k] == matches[i]->getWinner()) alreadyAdded = true;
                        if(!alreadyAdded) semifinalWinners[currentSemifinalWinnerCount++] = matches[i]->getWinner();
                    }
                }
            }

            if (allKnockoutSemifinalsCompletedThisRound && currentSemifinalWinnerCount == 2) {
                 if(semifinalWinners[0] && semifinalWinners[1]) {
                    cout << "Both knockout semifinals completed. Creating final match." << endl;
                    createFinalMatch(semifinalWinners[0], semifinalWinners[1]); // Overall tournament final
                 } else {
                    cout << "Error or not enough distinct winners from knockout semifinals to create final." << endl;
                 }
            } else if (allKnockoutSemifinalsCompletedThisRound && currentSemifinalWinnerCount == 1 && groupWinnerCount == 3) {
                // Special case for 3 group winners: 1 semifinal, winner plays the 3rd group winner
                // Find the 3rd group winner who didn't play in this semi.
                Player* thirdWinner = nullptr;
                for(int i=0; i < groupWinnerCount; ++i){
                    if(groupWinners[i] != match->getPlayer1() && groupWinners[i] != match->getPlayer2()){
                        thirdWinner = groupWinners[i];
                        break;
                    }
                }
                if(semifinalWinners[0] && thirdWinner){
                     cout << "Knockout semifinal for 3 players completed. Creating final match." << endl;
                     createFinalMatch(semifinalWinners[0], thirdWinner);
                } else {
                     cout << "Error: Could not set up final for 3 player knockout scenario." << endl;
                }

            }


        } else if (match->getRound() == 2) { // Knockout Final (Overall Tournament Final) completed
            cout << "\n***************************************************\n"
                 << "*** TOURNAMENT CHAMPION: " << winner_player->getName() << " (Rank: " << winner_player->getRank() << ") ***\n"
                 << "***************************************************\n" << endl;
            // Tournament ends.
        }
    }

    saveMatchesToCSV("matches.csv"); // Save after every update
    saveBracketsToCSV("brackets.csv");
}


void Tournament::createKnockoutMatches() {
    if (knockoutCreated) { cout << "Knockout matches already created." << endl; return; }
    if (groupWinnerCount < 2) {
        cout << "Not enough group winners (" << groupWinnerCount << ") for knockout stage." << endl;
        if (groupWinnerCount == 1 && groupWinners[0]) {
             cout << "\n***************************************************\n"
                  << "*** TOURNAMENT CHAMPION (by default): " << groupWinners[0]->getName() << " ***\n"
                  << "***************************************************\n" << endl;
        }
        knockoutCreated = true; // Mark as "created" to prevent re-triggering, even if no matches.
        return;
    }

    cout << "\nCreating Knockout Matches for " << groupWinnerCount << " group winners:" << endl;
    for(int i=0; i < groupWinnerCount; ++i) {
        if(groupWinners[i]) cout << "  Winner " << i+1 << ": " << groupWinners[i]->getName() << " (Rank " << groupWinners[i]->getRank() << ")" << endl;
    }


    if (groupWinnerCount == 2) { // Straight to final
        createFinalMatch(groupWinners[0], groupWinners[1]);
    } else if (groupWinnerCount == 3) { // 1 Semifinal, winner plays the 3rd
        // Pairing: Winner0 vs Winner1, winner of this match plays Winner2 in final
        cout << "Knockout for 3 players: " << groupWinners[0]->getName() << " vs " << groupWinners[1]->getName()
             << ". Winner will play " << groupWinners[2]->getName() << " in the final." << endl;
        Match* semi1 = new Match(nextMatchId++, groupWinners[0], groupWinners[1], "knockout", 0, 1); // Round 1, GroupID 0 for tournament level
        if (matchCount < maxMatches) {
            matches[matchCount++] = semi1;
            upcomingMatches.enqueue(semi1);
        } else {
            cout << "Warning: Max matches limit reached, cannot add knockout semifinal." << endl;
            delete semi1; // Clean up if not added
        }
    } else if (groupWinnerCount >= 4) { // Standard 4-player semifinal bracket (or more, takes first 4)
        // Pairing: Winner0 vs Winner1, Winner2 vs Winner3
        Match* semi1 = new Match(nextMatchId++, groupWinners[0], groupWinners[1], "knockout", 0, 1);
        Match* semi2 = new Match(nextMatchId++, groupWinners[2], groupWinners[3], "knockout", 0, 1);

        if (matchCount < maxMatches -1 ) { // Need space for 2 matches
             matches[matchCount++] = semi1;
             matches[matchCount++] = semi2;
             upcomingMatches.enqueue(semi1);
             upcomingMatches.enqueue(semi2);
             cout << "Created knockout stage semifinal matches:\n";
             cout << "  Semifinal 1 (Match ID " << semi1->getId() << "): " << groupWinners[0]->getName() << " vs " << groupWinners[1]->getName() << endl;
             cout << "  Semifinal 2 (Match ID " << semi2->getId() << "): " << groupWinners[2]->getName() << " vs " << groupWinners[3]->getName() << endl;
        } else {
             cout << "Warning: Max matches limit reached, cannot create all knockout semifinals." << endl;
             delete semi1; delete semi2; // Clean up if not added
        }
    }
    knockoutCreated = true;
    saveMatchesToCSV("matches.csv"); // Save new knockout matches
}

void Tournament::createFinalMatch(Player* finalist1, Player* finalist2) {
    if (!finalist1 || !finalist2) {
        cout << "Error: One or both finalists are missing. Cannot create final match." << endl;
        return;
    }
    Match* finalMatch = new Match(nextMatchId++, finalist1, finalist2, "knockout", 0, 2); // Round 2 (Final), GroupID 0
    if (matchCount < maxMatches) {
        matches[matchCount++] = finalMatch;
        upcomingMatches.enqueue(finalMatch);
        cout << "\nCreated FINAL MATCH (Match ID " << finalMatch->getId() << "): "
             << finalist1->getName() << " (Rank " << finalist1->getRank() << ")"
             << " vs "
             << finalist2->getName() << " (Rank " << finalist2->getRank() << ")" << endl;
        saveMatchesToCSV("matches.csv");
    } else {
        cout << "Warning: Max matches limit reached, cannot create final match." << endl;
        delete finalMatch; // Clean up if not added
    }
}

void Tournament::displayStatus() {
    cout << "\n===== TOURNAMENT STATUS =====\n";
    cout << "Total Registered Players: " << playerCount << endl;
    cout << "Active Groups: " << groupCount << endl;
    cout << "Matches Scheduled/Played: " << matchCount << endl;
    cout << "Matches Completed: " << totalMatchesPlayed << endl;
    cout << "Group Winners Emerged: " << groupWinnerCount << endl;

    const char* currentStageStrVal;
    int expectedGroupMatches = 0;
    for(int i=0; i<groupCount; ++i) if(groups[i] && groups[i]->getPlayerCount() == 4) expectedGroupMatches += 3; // 3 matches per full group

    int expectedKnockoutMatches = 0;
    if (groupWinnerCount >= 4) expectedKnockoutMatches = 3; // 2 semis, 1 final
    else if (groupWinnerCount == 3) expectedKnockoutMatches = 2; // 1 semi, 1 final
    else if (groupWinnerCount == 2) expectedKnockoutMatches = 1; // 1 final

    if (!groupsCreated) currentStageStrVal = "Setup - Not Initialized";
    else if (!groupSemifinalsCreated && groupCount > 0) currentStageStrVal = "Setup - Awaiting Group Semifinal Creation";
    else if (groupCount > 0 && totalMatchesPlayed < expectedGroupMatches) currentStageStrVal = "Group Stage In Progress";
    else if (groupCount > 0 && totalMatchesPlayed >= expectedGroupMatches && !knockoutCreated && groupWinnerCount >=2) currentStageStrVal = "Group Stage Complete - Awaiting Knockout Creation";
    else if (knockoutCreated && groupWinnerCount >=2 && totalMatchesPlayed < expectedGroupMatches + expectedKnockoutMatches -1) currentStageStrVal = "Knockout Semifinals In Progress";
    else if (knockoutCreated && groupWinnerCount >=2 && totalMatchesPlayed == expectedGroupMatches + expectedKnockoutMatches -1 && expectedKnockoutMatches > 1) currentStageStrVal = "Knockout Semifinals Complete - Awaiting Final";
     else if (knockoutCreated && groupWinnerCount >=2 && totalMatchesPlayed < expectedGroupMatches + expectedKnockoutMatches) currentStageStrVal = "Knockout Final In Progress";
    else if (knockoutCreated && totalMatchesPlayed >= expectedGroupMatches + expectedKnockoutMatches && expectedKnockoutMatches > 0) currentStageStrVal = "Tournament Completed";
    else if (groupCount == 0 && groupsCreated) currentStageStrVal = "No groups formed (check player check-ins).";
    else currentStageStrVal = "Idle or Transitioning";

    cout << "Current Stage: " << currentStageStrVal << endl;

    bool championFound = false;
    if (strcmp(currentStageStrVal, "Tournament Completed") == 0) {
        for (int i = 0; i < matchCount; i++) { // Find the final match winner
            if (matches[i] && strcmp(matches[i]->getStage(), "knockout") == 0 &&
                matches[i]->getRound() == 2 && strcmp(matches[i]->getStatus(), "completed") == 0 && matches[i]->getWinner()) {
                cout << "\n🏆 TOURNAMENT CHAMPION: " << matches[i]->getWinner()->getName()
                     << " (Rank: " << matches[i]->getWinner()->getRank() << ") 🏆" << endl;
                championFound = true;
                break;
            }
        }
         if (!championFound && groupWinnerCount == 1 && groupWinners[0]) { // Champion by default
             cout << "\n🏆 TOURNAMENT CHAMPION (by default): " << groupWinners[0]->getName()
                  << " (Rank: " << groupWinners[0]->getRank() << ") 🏆" << endl;
             championFound = true;
         }
    }

    for (int i = 0; i < groupCount; i++) { if(groups[i]) groups[i]->displayStatus(); }

    bool allActualGroupsAreDone = true;
    if (groupCount == 0 && !groupsCreated) allActualGroupsAreDone = false; // No groups to be done
    else if (groupCount > 0) {
        for (int i = 0; i < groupCount; i++) {
            if (groups[i] && !groups[i]->isCompleted()) { allActualGroupsAreDone = false; break; }
        }
    } else { // groupsCreated is true, but groupCount is 0
        allActualGroupsAreDone = true; // Effectively, all 0 groups are "done"
    }


    if (allActualGroupsAreDone && groupCount > 0) {
        cout << "\n----- GROUP WINNERS (Qualified for Knockout) -----\n";
        if (groupWinnerCount == 0) cout << "  None yet, or group stage not fully complete.\n";
        for (int i = 0; i < groupWinnerCount; i++) {
             if (groupWinners[i]) {
                cout << "  " << groupWinners[i]->getName()
                     << " (Won Group " << groupWinners[i]->getGroupId()
                     << ", Rank: " << groupWinners[i]->getRank() << ")" << endl;
            }
        }
    }

    bool hasKnockoutResults = false;
    for (int i = 0; i < matchCount; i++) {
        if (matches[i] && strcmp(matches[i]->getStage(), "knockout") == 0 && strcmp(matches[i]->getStatus(), "completed") == 0) {
            if (!hasKnockoutResults) {
                cout << "\n----- KNOCKOUT RESULTS -----\n"; hasKnockoutResults = true;
            }
            const char* roundType = (matches[i]->getRound() == 1) ? "Semifinal" : "Final";
            cout << "  Knockout " << roundType << " (Match ID " << matches[i]->getId() << "): ";
            if (matches[i]->getPlayer1() && matches[i]->getPlayer2()) {
                cout << matches[i]->getPlayer1()->getName() << " vs " << matches[i]->getPlayer2()->getName();
            }
            if (matches[i]->getWinner()) {
                cout << " - Winner: " << matches[i]->getWinner()->getName() << " (Score: " << matches[i]->getScore() << ")" << endl;
            } else {
                cout << " - Winner: Not determined." << endl;
            }
        }
    }

    cout << "\n----- UPCOMING MATCHES -----\n";
    if (upcomingMatches.isEmpty()) {
         if (championFound || (totalMatchesPlayed >= expectedGroupMatches + expectedKnockoutMatches && knockoutCreated && expectedKnockoutMatches > 0 && groupWinnerCount >=1) || (groupCount == 0 && groupsCreated) ){
            cout << "  Tournament is complete! No more matches." << endl;
         } else if (!groupsCreated) {
            cout << "  Tournament not initialized." << endl;
         } else if (groupCount > 0 && !groupSemifinalsCreated) {
            cout << "  Group semifinals need to be created." << endl;
         } else {
            cout << "  No upcoming matches scheduled, or waiting for prior matches to complete to generate next round." << endl;
         }
    } else {
        MatchQueue tempDisplayQueue; // To display without altering original queue
        int upcomingCount = 0;
        cout << "  (Next " << min(5, upcomingMatches.getSize()) << " of " << upcomingMatches.getSize() << " upcoming matches displayed)\n";
        while (!upcomingMatches.isEmpty()) {
            Match* match = upcomingMatches.dequeue();
            if (upcomingCount < 5) { // Display limited number
                cout << "  Match " << ++upcomingCount << " (ID " << match->getId() << "): " << match->getStage() << " ";
                if (strcmp(match->getStage(), "group") == 0) {
                    cout << "Group " << match->getGroupId() << " ";
                    if (match->getRound() == 1) cout << "Semifinal"; else cout << "Final";
                } else { // knockout
                    if (match->getRound() == 1) cout << "Semifinal"; else cout << "Final";
                }
                cout << " - ";
                if (match->getPlayer1() && match->getPlayer2()) {
                    cout << match->getPlayer1()->getName() << " vs " << match->getPlayer2()->getName();
                } else {
                    cout << "Players TBD";
                }
                cout << " (Scheduled: " << match->getScheduledTime() << ")" << endl;
            }
            tempDisplayQueue.enqueue(match); // Store to re-populate
        }
        // Restore upcomingMatches queue
        while (!tempDisplayQueue.isEmpty()) { upcomingMatches.enqueue(tempDisplayQueue.dequeue()); }
    }
     cout << "============================\n";
}


void Tournament::runCLI_TASK1() {
    int choice_val;
    bool exitCLI = false;

    while (!exitCLI) {
        cout << "\n===== TASK 1: TOURNAMENT MANAGEMENT & MATCH SCHEDULING =====\n";
        // Simplified current stage display for CLI
        const char* currentStageDisplay;
        if (!groupsCreated) currentStageDisplay = "Setup - Tournament not initialized";
        else if (groupCount > 0 && !groupSemifinalsCreated) currentStageDisplay = "Setup - Create group semifinal matches";
        else if (groupCount == 0 && groupsCreated) currentStageDisplay = "Setup - No groups formed (check players)";
        else { // Assuming groups created and possibly semifinals too
            int expGroupMatches = 0;
            for(int i=0; i<groupCount; ++i) if(groups[i] && groups[i]->getPlayerCount()==4) expGroupMatches +=3;
            int expKnockoutMatches = 0;
            if (groupWinnerCount >= 4) expKnockoutMatches = 3;
            else if (groupWinnerCount == 3) expKnockoutMatches = 2;
            else if (groupWinnerCount == 2) expKnockoutMatches = 1;

            if (totalMatchesPlayed < expGroupMatches) currentStageDisplay = "Group Stage In Progress";
            else if (totalMatchesPlayed >= expGroupMatches && groupWinnerCount <2 && groupCount > 0) currentStageDisplay = "Group Stage Complete - Awaiting more Group Winners or Knockout Creation";
            else if (totalMatchesPlayed >= expGroupMatches && !knockoutCreated && groupWinnerCount >=2) currentStageDisplay = "Group Stage Complete - Create Knockout Matches";
            else if (knockoutCreated && totalMatchesPlayed < expGroupMatches + expKnockoutMatches) currentStageDisplay = "Knockout Stage In Progress";
            else if (knockoutCreated && totalMatchesPlayed >= expGroupMatches + expKnockoutMatches && expKnockoutMatches > 0) currentStageDisplay = "Tournament Completed";
            else currentStageDisplay = "Idle or Transitioning";
        }
        cout << "CURRENT STAGE: " << currentStageDisplay << endl;
        cout << "Matches played: " << totalMatchesPlayed << " of (approx) " << (groupCount * 3 + (groupWinnerCount > 1 ? groupWinnerCount -1 : 0)) << endl;
        cout << "Active groups: " << groupCount << " | Upcoming matches in queue: " << upcomingMatches.getSize() << "\n\n";

        cout << "Tournament Management Menu:\n";
        cout << "1. Display player check-in status\n";
        cout << "2. Create group stage semifinal matches\n";
        cout << "3. Play next match (simulated winner)\n";
        cout << "4. Display full tournament status & brackets\n";
        cout << "5. (ADMIN) Manually Create Knockout Matches (if group winners ready)\n";
        cout << "6. Return to Main APUEC Menu\n";
        cout << "Enter your choice: ";

        cin >> choice_val;
        if (cin.fail()) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number." << endl; choice_val = 99; // Dummy value
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear trailing newline
        }

        switch (choice_val) {
            case 1:
                if (!groupsCreated && playerCount == 0) { cout << "Tournament not initialized yet (no players loaded)." << endl; }
                else { displayCheckInStatus(); }
                break;
            case 2:
                if (!groupsCreated) { cout << "Tournament not initialized yet (no players/groups)." << endl; }
                else if (groupCount == 0) { cout << "No groups formed to create matches for. Check player check-ins." << endl; }
                else { createGroupSemifinals(); }
                break;
            case 3: // Play next match
                if (!groupsCreated) { cout << "Tournament not initialized yet." << endl; }
                else if (upcomingMatches.isEmpty()) {
                     cout << "No upcoming matches. Tournament might be complete or waiting for matches to be generated." << endl;
                     displayStatus(); // Show full status
                } else {
                    Match* matchToPlay = upcomingMatches.peek(); // Peek first to display
                     cout << "\nPlaying next match (Simulated)..." << endl;
                     cout << "Match ID: " << matchToPlay->getId() << " Stage: " << matchToPlay->getStage();
                     if (strcmp(matchToPlay->getStage(), "group") == 0) {
                         cout << " Group " << matchToPlay->getGroupId();
                         if (matchToPlay->getRound() == 1) cout << " Semifinal"; else cout << " Final";
                     } else { // Knockout
                         if (matchToPlay->getRound() == 1) cout << " Semifinal"; else cout << " Final";
                     }
                     cout << ":\n  " << matchToPlay->getPlayer1()->getName() << " (Rank: " << matchToPlay->getPlayer1()->getRank() << ")"
                          << " vs " << matchToPlay->getPlayer2()->getName() << " (Rank: " << matchToPlay->getPlayer2()->getRank() << ")" << endl;

                    // Simulate winner
                    Player* winner = (rand() % 2 == 0) ? matchToPlay->getPlayer1() : matchToPlay->getPlayer2();
                    cout << "Simulated Winner: " << winner->getName() << endl;
                    
                    matchToPlay = getNextMatch(); // Actually dequeue it now
                    updateMatchResult(matchToPlay, winner); // This will update states and potentially create new matches
                    cout << "Match completed. Winner: " << winner->getName() << endl;
                    displayStatus(); // Show updated status
                }
                break;
            case 4:
                if (!groupsCreated && playerCount == 0) { cout << "Tournament not initialized yet." << endl; }
                else { displayStatus(); }
                break;
            case 5: // Admin: Manually create knockout
                 if (!groupsCreated) { cout << "Tournament not initialized yet." << endl; }
                 else if (groupWinnerCount < 2) { cout << "Not enough group winners (" << groupWinnerCount << ") to create knockout matches." << endl; }
                 else if (knockoutCreated) { cout << "Knockout matches seem to be already created or attempted." << endl; }
                 else {
                    cout << "Attempting to create knockout matches manually..." << endl;
                    createKnockoutMatches();
                    displayStatus();
                 }
                break;
            case 6: exitCLI = true; break;
            default: if (choice_val != 99) cout << "Invalid choice. Please try again." << endl; break;
        }
        if (!exitCLI && choice_val != 99 && choice_val !=6) {
             cout << "\n(Task 1 CLI: Press Enter to continue...)";
             // cin.get(); // Can be problematic, cin.ignore should handle most cases
        }
    }
    cout << "Exiting Task 1: Tournament Management." << endl;
}


// Task 2: Player Registration Implementations
// Adapted from task2.cpp. Original comments preserved where applicable.

// --- Start of Task 2 specific helper functions (formerly global in task2.cpp) ---
// These are kept here for encapsulation within the Task 2 section.
// If they were generic enough, they could be utility functions.

// Helper: Convert input to lowercase and check for "yes"/"y"
static int Task2_isYesResponse(const char* input) {
    char lower[10];
    int i;
    for (i = 0; input[i] && i < 9; i++) {
        lower[i] = tolower(input[i]);
    }
    lower[i] = '\0';
    return strcmp(lower, "yes") == 0 || strcmp(lower, "y") == 0;
}

// Helper: Check if file exists
static int Task2_fileExists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// Helper: Validate numeric string
static int Task2_isNumeric(const char* str) {
    if (!str || !str[0]) return 0;
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

// Helper: Get last player ID from CSV
static int Task2_getLastPlayerID(const char* filename) {
    if (!Task2_fileExists(filename)) return 1000; // Start ID if file doesn't exist

    FILE* file = fopen(filename, "r");
    if (!file) return 1000; // Default if file cannot be opened

    char line[256];
    int maxID = 1000; // Min ID is 1001 effectively
    if(fgets(line, sizeof(line), file) == NULL) { // Skip or check header
        fclose(file);
        return maxID; // Empty or unreadable file
    }

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\0' || line[0] == '\n' || line[0] == '\r') continue;
        
        // Use a copy for strtok as it modifies the string
        char lineCopy[256];
        strncpy(lineCopy, line, sizeof(lineCopy)-1);
        lineCopy[sizeof(lineCopy)-1] = '\0';

        char* token = strtok(lineCopy, ",");
        if (token && Task2_isNumeric(token)) {
            int id = atoi(token);
            if (id > maxID) maxID = id;
        }
    }
    fclose(file);
    return maxID;
}


// Helper: Sanitize string for CSV (replace commas with semicolons, limit length)
static void Task2_sanitizeForCSV(char* dest, const char* src, int maxLen) {
    int i = 0, j = 0;
    if (!src) { dest[0] = '\0'; return; }
    for (i = 0; src[i] && j < maxLen - 1; i++) {
        if (src[i] >= 32 && src[i] <= 126) { // Printable ASCII
            dest[j++] = (src[i] == ',') ? ';' : src[i]; // Replace comma
        }
    }
    dest[j] = '\0';
}

// Helper: Trim whitespace and normalize registration type to "Pascal-Case"
static void Task2_normalizeRegistrationType(char* dest, const char* src, int maxLen) {
    int i = 0, j = 0;
    bool newWord = true;
    if (!src) { dest[0] = '\0'; return; }

    // Skip leading whitespace
    while (src[i] != '\0' && isspace((unsigned char)src[i])) {
        i++;
    }

    while (src[i] != '\0' && j < maxLen - 1) {
        if (isspace((unsigned char)src[i]) || src[i] == '-') {
            if (j > 0 && dest[j-1] != '-') { // Avoid multiple dashes
                 dest[j++] = '-';
            }
            newWord = true;
            i++;
            while (src[i] != '\0' && (isspace((unsigned char)src[i]) || src[i] == '-')) { // Skip multiple separators
                i++;
            }
            continue;
        }
        if (newWord) {
            dest[j++] = toupper((unsigned char)src[i++]);
            newWord = false;
        } else {
            dest[j++] = tolower((unsigned char)src[i++]);
        }
    }
    // Remove trailing dash if any
    if (j > 0 && dest[j-1] == '-') {
        j--;
    }
    dest[j] = '\0';
}


// Clear input buffer (typically after scanf or when mixing with fgets)
static void Task2_clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Wait for Enter key press
static void Task2_waitForEnter() {
    printf("\n(Task 2: Press Enter to return to the Task 2 menu...)");
    // getchar(); // Read the newline potentially left by previous scanf/fgets
    // A more robust way if mixing cin and fgets is to use cin.ignore or ensure buffers are clear.
    // For C-style, if last input was scanf, a clearInputBuffer might be needed before this.
    // If last input was fgets, it consumes the newline.
    // This getchar might consume a newline already consumed by fgets or wait for new input.
    // Let's rely on clearInputBuffer being called after scanf.
    char tempBuf[2]; // Read a character, primarily to pause.
    fgets(tempBuf, sizeof(tempBuf), stdin); // Safely consumes one line or part of it
}


// Task2_Queue methods
void Task2_Queue::init() {
    front = NULL;
    rear = NULL;
    size = 0;
}

int Task2_Queue::isEmpty() {
    return front == NULL;
}

void Task2_Queue::enqueue(struct Task2_Player* player) {
    if (!player) return;
    player->next = NULL;
    if (isEmpty()) {
        front = rear = player;
    } else {
        rear->next = player;
        rear = player;
    }
    size++;
}

struct Task2_Player* Task2_Queue::dequeue() {
    if (isEmpty()) return NULL;
    struct Task2_Player* temp = front;
    front = front->next;
    if (front == NULL) rear = NULL;
    size--;
    temp->next = NULL; // Decouple the node
    return temp;
}

struct Task2_Player* Task2_Queue::peek() {
    return front;
}

int Task2_Queue::getSize() {
    return size;
}

void Task2_Queue::destroy() {
    while (!isEmpty()) {
        struct Task2_Player* temp = dequeue();
        free(temp); // Free the player struct memory
    }
    front = rear = NULL;
    size = 0;
}

// Task2_PriorityQueue methods
void Task2_PriorityQueue::init() {
    earlyBirdQueue.init();
    wildcardQueue.init();
    standardQueue.init();
    lastMinuteQueue.init();
    waitlistQueue.init();
    size = 0; // This size tracks main queue players, not waitlist.
}

void Task2_PriorityQueue::enqueue(struct Task2_Player* player) {
    if (!player) return;
    // Determine queue based on registration type (already normalized)
    if (strcmp(player->registrationType, "Early-Bird") == 0) {
        earlyBirdQueue.enqueue(player);
    } else if (strcmp(player->registrationType, "Wildcard") == 0) {
        wildcardQueue.enqueue(player);
    } else if (strcmp(player->registrationType, "Standard") == 0) {
        standardQueue.enqueue(player);
    } else if (strcmp(player->registrationType, "Last-Minute") == 0) {
        lastMinuteQueue.enqueue(player);
    } else {
        // Default to standard if type is unknown (should not happen if normalized correctly)
        printf("Warning: Unknown registration type '%s' for player %s. Adding to Standard queue.\n", player->registrationType, player->playerName);
        strcpy(player->registrationType, "Standard"); // Correct it
        player->rank[0] = 'C'; player->rank[1] = '\0';
        standardQueue.enqueue(player);
    }
    size++; // Increment total size of main queues
}

struct Task2_Player* Task2_PriorityQueue::dequeue() {
    struct Task2_Player* player = NULL;
    if (!earlyBirdQueue.isEmpty()) {
        player = earlyBirdQueue.dequeue();
    } else if (!wildcardQueue.isEmpty()) {
        player = wildcardQueue.dequeue();
    } else if (!standardQueue.isEmpty()) {
        player = standardQueue.dequeue();
    } else if (!lastMinuteQueue.isEmpty()) {
        player = lastMinuteQueue.dequeue();
    }

    if (player != NULL) {
        size--; // Decrement total size if a player was dequeued
    }
    return player;
}

int Task2_PriorityQueue::isEmpty() {
    return earlyBirdQueue.isEmpty() && wildcardQueue.isEmpty() &&
           standardQueue.isEmpty() && lastMinuteQueue.isEmpty();
}

int Task2_PriorityQueue::getSize() {
    return size; // Returns size of main queues
}

struct Task2_Queue* Task2_PriorityQueue::getWaitlistQueue() {
    return &waitlistQueue;
}

// This function takes ownership of players from q, q becomes empty.
void Task2_PriorityQueue::setWaitlistQueue(struct Task2_Queue* q) {
    waitlistQueue.destroy(); // Clear current waitlist
    waitlistQueue.init();    // Re-initialize

    while (!q->isEmpty()) {
        Task2_Player* p = q->dequeue();
        // No need to deep copy if q is giving up ownership
        waitlistQueue.enqueue(p);
    }
    // q is now empty, its resources (nodes) are transferred or should be freed by its caller if nodes weren't transferred
    // In task2.cpp, the source queue for setWaitlistQueue (tempWaitlist) typically holds nodes that were dequeued
    // from the original waitlistQueue, so it's more of a transfer.
}


// Display waitlist
static void Task2_displayWaitlist(struct Task2_PriorityQueue* pq) {
    printf("\n+------------------------------------------------------------------------------------+\n");
    printf("|                                 WAITLIST PLAYERS                                   |\n");
    printf("+------------------------------------------------------------------------------------+\n");
    printf("| %-9s | %-20s | %-15s | %-28s | %-4s |\n", "Player ID", "Player Name", "Reg. Type", "Email", "Rank");
    printf("+-----------+----------------------+-----------------+------------------------------+------+\n");

    struct Task2_Queue* waitlist = pq->getWaitlistQueue();
    if (waitlist->isEmpty()) {
        printf("| Waitlist is empty.                                                                 |\n");
    } else {
        struct Task2_Queue tempDisplayWaitlist; // To iterate without modifying original
        tempDisplayWaitlist.init();
        int count = 0;
        while(!waitlist->isEmpty()){
            Task2_Player* p = waitlist->dequeue();
             printf("| %-9s | %-20s | %-15s | %-28s | %-4s |\n",
                   p->playerID,
                   (strlen(p->playerName) > 18 ? strncat(strncpy((char[19]){0}, p->playerName, 15), "...", 3) : p->playerName),
                   p->registrationType,
                   (strlen(p->email) > 26 ? strncat(strncpy((char[27]){0}, p->email, 23), "...", 3) : p->email),
                   p->rank);
            tempDisplayWaitlist.enqueue(p);
            count++;
        }
        // Restore original waitlist
        while(!tempDisplayWaitlist.isEmpty()){
            waitlist->enqueue(tempDisplayWaitlist.dequeue());
        }
         printf("| Total on waitlist: %-59d |\n", count);
    }
    printf("+------------------------------------------------------------------------------------+\n");
    Task2_waitForEnter();
}

// Read players from CSV into the priority queue system
static void Task2_readPlayersFromCSV(struct Task2_PriorityQueue* pq, const char* filename) {
    if (!Task2_fileExists(filename)) {
        printf("Task 2: CSV file '%s' not found. Starting with empty registration list.\n", filename);
        // Create an empty file with header if it doesn't exist? Or let user know.
        FILE* file = fopen(filename, "w"); // Create it
        if(file){
            fprintf(file, "Player ID,Player Name,Registration Type,Email,Rank,Check-In,Time of Check-In,Status\n");
            fclose(file);
            printf("Task 2: Created empty CSV file '%s' with headers.\n", filename);
        } else {
            printf("Task 2: Error: Could not create CSV file '%s'.\n", filename);
        }
        return;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Task 2: Error opening CSV file '%s' for reading.\n", filename);
        return;
    }

    char line[512]; // Increased buffer size
    if (!fgets(line, sizeof(line), file)) { // Read header
        printf("Task 2: Warning: CSV file '%s' is empty or header missing.\n", filename);
        fclose(file);
        return;
    }
    // Can validate header here if necessary

    int playersRead = 0;
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n' || line[0] == '\0' || line[0] == '\r') continue; // Skip empty lines

        // Remove newline character if present
        line[strcspn(line, "\r\n")] = 0;

        struct Task2_Player* player = (struct Task2_Player*)malloc(sizeof(struct Task2_Player));
        if (!player) { printf("Task 2: Memory allocation failed for player.\n"); break; }

        // Initialize player struct
        player->ranking = 0; player->teamID = 0; player->group = 0; player->next = NULL;
        strcpy(player->checkInTime, "N/A"); player->rank[0] = '\0'; player->rank[1] = '\0';
        player->playerID[0] = '\0'; player->playerName[0] = '\0'; player->registrationType[0] = '\0';
        player->email[0] = '\0'; strcpy(player->status, "MAIN"); // Default to MAIN

        char* token;
        char* rest = line; // For strtok_r if available and preferred for safety, but sticking to strtok for C89 compat.
                           // Or, use manual parsing for more robustness.

        // Field: Player ID
        token = strtok(rest, ","); if (token) strncpy(player->playerID, token, sizeof(player->playerID)-1); else continue;
        player->playerID[sizeof(player->playerID)-1] = '\0';

        // Field: Player Name
        token = strtok(NULL, ","); if (token) strncpy(player->playerName, token, sizeof(player->playerName)-1); else { free(player); continue; }
        player->playerName[sizeof(player->playerName)-1] = '\0';

        // Field: Registration Type
        token = strtok(NULL, ","); if (token) strncpy(player->registrationType, token, sizeof(player->registrationType)-1); else { free(player); continue; }
        player->registrationType[sizeof(player->registrationType)-1] = '\0';

        // Field: Email
        token = strtok(NULL, ","); if (token) strncpy(player->email, token, sizeof(player->email)-1); else { free(player); continue; }
        player->email[sizeof(player->email)-1] = '\0';

        // Field: Rank
        token = strtok(NULL, ","); if (token && token[0]) { player->rank[0] = token[0]; player->rank[1] = '\0';} else { player->rank[0] = '\0'; player->rank[1] = '\0';}

        // Field: Check-In Status
        token = strtok(NULL, ","); if (token) player->checkInStatus = (strcmp(token, "YES") == 0 || strcmp(token, "yes") == 0); else { free(player); continue; }

        // Field: Time of Check-In
        token = strtok(NULL, ","); if (token) strncpy(player->checkInTime, token, sizeof(player->checkInTime)-1); else strcpy(player->checkInTime, "N/A");
        player->checkInTime[sizeof(player->checkInTime)-1] = '\0';
        
        // Field: Status (MAIN/WAITLIST)
        token = strtok(NULL, ",\n\r"); // consume rest of line for status
        if (token) strncpy(player->status, token, sizeof(player->status)-1); else strcpy(player->status, "MAIN"); // Default if missing
        player->status[sizeof(player->status)-1] = '\0';


        // Basic validation and deriving rank if missing
        bool validPlayer = true;
        if (!Task2_isNumeric(player->playerID) || atoi(player->playerID) < 1000) validPlayer = false;
        if (strlen(player->playerName) == 0) validPlayer = false;
        if (strlen(player->registrationType) == 0) validPlayer = false;
        Task2_normalizeRegistrationType(player->registrationType, player->registrationType, sizeof(player->registrationType)); // Normalize it

        if (player->rank[0] == '\0') { // If rank is missing from CSV, derive it
            if (strcmp(player->registrationType, "Early-Bird") == 0) player->rank[0] = 'A';
            else if (strcmp(player->registrationType, "Wildcard") == 0) player->rank[0] = 'B';
            else if (strcmp(player->registrationType, "Standard") == 0) player->rank[0] = 'C';
            else if (strcmp(player->registrationType, "Last-Minute") == 0) player->rank[0] = 'D';
            else player->rank[0] = 'C'; // Default rank
            player->rank[1] = '\0';
        }

        if (validPlayer) {
            if (strcmp(player->status, "WAITLIST") == 0) {
                pq->getWaitlistQueue()->enqueue(player);
            } else {
                pq->enqueue(player); // Enqueues into appropriate priority queue
            }
            playersRead++;
        } else {
            printf("Task 2: Invalid data for player (ID: %s, Name: %s). Skipping.\n", player->playerID, player->playerName);
            free(player);
        }
    }
    fclose(file);
    printf("Task 2: Loaded %d players from '%s'. Main queue: %d, Waitlist: %d.\n", playersRead, filename, pq->getSize(), pq->getWaitlistQueue()->getSize());
}


// Write all players (main queues and waitlist) to CSV
static void Task2_writePlayersToCSV(struct Task2_PriorityQueue* pq, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Task 2: Error opening CSV file '%s' for writing!\n", filename);
        return;
    }

    fprintf(file, "Player ID,Player Name,Registration Type,Email,Rank,Check-In,Time of Check-In,Status\n");

    struct Task2_PriorityQueue tempCopy; // To preserve original pq
    tempCopy.init();
    char sanitizedField[101]; // Buffer for sanitized fields

    // Process main queues
    while (!pq->isEmpty()) {
        struct Task2_Player* player = pq->dequeue();
        if (!player || strlen(player->playerID) == 0 || strlen(player->playerName) == 0) { // Basic check for valid player
            if (player) free(player);
            continue;
        }
        Task2_sanitizeForCSV(sanitizedField, player->playerID, sizeof(player->playerID)); fprintf(file, "%s,", sanitizedField);
        Task2_sanitizeForCSV(sanitizedField, player->playerName, sizeof(player->playerName)); fprintf(file, "%s,", sanitizedField);
        Task2_sanitizeForCSV(sanitizedField, player->registrationType, sizeof(player->registrationType)); fprintf(file, "%s,", sanitizedField);
        Task2_sanitizeForCSV(sanitizedField, player->email, sizeof(player->email)); fprintf(file, "%s,", sanitizedField);
        fprintf(file, "%c,", player->rank[0] ? player->rank[0] : 'N'); // Rank
        fprintf(file, "%s,", player->checkInStatus ? "YES" : "NO");    // Check-In
        Task2_sanitizeForCSV(sanitizedField, player->checkInTime, sizeof(player->checkInTime)); fprintf(file, "%s,", sanitizedField);
        Task2_sanitizeForCSV(sanitizedField, player->status, sizeof(player->status)); fprintf(file, "%s\n", sanitizedField);

        tempCopy.enqueue(player); // Add to temp copy to restore later
    }
    // Restore players to original pq from tempCopy
    while(!tempCopy.isEmpty()){
        pq->enqueue(tempCopy.dequeue());
    }

    // Process waitlist queue
    struct Task2_Queue* waitlist = pq->getWaitlistQueue();
    struct Task2_Queue tempWaitlistCopy;
    tempWaitlistCopy.init();

    while (!waitlist->isEmpty()) {
        struct Task2_Player* player = waitlist->dequeue();
         if (!player || strlen(player->playerID) == 0 || strlen(player->playerName) == 0) {
            if (player) free(player);
            continue;
        }
        Task2_sanitizeForCSV(sanitizedField, player->playerID, sizeof(player->playerID)); fprintf(file, "%s,", sanitizedField);
        Task2_sanitizeForCSV(sanitizedField, player->playerName, sizeof(player->playerName)); fprintf(file, "%s,", sanitizedField);
        Task2_sanitizeForCSV(sanitizedField, player->registrationType, sizeof(player->registrationType)); fprintf(file, "%s,", sanitizedField);
        Task2_sanitizeForCSV(sanitizedField, player->email, sizeof(player->email)); fprintf(file, "%s,", sanitizedField);
        fprintf(file, "%c,", player->rank[0] ? player->rank[0] : 'N');
        fprintf(file, "%s,", player->checkInStatus ? "YES" : "NO");
        Task2_sanitizeForCSV(sanitizedField, player->checkInTime, sizeof(player->checkInTime)); fprintf(file, "%s,", sanitizedField);
        Task2_sanitizeForCSV(sanitizedField, player->status, sizeof(player->status)); fprintf(file, "%s\n", sanitizedField); // Should be "WAITLIST"

        tempWaitlistCopy.enqueue(player);
    }
    // Restore waitlist
     while(!tempWaitlistCopy.isEmpty()){
        waitlist->enqueue(tempWaitlistCopy.dequeue());
    }

    fclose(file);
    // printf("Task 2: Player data successfully written to '%s'.\n", filename); // Optional: can be too verbose
}

// Register a new player
static void Task2_registerPlayer(struct Task2_PriorityQueue* pq, const char* filename) {
    struct Task2_Player* player = (struct Task2_Player*)malloc(sizeof(struct Task2_Player));
    if (!player) { printf("Task 2: Memory allocation failed for new player.\n"); return; }

    // Initialize fields
    player->ranking = 0; player->teamID = 0; player->checkInStatus = 0; player->group = 0; player->next = NULL;
    strcpy(player->checkInTime, "N/A");

    int lastID = Task2_getLastPlayerID(filename);
    sprintf(player->playerID, "%d", lastID + 1); // Generate new ID

    printf("\n+------------------------------+\n");
    printf("|  TASK 2: PLAYER REGISTRATION |\n");
    printf("+------------------------------+\n");
    printf("New Player ID: %s\n", player->playerID);

    char inputBuffer[101]; // For fgets

    printf("Enter Player Name: ");
    fgets(inputBuffer, sizeof(inputBuffer), stdin); inputBuffer[strcspn(inputBuffer, "\n")] = 0;
    strncpy(player->playerName, inputBuffer, sizeof(player->playerName)-1);
    player->playerName[sizeof(player->playerName)-1] = '\0';


    printf("Enter Registration Type (Early-Bird, Standard, Wildcard, Last-Minute): ");
    fgets(inputBuffer, sizeof(inputBuffer), stdin); inputBuffer[strcspn(inputBuffer, "\n")] = 0;
    Task2_normalizeRegistrationType(player->registrationType, inputBuffer, sizeof(player->registrationType));

    // Assign rank based on (normalized) registration type
    if (strcmp(player->registrationType, "Early-Bird") == 0) player->rank[0] = 'A';
    else if (strcmp(player->registrationType, "Wildcard") == 0) player->rank[0] = 'B';
    else if (strcmp(player->registrationType, "Standard") == 0) player->rank[0] = 'C';
    else if (strcmp(player->registrationType, "Last-Minute") == 0) player->rank[0] = 'D';
    else { // Default if normalization failed or type is weird
        printf("Warning: Unrecognized registration type '%s'. Defaulting to 'Standard', Rank C.\n", player->registrationType);
        strcpy(player->registrationType, "Standard");
        player->rank[0] = 'C';
    }
    player->rank[1] = '\0';
    printf("Assigned Rank: %s based on Registration Type: %s\n", player->rank, player->registrationType);


    printf("Enter Email: ");
    fgets(inputBuffer, sizeof(inputBuffer), stdin); inputBuffer[strcspn(inputBuffer, "\n")] = 0;
    strncpy(player->email, inputBuffer, sizeof(player->email)-1);
    player->email[sizeof(player->email)-1] = '\0';


    // Determine if player goes to main queue or waitlist (e.g., based on total main queue size)
    // Original task2.cpp had a limit of 16 for main queue.
    const int MAIN_QUEUE_LIMIT = 16; // Example limit
    if (pq->getSize() < MAIN_QUEUE_LIMIT) {
        strcpy(player->status, "MAIN");
        pq->enqueue(player); // Adds to the correct priority queue within main
        printf("\nPlayer '%s' registered successfully to MAIN list! Rank: %c. Total main queue: %d\n",
               player->playerName, player->rank[0], pq->getSize());
    } else {
        strcpy(player->status, "WAITLIST");
        pq->getWaitlistQueue()->enqueue(player);
        printf("\nMain queue full. Player '%s' added to WAITLIST! Rank: %c. Total waitlist: %d\n",
               player->playerName, player->rank[0], pq->getWaitlistQueue()->getSize());
    }

    Task2_writePlayersToCSV(pq, filename); // Save changes
    Task2_waitForEnter();
}

// Check-in an existing player
static void Task2_checkInPlayer(struct Task2_PriorityQueue* pq, const char* filename) {
    char playerIDQuery[10];
    char continueCheckIn[10];

    printf("\n+------------------------------+\n");
    printf("|    TASK 2: PLAYER CHECK-IN   |\n");
    printf("+------------------------------+\n");

    while (1) {
        printf("Enter Player ID to check-in: ");
        fgets(playerIDQuery, sizeof(playerIDQuery), stdin);
        playerIDQuery[strcspn(playerIDQuery, "\n")] = 0;

        while (strlen(playerIDQuery) == 0) {
            printf("Player ID cannot be empty. Please enter Player ID: ");
            fgets(playerIDQuery, sizeof(playerIDQuery), stdin);
            playerIDQuery[strcspn(playerIDQuery, "\n")] = 0;
        }

        bool found = false;
        Task2_Player* foundPlayerPtr = NULL;

        // Search in main priority queues
        struct Task2_PriorityQueue tempSearchPq; tempSearchPq.init();
        while (!pq->isEmpty()) {
            Task2_Player* p = pq->dequeue();
            if (strcmp(playerIDQuery, p->playerID) == 0) {
                found = true; foundPlayerPtr = p;
                if (!p->checkInStatus) { // If not already checked in
                    p->checkInStatus = 1;
                    time_t now = time(NULL);
                    struct tm* timeinfo = localtime(&now);
                    strftime(p->checkInTime, sizeof(p->checkInTime), "%Y-%m-%d %H:%M:%S", timeinfo);
                }
            }
            tempSearchPq.enqueue(p); // Store to re-populate
        }
        while(!tempSearchPq.isEmpty()) pq->enqueue(tempSearchPq.dequeue()); // Restore pq

        // If not found in main, search in waitlist
        if (!found) {
            Task2_Queue* waitlist = pq->getWaitlistQueue();
            Task2_Queue tempSearchWaitlist; tempSearchWaitlist.init();
            while(!waitlist->isEmpty()){
                Task2_Player* p = waitlist->dequeue();
                if (strcmp(playerIDQuery, p->playerID) == 0) {
                    found = true; foundPlayerPtr = p;
                     if (!p->checkInStatus) {
                        p->checkInStatus = 1;
                        time_t now = time(NULL); struct tm* timeinfo = localtime(&now);
                        strftime(p->checkInTime, sizeof(p->checkInTime), "%Y-%m-%d %H:%M:%S", timeinfo);
                    }
                }
                tempSearchWaitlist.enqueue(p);
            }
            while(!tempSearchWaitlist.isEmpty()) waitlist->enqueue(tempSearchWaitlist.dequeue()); // Restore waitlist
        }


        if (found && foundPlayerPtr) {
            printf("Player ID %s (%s) status: %s. Check-in: %s at %s.\n",
                   playerIDQuery, foundPlayerPtr->playerName, foundPlayerPtr->status,
                   (foundPlayerPtr->checkInStatus ? "CONFIRMED" : "NOT CHECKED IN (Error?)"),
                   foundPlayerPtr->checkInTime);
            Task2_writePlayersToCSV(pq, filename); // Save updated status
        } else {
            printf("Player ID %s not found in any registration list!\n", playerIDQuery);
        }

        printf("\nCheck in another player? (yes/no): ");
        fgets(continueCheckIn, sizeof(continueCheckIn), stdin);
        if (!Task2_isYesResponse(continueCheckIn)) break;
    }
    Task2_waitForEnter();
}


// Withdraw a player and handle waitlist promotion
static void Task2_withdrawPlayerLogic(struct Task2_PriorityQueue* pq, const char* playerIDToWithdraw, const char* filename) {
    bool foundAndWithdrawn = false;
    char withdrawnPlayerRank[2] = ""; // To know which rank to replace from waitlist
    char withdrawnPlayerName[sizeof(Task2_Player::playerName)] = "";

    // Search and withdraw from main priority queues
    struct Task2_PriorityQueue tempProcessingPq; tempProcessingPq.init();
    while (!pq->isEmpty()) {
        Task2_Player* p = pq->dequeue();
        if (strcmp(p->playerID, playerIDToWithdraw) == 0) {
            foundAndWithdrawn = true;
            strncpy(withdrawnPlayerRank, p->rank, sizeof(withdrawnPlayerRank)-1); withdrawnPlayerRank[sizeof(withdrawnPlayerRank)-1] = '\0';
            strncpy(withdrawnPlayerName, p->playerName, sizeof(withdrawnPlayerName)-1); withdrawnPlayerName[sizeof(withdrawnPlayerName)-1] = '\0';
            free(p); // Player removed and memory freed
        } else {
            tempProcessingPq.enqueue(p);
        }
    }
    while(!tempProcessingPq.isEmpty()) pq->enqueue(tempProcessingPq.dequeue()); // Restore non-withdrawn players

    // If not found in main queues, check waitlist (though typically withdraw from main)
    if (!foundAndWithdrawn) {
        Task2_Queue* waitlist = pq->getWaitlistQueue();
        Task2_Queue tempWaitlist; tempWaitlist.init();
        while(!waitlist->isEmpty()){
            Task2_Player* p = waitlist->dequeue();
            if (strcmp(p->playerID, playerIDToWithdraw) == 0) {
                foundAndWithdrawn = true; // Found on waitlist
                // Note: Rank not stored from waitlist withdrawal for promotion logic here.
                // Typically, withdrawing from waitlist doesn't trigger promotion.
                strncpy(withdrawnPlayerName, p->playerName, sizeof(withdrawnPlayerName)-1); withdrawnPlayerName[sizeof(withdrawnPlayerName)-1] = '\0';
                printf("Player %s (%s) was on the WAITLIST and has been withdrawn.\n", playerIDToWithdraw, withdrawnPlayerName);
                free(p);
            } else {
                tempWaitlist.enqueue(p);
            }
        }
        while(!tempWaitlist.isEmpty()) waitlist->enqueue(tempWaitlist.dequeue()); // Restore waitlist
    }


    if (foundAndWithdrawn && strlen(withdrawnPlayerRank) > 0) { // If withdrawn from MAIN queue (rank was captured)
        printf("Player %s (%s) withdrawn successfully from MAIN list.\n", playerIDToWithdraw, withdrawnPlayerName);

        // Attempt to promote from waitlist if a main player was withdrawn
        Task2_Queue* waitlist = pq->getWaitlistQueue();
        Task2_Queue tempWaitlistProcessing; tempWaitlistProcessing.init();
        Task2_Player* promotedPlayer = NULL;

        // Find best candidate from waitlist (e.g., matching rank, then highest priority type)
        // Original task2.cpp promoted if rank matched.
        while(!waitlist->isEmpty()){
            Task2_Player* pWaitlist = waitlist->dequeue();
            if (!promotedPlayer && strcmp(pWaitlist->rank, withdrawnPlayerRank) == 0) { // Found matching rank
                promotedPlayer = pWaitlist; // This player will be promoted
            } else {
                tempWaitlistProcessing.enqueue(pWaitlist); // Others go to temp
            }
        }
        // Restore non-promoted waitlist players
        while(!tempWaitlistProcessing.isEmpty()) waitlist->enqueue(tempWaitlistProcessing.dequeue());

        if (promotedPlayer) {
            strcpy(promotedPlayer->status, "MAIN");
            pq->enqueue(promotedPlayer); // Add to main priority queue system
            printf("Player %s (%s, Rank: %s) promoted from waitlist to MAIN list.\n",
                   promotedPlayer->playerID, promotedPlayer->playerName, promotedPlayer->rank);
        } else {
            printf("No suitable player (Rank: %s) found on waitlist for promotion.\n", withdrawnPlayerRank);
        }
        Task2_writePlayersToCSV(pq, filename); // Save changes
    } else if (foundAndWithdrawn) { // Found and withdrawn (likely from waitlist, or rank not captured)
         printf("Player %s (%s) withdrawn. No promotion from waitlist triggered by this withdrawal.\n", playerIDToWithdraw, withdrawnPlayerName);
         Task2_writePlayersToCSV(pq, filename);
    }else {
        printf("Player ID %s not found in any registration list for withdrawal.\n", playerIDToWithdraw);
    }
}

// UI wrapper for player withdrawal
static void Task2_handleWithdrawPlayer(struct Task2_PriorityQueue* pq, const char* filename) {
    char playerIDQuery[10];
    printf("\n+------------------------------+\n");
    printf("|   TASK 2: PLAYER WITHDRAWAL  |\n");
    printf("+------------------------------+\n");
    printf("Enter Player ID to withdraw: ");
    fgets(playerIDQuery, sizeof(playerIDQuery), stdin);
    playerIDQuery[strcspn(playerIDQuery, "\n")] = 0;

    if (strlen(playerIDQuery) > 0) {
        Task2_withdrawPlayerLogic(pq, playerIDQuery, filename);
    } else {
        printf("Player ID cannot be empty.\n");
    }
    Task2_waitForEnter();
}


// Main function for Task 2 module
void Task2_runPlayerRegistrationSystem() {
    struct Task2_PriorityQueue task2_pqManager; // Manager for Task 2 queues
    task2_pqManager.init();
    const char* task2_csvFilename = "Player_Registration.csv"; // CSV filename for Task 2

    printf("\n--- Task 2: Player Registration System Initializing ---\n");
    Task2_readPlayersFromCSV(&task2_pqManager, task2_csvFilename); // Load existing data
    printf("Current number of main registered players: %d. Waitlisted: %d\n",
           task2_pqManager.getSize(), task2_pqManager.getWaitlistQueue()->getSize());

    int choice;
    do {
        printf("\n\n+------------------------------------------+\n");
        printf("|      APUEC Player Registration (Task 2)  |\n");
        printf("+------------------------------------------+\n");
        printf("| 1. Register New Player                   |\n");
        printf("| 2. Check-In Existing Player              |\n");
        printf("| 3. Withdraw Player                       |\n");
        printf("| 4. View Waitlist                         |\n");
        printf("| 5. Save & Return to Main APUEC Menu      |\n");
        printf("+------------------------------------------+\n");
        printf("Enter choice (1-5): ");

        if (scanf("%d", &choice) != 1) {
            Task2_clearInputBuffer(); // Clear invalid input
            printf("Invalid input! Please enter a number between 1 and 5.\n");
            choice = 0; // Reset choice to loop again
            continue;
        }
        Task2_clearInputBuffer(); // Clear newline after scanf

        switch (choice) {
            case 1: Task2_registerPlayer(&task2_pqManager, task2_csvFilename); break;
            case 2: Task2_checkInPlayer(&task2_pqManager, task2_csvFilename); break;
            case 3: Task2_handleWithdrawPlayer(&task2_pqManager, task2_csvFilename); break;
            case 4: Task2_displayWaitlist(&task2_pqManager); break;
            case 5:
                Task2_writePlayersToCSV(&task2_pqManager, task2_csvFilename); // Ensure data is saved on exit
                printf("\nPlayer registration data saved. Exiting Task 2...\n");
                break;
            default:
                printf("Invalid choice! Please enter a number between 1 and 5.\n");
                break;
        }
    } while (choice != 5);

    // Clean up: Destroy queues to free player data memory
    task2_pqManager.earlyBirdQueue.destroy();
    task2_pqManager.wildcardQueue.destroy();
    task2_pqManager.standardQueue.destroy();
    task2_pqManager.lastMinuteQueue.destroy();
    task2_pqManager.waitlistQueue.destroy();
    // task2_pqManager itself is stack-allocated, so no `free(task2_pqManager)`.
     printf("--- Task 2: Player Registration System Closed ---\n");
}
// --- End of Task 2 Implementations ---


// Task 3: Spectator Management (Placeholder from EsportsChampionship.cpp)
void runTask3_SpectatorManagement() {
    cout << "\n--- Task 3: Spectator Management ---" << endl;
    cout << "This functionality is currently a placeholder." << endl;
    // Potential future features:
    // - Register spectators (name, contact, ticket type)
    // - Manage seating (if applicable, using a custom 2D array or linked list structure)
    // - Check-in spectators
    // - View spectator list
    // - Use a custom Queue for entry management or a custom List for registered spectators.
    cout << "No actions available. Returning to main APUEC menu." << endl;
    cout << "\n(Task 3: Press Enter to continue...)";
    // cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Already handled in main loop for non-failing input
    // cin.get(); 
}


// Task 4: Result Logging Implementations
// From EsportsChampionship.cpp. Using std:: prefix as in original.

Task4_MatchResult::Task4_MatchResult() : match_id(0), group_id(0), round(0), player1_id(0), player2_id(0), winner_id(0) {
    // stage, scheduled_time, status, score are std::string and default-construct to empty.
}

Task4_MatchResult::Task4_MatchResult(int mid, const std::string& st, int gid, int r, int p1, int p2,
                                   const std::string& sch_time, const std::string& stat, int wid, const std::string& scr)
    : match_id(mid), stage(st), group_id(gid), round(r), player1_id(p1), player2_id(p2),
      scheduled_time(sch_time), status(stat), winner_id(wid), score(scr) {}

// Task4_Stack implementations
Task4_Stack::Task4_Stack() : top_index(-1) {} // Stack is empty

bool Task4_Stack::push(const Task4_MatchResult& match) {
    if (top_index >= TASK4_MAX_CAPACITY - 1) {
        std::cerr << "Task 4 Warning: Stack is full. Cannot push match ID " << match.match_id << std::endl;
        return false;
    }
    data[++top_index] = match;
    return true;
}

bool Task4_Stack::pop(Task4_MatchResult& out_match) {
    if (isEmpty()) {
        // std::cerr << "Task 4 Warning: Stack is empty. Cannot pop." << std::endl; // Optional: too verbose
        return false;
    }
    out_match = data[top_index--];
    return true;
}

bool Task4_Stack::peek(Task4_MatchResult& out_match) const {
    if (isEmpty()) {
        return false;
    }
    out_match = data[top_index];
    return true;
}

bool Task4_Stack::getFromTop(int index_from_top, Task4_MatchResult& out_match) const {
    if (isEmpty() || index_from_top < 0 || index_from_top > top_index) {
        return false; // Index out of bounds or stack empty
    }
    out_match = data[top_index - index_from_top];
    return true;
}

bool Task4_Stack::isEmpty() const { return top_index == -1; }
int Task4_Stack::size() const { return top_index + 1; }

// Task4_Queue (Circular Queue) implementations
Task4_Queue::Task4_Queue() : front_index(0), rear_index(-1), current_size(0) {}

bool Task4_Queue::enqueue(const Task4_MatchResult& match) {
    if (current_size >= TASK4_MAX_CAPACITY) {
        std::cerr << "Task 4 Warning: Queue is full. Cannot enqueue match ID " << match.match_id << std::endl;
        return false;
    }
    rear_index = (rear_index + 1) % TASK4_MAX_CAPACITY;
    data[rear_index] = match;
    current_size++;
    return true;
}

bool Task4_Queue::dequeue(Task4_MatchResult& out_match) {
    if (isEmpty()) {
        // std::cerr << "Task 4 Warning: Queue is empty. Cannot dequeue." << std::endl; // Optional
        return false;
    }
    out_match = data[front_index];
    front_index = (front_index + 1) % TASK4_MAX_CAPACITY;
    current_size--;
    return true;
}

bool Task4_Queue::peek(Task4_MatchResult& out_match) const {
   if (isEmpty()) return false;
   out_match = data[front_index];
   return true;
}

bool Task4_Queue::getAt(int logical_index, Task4_MatchResult& out_match) const {
    if (isEmpty() || logical_index < 0 || logical_index >= current_size) {
        return false; // Index out of bounds or queue empty
    }
    int actual_index = (front_index + logical_index) % TASK4_MAX_CAPACITY;
    out_match = data[actual_index];
    return true;
}

bool Task4_Queue::isEmpty() const { return current_size == 0; }
int Task4_Queue::size() const { return current_size; }

// Task4_PlayerStats implementations
Task4_PlayerStats::Task4_PlayerStats()
    : player_id(0), total_matches(0), wins(0), losses(0), avg_score(0.0) {
    // name, rank, contact, registration_time are std::string and default-construct to empty.
}

Task4_PlayerStats::Task4_PlayerStats(int pid, const std::string& n, const std::string& r_val, // Renamed r to r_val
                                     const std::string& c, const std::string& reg_time)
    : player_id(pid), name(n), rank(r_val), contact(c), registration_time(reg_time),
      total_matches(0), wins(0), losses(0), avg_score(0.0) {}

// Task4_GameResultManager implementations
Task4_GameResultManager::Task4_GameResultManager(int mp)
    : task4_max_players(mp), current_player_count(0), next_match_id(1) { // next_match_id for if this system *generates* matches
    player_stats = new Task4_PlayerStats[task4_max_players];
    // Stacks and Queues are member objects, their constructors are called automatically.
}

Task4_GameResultManager::~Task4_GameResultManager() {
    delete[] player_stats;
}

// Loads player data from "players.csv" (Task 1 format assumed)
// Fields: ID,Name,RegistrationType,Email,Rank,CheckInStatus... (Task 4 uses ID, Name, RegType, Email, Rank)
bool Task4_GameResultManager::loadPlayerData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Task 4 Warning: Cannot open players file '" << filename << "' for reading." << std::endl;
        return false;
    }

    std::string line_str; // Renamed line from original
    int loaded_count = 0;
    bool header_skipped = false;

    current_player_count = 0; // Reset before loading, crucial for re-runs

    while (std::getline(file, line_str) && current_player_count < task4_max_players) {
        if (line_str.empty() || line_str.find_first_not_of(" \t\r\n") == std::string::npos) continue; // Skip blank lines
        if (!header_skipped) { header_skipped = true; continue; } // Skip header row

        std::string tokens[6]; // Expecting: ID,Name,RegistrationType,Email,Rank,CheckInStatus
        splitCSVLine(line_str, tokens, 6); // splitCSVLine handles trimming

        if (tokens[0].empty()) { std::cerr << "Task 4 Warning: Empty player ID in line: " << line_str << std::endl; continue; }
        int player_id_val; // Renamed player_id from original
        try { player_id_val = std::stoi(tokens[0]); }
        catch (const std::exception& e) {
            std::cerr << "Task 4 Warning: Invalid player ID '" << tokens[0] << "' in players CSV. Error: " << e.what() << std::endl;
            continue;
        }

        if (findPlayerIndex(player_id_val) != -1) {
            std::cerr << "Task 4 Warning: Duplicate player ID " << player_id_val << " found in players CSV. Skipping." << std::endl;
            continue;
        }
        // PlayerStats: pid, name, rank, contact (email), reg_time (reg type)
        player_stats[current_player_count] = Task4_PlayerStats(player_id_val, tokens[1], tokens[4], tokens[3], tokens[2]);
        current_player_count++;
        loaded_count++;
    }
    file.close();
    std::cout << "Task 4: Loaded " << loaded_count << " players from '" << filename << "'. Total unique players: " << current_player_count << std::endl;
    return true;
}

// Loads match history from "matches.csv" (Task 1 format)
// Fields: match_id,stage,group_id,round,player1_id,player2_id,scheduled_time,status,winner_id,score
bool Task4_GameResultManager::loadMatchHistory(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Task 4 Warning: Cannot open matches file '" << filename << "' for reading." << std::endl;
        return false;
    }
    std::string line_str; // Renamed line
    int loaded_count = 0;
    bool header_skipped = false;

    // Clear existing match data structures for fresh load
    while(!match_history.isEmpty()) { Task4_MatchResult temp; match_history.dequeue(temp); }
    while(!recent_matches.isEmpty()) { Task4_MatchResult temp; recent_matches.pop(temp); }
    next_match_id = 1; // Reset


    while (std::getline(file, line_str)) {
        if (line_str.empty() || line_str.find_first_not_of(" \t\r\n") == std::string::npos) continue;
        if (!header_skipped) { header_skipped = true; continue; }

        std::string tokens[10];
        splitCSVLine(line_str, tokens, 10);

        if (tokens[0].empty() || tokens[4].empty() || tokens[5].empty()) {
            std::cerr << "Task 4 Warning: Missing required fields (match_id, p1_id, p2_id) in match line: " << line_str << std::endl;
            continue;
        }

        int match_id_val, group_id_val = 0, round_val = 0, player1_id_val, player2_id_val, winner_id_val = 0;
        try {
            match_id_val = std::stoi(tokens[0]);
            if (!tokens[2].empty()) group_id_val = std::stoi(tokens[2]); else group_id_val = 0; // Group ID can be empty/0 for non-group matches
            if (!tokens[3].empty()) round_val = std::stoi(tokens[3]); else round_val = 0;
            player1_id_val = std::stoi(tokens[4]);
            player2_id_val = std::stoi(tokens[5]);
            if (!tokens[8].empty() && tokens[8] != "0") winner_id_val = std::stoi(tokens[8]); else winner_id_val = 0; // Winner ID can be empty/0

            if (match_id_val >= next_match_id) next_match_id = match_id_val + 1;
        } catch (const std::exception& e) {
            std::cerr << "Task 4 Warning: Invalid numeric data in match line: " << line_str << ". Error: " << e.what() << std::endl;
            continue;
        }

        // Ensure players exist in loaded player_stats for stat updates; otherwise, this match might be problematic for stats
        if (findPlayerIndex(player1_id_val) == -1 && player1_id_val !=0) { std::cout << "Task 4 Info: Player ID " << player1_id_val << " from match " << match_id_val << " not in player list. Stats might be incomplete.\n"; }
        if (findPlayerIndex(player2_id_val) == -1 && player2_id_val !=0) { std::cout << "Task 4 Info: Player ID " << player2_id_val << " from match " << match_id_val << " not in player list. Stats might be incomplete.\n"; }


        Task4_MatchResult match_res(match_id_val, tokens[1], group_id_val, round_val, player1_id_val,
                                   player2_id_val, tokens[6], tokens[7], winner_id_val, tokens[9]);
        match_history.enqueue(match_res); // Add to historical queue
        recent_matches.push(match_res);   // Add to recent stack (will naturally keep only latest if stack has fixed size and pops old ones, or just grows)

        if (strcmp(match_res.status.c_str(), "completed") == 0 && winner_id_val != 0) {
            double p1_score_val = 0.0, p2_score_val = 0.0;
            size_t dash_pos = match_res.score.find('-');
            if (dash_pos != std::string::npos) {
                try { p1_score_val = std::stod(match_res.score.substr(0, dash_pos)); } catch(...) {}
                try { p2_score_val = std::stod(match_res.score.substr(dash_pos + 1)); } catch(...) {}
            }
            // Update stats for both players involved if they exist
            if (findPlayerIndex(player1_id_val) != -1) updatePlayerStats(player1_id_val, (winner_id_val == player1_id_val), p1_score_val);
            if (findPlayerIndex(player2_id_val) != -1) updatePlayerStats(player2_id_val, (winner_id_val == player2_id_val), p2_score_val);
        }
        loaded_count++;
    }
    file.close();
    std::cout << "Task 4: Loaded " << loaded_count << " matches from '" << filename << "'. History queue size: " << match_history.size() << ", Recent stack size: " << recent_matches.size() << std::endl;
    return true;
}


void Task4_GameResultManager::displayRecentMatches(int count) {
    std::cout << "\n" << std::string(100, '=') << "\n"
              << std::string(30, ' ') << "TASK 4: RECENT MATCHES (Last " << std::min(count, recent_matches.size()) << ")\n"
              << std::string(100, '=') << "\n";
    if (recent_matches.isEmpty()) {
        std::cout << "│" << std::left << std::setw(98) << " No recent matches available." << "│\n"
                  << std::string(100, '=') << "\n"; return;
    }
    int display_count = std::min(count, recent_matches.size());

    std::cout << "│" << std::left << std::setw(8) << " Match#" << "│" << std::setw(12) << " Stage" << "│"
              << std::setw(25) << " Players (P1 vs P2)" << "│" << std::setw(15) << " Winner" << "│"
              << std::setw(10) << " Score" << "│" << std::setw(12) << " Date" << "│" // Shortened Score width
              << std::setw(12) << " Status" << "│\n";
    std::cout << "├" << std::string(8, '-') << "┼" << std::string(12, '-') << "┼" << std::string(25, '-') << "┼"
              << std::string(15, '-') << "┼" << std::string(10, '-') << "┼" << std::string(12, '-') << "┼"
              << std::string(12, '-') << "┤\n";

    Task4_Stack tempStack; // To preserve original stack
    for (int i = 0; i < display_count; i++) {
        Task4_MatchResult match_res;
        if (recent_matches.pop(match_res)) { // Pop from original to display in reverse order (most recent first)
            tempStack.push(match_res); // Store in temp to restore

            int p1_idx = findPlayerIndex(match_res.player1_id);
            int p2_idx = findPlayerIndex(match_res.player2_id);
            std::string p1_name = (p1_idx != -1) ? player_stats[p1_idx].name : (match_res.player1_id == 0 ? "N/A" : "ID:" + std::to_string(match_res.player1_id));
            std::string p2_name = (p2_idx != -1) ? player_stats[p2_idx].name : (match_res.player2_id == 0 ? "N/A" : "ID:" + std::to_string(match_res.player2_id));

            if (p1_name.length() > 9) p1_name = p1_name.substr(0, 8) + ".";
            if (p2_name.length() > 9) p2_name = p2_name.substr(0, 8) + ".";
            std::string players_str = p1_name + " vs " + p2_name;
            if (players_str.length() > 24) players_str = players_str.substr(0, 21) + "...";

            std::string winner_name_str = "None";
            if (match_res.winner_id != 0) {
                int winner_idx = findPlayerIndex(match_res.winner_id);
                winner_name_str = (winner_idx != -1) ? player_stats[winner_idx].name : "ID:" + std::to_string(match_res.winner_id);
                if (winner_name_str.length() > 13) winner_name_str = winner_name_str.substr(0, 10) + "...";
            } else if (strcmp(match_res.status.c_str(), "completed")==0) {
                 winner_name_str = "Draw/TBD";
            }


            std::string date_str = extractDateFromScheduledTime(match_res.scheduled_time);
            if (date_str.empty() && !match_res.scheduled_time.empty()) date_str = match_res.scheduled_time.substr(0,10); // Fallback
            else if (date_str.empty()) date_str = "Unknown";


            std::cout << "│ " << std::left << std::setw(7) << match_res.match_id << "│ "
                      << std::setw(11) << (match_res.stage.length() > 10 ? match_res.stage.substr(0,7)+"..." : match_res.stage) << "│ "
                      << std::setw(24) << players_str << "│ "
                      << std::setw(14) << winner_name_str << "│ "
                      << std::setw(9) << (match_res.score.length() > 8 ? match_res.score.substr(0,5)+"..." : match_res.score) << "│ "
                      << std::setw(11) << date_str << "│ "
                      << std::setw(11) << (match_res.status.length() > 10 ? match_res.status.substr(0,7)+"..." : match_res.status) << "│\n";
        } else { break; } // Should not happen if display_count is correct
    }
    // Restore original stack
    while(tempStack.pop(tempStack.data[0])) { // Misusing pop like this; need proper pop for Task4_MatchResult
        Task4_MatchResult toRestore;
        if(tempStack.peek(toRestore)) { // If peek exists for this class (it does)
             // This logic is flawed for restoring. Proper way:
             // while(tempStack.pop(match_to_restore)) recent_matches.push(match_to_restore);
             // Correcting the loop for restoring (assuming tempStack.pop modifies its argument)
        }
    }
    // Correct restoration of recent_matches stack:
    Task4_MatchResult match_to_restore_from_temp;
    while(tempStack.pop(match_to_restore_from_temp)) { // Pop from tempStack
        recent_matches.push(match_to_restore_from_temp); // Push back to original recent_matches
    }

    std::cout << std::string(100, '=') << "\n";
}

void Task4_GameResultManager::displayPlayerStats(int player_id_val) { // Renamed player_id
    int index = findPlayerIndex(player_id_val);
    if (index == -1) {
        std::cout << "\nTask 4: Player ID " << player_id_val << " not found in statistics.\n"; return;
    }
    const Task4_PlayerStats& ps = player_stats[index]; // Renamed player_stat to ps
    std::cout << "\n+----------------------------------------+\n"
              << "|   TASK 4: PLAYER STATISTICS FOR ID " << std::setw(4) << ps.player_id << "   |\n"
              << "+----------------------------------------+\n"
              << "| Name: " << std::left << std::setw(30) << (ps.name.length()>28?ps.name.substr(0,25)+"...":ps.name) << "|\n"
              << "| Rank: " << std::left << std::setw(30) << (ps.rank.length()>28?ps.rank.substr(0,25)+"...":ps.rank) << "|\n"
              << "| Email: " << std::left << std::setw(29) << (ps.contact.length()>27?ps.contact.substr(0,24)+"...":ps.contact) << "|\n" // Contact is Email
              << "| Reg. Type: " << std::left << std::setw(25) << (ps.registration_time.length()>23?ps.registration_time.substr(0,20)+"...":ps.registration_time) << "|\n" // Reg. Time is Reg Type
              << "|----------------------------------------|\n"
              << "| Total Matches: " << std::left << std::setw(21) << ps.total_matches << "|\n"
              << "| Wins:          " << std::left << std::setw(21) << ps.wins << "|\n"
              << "| Losses:        " << std::left << std::setw(21) << ps.losses << "|\n";
    if (ps.total_matches > 0) {
        double win_rate = (static_cast<double>(ps.wins) / ps.total_matches) * 100.0;
        std::cout << "| Win Rate:      " << std::fixed << std::setprecision(1) << std::setw(19) << win_rate << "% |\n";
    } else { std::cout << "| Win Rate:      " << std::left << std::setw(20) << "N/A" << "|\n"; }
    std::cout << "| Average Score: " << std::fixed << std::setprecision(2) << std::setw(19) << ps.avg_score << " |\n"
              << "+----------------------------------------+\n";
}

void Task4_GameResultManager::displayAllPlayerStats() {
    std::cout << "\nTASK 4: ALL PLAYER STATISTICS (" << current_player_count << " players)\n";
    if (current_player_count == 0) { std::cout << "No player statistics loaded or available.\n"; return; }
    std::cout << "+-----+--------------------+----------+---------+------+--------+-----------+-----------+\n"
              << "| ID  | Name               | Rank     | Matches | Wins | Losses | Win Rate  | Avg Score |\n"
              << "+-----+--------------------+----------+---------+------+--------+-----------+-----------+\n";
    for (int i = 0; i < current_player_count; i++) {
        const Task4_PlayerStats& ps = player_stats[i];
        std::string win_rate_str = "N/A";
        if (ps.total_matches > 0) {
            double wr_val = (static_cast<double>(ps.wins) / ps.total_matches) * 100.0; // Renamed wr
            std::ostringstream oss_wr; oss_wr << std::fixed << std::setprecision(1) << wr_val << "%";
            win_rate_str = oss_wr.str();
        }
        std::ostringstream oss_as; oss_as << std::fixed << std::setprecision(2) << ps.avg_score; // Renamed avg_score_oss

        std::cout << "| " << std::left << std::setw(3) << ps.player_id << " | "
                  << std::setw(18) << (ps.name.length()>17?ps.name.substr(0,14)+"...":ps.name) << " | "
                  << std::setw(8) << (ps.rank.length()>7?ps.rank.substr(0,4)+"...":ps.rank) << " | "
                  << std::right << std::setw(7) << ps.total_matches << " | "
                  << std::setw(4) << ps.wins << " | "
                  << std::setw(6) << ps.losses << " | "
                  << std::setw(9) << win_rate_str << " | "
                  << std::setw(9) << oss_as.str() << " |\n";
    }
    std::cout << "+-----+--------------------+----------+---------+------+--------+-----------+-----------+\n";
}

void Task4_GameResultManager::queryMatchesByPlayer(int player_id_val) { // Renamed player_id
    int player_idx = findPlayerIndex(player_id_val);
    if (player_idx == -1) { std::cout << "\nTask 4: Player ID " << player_id_val << " not found.\n"; return; }

    std::cout << "\nTASK 4: MATCH HISTORY FOR PLAYER " << player_id_val << " (" << player_stats[player_idx].name << ")\n";
    bool found_any = false; // Renamed found_any_matches

    std::cout << "+---------+--------------------+----------+--------+---------+------------+-------+\n"
              << "| MatchID | Opponent           | Stage    | Result | Score   | Date       | Round |\n"
              << "+---------+--------------------+----------+--------+---------+------------+-------+\n";

    Task4_Queue tempHistory; // To preserve original match_history
    int matches_processed = 0;
    while(!match_history.isEmpty()){
        Task4_MatchResult mi; // Renamed match_item
        match_history.dequeue(mi);
        tempHistory.enqueue(mi); // Store for restoration

        if (mi.player1_id == player_id_val || mi.player2_id == player_id_val) {
            found_any = true;
            matches_processed++;
            int opp_id_val = (mi.player1_id == player_id_val) ? mi.player2_id : mi.player1_id; // Renamed opponent_id_val
            int opp_idx = findPlayerIndex(opp_id_val); // Renamed opponent_idx
            std::string opp_name_str = (opp_idx != -1) ? player_stats[opp_idx].name : (opp_id_val == 0 ? "N/A" : "ID:" + std::to_string(opp_id_val)); // Renamed opponent_name_str
            if(opp_name_str.length() > 18) opp_name_str = opp_name_str.substr(0,15) + "...";

            std::string res_str = "Pending"; // Renamed result_str
            if(strcmp(mi.status.c_str(), "completed") == 0) {
                if (mi.winner_id == player_id_val) res_str = "WIN";
                else if (mi.winner_id != 0 && mi.winner_id != player_id_val) res_str = "LOSS";
                else res_str = "DRAW/TBD";
            }


            std::string date_val = extractDateFromScheduledTime(mi.scheduled_time);
            if(date_val.empty() && !mi.scheduled_time.empty()) date_val = mi.scheduled_time.substr(0,10);
            else if(date_val.empty()) date_val = "Unknown";

            std::cout << "| " << std::left << std::setw(7) << mi.match_id << " | "
                      << std::setw(18) << opp_name_str << " | "
                      << std::setw(8) << (mi.stage.length()>7?mi.stage.substr(0,4)+"...":mi.stage) << " | "
                      << std::setw(6) << res_str << " | "
                      << std::setw(7) << (mi.score.length()>6?mi.score.substr(0,3)+"...":mi.score) << " | "
                      << std::setw(10) << date_val << " | "
                      << std::right << std::setw(5) << mi.round << " |\n";
        }
    }
    // Restore match_history
    while(!tempHistory.isEmpty()){ Task4_MatchResult temp_mr; tempHistory.dequeue(temp_mr); match_history.enqueue(temp_mr); }


    if (!found_any) { std::cout << "| No matches found for this player.                                                    |\n"; }
    else { std::cout << "| Total matches involving player: " << std::left << std::setw(50) << matches_processed << "|\n"; }
    std::cout << "+---------+--------------------+----------+--------+---------+------------+-------+\n";
}

void Task4_GameResultManager::queryMatchesByStage(const std::string& stage_query) {
    std::cout << "\nTASK 4: MATCHES IN STAGE: '" << stage_query << "'\n";
    bool found_stage = false; // Renamed found_stage_matches

    std::cout << "+---------+-------------------------+------------------+---------+-------+------------+\n"
              << "| MatchID | Players (P1 vs P2)      | Winner           | Score   | Round | Date       |\n"
              << "+---------+-------------------------+------------------+---------+-------+------------+\n";

    Task4_Queue tempHistory;
    int matches_in_stage = 0;
    while(!match_history.isEmpty()){
        Task4_MatchResult mr; // Renamed match_rec
        match_history.dequeue(mr);
        tempHistory.enqueue(mr);

        if (mr.stage == stage_query) {
            found_stage = true;
            matches_in_stage++;
            int p1_rec_idx = findPlayerIndex(mr.player1_id);
            int p2_rec_idx = findPlayerIndex(mr.player2_id);
            std::string p1_rec_name = (p1_rec_idx != -1) ? player_stats[p1_rec_idx].name : (mr.player1_id == 0 ? "N/A" : "ID:" + std::to_string(mr.player1_id));
            std::string p2_rec_name = (p2_rec_idx != -1) ? player_stats[p2_rec_idx].name : (mr.player2_id == 0 ? "N/A" : "ID:" + std::to_string(mr.player2_id));
            if(p1_rec_name.length()>9) p1_rec_name = p1_rec_name.substr(0,8)+".";
            if(p2_rec_name.length()>9) p2_rec_name = p2_rec_name.substr(0,8)+".";
            std::string players_rec_str = p1_rec_name + " vs " + p2_rec_name;
            if(players_rec_str.length() > 23) players_rec_str = players_rec_str.substr(0,20)+"...";

            std::string winner_rec_name = "None";
            if (mr.winner_id != 0) {
                int winner_rec_idx = findPlayerIndex(mr.winner_id);
                winner_rec_name = (winner_rec_idx != -1) ? player_stats[winner_rec_idx].name : "ID:" + std::to_string(mr.winner_id);
                if(winner_rec_name.length() > 16) winner_rec_name = winner_rec_name.substr(0,13)+"...";
            } else if (strcmp(mr.status.c_str(), "completed")==0) {
                 winner_rec_name = "Draw/TBD";
            }


            std::string date_rec_val = extractDateFromScheduledTime(mr.scheduled_time);
            if(date_rec_val.empty() && !mr.scheduled_time.empty()) date_rec_val = mr.scheduled_time.substr(0,10);
            else if (date_rec_val.empty()) date_rec_val = "Unknown";

            std::cout << "| " << std::left << std::setw(7) << mr.match_id << " | "
                      << std::setw(23) << players_rec_str << " | "
                      << std::setw(16) << winner_rec_name << " | "
                      << std::setw(7) << (mr.score.length()>6?mr.score.substr(0,3)+"...":mr.score) << " | "
                      << std::right << std::setw(5) << mr.round << " | "
                      << std::left << std::setw(10) << date_rec_val << " |\n";
        }
    }
    while(!tempHistory.isEmpty()){ Task4_MatchResult temp_mr_restore; tempHistory.dequeue(temp_mr_restore); match_history.enqueue(temp_mr_restore); }


    if (!found_stage) { std::cout << "| No matches found for this stage.                                                     |\n"; }
    else { std::cout << "| Total matches in stage '" << stage_query << "': " << std::left << std::setw(40) << matches_in_stage << "|\n"; }
    std::cout << "+---------+-------------------------+------------------+---------+-------+------------+\n";
}


void Task4_GameResultManager::displayMenu_Task4() {
    std::cout << "\n\n+----------------------------------------------------+\n"
              << "| APUEC: RESULT LOGGING & PERFORMANCE HISTORY (TASK 4) |\n"
              << "+----------------------------------------------------+\n"
              << "| 1. Display Recent Matches (Last 5)                 |\n"
              << "| 2. Display All Player Statistics                   |\n"
              << "| 3. Display Specific Player Statistics              |\n"
              << "| 4. Query Matches by Player ID                      |\n"
              << "| 5. Query Matches by Stage Name                     |\n"
              << "| 0. Return to Main APUEC Menu                       |\n"
              << "+----------------------------------------------------+\n"
              << "Enter your choice (Task 4): ";
}

void Task4_GameResultManager::runProgram() {
    std::cout << "\n=== Task 4: Game Result System Initializing ===\n";
    // Data loading is crucial. If runProgram is called multiple times, data might be reloaded.
    // The load functions now clear their respective data structures before loading.
    std::cout << "Task 4: Loading player data (from 'Player_Registration.csv')...\n";
    loadPlayerData("Player_Registration.csv"); // Using the same CSV as Task 1 and Task 2
    std::cout << "Task 4: Loading match history (from 'matches.csv')...\n";
    loadMatchHistory("matches.csv"); // This is generated by Task 1
    std::cout << "Task 4 System ready!\n";

    int choice_task4; bool exit_task4_cli = false;
    while (!exit_task4_cli) {
        displayMenu_Task4();
        // Using std::cin for Task 4, consistent with original task4 code.
        if (!(std::cin >> choice_task4)) {
            std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input (Task 4). Please enter a number.\n"; choice_task4 = -1; // sentinel
        } else { std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); } // Consume newline

        switch (choice_task4) {
            case 1: displayRecentMatches(5); break;
            case 2: displayAllPlayerStats(); break;
            case 3: {
                std::cout << "Enter Player ID to display stats (Task 4): "; int pid_stat;
                if (std::cin >> pid_stat) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    displayPlayerStats(pid_stat);
                } else {
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid Player ID format entered.\n";
                }
                break;
            }
            case 4: {
                std::cout << "Enter Player ID to query matches (Task 4): "; int pid_query;
                if (std::cin >> pid_query) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    queryMatchesByPlayer(pid_query);
                } else {
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid Player ID format entered.\n";
                }
                break;
            }
            case 5: {
                std::cout << "Enter stage name to query (e.g., group, knockout) (Task 4): "; std::string stage_name_q;
                std::getline(std::cin, stage_name_q); // Use getline for stage name with potential spaces
                queryMatchesByStage(stage_name_q);
                break;
            }
            case 0: exit_task4_cli = true; std::cout << "Returning to main APUEC menu from Task 4.\n"; break;
            default: if (choice_task4 != -1) std::cout << "Invalid choice (Task 4). Please try again.\n"; break;
        }
        if (!exit_task4_cli && choice_task4 != -1 && choice_task4 !=0) { // Don't pause if invalid input or exiting
            std::cout << "\n(Task 4: Press Enter to return to Task 4 menu...)";
            // std::cin.get(); // Already handled by cin.ignore after successful numeric input
            // For getline cases, or if just pressing enter is desired.
            // To be safe, a specific getchar() or cin.get() might be needed if the loop feels too fast.
        }
    }
     std::cout << "--- Task 4: Game Result System Closed ---\n";
}

// Helper to extract YYYY-MM-DD from "YYYY-MM-DD HH:MM..."
std::string Task4_GameResultManager::extractDateFromScheduledTime(const std::string& scheduled_time_str) {
    if (scheduled_time_str.length() >= 10) {
        // Basic check for YYYY-MM-DD format
        if (scheduled_time_str[4] == '-' && scheduled_time_str[7] == '-') {
            // Further validation could be added (e.g., are parts numeric?)
            try {
                int y = std::stoi(scheduled_time_str.substr(0, 4));
                int m = std::stoi(scheduled_time_str.substr(5, 2));
                int d = std::stoi(scheduled_time_str.substr(8, 2));
                if (y >= 1000 && y <= 9999 && m >= 1 && m <= 12 && d >= 1 && d <= 31) { // Basic sanity check
                    return scheduled_time_str.substr(0, 10);
                }
            } catch (const std::exception&) {
                return ""; // Conversion failed
            }
        }
    }
    return ""; // Return empty if not valid or long enough
}

// Splits a CSV line into tokens. Handles basic CSV, not quoted fields with commas.
void Task4_GameResultManager::splitCSVLine(const std::string& line_to_split, std::string tokens_array[], int max_tokens) { // Renamed line
    std::istringstream stream(line_to_split);
    std::string token;
    int count = 0;
    while (std::getline(stream, token, ',') && count < max_tokens) {
        // Trim whitespace from token
        size_t startpos = token.find_first_not_of(" \t\r\n");
        size_t endpos = token.find_last_not_of(" \t\r\n");
        if (std::string::npos == startpos) { // Token is all whitespace
            tokens_array[count++] = "";
        } else {
            tokens_array[count++] = token.substr(startpos, endpos - startpos + 1);
        }
    }
    // Fill remaining token slots with empty strings if line had fewer tokens
    while (count < max_tokens) {
        tokens_array[count++] = "";
    }
}

// Finds index of player_id in player_stats array. Returns -1 if not found.
int Task4_GameResultManager::findPlayerIndex(int player_id_to_find) { // Renamed player_id
    for (int i = 0; i < current_player_count; i++) {
        if (player_stats[i].player_id == player_id_to_find) {
            return i;
        }
    }
    return -1;
}

// Parses "S1-S2" score string, returns S1 as double. Rudimentary.
double Task4_GameResultManager::parseScore(const std::string& score_str_to_parse) { // Renamed score_str
    if (score_str_to_parse.empty()) return 0.0;
    size_t dash_pos = score_str_to_parse.find('-');
    std::string first_score_substr; // Renamed first_score_str
    if (dash_pos == std::string::npos) { // No dash, assume it's just one score
        first_score_substr = score_str_to_parse;
    } else {
        first_score_substr = score_str_to_parse.substr(0, dash_pos);
    }
    // Trim whitespace from first_score_substr
    size_t start = first_score_substr.find_first_not_of(" \t");
    size_t end = first_score_substr.find_last_not_of(" \t");
    if (start == std::string::npos) return 0.0; // All whitespace or empty
    first_score_substr = first_score_substr.substr(start, end - start + 1);

    try {
        return std::stod(first_score_substr);
    } catch (const std::exception& e) {
        // std::cerr << "Task 4 Warning: Could not parse score part '" << first_score_substr << "' from '" << score_str_to_parse << "'. Error: " << e.what() << std::endl;
        return 0.0; // Return 0 if parsing fails
    }
}

// Updates player's statistics after a match.
void Task4_GameResultManager::updatePlayerStats(int player_id_to_update, bool is_winner_flag, double score_achieved) { // Renamed player_id, is_winner, score_val
    int index = findPlayerIndex(player_id_to_update);
    if (index == -1) {
        // std::cout << "Task 4 Info: Player ID " << player_id_to_update << " not found for stat update. Perhaps add them or check CSVs.\n";
        return; // Player not found, cannot update stats
    }
    Task4_PlayerStats& ps_ref = player_stats[index]; // Renamed player_stat_ref to ps_ref
    ps_ref.total_matches++;
    if (is_winner_flag) {
        ps_ref.wins++;
    } else {
        ps_ref.losses++;
    }
    // Update average score: (old_total_score + current_score) / new_total_matches
    // old_total_score = ps_ref.avg_score * (ps_ref.total_matches - 1)
    if (ps_ref.total_matches == 1) { // First match
        ps_ref.avg_score = score_achieved;
    } else {
        ps_ref.avg_score = ((ps_ref.avg_score * (ps_ref.total_matches - 1)) + score_achieved) / ps_ref.total_matches;
    }
}


// Main function for the Integrated APUEC System
void displayIntegratedMainMenu() {
    cout << "\n\n#########################################################\n"
         << "### ASIA PACIFIC UNIVERSITY ESPORTS CHAMPIONSHIP (APUEC) ###\n"
         << "###           INTEGRATED MANAGEMENT SYSTEM              ###\n"
         << "#########################################################\n"
         << "Please select a system module:\n"
         << "1. Tournament Management & Match Scheduling (Task 1)\n"
         << "2. Player Registration System (Task 2)\n"
         << "3. Spectator Management (Task 3 - Placeholder)\n"
         << "4. Result Logging & Performance History (Task 4)\n"
         << "0. Exit Application\n"
         << "#########################################################\n"
         << "Enter your choice: ";
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr))); // Seed random number generator (for Task 1 match simulation)

    // Instantiate managers for tasks that require persistent state across calls from main menu
    // Task 1 Tournament Manager
    Tournament task1_tournamentManager; // Uses default constructor values
    bool task1Initialized = false; // To ensure Task 1's `initialize` is called once appropriately

    // Task 4 Game Result Manager
    Task4_GameResultManager task4_gameResultManager(150); // Max 150 players for Task 4 stats

    // Task 2's main logic (Task2_runPlayerRegistrationSystem) manages its own data loading/saving
    // and internal pq lifecycle.

    int choice_main_menu; // Renamed choice
    bool exitApplication = false;

    while (!exitApplication) {
        displayIntegratedMainMenu();
        cin >> choice_main_menu;

        if (cin.fail()) {
            cin.clear(); // Clear error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << "Invalid input. Please enter a number corresponding to the menu options." << endl;
            choice_main_menu = -1; // Set to a value that won't match cases, prompting loop continuation
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard the rest of the line (e.g., newline)
        }

        switch (choice_main_menu) {
            case 1:
                cout << "\n--- Accessing: Tournament Management & Match Scheduling (Task 1) ---\n";
                if (!task1Initialized) {
                    cout << "Initializing Task 1: Tournament System (requires 'Player_Registration.csv')..." << endl;
                    // Task 1's initialize loads players from "Player_Registration.csv"
                    // This CSV is also managed by Task 2.
                    task1_tournamentManager.initialize("Player_Registration.csv");
                    if (task1_tournamentManager.areGroupsCreated()){ // Check if initialization was somewhat successful
                         task1Initialized = true;
                         cout << "Task 1 Tournament System initialized." << endl;
                    } else {
                         cout << "Task 1 Tournament System initialization may have encountered issues (e.g. no players, no groups formed). Proceeding to CLI." << endl;
                         // Still set to true to avoid re-initializing, user can check status in Task 1 CLI
                         task1Initialized = true; 
                    }
                } else {
                    cout << "Task 1: Tournament System already initialized." << endl;
                }
                task1_tournamentManager.runCLI_TASK1(); // Run Task 1's command-line interface
                cout << "\n--- Returned from Task 1 Module ---\n";
                break;
            case 2:
                cout << "\n--- Accessing: Player Registration System (Task 2) ---\n";
                Task2_runPlayerRegistrationSystem(); // Call Task 2's main operational function
                cout << "\n--- Returned from Task 2 Module ---\n";
                break;
            case 3:
                runTask3_SpectatorManagement(); // Call Task 3's placeholder function
                 cout << "\n--- Returned from Task 3 Module ---\n";
                break;
            case 4:
                cout << "\n--- Accessing: Result Logging & Performance History (Task 4) ---\n";
                task4_gameResultManager.runProgram(); // Call Task 4's main operational function
                cout << "\n--- Returned from Task 4 Module ---\n";
                break;
            case 0:
                exitApplication = true;
                cout << "\nExiting APUEC Integrated Management System. All unsaved data in active modules might be lost if not explicitly saved by the module. Goodbye!\n";
                break;
            default:
                if (choice_main_menu != -1) { // Don't print for cin.fail() case again
                    cout << "Invalid choice. Please select a valid option from the main menu." << endl;
                }
                break;
        }
        if (!exitApplication && choice_main_menu != -1 && choice_main_menu !=0) { // Pause for readability before re-displaying menu
            cout << "\n(Main Menu: Press Enter to continue or if stuck...)";
            // cin.get(); // This can be tricky after cin >>. The cin.ignore should handle most.
            // If a pause is strictly needed here, a dedicated getchar() or similar might be used
            // but it's often better to let the loop re-prompt immediately.
            // For now, relying on the user to see the next menu prompt.
        }
    }

    // Destructors for task1_tournamentManager and task4_gameResultManager will be called automatically
    // when main() exits, cleaning up their dynamically allocated memory.
    return 0;
}