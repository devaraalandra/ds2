#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "Registration.hpp"

// Queue methods
void Queue::init() {
    front = NULL;
    rear = NULL;
    size = 0;
}

int Queue::isEmpty() {
    return front == NULL;
}

void Queue::enqueue(struct Player* player) {
    player->next = NULL;
    if (isEmpty()) {
        front = rear = player;
    } else {
        rear->next = player;
        rear = player;
    }
    size++;
}

struct Player* Queue::dequeue() {
    if (isEmpty()) return NULL;
    struct Player* temp = front;
    front = front->next;
    if (front == NULL) rear = NULL;
    size--;
    return temp;
}

struct Player* Queue::peek() {
    return front;
}

int Queue::getSize() {
    return size;
}

void Queue::destroy() {
    while (!isEmpty()) {
        struct Player* temp = dequeue();
        free(temp);
    }
}

// PriorityQueue methods
void PriorityQueue::init() {
    earlyBirdQueue.init();
    wildcardQueue.init();
    standardQueue.init();
    lastMinuteQueue.init();
    waitlistQueue.init();
    size = 0;
}

void PriorityQueue::enqueue(struct Player* player) {
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

struct Player* PriorityQueue::dequeue() {
    struct Player* player = NULL;
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

int PriorityQueue::isEmpty() {
    return earlyBirdQueue.isEmpty() && wildcardQueue.isEmpty() &&
           standardQueue.isEmpty() && lastMinuteQueue.isEmpty();
}

int PriorityQueue::getSize() {
    return size;
}

struct Queue* PriorityQueue::getWaitlistQueue() {
    return &waitlistQueue;
}

void PriorityQueue::setWaitlistQueue(struct Queue* q) {
    waitlistQueue.destroy();
    waitlistQueue.init();
    struct Player* current = q->front;
    while (current) {
        struct Player* newPlayer = (struct Player*)malloc(sizeof(struct Player));
        memcpy(newPlayer, current, sizeof(struct Player));
        newPlayer->next = NULL;
        waitlistQueue.enqueue(newPlayer);
        current = current->next;
    }
    while (!q->isEmpty()) {
        struct Player* temp = q->dequeue();
        free(temp);
    }
}

// Helper: Convert input to lowercase and check for "yes"/"y"
int isYesResponse(const char* input) {
    char lower[10];
    int i;
    for (i = 0; input[i] && i < 9; i++) {
        lower[i] = tolower(input[i]);
    }
    lower[i] = '\0';
    return strcmp(lower, "yes") == 0 || strcmp(lower, "y") == 0;
}

// Helper: Check if file exists
int fileExists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// Forward declaration for isNumeric
int isNumeric(const char* str);

// Helper: Get last player ID from CSV
int getLastPlayerID(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return 1000;
    char line[256];
    int maxID = 1000;
    fgets(line, 256, file); // Skip header
    while (fgets(line, 256, file)) {
        if (line[0] == '\0' || line[0] == '\n') continue;
        char* token = strtok(line, ",");
        if (token && isNumeric(token)) {
            int id = atoi(token);
            if (id > maxID) maxID = id;
        }
    }
    fclose(file);
    return maxID;
}

// Helper: Sanitize string for CSV
void sanitizeForCSV(char* dest, const char* src, int maxLen) {
    int i, j = 0;
    for (i = 0; src[i] && j < maxLen - 1; i++) {
        if (src[i] >= 32 && src[i] <= 126) {
            dest[j++] = (src[i] == ',') ? ';' : src[i];
        }
    }
    dest[j] = '\0';
}

// Helper: Trim whitespace and normalize registration type
void normalizeRegistrationType(char* dest, const char* src, int maxLen) {
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

// Helper: Validate numeric string
int isNumeric(const char* str) {
    if (!str || !str[0]) return 0;
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

// Clear input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Wait for Enter
void waitForEnter() {
    printf("\nPress Enter to return to the main menu...");
    clearInputBuffer();
}

// Display waitlist
void displayWaitlist(struct PriorityQueue* pq) {
    printf("\n+------------------------------+\n");
    printf("|        WAITLIST PLAYERS      |\n");
    printf("+------------------------------+\n");
    
    struct Queue* waitlist = pq->getWaitlistQueue();
    if (waitlist->isEmpty()) {
        printf("Waitlist is empty.\n");
        waitForEnter();
        return;
    }
    
    printf("%-10s %-20s %-15s %-30s %-5s\n",
           "Player ID", "Player Name", "Type", "Email", "Rank");
    printf("------------------------------------------------------------\n");
    
    struct Queue tempWaitlist;
    tempWaitlist.init();
    
    while (!waitlist->isEmpty()) {
        struct Player* player = waitlist->dequeue();
        printf("%-10s %-20s %-15s %-30s %-5c\n",
               player->playerID, player->playerName, player->registrationType,
               player->email, player->rank[0]);
        tempWaitlist.enqueue(player);
    }
    
    pq->setWaitlistQueue(&tempWaitlist);
    waitForEnter();
}

// Read players from CSV
void readPlayersFromCSV(struct PriorityQueue* pq, const char* filename) {
    if (!fileExists(filename)) {
        printf("CSV file not found. Starting with empty registration list.\n");
        return;
    }
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening CSV file '%s'.\n", filename);
        return;
    }

    char line[256];
    if (!fgets(line, 256, file)) {
        printf("Warning: CSV file is empty or header missing.\n");
        fclose(file);
        return;
    }

    while (fgets(line, 256, file)) {
        if (line[0] == '\n' || line[0] == '\0') continue;
        
        struct Player* player = (struct Player*)malloc(sizeof(struct Player));
        player->ranking = 0;
        player->teamID = 0;
        player->checkInStatus = 0;
        player->group = 0;
        player->next = NULL;
        strcpy(player->checkInTime, "N/A");
        player->rank[0] = '\0';
        player->rank[1] = '\0';
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
                case 6: if (token[0]) {
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
            strncpy(player->status, token, 9);
            player->status[9] = '\0';
        }

        int valid = 1;
        if (!isNumeric(player->playerID) || atoi(player->playerID) < 1000) valid = 0;
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
            free(player);
        }
    }
    fclose(file);
}

// Write players to CSV
void writePlayersToCSV(struct PriorityQueue* pq, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error opening CSV file for writing!\n");
        return;
    }

    fprintf(file, "Player ID,Player Name,Registration Type,Email,Rank,Check-In,Time of Check-In,Status\n");
    
    struct PriorityQueue temp;
    temp.init();
    char sanitized[100];
    
    while (!pq->isEmpty()) {
        struct Player* player = pq->dequeue();
        if (!player->playerID[0] || !player->playerName[0]) {
            free(player);
            continue;
        }
        sanitizeForCSV(sanitized, player->playerID, 10);
        fprintf(file, "%s,", sanitized);
        sanitizeForCSV(sanitized, player->playerName, 50);
        fprintf(file, "%s,", sanitized);
        sanitizeForCSV(sanitized, player->registrationType, 20);
        fprintf(file, "%s,", sanitized);
        sanitizeForCSV(sanitized, player->email, 100);
        fprintf(file, "%s,", sanitized);
        fprintf(file, "%c,%s,", player->rank[0], player->checkInStatus ? "YES" : "NO");
        sanitizeForCSV(sanitized, player->checkInTime, 50);
        fprintf(file, "%s,", sanitized);
        sanitizeForCSV(sanitized, player->status, 10);
        fprintf(file, "%s\n", sanitized);
        temp.enqueue(player);
    }
    
    while (!temp.isEmpty()) {
        pq->enqueue(temp.dequeue());
    }
    
    struct Queue* waitlist = pq->getWaitlistQueue();
    struct Queue tempWaitlist;
    tempWaitlist.init();
    
    while (!waitlist->isEmpty()) {
        struct Player* player = waitlist->dequeue();
        if (!player->playerID[0] || !player->playerName[0]) {
            free(player);
            continue;
        }
        sanitizeForCSV(sanitized, player->playerID, 10);
        fprintf(file, "%s,", sanitized);
        sanitizeForCSV(sanitized, player->playerName, 50);
        fprintf(file, "%s,", sanitized);
        sanitizeForCSV(sanitized, player->registrationType, 20);
        fprintf(file, "%s,", sanitized);
        sanitizeForCSV(sanitized, player->email, 100);
        fprintf(file, "%s,", sanitized);
        fprintf(file, "%c,%s,", player->rank[0], player->checkInStatus ? "YES" : "NO");
        sanitizeForCSV(sanitized, player->checkInTime, 50);
        fprintf(file, "%s,", sanitized);
        sanitizeForCSV(sanitized, player->status, 10);
        fprintf(file, "%s\n", sanitized);
        tempWaitlist.enqueue(player);
    }
    
    pq->setWaitlistQueue(&tempWaitlist);
    fclose(file);
}

// Register player
void registerPlayer(struct PriorityQueue* pq, const char* filename) {
    struct Player* player = (struct Player*)malloc(sizeof(struct Player));
    player->ranking = 0;
    player->teamID = 0;
    player->checkInStatus = 0;
    player->group = 0;
    player->next = NULL;
    strcpy(player->checkInTime, "N/A");
    
    int lastID = getLastPlayerID(filename);
    sprintf(player->playerID, "%d", lastID + 1);
    
    printf("\n+------------------------------+\n");
    printf("|      PLAYER REGISTRATION      |\n");
    printf("+------------------------------+\n");
    printf("Player ID: %s\n", player->playerID);
    
    printf("Enter Player Name: ");
    fgets(player->playerName, 50, stdin);
    player->playerName[strcspn(player->playerName, "\n")] = '\0';
    
    printf("Enter Registration Type (Early-Bird/Standard/Wildcard/Last-Minute): ");
    char inputType[20];
    fgets(inputType, 20, stdin);
    inputType[strcspn(inputType, "\n")] = '\0';
    
    normalizeRegistrationType(player->registrationType, inputType, 20);
    
    if (strcmp(player->registrationType, "Early-Bird") == 0) {
        player->rank[0] = 'A';
    } else if (strcmp(player->registrationType, "Wildcard") == 0) {
        player->rank[0] = 'B';
    } else if (strcmp(player->registrationType, "Standard") == 0) {
        player->rank[0] = 'C';
    } else if (strcmp(player->registrationType, "Last-Minute") == 0) {
        player->rank[0] = 'D';
    } else {
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
    
    writePlayersToCSV(pq, filename);
    waitForEnter();
}

// Check-in player
void checkInPlayer(struct PriorityQueue* pq, const char* filename) {
    char playerID[10], continueCheckIn[10];
    int found;
    struct Player* foundPlayer;

    printf("\n+------------------------------+\n");
    printf("|       PLAYER CHECK-IN         |\n");
    printf("+------------------------------+\n");
    
    while (1) {
        printf("Enter Player ID to check-in: ");
        fgets(playerID, 10, stdin);
        playerID[strcspn(playerID, "\n")] = '\0';
        
        while (!playerID[0]) {
            printf("Player ID cannot be empty. Please enter Player ID: ");
            fgets(playerID, 10, stdin);
            playerID[strcspn(playerID, "\n")] = '\0';
        }

        struct PriorityQueue temp;
        temp.init();
        found = 0;
        foundPlayer = NULL;

        while (!pq->isEmpty()) {
            struct Player* player = pq->dequeue();
            if (strcmp(playerID, player->playerID) == 0) {
                found = 1;
                foundPlayer = player;
                if (!player->checkInStatus) {
                    player->checkInStatus = 1;
                    time_t now = time(NULL);
                    struct tm* timeinfo = localtime(&now);
                    char buffer[50];
                    strftime(buffer, 50, "%Y-%m-%d %H:%M:%S", timeinfo);
                    strncpy(player->checkInTime, buffer, 49);
                    player->checkInTime[49] = '\0';
                }
            }
            temp.enqueue(player);
        }
        
        while (!temp.isEmpty()) {
            pq->enqueue(temp.dequeue());
        }

        if (found) {
            printf("Player %s (%s) checked in successfully at %s!\n",
                   playerID, foundPlayer->playerName, foundPlayer->checkInTime);
            writePlayersToCSV(pq, filename);
        } else {
            printf("Player %s not found in main queue!\n", playerID);
        }

        printf("\nDo you have more players to check in? (yes/no): ");
        fgets(continueCheckIn, 10, stdin);
        continueCheckIn[strcspn(continueCheckIn, "\n")] = '\0';
        if (!isYesResponse(continueCheckIn)) break;
    }

    waitForEnter();
}

// Withdraw player
void withdrawPlayer(struct PriorityQueue* pq, const char* playerID, const char* filename) {
    struct PriorityQueue temp;
    temp.init();
    int found = 0;
    char withdrawnRank[2] = "";
    char withdrawnName[50] = "";

    while (!pq->isEmpty()) {
        struct Player* player = pq->dequeue();
        if (strcmp(player->playerID, playerID) == 0) {
            found = 1;
            strcpy(withdrawnRank, player->rank);
            strcpy(withdrawnName, player->playerName);
            free(player);
        } else {
            temp.enqueue(player);
        }
    }
    
    while (!temp.isEmpty()) {
        pq->enqueue(temp.dequeue());
    }

    if (found) {
        printf("Player %s (%s) withdrawn successfully.\n", playerID, withdrawnName);
        
        struct Queue* waitlist = pq->getWaitlistQueue();
        struct Queue tempWaitlist;
        tempWaitlist.init();
        int promoted = 0;
        struct Player* promotedPlayer = NULL;
        
        while (!waitlist->isEmpty()) {
            struct Player* player = waitlist->dequeue();
            if (!promoted && strcmp(player->rank, withdrawnRank) == 0) {
                promotedPlayer = player;
                strcpy(promotedPlayer->status, "MAIN");
                pq->enqueue(promotedPlayer);
                promoted = 1;
                printf("Player %s (%s) promoted from waitlist.\n",
                       promotedPlayer->playerID, promotedPlayer->playerName);
            } else {
                tempWaitlist.enqueue(player);
            }
        }
        
        pq->setWaitlistQueue(&tempWaitlist);
        
        if (!promoted) {
            printf("No player with rank %s found on waitlist for replacement.\n", withdrawnRank);
        }
        
        writePlayersToCSV(pq, filename);
    } else {
        printf("Player %s not found in main queue.\n", playerID);
    }
}

// Handle player withdrawal
void handleWithdrawPlayer(struct PriorityQueue* pq, const char* filename) {
    char playerID[10];
    
    printf("\n+------------------------------+\n");
    printf("|      PLAYER WITHDRAWAL       |\n");
    printf("+------------------------------+\n");
    printf("Enter Player ID to withdraw: ");
    fgets(playerID, 10, stdin);
    playerID[strcspn(playerID, "\n")] = '\0';
    
    withdrawPlayer(pq, playerID, filename);
    waitForEnter();
}

// Main program
int main() {
    struct PriorityQueue pq;
    pq.init();
    const char* filename = "Player_Registration.csv";
    
    printf("+------------------------------------------+\n");
    printf("|      APUEC Registration System v1.0      |\n");
    printf("+------------------------------------------+\n");
    printf("Loading player data...\n");
    readPlayersFromCSV(&pq, filename);
    printf("Current number of registered players: %d\n", pq.getSize());

    int choice;
    do {
        printf("\n+------------------------------------------+\n");
        printf("|           APUEC Main Menu                |\n");
        printf("+------------------------------------------+\n");
        printf("| 1. Register Player                       |\n");
        printf("| 2. Check-In Player                       |\n");
        printf("| 3. Withdraw Player                       |\n");
        printf("| 4. View Waitlist                         |\n");
        printf("| 5. Exit                                  |\n");
        printf("+------------------------------------------+\n");
        printf("Enter choice (1-5): ");
        
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("Invalid input! Please enter a number between 1 and 5.\n");
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: registerPlayer(&pq, filename); break;
            case 2: checkInPlayer(&pq, filename); break;
            case 3: handleWithdrawPlayer(&pq, filename); break;
            case 4: displayWaitlist(&pq); break;
            case 5: printf("\nExiting... Thank you for using APUEC Registration System!\n"); break;
            default: printf("Invalid choice! Please enter a number between 1 and 5.\n"); break;
        }
    } while (choice != 5);

    pq.earlyBirdQueue.destroy();
    pq.wildcardQueue.destroy();
    pq.standardQueue.destroy();
    pq.lastMinuteQueue.destroy();
    pq.waitlistQueue.destroy();
    return 0;
}