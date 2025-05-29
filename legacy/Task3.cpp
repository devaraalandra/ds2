#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <string>
using namespace std;

// -------- Spectator Class --------
class Spectator {
public:
    string id, name, supportedPlayer, category, day;
    bool wantsLiveStream;
    int paymentAmount;
    bool active; // active in tournament or left
    Spectator* next;

    Spectator(string _id, string _name, bool _wantsLiveStream, string _supportedPlayer,
              string _category, string _day)
        : id(_id), name(_name), wantsLiveStream(_wantsLiveStream),
          supportedPlayer(_supportedPlayer), category(_category), day(_day),
          next(nullptr), active(true) {
        assignPaymentAmount();
    }

    void assignPaymentAmount() {
        if (category == "VIP") paymentAmount = 500;
        else if (category == "Influencer") paymentAmount = 350;
        else paymentAmount = 200;
    }
};

// -------- SpectatorList (Linked List) --------
class SpectatorList {
    Spectator* head;
public:
    SpectatorList() : head(nullptr) {}

    Spectator* getHead() const { return head; }

    void registerSpectator(const string& id, const string& name, bool wantsLiveStream,
                           const string& supportedPlayer, const string& category, const string& day) {
        Spectator* newNode = new Spectator(id, name, wantsLiveStream, supportedPlayer, category, day);
        if (!head) head = newNode;
        else {
            Spectator* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
    }

    void loadFromCSV(const char* filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "No existing spectators file found, starting fresh.\n";
            return;
        }
        string line;
        getline(file, line); // skip header
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string id, name, category, wantsLiveStreamStr, supportedPlayer, day, paymentStr;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, category, ',');
            getline(ss, wantsLiveStreamStr, ',');
            getline(ss, supportedPlayer, ',');
            getline(ss, day, ',');
            getline(ss, paymentStr, ',');

            bool wantsLiveStream = (wantsLiveStreamStr == "Yes" || wantsLiveStreamStr == "yes");
            registerSpectator(id, name, wantsLiveStream, supportedPlayer, category, day);
        }
        file.close();
    }

    void displaySpectators() const {
        Spectator* temp = head;
        cout << "\n[All Spectators]:\n";
        while (temp) {
            cout << "ID: " << temp->id
                 << ", Name: " << temp->name
                 << ", Category: " << temp->category
                 << ", Wants Live Stream: " << (temp->wantsLiveStream ? "Yes" : "No")
                 << ", Supported Player: " << temp->supportedPlayer
                 << ", Attendance Day: " << temp->day
                 << ", Payment: " << temp->paymentAmount
                 << ", Active: " << (temp->active ? "Yes" : "No") << "\n";
            temp = temp->next;
        }
    }

    void displayActiveSpectators(const string& roundName) const {
        cout << "\n[Active Spectators - " << roundName << "]:\n";
        Spectator* temp = head;
        int count = 1;
        while (temp) {
            if (temp->active) {
                cout << count++ << ". " << temp->name << " (Supports Player ID: " << temp->supportedPlayer << ")\n";
            }
            temp = temp->next;
        }
    }
};

// -------- Match Class --------
class Match {
public:
    int matchId;
    string team1, team2;
    string scheduledTime;
    string stage;
    int round;
    string status;
    string winnerId;
    Match* next;

    Match(int id, const string& t1, const string& t2, const string& sched, const string& stg,
          int rnd, const string& sts = "", const string& winner = "")
        : matchId(id), team1(t1), team2(t2), scheduledTime(sched), stage(stg), round(rnd),
          status(sts), winnerId(winner), next(nullptr) {}
};

// -------- MatchList (Linked List) --------
class MatchList {
    Match* head;
public:
    MatchList() : head(nullptr) {}
    ~MatchList() {
        Match* cur = head;
        while (cur) {
            Match* tmp = cur;
            cur = cur->next;
            delete tmp;
        }
    }
    Match* getHead() const { return head; }

    void loadFromCSV(const char* filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Cannot open file: " << filename << "\n";
            return;
        }
        string line;
        getline(file, line); // skip header
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string idStr, stage, groupId, roundStr, p1, p2, sched, status, winnerId, score;
            getline(ss, idStr, ',');
            getline(ss, stage, ',');
            getline(ss, groupId, ','); // ignore
            getline(ss, roundStr, ',');
            getline(ss, p1, ',');
            getline(ss, p2, ',');
            getline(ss, sched, ',');
            getline(ss, status, ',');
            getline(ss, winnerId, ',');
            getline(ss, score, ',');

            int matchId = stoi(idStr);
            int round = stoi(roundStr);
            Match* newNode = new Match(matchId, p1, p2, sched, stage, round, status, winnerId);

            if (!head) head = newNode;
            else {
                Match* cur = head;
                while (cur->next) cur = cur->next;
                cur->next = newNode;
            }
        }
        file.close();
    }

    void printGroupStageRound1Matches(const string& day) const {
        Match* cur = head;
        int matchIndex = 1;
        while (cur) {
            if (cur->stage == "group" && cur->round == 1 && cur->scheduledTime.find(day) == 0) {
                cout << "Match " << matchIndex++ << ": Player " << cur->team1 << " vs Player " << cur->team2
                     << ", Scheduled: " << cur->scheduledTime << ", Round: " << cur->round << "\n";
            }
            cur = cur->next;
        }
    }
};

// -------- StringSet (linked list based set) --------
class StringSet {
    struct Node {
        string val;
        Node* next;
        Node(const string& v) : val(v), next(nullptr) {}
    };
    Node* head;
public:
    StringSet() : head(nullptr) {}
    ~StringSet() {
        Node* cur = head;
        while (cur) {
            Node* tmp = cur;
            cur = cur->next;
            delete tmp;
        }
    }
    bool contains(const string& s) const {
        Node* cur = head;
        while (cur) {
            if (cur->val == s) return true;
            cur = cur->next;
        }
        return false;
    }
    void insert(const string& s) {
        if (contains(s)) return;
        Node* newNode = new Node(s);
        newNode->next = head;
        head = newNode;
    }
};

// -------- Priority Seating Queue (linked list) --------
class PrioritySeatingQueue {
    struct Node {
        Spectator* data;
        Node* next;
        Node(Spectator* sp) : data(sp), next(nullptr) {}
    };
    Node* front;

    int getPriority(const string& category) {
        if (category == "VIP") return 1;
        if (category == "Influencer") return 2;
        return 3;
    }
public:
    PrioritySeatingQueue() : front(nullptr) {}

    void enqueue(Spectator* sp) {
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

    void displayQueue() const {
        cout << "\n[Seating Queue - Priority Order]:\n";
        Node* temp = front;
        int pos = 1;
        while (temp) {
            cout << pos++ << ". " << temp->data->name << " (" << temp->data->category << ")\n";
            temp = temp->next;
        }
    }
};

// -------- MultiLevelLiveStreamQueue (linked list) --------
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

    int countList(Node* head) const {
        int cnt = 0;
        while (head) {
            cnt++;
            head = head->next;
        }
        return cnt;
    }

public:
    MultiLevelLiveStreamQueue(int cap)
        : influencerHead(nullptr), vipHead(nullptr), generalHead(nullptr), capacity(cap) {}

    void enqueue(Spectator* sp) {
        if (getTotalCount() >= capacity) {
            cout << "Live stream full, " << sp->name << " cannot join.\n";
            return;
        }
        Node** target;
        if (sp->category == "Influencer") target = &influencerHead;
        else if (sp->category == "VIP") target = &vipHead;
        else target = &generalHead;

        Node* newNode = new Node(sp);
        if (!*target) *target = newNode;
        else {
            Node* temp = *target;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
    }

    void displayQueue() const {
        cout << "\n[Live Stream Queue - Priority Groups]:\n";
        int count = 1;
        count = displayGroup("Influencers", influencerHead, count);
        count = displayGroup("VIPs", vipHead, count);
        displayGroup("Generals", generalHead, count);
    }

private:
    int displayGroup(const string& label, Node* head, int count) const {
        cout << "-- " << label << " --\n";
        while (head) {
            cout << count++ << ". " << head->data->name << "\n";
            head = head->next;
        }
        return count;
    }

    int getTotalCount() const {
        return countList(influencerHead) + countList(vipHead) + countList(generalHead);
    }
};

// -------- CircularStreamRotation (linked list) --------
// Implementing circular rotation with linked list

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
    CircularStreamRotation(int cap) : tail(nullptr), size(0), capacity(cap) {}

    ~CircularStreamRotation() {
        if (!tail) return;
        Node* current = tail->next;
        tail->next = nullptr;
        while (current) {
            Node* tmp = current;
            current = current->next;
            delete tmp;
        }
    }

    bool isFull() const { return size == capacity; }
    bool isEmpty() const { return size == 0; }

    void enqueue(Spectator* sp) {
        if (isFull()) {
            cout << "Rotation full, cannot add: " << sp->name << "\n";
            return;
        }
        Node* newNode = new Node(sp);
        if (!tail) {
            tail = newNode;
            tail->next = tail;
        } else {
            newNode->next = tail->next;
            tail->next = newNode;
            tail = newNode;
        }
        size++;
    }

    Spectator* dequeue() {
        if (isEmpty()) return nullptr;
        Node* head = tail->next;
        Spectator* sp = head->data;
        if (tail == head) { // only one node
            delete head;
            tail = nullptr;
        } else {
            tail->next = head->next;
            delete head;
        }
        size--;
        return sp;
    }

    void displayQueue() const {
        cout << "\n[Circular Live Stream Rotation]:\n";
        if (isEmpty()) {
            cout << "(empty)\n";
            return;
        }
        Node* current = tail->next;
        int count = 1;
        do {
            cout << count++ << ". " << current->data->name << "\n";
            current = current->next;
        } while (current != tail->next);
    }
};

// -------- WatchHistoryStack (linked list stack) --------
class WatchHistoryStack {
    struct Node {
        Spectator* data;
        Node* next;
        Node(Spectator* sp) : data(sp), next(nullptr) {}
    };
    Node* top;

public:
    WatchHistoryStack() : top(nullptr) {}

    void push(Spectator* sp) {
        Node* newNode = new Node(sp);
        newNode->next = top;
        top = newNode;
    }

    void display() const {
        cout << "\n[Watch History - Stack]:\n";
        Node* current = top;
        int count = 1;
        while (current) {
            cout << count++ << ". " << current->data->name << "\n";
            current = current->next;
        }
    }
};

// -------- WaitingQueue (linked list queue) --------
class WaitingQueue {
    struct Node {
        Spectator* data;
        Node* next;
        Node(Spectator* sp) : data(sp), next(nullptr) {}
    };
    Node* front;
    Node* rear;
public:
    WaitingQueue() : front(nullptr), rear(nullptr) {}

    bool isEmpty() const { return front == nullptr; }

    void enqueue(Spectator* sp) {
        Node* newNode = new Node(sp);
        if (!rear) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
    }

    Spectator* dequeue() {
        if (!front) return nullptr;
        Node* tmp = front;
        Spectator* sp = tmp->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete tmp;
        return sp;
    }
};

// -------- Update spectators based on match results --------
void updateSpectatorsByMatchResults(SpectatorList& spectators, MatchList& matches,
                                    const string& stage, int round) {
    StringSet winners;
    Match* cur = matches.getHead();
    while (cur) {
        if (cur->stage == stage && cur->round == round && cur->status == "completed") {
            winners.insert(cur->winnerId);
        }
        cur = cur->next;
    }

    Spectator* sp = spectators.getHead();
    while (sp) {
        if (sp->active && !winners.contains(sp->supportedPlayer)) {
            sp->active = false;
            cout << "Spectator " << sp->name << " left because supported player " << sp->supportedPlayer << " lost.\n";
        }
        sp = sp->next;
    }
}

// -------- Simulate queue management --------
void simulateQueueManagement(SpectatorList& regList) {
    PrioritySeatingQueue seatQueue;
    MultiLevelLiveStreamQueue streamQueue(10);
    CircularStreamRotation rotation(5);
    WatchHistoryStack history;
    WaitingQueue waitingQueue;

    Spectator* ptr = regList.getHead();
    while (ptr) {
        if (!ptr->active) {
            ptr = ptr->next;
            continue;
        }
        seatQueue.enqueue(ptr);
        if (ptr->wantsLiveStream) {
            streamQueue.enqueue(ptr);
            if (!rotation.isFull()) {
                rotation.enqueue(ptr);
            } else {
                cout << ptr->name << " added to waiting queue for rotation\n";
                waitingQueue.enqueue(ptr);
            }
            history.push(ptr);
        }
        ptr = ptr->next;
    }

    seatQueue.displayQueue();
    streamQueue.displayQueue();
    rotation.displayQueue();
    history.display();

    cout << "\nRotating live stream...\n";
    Spectator* removed = rotation.dequeue();
    if (removed) {
        cout << "Removed from rotation: " << removed->name << "\n";
    }
    if (!waitingQueue.isEmpty()) {
        Spectator* nextInLine = waitingQueue.dequeue();
        rotation.enqueue(nextInLine);
        cout << "Moved from waiting queue to rotation: " << nextInLine->name << "\n";
    } else {
        cout << "Moved from waiting queue to rotation: (none, waiting queue empty)\n";
    }

    rotation.displayQueue();
}

// -------- Main --------
int main() {
    SpectatorList registrationList;
    MatchList matches;

    registrationList.loadFromCSV("spectators.csv");
    matches.loadFromCSV("matches.csv");

    int numSpectators = 0;
    cout << "Enter number of spectators to register: ";
    cin >> numSpectators;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    for (int i = 0; i < numSpectators; i++) {
        string id, name, supportedTeam, supportedPlayer, category, day;
        bool wantsLiveStream = false;
        string wantsLiveStreamStr;
        int paymentChoice = 3;

        cout << "\nEnter details for Spectator #" << (i + 1) << "\n";

        cout << "ID: ";
        getline(cin, id);
        cout << "Name: ";
        getline(cin, name);

        while (true) {
            cout << "Enter attendance date (YYYY-MM-DD): ";
            getline(cin, day);

            cout << "\nMatches on " << day << " (Group Stage Round 1):\n";
            matches.printGroupStageRound1Matches(day);

            int matchChoice = 0;
            do {
                cout << "Select a match to watch (1-8): ";
                cin >> matchChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } while (matchChoice < 1 || matchChoice > 8);

            Match* selectedMatch = matches.getHead();
            for (int j = 1; j < matchChoice; j++) {
                selectedMatch = selectedMatch->next;
            }

            cout << "Choose a team to support:\n";
            cout << "1. " << selectedMatch->team1 << "\n";
            cout << "2. " << selectedMatch->team2 << "\n";

            int teamChoice = 0;
            do {
                cout << "Enter choice (1 or 2): ";
                cin >> teamChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } while (teamChoice != 1 && teamChoice != 2);

            supportedTeam = (teamChoice == 1) ? selectedMatch->team1 : selectedMatch->team2;
            supportedPlayer = supportedTeam;
            break;
        }

        cout << "Choose payment tier:\n1. VIP (500)\n2. Influencer (350)\n3. General (200)\nEnter choice: ";
        cin >> paymentChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (paymentChoice) {
            case 1: category = "VIP"; break;
            case 2: category = "Influencer"; break;
            case 3: category = "General"; break;
            default:
                category = "General";
                cout << "Invalid choice, defaulting to General.\n";
        }

        cout << "Wants live stream (Yes/No): ";
        getline(cin, wantsLiveStreamStr);
        wantsLiveStream = (wantsLiveStreamStr == "Yes" || wantsLiveStreamStr == "yes");

        registrationList.registerSpectator(id, name, wantsLiveStream, supportedPlayer, category, day);
    }

    cout << "\nRegistered Spectators:\n";
    registrationList.displaySpectators();

    ofstream file("spectators.csv");
    if (file.is_open()) {
        file << "spectator_id,name,category,wants_live_stream,supported_player,day,payment_amount\r\n";
        Spectator* sptr = registrationList.getHead();
        while (sptr) {
            file << sptr->id << "," << sptr->name << "," << sptr->category << ","
                 << (sptr->wantsLiveStream ? "Yes" : "No") << "," << sptr->supportedPlayer << ","
                 << sptr->day << "," << sptr->paymentAmount << "\r\n";
            sptr = sptr->next;
        }
        file.close();
        cout << "Spectator data saved to spectators.csv\n";
    } else {
        cout << "Failed to open spectators.csv for writing\n";
    }

    // Run tournament rounds with spectator updates

    cout << "\n=== Round 1 Update ===\n";
    updateSpectatorsByMatchResults(registrationList, matches, "group", 1);
    registrationList.displayActiveSpectators("Round 1");
    simulateQueueManagement(registrationList);

    cout << "\n=== Round 2 Update ===\n";
    updateSpectatorsByMatchResults(registrationList, matches, "group", 2);
    registrationList.displayActiveSpectators("Round 2");
    simulateQueueManagement(registrationList);

    cout << "\n=== Knockout Round 1 Update ===\n";
    updateSpectatorsByMatchResults(registrationList, matches, "knockout", 1);
    registrationList.displayActiveSpectators("Knockout Round 1");
    simulateQueueManagement(registrationList);

    cout << "\n=== Knockout Round 2 Update ===\n";
    updateSpectatorsByMatchResults(registrationList, matches, "knockout", 2);
    registrationList.displayActiveSpectators("Knockout Round 2");
    simulateQueueManagement(registrationList);

    return 0;
}
