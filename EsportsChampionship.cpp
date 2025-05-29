#include "EsportsChampionship.hpp"

// It's common to use 'using namespace std;' in .cpp files for convenience,
// especially if the original codebases did so (Task3.cpp did).
using namespace std;

// ======== TASK 3 IMPLEMENTATIONS ========

// ---- Spectator Class Implementation (from Task3.cpp) ----
Spectator::Spectator(string _id, string _name, bool _wantsLiveStream, string _supportedPlayer,
                     string _category, string _day)
    : id(_id), name(_name), wantsLiveStream(_wantsLiveStream),
      supportedPlayer(_supportedPlayer), category(_category), day(_day),
      next(nullptr), active(true) {
    assignPaymentAmount();
}

void Spectator::assignPaymentAmount() {
    if (category == "VIP") paymentAmount = 500;
    else if (category == "Influencer") paymentAmount = 350;
    else paymentAmount = 200; // General or default
}

// ---- SpectatorList Implementation (from Task3.cpp) ----
SpectatorList::SpectatorList() : head(nullptr) {}

Spectator* SpectatorList::getHead() const { return head; }

void SpectatorList::registerSpectator(const string& id, const string& name, bool wantsLiveStream,
                                      const string& supportedPlayer, const string& category, const string& day) {
    Spectator* newNode = new Spectator(id, name, wantsLiveStream, supportedPlayer, category, day);
    if (!head) {
        head = newNode;
    } else {
        Spectator* temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    cout << "Spectator " << name << " registered successfully.\n";
}

void SpectatorList::loadFromCSV(const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Spectators file (" << filename << ") not found or cannot be opened. Starting with no pre-registered spectators.\n";
        return;
    }
    string line;
    // Skip header if it exists
    if (!getline(file, line) || line.find("spectator_id") == string::npos) {
        // If the first line doesn't seem like a header, rewind and process it
        // This simple check might not be robust for all header variations
        file.clear();
        file.seekg(0);
    }
    
    int count = 0;
    while (getline(file, line)) {
        if (line.empty() || line.find_first_not_of(" \t\r\n") == string::npos) continue; // Skip empty or whitespace-only lines
        
        stringstream ss(line);
        string id_str, name_str, category_str, wantsLiveStreamStr, supportedPlayer_str, day_str, paymentStr;
        
        getline(ss, id_str, ',');
        getline(ss, name_str, ',');
        getline(ss, category_str, ',');
        getline(ss, wantsLiveStreamStr, ',');
        getline(ss, supportedPlayer_str, ',');
        getline(ss, day_str, ',');
        getline(ss, paymentStr, ','); // Payment amount is calculated, but might be in CSV for persistence

        bool wantsLiveStream = (wantsLiveStreamStr == "Yes" || wantsLiveStreamStr == "yes");
        // Using the constructor that calculates payment amount
        Spectator* newNode = new Spectator(id_str, name_str, wantsLiveStream, supportedPlayer_str, category_str, day_str);
        // If paymentStr is not empty and represents a valid integer, you could optionally set it, but assignPaymentAmount takes precedence.
        // newNode->paymentAmount = stoi(paymentStr); // If you want to load payment amount directly

        if (!head) {
            head = newNode;
        } else {
            Spectator* temp = head;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
        count++;
    }
    if(count > 0) cout << "Loaded " << count << " spectators from " << filename << endl;
    file.close();
}

void SpectatorList::displaySpectators() const {
    Spectator* temp = head;
    cout << "\n[All Registered Spectators]:\n";
    if (!temp) {
        cout << "No spectators registered.\n";
        return;
    }
    cout << setw(5) << left << "ID" << setw(20) << "Name" << setw(15) << "Category" 
         << setw(10) << "Stream?" << setw(15) << "Supports" << setw(15) << "Day"
         << setw(10) << "Payment" << setw(8) << "Active" << endl;
    cout << string(98, '-') << endl;
    while (temp) {
        cout << setw(5) << left << temp->id
             << setw(20) << temp->name
             << setw(15) << temp->category
             << setw(10) << (temp->wantsLiveStream ? "Yes" : "No")
             << setw(15) << temp->supportedPlayer
             << setw(15) << temp->day
             << setw(10) << temp->paymentAmount
             << setw(8) << (temp->active ? "Yes" : "No") << "\n";
        temp = temp->next;
    }
}

void SpectatorList::displayActiveSpectators(const string& roundName) const {
    cout << "\n[Active Spectators - " << roundName << "]:\n";
    Spectator* temp = head;
    int count = 1;
    bool found = false;
    while (temp) {
        if (temp->active) {
            if (!found) {
                 cout << setw(5) << left << "#" << setw(20) << "Name" << setw(20) << "Supports Player ID" << endl;
                 cout << string(45, '-') << endl;
                 found = true;
            }
            cout << setw(5) << left << count++ << setw(20) << temp->name 
                 << setw(20) << temp->supportedPlayer << "\n";
        }
        temp = temp->next;
    }
    if (!found) {
        cout << "No active spectators for " << roundName << ".\n";
    }
}

void SpectatorList::saveToCSV(const char* filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open " << filename << " for writing spectator data." << endl;
        return;
    }
    // Header
    file << "spectator_id,name,category,wants_live_stream,supported_player,day,payment_amount\r\n"; // Added \r for windows compatibility
    Spectator* current = head;
    while (current) {
        file << current->id << ","
             << current->name << ","
             << current->category << ","
             << (current->wantsLiveStream ? "Yes" : "No") << ","
             << current->supportedPlayer << ","
             << current->day << ","
             << current->paymentAmount << "\r\n";
        current = current->next;
    }
    file.close();
    cout << "Spectator data saved to " << filename << endl;
}


// ---- Task3Match Class Implementation (from Task3.cpp) ----
Task3Match::Task3Match(int id, const string& t1, const string& t2, const string& sched, const string& stg,
                       int rnd, const string& sts, const string& winner)
    : matchId(id), team1(t1), team2(t2), scheduledTime(sched), stage(stg), round(rnd),
      status(sts), winnerId(winner), next(nullptr) {}

// ---- Task3MatchList Implementation (from Task3.cpp) ----
Task3MatchList::Task3MatchList() : head(nullptr) {}

Task3MatchList::~Task3MatchList() {
    Task3Match* cur = head;
    while (cur) {
        Task3Match* tmp = cur;
        cur = cur->next;
        delete tmp;
    }
    head = nullptr;
}

Task3Match* Task3MatchList::getHead() const { return head; }

void Task3MatchList::loadFromCSV(const char* filename) {
    // Clear existing list first
    while(head) {
        Task3Match* temp = head;
        head = head->next;
        delete temp;
    }

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Matches file (" << filename << ") for Task 3 system not found or cannot be opened.\n";
        return;
    }
    string line;
    getline(file, line); // Skip header line

    int count = 0;
    while (getline(file, line)) {
        if (line.empty() || line.find_first_not_of(" \t\r\n") == string::npos) continue;
        
        stringstream ss(line);
        string idStr, stage_str, groupId_str, roundStr, p1_str, p2_str, sched_str, status_str, winnerId_str, score_str;
        
        getline(ss, idStr, ',');
        getline(ss, stage_str, ',');
        getline(ss, groupId_str, ','); // Ignored by Task3Match
        getline(ss, roundStr, ',');
        getline(ss, p1_str, ',');
        getline(ss, p2_str, ',');
        getline(ss, sched_str, ',');
        getline(ss, status_str, ',');
        getline(ss, winnerId_str, ',');
        getline(ss, score_str, ','); // Score not directly stored in Task3Match but available

        try {
            int matchId_val = stoi(idStr);
            int round_val = stoi(roundStr);
            // Note: winnerId_str might be empty if match not completed or no winner. stoi would fail.
            // Task3Match constructor takes winnerId as string, so it's fine.
            
            Task3Match* newNode = new Task3Match(matchId_val, p1_str, p2_str, sched_str, stage_str, round_val, status_str, winnerId_str);

            if (!head) {
                head = newNode;
            } else {
                Task3Match* cur = head;
                while (cur->next) {
                    cur = cur->next;
                }
                cur->next = newNode;
            }
            count++;
        } catch (const std::invalid_argument& ia) {
            cerr << "Warning: Invalid number in match line (Task 3 load): " << line << " - " << ia.what() << endl;
        } catch (const std::out_of_range& oor) {
            cerr << "Warning: Number out of range in match line (Task 3 load): " << line << " - " << oor.what() << endl;
        }
    }
    if(count > 0) cout << "Loaded " << count << " matches into Task 3 system from " << filename << endl;
    file.close();
}

void Task3MatchList::printGroupStageRound1Matches(const string& day) const {
    Task3Match* cur = head;
    int matchIndex = 1;
    bool found = false;
    cout << "\nMatches on " << day << " (Filtered by Stage='group', Round=1):\n";
    cout << setw(8) << left << "Option" << setw(25) << "Player 1" << setw(10) << "vs" << setw(25) << "Player 2" << setw(20) << "Scheduled Time" << endl;
    cout << string(88, '-') << endl;

    Task3Match* temp_match_for_selection[8]; // Assuming max 8 matches to show as per original prompt context
    int current_display_count = 0;

    while (cur) {
        // Filter for group stage, round 1, and the specified day
        if (cur->stage == "group" && cur->round == 1 && cur->scheduledTime.rfind(day, 0) == 0) { // rfind to check prefix
            if (current_display_count < 8) { // Limit to 8 displayable options
                 cout << setw(8) << left << matchIndex 
                      << setw(25) << ("Player " + cur->team1) 
                      << setw(10) << "vs" 
                      << setw(25) << ("Player " + cur->team2)
                      << setw(20) << cur->scheduledTime << "\n";
                temp_match_for_selection[matchIndex-1] = cur; // Store for selection
                matchIndex++;
            }
            found = true;
            current_display_count++;
        }
        cur = cur->next;
    }
    if (!found) {
        cout << "No Group Stage Round 1 matches found for " << day << ".\n";
    } else if (current_display_count >= 8) {
        cout << "Showing first 8 available matches. More might exist.\n";
    }
}


// ---- StringSet Implementation (from Task3.cpp) ----
StringSet::StringSet() : head(nullptr) {}

StringSet::~StringSet() {
    Node* cur = head;
    while (cur) {
        Node* tmp = cur;
        cur = cur->next;
        delete tmp;
    }
    head = nullptr;
}

bool StringSet::contains(const string& s) const {
    Node* cur = head;
    while (cur) {
        if (cur->val == s) return true;
        cur = cur->next;
    }
    return false;
}

void StringSet::insert(const string& s) {
    if (contains(s) || s.empty() || s == "0") return; // Also don't insert if winnerId is "0" or empty
    Node* newNode = new Node(s);
    newNode->next = head;
    head = newNode;
}

// ---- PrioritySeatingQueue Implementation (from Task3.cpp) ----
PrioritySeatingQueue::PrioritySeatingQueue() : front(nullptr) {}

PrioritySeatingQueue::~PrioritySeatingQueue() {
    Node* current = front;
    while (current) {
        Node* temp = current;
        current = current->next;
        delete temp;
    }
    front = nullptr;
}

int PrioritySeatingQueue::getPriority(const string& category) {
    if (category == "VIP") return 1;
    if (category == "Influencer") return 2;
    return 3; // General or other
}

void PrioritySeatingQueue::enqueue(Spectator* sp) {
    Node* newNode = new Node(sp);
    if (!front || getPriority(sp->category) < getPriority(front->data->category)) {
        newNode->next = front;
        front = newNode;
    } else {
        Node* current = front;
        while (current->next && getPriority(sp->category) >= getPriority(current->next->data->category)) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

void PrioritySeatingQueue::displayQueue() const {
    cout << "\n[Seating Queue - Priority Order]:\n";
    if (!front) {
        cout << "(empty)\n";
        return;
    }
    Node* temp = front;
    int pos = 1;
    cout << setw(5) << left << "#" << setw(20) << "Name" << setw(15) << "Category" << endl;
    cout << string(40, '-') << endl;
    while (temp) {
        cout << setw(5) << left << pos++ << setw(20) << temp->data->name 
             << setw(15) << "(" + temp->data->category + ")" << "\n";
        temp = temp->next;
    }
}

// ---- MultiLevelLiveStreamQueue Implementation (from Task3.cpp) ----
MultiLevelLiveStreamQueue::MultiLevelLiveStreamQueue(int cap)
    : influencerHead(nullptr), vipHead(nullptr), generalHead(nullptr), capacity(cap) {}

MultiLevelLiveStreamQueue::~MultiLevelLiveStreamQueue() {
    clearList(influencerHead);
    clearList(vipHead);
    clearList(generalHead);
}

void MultiLevelLiveStreamQueue::clearList(Node*& head_ptr) {
    while (head_ptr) {
        Node* temp = head_ptr;
        head_ptr = head_ptr->next;
        delete temp;
    }
}

int MultiLevelLiveStreamQueue::countList(Node* head_ptr) const {
    int cnt = 0;
    Node* current = head_ptr;
    while (current) {
        cnt++;
        current = current->next;
    }
    return cnt;
}

int MultiLevelLiveStreamQueue::getTotalCount() const {
    return countList(influencerHead) + countList(vipHead) + countList(generalHead);
}


void MultiLevelLiveStreamQueue::enqueue(Spectator* sp) {
    if (getTotalCount() >= capacity) {
        cout << "Live stream queue is full. " << sp->name << " (Category: " << sp->category << ") cannot join at this time.\n";
        return;
    }
    
    Node** targetHead; // Pointer to the head pointer of the target queue
    if (sp->category == "Influencer") {
        targetHead = &influencerHead;
    } else if (sp->category == "VIP") {
        targetHead = &vipHead;
    } else { // General or other
        targetHead = &generalHead;
    }

    Node* newNode = new Node(sp);
    if (!*targetHead) { // If the target list is empty
        *targetHead = newNode;
    } else {
        Node* temp = *targetHead;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    // cout << sp->name << " (Category: " << sp->category << ") added to live stream queue.\n"; // Optional: confirmation
}


int MultiLevelLiveStreamQueue::displayGroup(const string& label, Node* head_ptr, int start_count) const {
    cout << "-- " << label << " (" << countList(head_ptr) << " joined) --\n";
    Node* current = head_ptr;
    if (!current) {
        cout << "   (empty)\n";
    }
    while (current) {
        cout << start_count++ << ". " << current->data->name << "\n";
        current = current->next;
    }
    return start_count;
}

void MultiLevelLiveStreamQueue::displayQueue() const {
    cout << "\n[Live Stream Queue - Priority Groups (Capacity: " << capacity << ", Current Size: " << getTotalCount() << ")]:\n";
    int count = 1;
    count = displayGroup("Influencers", influencerHead, count);
    count = displayGroup("VIPs", vipHead, count);
    displayGroup("General Public", generalHead, count);
}


// ---- CircularStreamRotation Implementation (from Task3.cpp) ----
CircularStreamRotation::CircularStreamRotation(int cap) : tail(nullptr), size(0), capacity(cap) {}

CircularStreamRotation::~CircularStreamRotation() {
    if (!tail) return;
    Node* current = tail->next; // Start from head
    tail->next = nullptr;    // Break the circle to prevent infinite loop if current is modified elsewhere
    
    Node* head_node = current; // Keep track of head to stop iteration
    if (current == nullptr) return; // Should not happen if tail is not null, but defensive

    do {
        Node* temp = current;
        current = current->next;
        delete temp;
        if (current == head_node && size > 0) { // if it looped back to head
             // This check ensures we don't get stuck if tail was the only node and already deleted.
             // The size check helps. If size becomes 0, the loop should naturally terminate.
             break; 
        }
    } while (current != head_node && current != nullptr && size > 0); // size check added for robustness
    tail = nullptr;
    size = 0;
}


bool CircularStreamRotation::isFull() const { return size == capacity; }
bool CircularStreamRotation::isEmpty() const { return size == 0; }

void CircularStreamRotation::enqueue(Spectator* sp) {
    if (isFull()) {
        cout << "Circular stream rotation is full. " << sp->name << " cannot be added.\n";
        return;
    }
    Node* newNode = new Node(sp);
    if (!tail) { // Queue is empty
        tail = newNode;
        tail->next = tail; // Points to itself
    } else { // Queue is not empty
        newNode->next = tail->next; // New node points to current head
        tail->next = newNode;       // Old tail points to new node
        tail = newNode;             // New node becomes the new tail
    }
    size++;
    // cout << sp->name << " added to circular stream rotation.\n"; // Optional
}

Spectator* CircularStreamRotation::dequeue() {
    if (isEmpty()) {
        // cout << "Circular stream rotation is empty. Cannot dequeue.\n"; // Optional
        return nullptr;
    }
    Node* headNode = tail->next; // This is the node to be dequeued
    Spectator* sp = headNode->data;

    if (tail == headNode) { // Only one node in the queue
        delete headNode;
        tail = nullptr;
    } else {
        tail->next = headNode->next; // Tail points to the new head
        delete headNode;
    }
    size--;
    return sp;
}

void CircularStreamRotation::displayQueue() const {
    cout << "\n[Circular Live Stream Rotation (Capacity: " << capacity << ", Current Size: " << size << ")]:\n";
    if (isEmpty()) {
        cout << "(empty)\n";
        return;
    }
    Node* current = tail->next; // Start from the head
    int count = 1;
    cout << setw(5) << left << "#" << setw(20) << "Name" << endl;
    cout << string(25, '-') << endl;
    do {
        cout << setw(5) << left << count++ << setw(20) << current->data->name << "\n";
        current = current->next;
    } while (current != tail->next); // Loop until we are back at the head
}

// ---- WatchHistoryStack Implementation (from Task3.cpp) ----
WatchHistoryStack::WatchHistoryStack() : top(nullptr) {}

WatchHistoryStack::~WatchHistoryStack() {
    Node* current = top;
    while (current) {
        Node* temp = current;
        current = current->next;
        delete temp;
    }
    top = nullptr;
}

void WatchHistoryStack::push(Spectator* sp) {
    Node* newNode = new Node(sp);
    newNode->next = top;
    top = newNode;
}

void WatchHistoryStack::display() const {
    cout << "\n[Watch History - Stack (Most Recent First)]:\n";
    if(!top) {
        cout << "(empty)\n";
        return;
    }
    Node* current = top;
    int count = 1;
    cout << setw(5) << left << "#" << setw(20) << "Name" << endl;
    cout << string(25, '-') << endl;
    while (current) {
        cout << setw(5) << left << count++ << setw(20) << current->data->name << "\n";
        current = current->next;
    }
}

// ---- WaitingQueue Implementation (from Task3.cpp) ----
WaitingQueue::WaitingQueue() : front(nullptr), rear(nullptr) {}

WaitingQueue::~WaitingQueue() {
    Node* current = front;
    while (current) {
        Node* temp = current;
        current = current->next;
        delete temp;
    }
    front = rear = nullptr;
}

bool WaitingQueue::isEmpty() const { return front == nullptr; }

void WaitingQueue::enqueue(Spectator* sp) {
    Node* newNode = new Node(sp);
    if (!rear) { // Queue is empty
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
    // cout << sp->name << " added to the general waiting queue.\n"; // Optional
}

Spectator* WaitingQueue::dequeue() {
    if (!front) {
        // cout << "General waiting queue is empty. Cannot dequeue.\n"; // Optional
        return nullptr;
    }
    Node* temp = front;
    Spectator* sp = temp->data;
    front = front->next;
    if (!front) { // If queue becomes empty
        rear = nullptr;
    }
    delete temp;
    return sp;
}

// ---- Task 3 Utility Function Implementations (from Task3.cpp) ----
void updateSpectatorsByMatchResults(SpectatorList& spectators, Task3MatchList& matches,
                                    const string& stage_filter, int round_filter) {
    cout << "\nUpdating spectator status based on match results for Stage: '" << stage_filter 
         << "', Round: " << round_filter << "...\n";
    StringSet winners; // Stores IDs of winning players for the specified stage/round
    Task3Match* currentMatch = matches.getHead();
    
    while (currentMatch) {
        if (currentMatch->stage == stage_filter && currentMatch->round == round_filter && 
            (currentMatch->status == "completed" || currentMatch->status == "Completed") && 
            !currentMatch->winnerId.empty() && currentMatch->winnerId != "0" ) { // Check for actual winner
            winners.insert(currentMatch->winnerId);
        }
        currentMatch = currentMatch->next;
    }

    Spectator* currentSpectator = spectators.getHead();
    int spectators_updated_count = 0;
    while (currentSpectator) {
        // Only consider active spectators whose supported player is not in the winners set for this round
        if (currentSpectator->active && !currentSpectator->supportedPlayer.empty() &&
            !winners.contains(currentSpectator->supportedPlayer)) {
            
            // We need to confirm if the player actually played and lost, or if their match wasn't in this round's results.
            // For simplicity as per original logic: if their player isn't a winner OF THIS ROUND/STAGE, they MIGHT leave.
            // A more robust check would be if their player played AND lost.
            // The current logic implies: if your player didn't win (in this specific set of matches), you're out.
            bool player_participated_and_lost = false;
            Task3Match* match_check = matches.getHead();
            while(match_check){
                if(match_check->stage == stage_filter && match_check->round == round_filter &&
                   (match_check->status == "completed" || match_check->status == "Completed") &&
                   (match_check->team1 == currentSpectator->supportedPlayer || match_check->team2 == currentSpectator->supportedPlayer) &&
                   match_check->winnerId != currentSpectator->supportedPlayer && !match_check->winnerId.empty() && match_check->winnerId != "0"){
                    player_participated_and_lost = true;
                    break;
                   }
                match_check = match_check->next;
            }

            if(player_participated_and_lost){
                currentSpectator->active = false;
                cout << "Spectator " << currentSpectator->name << " (supporting Player ID: " << currentSpectator->supportedPlayer 
                     << ") has left as their player lost in " << stage_filter << " Round " << round_filter << ".\n";
                spectators_updated_count++;
            }
        }
        currentSpectator = currentSpectator->next;
    }
    if(spectators_updated_count == 0){
        cout << "No spectators were made inactive based on these match results (either their players won, didn't play this round, or matches incomplete).\n";
    }
}


void simulateQueueManagement(SpectatorList& regList) {
    cout << "\n--- Simulating Queue Management for Current Active Spectators ---" << endl;
    PrioritySeatingQueue seatQueue;
    MultiLevelLiveStreamQueue streamQueue(10); // Capacity from original Task3.cpp
    CircularStreamRotation rotation(5);      // Capacity from original Task3.cpp
    WatchHistoryStack history;
    WaitingQueue waitingForRotationQueue; // Spectators waiting for a spot in rotation

    Spectator* currentSpec = regList.getHead();
    int activeSpectatorCount = 0;
    while (currentSpec) {
        if (currentSpec->active) {
            activeSpectatorCount++;
            seatQueue.enqueue(currentSpec); // All active spectators join seating queue

            if (currentSpec->wantsLiveStream) {
                streamQueue.enqueue(currentSpec); // Join multi-level live stream queue

                if (!rotation.isFull()) {
                    rotation.enqueue(currentSpec); // Join circular rotation if space
                } else {
                    cout << currentSpec->name << " (Category: " << currentSpec->category 
                         << ") wants live stream, but rotation is full. Added to waiting queue for rotation.\n";
                    waitingForRotationQueue.enqueue(currentSpec);
                }
                history.push(currentSpec); // Add to watch history if they want stream
            }
        }
        currentSpec = currentSpec->next;
    }
    
    if (activeSpectatorCount == 0) {
        cout << "No active spectators to simulate queue management for." << endl;
        return;
    }

    seatQueue.displayQueue();
    streamQueue.displayQueue();
    rotation.displayQueue();
    history.display();

    if (!waitingForRotationQueue.isEmpty()) {
        cout << "\n[Spectators Waiting for Circular Stream Rotation]:\n";
        // Temporary display for waiting queue (WaitingQueue class doesn't have display)
        // This requires temporarily dequeuing and re-queuing or adding a display method.
        // For simplicity, just state that there are waiters.
        // To display, you'd iterate carefully or add display to WaitingQueue.
        cout << "There are spectators in the waiting queue for rotation." << endl;
    }


    cout << "\n--- Simulating Rotation Event --- \n";
    if (!rotation.isEmpty()) {
        Spectator* dequeuedFromRotation = rotation.dequeue();
        if (dequeuedFromRotation) {
            cout << dequeuedFromRotation->name << " has been rotated out of the live stream focus.\n";
            // Depending on logic, this spectator might rejoin waiting or just be "out of focus"
            // Original logic didn't specify re-queuing them to waitingForRotationQueue.
        }

        if (!waitingForRotationQueue.isEmpty()) {
            Spectator* movedToRotation = waitingForRotationQueue.dequeue();
            if (movedToRotation) {
                rotation.enqueue(movedToRotation);
                cout << movedToRotation->name << " from the waiting queue has been moved into the live stream rotation.\n";
            }
        } else {
            cout << "Waiting queue for rotation is empty, no new spectator moved into rotation.\n";
        }
        rotation.displayQueue(); // Show updated rotation
    } else {
        cout << "Circular stream rotation is empty, no rotation event occurred.\n";
    }
    cout << "--- End of Queue Management Simulation ---" << endl;
}


// ======== TASK 4 IMPLEMENTATIONS ========

// ---- MatchResult Struct Implementation (from task4.hpp, constructors) ----
MatchResult::MatchResult() : match_id(0), group_id(0), round(0), player1_id(0), 
                             player2_id(0), winner_id(0) {}

MatchResult::MatchResult(int mid, const std::string& st, int gid, int r, int p1, int p2,
                         const std::string& sched_time, const std::string& stat, int winner, const std::string& sc)
    : match_id(mid), stage(st), group_id(gid), round(r), player1_id(p1), 
      player2_id(p2), scheduled_time(sched_time), status(stat), winner_id(winner), score(sc) {}

// ---- PlayerStats Struct Implementation (from task4.hpp, constructors) ----
PlayerStats::PlayerStats() : player_id(0), wins(0), losses(0), total_matches(0), avg_score(0.0) {}

PlayerStats::PlayerStats(int pid, const std::string& n, const std::string& r_str, const std::string& cont, 
                         const std::string& reg_time)
    : player_id(pid), name(n), rank(r_str), contact(cont), registration_time(reg_time),
      wins(0), losses(0), total_matches(0), avg_score(0.0) {}


// ---- GameResultManager Class Implementation (from task4.cpp) ----
GameResultManager::GameResultManager(int max_player_capacity) 
    : max_players(max_player_capacity), current_player_count(0) {
    player_stats = new PlayerStats[max_players];
}

GameResultManager::~GameResultManager() {
    delete[] player_stats;
}

void GameResultManager::splitCSVLine(const std::string& line, std::string tokens[], int max_tokens) {
    std::stringstream ss(line);
    std::string token;
    int index = 0;
    
    while (std::getline(ss, token, ',') && index < max_tokens) {
        size_t start = token.find_first_not_of(" \t\r\n");
        size_t end = token.find_last_not_of(" \t\r\n");
        
        if (start != std::string::npos && end != std::string::npos) {
            tokens[index] = token.substr(start, end - start + 1);
        } else {
            tokens[index] = ""; // Handle empty or whitespace-only fields
        }
        index++;
    }
    while (index < max_tokens) { // Fill remaining tokens if line had fewer fields
        tokens[index++] = "";
    }
}

int GameResultManager::findPlayerIndex(int player_id) {
    for (int i = 0; i < current_player_count; i++) {
        if (player_stats[i].player_id == player_id) {
            return i;
        }
    }
    return -1;
}

double GameResultManager::parseScore(const std::string& score_str) {
    if (score_str.empty()) return 0.0;
    std::stringstream ss(score_str);
    std::string first_score_part;
    std::getline(ss, first_score_part, '-'); // Get part before '-'
    try {
        return std::stod(first_score_part);
    } catch (const std::invalid_argument& ia) {
        // std::cerr << "Warning: Invalid score format for parsing first part: " << score_str << std::endl;
        return 0.0;
    } catch (const std::out_of_range& oor) {
        // std::cerr << "Warning: Score out of range for parsing first part: " << score_str << std::endl;
        return 0.0;
    }
}

void GameResultManager::updatePlayerStats(const MatchResult& match) {
    int p1_idx = findPlayerIndex(match.player1_id);
    int p2_idx = findPlayerIndex(match.player2_id);

    if (p1_idx != -1) {
        player_stats[p1_idx].total_matches++;
        if (match.winner_id == match.player1_id) {
            player_stats[p1_idx].wins++;
        } else if (match.winner_id != 0 && match.winner_id != match.player1_id) { // Check if there was a winner and it wasn't p1
            player_stats[p1_idx].losses++;
        }
        // Average score for player 1 (first part of score)
        double p1_score_val = parseScore(match.score);
        if (player_stats[p1_idx].total_matches == 1) {
             player_stats[p1_idx].avg_score = p1_score_val;
        } else if (player_stats[p1_idx].total_matches > 1) {
             player_stats[p1_idx].avg_score = ((player_stats[p1_idx].avg_score * (player_stats[p1_idx].total_matches - 1)) + p1_score_val) / player_stats[p1_idx].total_matches;
        }
    }

    if (p2_idx != -1) {
        player_stats[p2_idx].total_matches++;
        if (match.winner_id == match.player2_id) {
            player_stats[p2_idx].wins++;
        } else if (match.winner_id != 0 && match.winner_id != match.player2_id) { // Check if there was a winner and it wasn't p2
             player_stats[p2_idx].losses++;
        }
        // Average score for player 2 (second part of score)
        size_t dash_pos = match.score.find('-');
        double p2_score_val = 0.0;
        if (dash_pos != std::string::npos && dash_pos + 1 < match.score.length()) {
            try {
                p2_score_val = std::stod(match.score.substr(dash_pos + 1));
            } catch (...) { /* ignore parse error for p2 score */ }
        }
        if (player_stats[p2_idx].total_matches == 1) {
            player_stats[p2_idx].avg_score = p2_score_val;
        } else if (player_stats[p2_idx].total_matches > 1) {
            player_stats[p2_idx].avg_score = ((player_stats[p2_idx].avg_score * (player_stats[p2_idx].total_matches - 1)) + p2_score_val) / player_stats[p2_idx].total_matches;
        }
    }
}


bool GameResultManager::loadPlayerData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open players file: " << filename << std::endl;
        return false;
    }
    std::string line;
    current_player_count = 0; // Reset before loading
    
    if (std::getline(file, line)) { // Skip header
        while (std::getline(file, line) && current_player_count < max_players) {
            if (line.empty() || line.find_first_not_of(" \t\r\n") == string::npos) continue;

            std::string tokens[5]; // player_id, name, rank, contact, registration_time
            splitCSVLine(line, tokens, 5);
            
            try {
                int pid = std::stoi(tokens[0]);
                player_stats[current_player_count] = PlayerStats(pid, tokens[1], tokens[2], tokens[3], tokens[4]);
                current_player_count++;
            } catch (const std::invalid_argument& ia) {
                std::cerr << "Warning: Invalid player ID in players.csv: " << tokens[0] << " Line: " << line << std::endl;
            } catch (const std::out_of_range& oor) {
                 std::cerr << "Warning: Player ID out of range in players.csv: " << tokens[0] << " Line: " << line << std::endl;
            }
        }
    }
    file.close();
    std::cout << "Loaded " << current_player_count << " players into GameResultManager from " << filename << std::endl;
    return true;
}

bool GameResultManager::loadMatchHistory(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open matches file for GameResultManager: " << filename << std::endl;
        return false;
    }
    // Clear existing history before loading
    MatchResult temp_match;
    while(recent_matches.pop(temp_match)); // Clear stack
    while(match_history.dequeue(temp_match)); // Clear queue


    std::string line;
    int matches_loaded_count = 0;
    if (std::getline(file, line)) { // Skip header
        while (std::getline(file, line)) {
             if (line.empty() || line.find_first_not_of(" \t\r\n") == string::npos) continue;

            std::string tokens[10]; // match_id,stage,group_id,round,p1_id,p2_id,sched_time,status,winner_id,score
            splitCSVLine(line, tokens, 10);
            
            try {
                int mid = std::stoi(tokens[0]);
                int gid = tokens[2].empty() ? 0 : std::stoi(tokens[2]); // group_id can be empty for non-group stages
                int rnd = std::stoi(tokens[3]);
                int p1id = std::stoi(tokens[4]);
                int p2id = std::stoi(tokens[5]);
                // winner_id can be 0 or empty if not completed, handle stoi
                int wid = tokens[8].empty() ? 0 : std::stoi(tokens[8]);

                MatchResult match(mid, tokens[1], gid, rnd, p1id, p2id, tokens[6], tokens[7], wid, tokens[9]);
                
                if (!match_history.isFull()) match_history.enqueue(match);
                else std::cerr << "Warning: Match history queue is full. Cannot add more matches." << std::endl;
                
                if (!recent_matches.isFull()) recent_matches.push(match);
                // If recent_matches stack is full, the original didn't specify behavior. Oldest recent match could be dropped if it were a circular buffer.
                // Current stack just fails to push.

                updatePlayerStats(match); // Update stats for players based on this loaded match
                matches_loaded_count++;
            } catch (const std::invalid_argument& ia) {
                std::cerr << "Warning: Invalid number in match data line (GRM load): " << line << " - " << ia.what() << std::endl;
            } catch (const std::out_of_range& oor) {
                std::cerr << "Warning: Number out of range in match data line (GRM load): " << line << " - " << oor.what() << std::endl;
            }
        }
    }
    file.close();
    std::cout << "Loaded " << matches_loaded_count << " matches into GameResultManager from " << filename << std::endl;
    return true;
}

bool GameResultManager::logMatchResult(const MatchResult& match, const std::string& filename) {
    if(!recent_matches.push(match)) {
        std::cerr << "Failed to push match to recent_matches stack (possibly full)." << std::endl;
        // Continue, as history and file are more critical
    }
    if(!match_history.enqueue(match)) {
        std::cerr << "Failed to enqueue match to match_history queue (possibly full)." << std::endl;
        // Continue to attempt file write
    }
    
    updatePlayerStats(match);
    
    std::ofstream file(filename, std::ios::app); // Append mode
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << " for appending match result." << std::endl;
        return false;
    }
    // Check if file is empty to add header (only if appending to a new/empty file)
    file.seekp(0, std::ios::end); // Go to end of file
    if (file.tellp() == 0) { // If file is empty
        file << "match_id,stage,group_id,round,player1_id,player2_id,scheduled_time,status,winner_id,score\r\n";
    }

    file << match.match_id << "," << match.stage << "," << match.group_id << ","
         << match.round << "," << match.player1_id << "," << match.player2_id << ","
         << match.scheduled_time << "," << match.status << "," << match.winner_id << ","
         << match.score << "\r\n"; // Using \r\n for windows EOL
    file.close();
    std::cout << "Match " << match.match_id << " logged successfully to " << filename << " and internal history." << std::endl;
    return true;
}

void GameResultManager::displayRecentMatches(int count) {
    std::cout << "\n=== Recent Matches (Last " << std::min(count, recent_matches.size()) << ") ===\n";
    if (recent_matches.isEmpty()) {
        std::cout << "No recent matches available.\n";
        return;
    }
    std::cout << std::setfill('-') << std::setw(100) << "" << std::setfill(' ') << std::endl;
    std::cout << std::left << std::setw(10) << "MatchID" << std::setw(15) << "Stage" << std::setw(8) << "Round"
              << std::setw(20) << "Player1" << std::setw(5) << "vs" << std::setw(20) << "Player2"
              << std::setw(15) << "Winner" << std::setw(10) << "Score" << std::endl;
    std::cout << std::setfill('-') << std::setw(100) << "" << std::setfill(' ') << std::endl;


    int num_to_show = std::min(count, recent_matches.size());
    for (int i = 0; i < num_to_show; ++i) {
        MatchResult match;
        if (recent_matches.getFromTop(i, match)) {
            std::string p1_name = "P_ID:" + std::to_string(match.player1_id);
            std::string p2_name = "P_ID:" + std::to_string(match.player2_id);
            std::string winner_name = "P_ID:" + std::to_string(match.winner_id);

            int p1_idx = findPlayerIndex(match.player1_id);
            if (p1_idx != -1) p1_name = player_stats[p1_idx].name;
            int p2_idx = findPlayerIndex(match.player2_id);
            if (p2_idx != -1) p2_name = player_stats[p2_idx].name;
            int w_idx = findPlayerIndex(match.winner_id);
            if (w_idx != -1) winner_name = player_stats[w_idx].name;
            else if (match.winner_id == 0) winner_name = "N/A (Draw/TBD)";


            std::cout << std::left << std::setw(10) << match.match_id 
                      << std::setw(15) << match.stage 
                      << std::setw(8) << match.round
                      << std::setw(20) << p1_name 
                      << std::setw(5) << "vs" 
                      << std::setw(20) << p2_name
                      << std::setw(15) << winner_name 
                      << std::setw(10) << match.score << std::endl;
        }
    }
    std::cout << std::setfill('-') << std::setw(100) << "" << std::setfill(' ') << std::endl;
}

void GameResultManager::displayPlayerStats(int player_id_lookup) {
    int index = findPlayerIndex(player_id_lookup);
    if (index == -1) {
        std::cout << "Player with ID " << player_id_lookup << " not found.\n";
        return;
    }
    PlayerStats& stats = player_stats[index];
    std::cout << "\n=== Player Statistics for " << stats.name << " (ID: " << stats.player_id << ") ===\n";
    std::cout << "Rank: " << stats.rank << "\n";
    std::cout << "Contact: " << stats.contact << "\n";
    std::cout << "Registered: " << stats.registration_time << "\n";
    std::cout << "Total Matches Played: " << stats.total_matches << "\n";
    std::cout << "Wins: " << stats.wins << "\n";
    std::cout << "Losses: " << stats.losses << "\n";
    double win_rate = (stats.total_matches > 0) ? (static_cast<double>(stats.wins) / stats.total_matches * 100.0) : 0.0;
    std::cout << "Win Rate: " << std::fixed << std::setprecision(1) << win_rate << "%\n";
    std::cout << "Average Score (points scored by player): " << std::fixed << std::setprecision(2) << stats.avg_score << "\n";
    std::cout << "----------------------------------------\n";
}

void GameResultManager::displayAllPlayerStats() {
    std::cout << "\n=== All Player Statistics ===\n";
    if (current_player_count == 0) {
        std::cout << "No players loaded or no player data available.\n";
        return;
    }
    std::cout << std::setfill('-') << std::setw(120) << "" << std::setfill(' ') << std::endl;
    std::cout << std::left << std::setw(8) << "ID" << std::setw(20) << "Name" << std::setw(10) << "Rank"
              << std::setw(10) << "Matches" << std::setw(7) << "Wins" << std::setw(8) << "Losses"
              << std::setw(12) << "Win Rate" << std::setw(15) << "Avg Score" 
              << std::setw(25) << "Contact" << std::endl;
    std::cout << std::setfill('-') << std::setw(120) << "" << std::setfill(' ') << std::endl;

    for (int i = 0; i < current_player_count; ++i) {
        PlayerStats& stats = player_stats[i];
        double win_rate = (stats.total_matches > 0) ? (static_cast<double>(stats.wins) / stats.total_matches * 100.0) : 0.0;
        std::cout << std::left << std::setw(8) << stats.player_id
                  << std::setw(20) << stats.name
                  << std::setw(10) << stats.rank
                  << std::setw(10) << stats.total_matches
                  << std::setw(7) << stats.wins
                  << std::setw(8) << stats.losses
                  << std::fixed << std::setprecision(1) << std::setw(11) << win_rate << "%"
                  << std::fixed << std::setprecision(2) << std::setw(15) << stats.avg_score
                  << std::setw(25) << stats.contact << std::endl;
    }
    std::cout << std::setfill('-') << std::setw(120) << "" << std::setfill(' ') << std::endl;
}

void GameResultManager::queryMatchesByPlayer(int player_id_lookup) {
    int player_idx = findPlayerIndex(player_id_lookup);
    if (player_idx == -1) {
        std::cout << "Player with ID " << player_id_lookup << " not found.\n";
        return;
    }
    std::string player_name = player_stats[player_idx].name;
    std::cout << "\n=== Match History for Player: " << player_name << " (ID: " << player_id_lookup << ") ===\n";
    std::cout << std::setfill('-') << std::setw(100) << "" << std::setfill(' ') << std::endl;
    std::cout << std::left << std::setw(10) << "MatchID" << std::setw(15) << "Stage" << std::setw(8) << "Round"
              << std::setw(20) << "Opponent" << std::setw(10) << "Result" << std::setw(10) << "Score" 
              << std::setw(20) << "Scheduled Time" << std::endl;
    std::cout << std::setfill('-') << std::setw(100) << "" << std::setfill(' ') << std::endl;

    int found_count = 0;
    for (int i = 0; i < match_history.size(); ++i) {
        MatchResult match;
        if (match_history.getAt(i, match)) {
            if (match.player1_id == player_id_lookup || match.player2_id == player_id_lookup) {
                found_count++;
                std::string opponent_name;
                int opponent_id = (match.player1_id == player_id_lookup) ? match.player2_id : match.player1_id;
                int opp_idx = findPlayerIndex(opponent_id);
                if (opp_idx != -1) opponent_name = player_stats[opp_idx].name;
                else opponent_name = "P_ID:" + std::to_string(opponent_id);

                std::string result_str = "N/A";
                if (match.winner_id == player_id_lookup) result_str = "WIN";
                else if (match.winner_id != 0 && match.winner_id != player_id_lookup) result_str = "LOSS";
                else if (match.winner_id == 0 && match.status == "completed") result_str = "DRAW/TBD";


                std::cout << std::left << std::setw(10) << match.match_id
                          << std::setw(15) << match.stage
                          << std::setw(8) << match.round
                          << std::setw(20) << opponent_name
                          << std::setw(10) << result_str
                          << std::setw(10) << match.score
                          << std::setw(20) << match.scheduled_time << std::endl;
            }
        }
    }
    if (found_count == 0) {
        std::cout << "No matches found for player " << player_name << ".\n";
    }
    std::cout << std::setfill('-') << std::setw(100) << "" << std::setfill(' ') << std::endl;
}

void GameResultManager::queryMatchesByStage(const std::string& stage_filter) {
    std::cout << "\n=== Matches in Stage: " << stage_filter << " ===\n";
    std::cout << std::setfill('-') << std::setw(100) << "" << std::setfill(' ') << std::endl;
    std::cout << std::left << std::setw(10) << "MatchID" << std::setw(8) << "Round"
              << std::setw(20) << "Player1" << std::setw(5) << "vs" << std::setw(20) << "Player2"
              << std::setw(15) << "Winner" << std::setw(10) << "Score" 
              << std::setw(20) << "Scheduled Time" << std::endl;
    std::cout << std::setfill('-') << std::setw(100) << "" << std::setfill(' ') << std::endl;

    int found_count = 0;
    for (int i = 0; i < match_history.size(); ++i) {
        MatchResult match;
        if (match_history.getAt(i, match)) {
            if (match.stage == stage_filter) {
                found_count++;
                std::string p1_name = "P_ID:" + std::to_string(match.player1_id);
                std::string p2_name = "P_ID:" + std::to_string(match.player2_id);
                std::string winner_name = "P_ID:" + std::to_string(match.winner_id);

                int p1_idx = findPlayerIndex(match.player1_id);
                if (p1_idx != -1) p1_name = player_stats[p1_idx].name;
                int p2_idx = findPlayerIndex(match.player2_id);
                if (p2_idx != -1) p2_name = player_stats[p2_idx].name;
                int w_idx = findPlayerIndex(match.winner_id);
                if (w_idx != -1) winner_name = player_stats[w_idx].name;
                else if (match.winner_id == 0) winner_name = "N/A (Draw/TBD)";

                std::cout << std::left << std::setw(10) << match.match_id
                          << std::setw(8) << match.round
                          << std::setw(20) << p1_name
                          << std::setw(5) << "vs"
                          << std::setw(20) << p2_name
                          << std::setw(15) << winner_name
                          << std::setw(10) << match.score
                          << std::setw(20) << match.scheduled_time << std::endl;
            }
        }
    }
    if (found_count == 0) {
        std::cout << "No matches found for stage '" << stage_filter << "'.\n";
    }
    std::cout << std::setfill('-') << std::setw(100) << "" << std::setfill(' ') << std::endl;
}


int GameResultManager::getTotalMatches() const {
    return match_history.size();
}

int GameResultManager::getTotalPlayers() const {
    return current_player_count;
}

void GameResultManager::createSampleFiles() {
    // Create sample players.csv (overwrites if exists)
    std::ofstream playersFile("players.csv");
    if (playersFile.is_open()) {
        playersFile << "player_id,name,rank,contact,registration_time\r\n"; // Header
        playersFile << "101,Alice Johnson,Pro,alice@email.com,2025-01-15 09:00:00\r\n";
        playersFile << "102,Bob Smith,Amateur,bob@email.com,2025-01-16 10:30:00\r\n";
        playersFile << "103,Charlie Brown,Pro,charlie@email.com,2025-01-17 11:15:00\r\n";
        playersFile << "104,Diana Prince,Expert,diana@email.com,2025-01-18 14:20:00\r\n";
        playersFile << "105,Eve Wilson,Amateur,eve@email.com,2025-01-19 16:45:00\r\n";
        playersFile.close();
        std::cout << "✓ Sample players.csv created/updated.\n";
    } else {
        std::cerr << "✗ Error creating sample players.csv.\n";
    }

    // Create sample matches.csv (overwrites if exists)
    std::ofstream matchesFile("matches.csv");
    if (matchesFile.is_open()) {
        matchesFile << "match_id,stage,group_id,round,player1_id,player2_id,scheduled_time,status,winner_id,score\r\n"; // Header
        matchesFile << "501,group,1,1,101,102,2025-05-20 10:00:00,completed,101,18-15\r\n";
        matchesFile << "502,group,1,1,103,104,2025-05-20 11:00:00,completed,104,21-17\r\n";
        matchesFile << "503,group,2,1,105,101,2025-05-21 09:30:00,completed,105,22-20\r\n";
        matchesFile << "504,knockout,0,2,104,105,2025-05-22 15:00:00,completed,104,25-23\r\n";
        matchesFile.close();
        std::cout << "✓ Sample matches.csv created/updated.\n";
    } else {
        std::cerr << "✗ Error creating sample matches.csv.\n";
    }

    // Create empty spectators.csv if it doesn't exist, or leave it if it does.
    // Task3 loads it and mentions "starting fresh" if not found.
    std::ofstream spectatorsFile("spectators.csv", std::ios::app); // Open in append to not clear it if it exists
     if (spectatorsFile.is_open()) {
        spectatorsFile.seekp(0, std::ios::end);
        if (spectatorsFile.tellp() == 0) { // File is empty, add header
             spectatorsFile << "spectator_id,name,category,wants_live_stream,supported_player,day,payment_amount\r\n";
             std::cout << "✓ Sample (empty) spectators.csv with header created.\n";
        } else {
            // std::cout << "✓ spectators.csv already exists.\n";
        }
        spectatorsFile.close();
    } else {
        std::cerr << "✗ Error creating/checking sample spectators.csv.\n";
    }
}


void GameResultManager::demonstrateMatchLogging() {
    std::cout << "\n=== Demonstrating Task 4 Match Logging ===\n";
    MatchResult demo_matches[] = {
        MatchResult(2001, "group", 3, 2, 102, 103, "2025-05-28 10:00:00", "completed", 103, "19-21"),
        MatchResult(2002, "knockout", 0, 1, 101, 105, "2025-05-28 11:00:00", "completed", 101, "2-0"),
        MatchResult(2003, "final", 0, 1, 101, 104, "2025-05-28 14:00:00", "pending", 0, "0-0")
    };
    int num_demo_matches = sizeof(demo_matches) / sizeof(MatchResult);
    std::cout << "Logging " << num_demo_matches << " new sample match results to matches.csv and Task 4 system...\n";
    for (int i = 0; i < num_demo_matches; ++i) {
        logMatchResult(demo_matches[i], "matches.csv");
    }
    std::cout << "Task 4 match logging demonstration complete.\n";
    std::cout << "Note: Task 3's internal match list is NOT updated by this demo automatically.\n";
    std::cout << "Task 4 features (recent matches, player stats) will reflect these new matches.\n";
    std::cout << "To use these matches in Task 3 logic, you would typically restart or reload matches for Task 3.\n";
}


// ======== INTEGRATED MAIN FUNCTION ========
int main() {
    // Initialize managers/lists from both tasks
    GameResultManager grm(100); // Max 100 players for GameResultManager stats
    SpectatorList spectatorRegistry;
    Task3MatchList task3MatchData; // For Task 3 specific match logic

    std::cout << "=====================================================================\n";
    std::cout << "=== Asia Pacific University Esports Championship Management System ===\n";
    std::cout << "=====================================================================\n";
    std::cout << "Initializing system...\n\n";

    // 1. Create sample CSV files (players.csv, matches.csv with initial data; empty spectators.csv with header if new)
    // This step ensures essential files exist for loading. It will overwrite players.csv and matches.csv.
    grm.createSampleFiles();

    // 2. Load data from CSV files
    std::cout << "\n--- Loading Data ---\n";
    spectatorRegistry.loadFromCSV("spectators.csv");
    task3MatchData.loadFromCSV("matches.csv"); // Task 3 uses its own match list structure
    grm.loadPlayerData("players.csv");      // Task 4 loads player data
    grm.loadMatchHistory("matches.csv");    // Task 4 loads match history (and updates player stats from it)
    std::cout << "--- Data Loading Complete ---\n";    int choice;
    do {        std::cout << "\n\n========= Main Menu =========\n";
        // --- Task 3 Options ---
        std::cout << "--- Spectator & Live Stream (Task 3) ---\n";
        std::cout << " 1. Register New Spectator(s)\n";
        std::cout << " 2. Display All Registered Spectators\n";
        std::cout << " 3. Display Active Spectators (for a round/event name)\n";
        std::cout << " 4. Update Spectator Status (based on match results for a stage/round)\n";
        std::cout << " 5. Simulate Spectator Queue Management (for current active spectators)\n";
        std::cout << " 6. Save All Spectator Data to spectators.csv\n";
        
        // --- Task 4 Options ---
        std::cout << "--- Game Results & Performance (Task 4) ---\n";
        std::cout << " 7. Display Recent Matches (Last 5)\n";
        std::cout << " 8. Display All Player Statistics\n";
        std::cout << " 9. Display Specific Player Statistics\n";
        std::cout << "10. Query Matches for a Player\n";
        std::cout << "11. Query Matches by Stage\n";
        std::cout << "12. Log New Match Result (to matches.csv & Task 4 system)\n";
        std::cout << "13. Display Task 4 System Info (Data Structures, Files)\n";
        std::cout << "14. (Dev) Demonstrate Task 4 Match Logging (adds 3 sample matches)\n";

        std::cout << "--- General ---\n";
        std::cout << " 0. Exit\n";
        std::cout << "=============================\n";
        std::cout << "Choose an option: ";
        
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            choice = -1; // Invalid choice to loop again
            continue;        
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume newline

        switch (choice) {
            // --- Task 3 Cases ---
            case 1: { // Register New Spectator(s)
                int num_spectators;
                std::cout << "Enter number of spectators to register: ";
                std::cin >> num_spectators;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                for (int i = 0; i < num_spectators; ++i) {
                    std::cout << "\n--- Registering Spectator #" << (i + 1) << " ---\n";
                    std::string spec_id, spec_name, spec_cat, spec_day, spec_supported_player, live_stream_choice_str;
                    bool wants_live_stream;
                    int payment_choice;

                    std::cout << "Spectator ID: "; getline(std::cin, spec_id);
                    std::cout << "Spectator Name: "; getline(std::cin, spec_name);
                    
                    std::cout << "Enter attendance date (YYYY-MM-DD): "; getline(std::cin, spec_day);
                    task3MatchData.printGroupStageRound1Matches(spec_day); // Show matches for the day
                    
                    // Simplified player selection for brevity; original had more complex match picking.
                    // This assumes player IDs are known or will be manually entered.
                    // For a full merge, the match selection logic from Task3 main would be here.
                    // The original selected a match, then one of the two players from that match.
                    // For now, direct input of supported player ID.
                    std::cout << "Enter Player ID they support (e.g., 101): "; getline(std::cin, spec_supported_player);


                    std::cout << "Choose payment tier:\n1. VIP (500)\n2. Influencer (350)\n3. General (200)\nEnter choice (1-3): ";
                    std::cin >> payment_choice;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    switch (payment_choice) {
                        case 1: spec_cat = "VIP"; break;
                        case 2: spec_cat = "Influencer"; break;
                        case 3: spec_cat = "General"; break;
                        default: std::cout << "Invalid choice, defaulting to General.\n"; spec_cat = "General"; break;
                    }

                    std::cout << "Wants live stream access? (yes/no): "; getline(std::cin, live_stream_choice_str);
                    wants_live_stream = (live_stream_choice_str == "yes" || live_stream_choice_str == "Yes");

                    spectatorRegistry.registerSpectator(spec_id, spec_name, wants_live_stream, spec_supported_player, spec_cat, spec_day);
                }
                break;
            }
            case 2: spectatorRegistry.displaySpectators(); break;
            case 3: {
                std::string round_event_name;
                std::cout << "Enter round/event name to display active spectators for (e.g., 'Group Round 1'): ";
                getline(std::cin, round_event_name);
                spectatorRegistry.displayActiveSpectators(round_event_name);
                break;
            }
            case 4: { // Update Spectator Status
                std::string stage_to_update, round_num_str;
                int round_to_update;
                std::cout << "Enter stage to update spectators for (e.g., group, knockout): ";
                getline(std::cin, stage_to_update);
                std::cout << "Enter round number for this stage: ";
                std::cin >> round_to_update;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                updateSpectatorsByMatchResults(spectatorRegistry, task3MatchData, stage_to_update, round_to_update);
                break;
            }
            case 5: simulateQueueManagement(spectatorRegistry); break;            case 6: spectatorRegistry.saveToCSV("spectators.csv"); break;

            // --- Task 4 Cases ---
            case 7: grm.displayRecentMatches(5); break;
            case 8: grm.displayAllPlayerStats(); break;
            case 9: {
                int pid;
                std::cout << "Enter Player ID to display stats: ";
                std::cin >> pid;
                if (std::cin.fail()) {
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid Player ID.\n"; break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                grm.displayPlayerStats(pid);
                break;
            }            
            case 10: {
                int pid;
                std::cout << "Enter Player ID to query matches: ";
                std::cin >> pid;
                 if (std::cin.fail()) {
                    std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid Player ID.\n"; break;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                grm.queryMatchesByPlayer(pid);
                break;
            }            case 11: {
                std::string stage_str;
                std::cout << "Enter Stage to query (e.g., group, knockout, final): ";
                getline(std::cin, stage_str);
                grm.queryMatchesByStage(stage_str);
                break;
            }
            case 12: { // Log New Match Result (Task 4)
                MatchResult new_match;
                std::cout << "--- Log New Match Result ---\n";
                std::cout << "Enter Match ID: "; std::cin >> new_match.match_id;
                std::cout << "Enter Stage (e.g., group, knockout): "; std::cin >> new_match.stage;
                std::cout << "Enter Group ID (0 if not applicable): "; std::cin >> new_match.group_id;
                std::cout << "Enter Round number: "; std::cin >> new_match.round;
                std::cout << "Enter Player 1 ID: "; std::cin >> new_match.player1_id;
                std::cout << "Enter Player 2 ID: "; std::cin >> new_match.player2_id;
                std::cout << "Enter Scheduled Time (YYYY-MM-DD HH:MM:SS): "; std::cin.ignore(); getline(std::cin, new_match.scheduled_time);
                std::cout << "Enter Status (e.g., completed, pending): "; std::cin >> new_match.status;
                std::cout << "Enter Winner ID (0 if no winner/pending): "; std::cin >> new_match.winner_id;
                std::cout << "Enter Score (e.g., 21-19, 2-0): "; std::cin >> new_match.score;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if(grm.logMatchResult(new_match, "matches.csv")) {
                    // Player stats are updated within logMatchResult
                    // Recent matches stack and history queue in GRM are updated
                } else {
                    std::cout << "Failed to log match.\n";
                }
                break;
            }
            case 13: { // Display Task 4 System Info
                std::cout << "\n=== Task 4 System Information ===\n";
                std::cout << "Data Structures Used by GameResultManager:\n";
                std::cout << "  - ResultStack<MatchResult>: For recent matches (LIFO). Max capacity: 100. Current size: " << grm.getTotalMatches() /*approx, as stack might be smaller*/ << "\n";
                std::cout << "  - HistoryQueue<MatchResult>: For chronological match history (FIFO). Max capacity: 1000. Current size: " << grm.getTotalMatches() << "\n";
                std::cout << "  - PlayerStats[] (Dynamic Array): For player statistics. Max capacity: " << 100 /*as per grm init*/ << ". Current players: " << grm.getTotalPlayers() << "\n";
                std::cout << "Files Managed:\n";
                std::cout << "  - players.csv: Stores player details.\n";
                std::cout << "  - matches.csv: Stores all match results. Appended to by logging.\n";
                break;
            }
             case 14: grm.demonstrateMatchLogging(); break;

            // --- General ---
            case 0: std::cout << "Exiting system. Thank you!\n"; break;
            default: std::cout << "Invalid option. Please try again.\n"; break;
        }
    } while (choice != 0);

    // Optional: Auto-save data on exit
    // spectatorRegistry.saveToCSV("spectators.csv");
    // No automatic save for matches.csv as it's appended during logging. players.csv is mostly static after load.

    return 0;
}