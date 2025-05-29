#include "task4.hpp"

// Constructor - Initialize the game result manager with specified capacity
GameResultManager::GameResultManager(int max_player_capacity) 
    : max_players(max_player_capacity), current_player_count(0) {
    // Allocate memory for player statistics array
    player_stats = new PlayerStats[max_players];
}

// Destructor - Clean up dynamically allocated memory
GameResultManager::~GameResultManager() {
    delete[] player_stats;
}

// Helper function to split CSV line into tokens
// This handles basic CSV parsing by splitting on commas
void GameResultManager::splitCSVLine(const std::string& line, std::string tokens[], int max_tokens) {
    std::stringstream ss(line);
    std::string token;
    int index = 0;
    
    while (std::getline(ss, token, ',') && index < max_tokens) {
        // Remove leading and trailing whitespace
        size_t start = token.find_first_not_of(" \t\r\n");
        size_t end = token.find_last_not_of(" \t\r\n");
        
        if (start != std::string::npos && end != std::string::npos) {
            tokens[index] = token.substr(start, end - start + 1);
        } else {
            tokens[index] = "";
        }
        index++;
    }
    
    // Fill remaining tokens with empty strings
    while (index < max_tokens) {
        tokens[index] = "";
        index++;
    }
}

// Helper function to find player index by player ID
int GameResultManager::findPlayerIndex(int player_id) {
    for (int i = 0; i < current_player_count; i++) {
        if (player_stats[i].player_id == player_id) {
            return i;
        }
    }
    return -1;  // Player not found
}

// Helper function to parse score string and extract numeric value
// Handles various score formats like "3-1", "15-10", "21-19"
double GameResultManager::parseScore(const std::string& score_str) {
    if (score_str.empty()) return 0.0;
    
    // Find the first number in the score string
    std::stringstream ss(score_str);
    std::string first_score;
    std::getline(ss, first_score, '-');
    
    try {
        return std::stod(first_score);
    } catch (...) {
        return 0.0;
    }
}

// Helper function to update player statistics based on match result
void GameResultManager::updatePlayerStats(const MatchResult& match) {
    // Update statistics for both players involved in the match
    int player1_index = findPlayerIndex(match.player1_id);
    int player2_index = findPlayerIndex(match.player2_id);
    
    if (player1_index != -1) {
        player_stats[player1_index].total_matches++;
        if (match.winner_id == match.player1_id) {
            player_stats[player1_index].wins++;
        } else {
            player_stats[player1_index].losses++;
        }
        
        // Update average score (simple running average)
        double new_score = parseScore(match.score);
        int total_matches = player_stats[player1_index].total_matches;
        player_stats[player1_index].avg_score = 
            ((player_stats[player1_index].avg_score * (total_matches - 1)) + new_score) / total_matches;
    }
    
    if (player2_index != -1) {
        player_stats[player2_index].total_matches++;
        if (match.winner_id == match.player2_id) {
            player_stats[player2_index].wins++;
        } else {
            player_stats[player2_index].losses++;
        }
        
        // Update average score for player 2 (assuming second part of score)
        std::string score_copy = match.score;
        size_t dash_pos = score_copy.find('-');
        double new_score = 0.0;
        if (dash_pos != std::string::npos && dash_pos + 1 < score_copy.length()) {
            try {
                new_score = std::stod(score_copy.substr(dash_pos + 1));
            } catch (...) {
                new_score = 0.0;
            }
        }
        
        int total_matches = player_stats[player2_index].total_matches;
        player_stats[player2_index].avg_score = 
            ((player_stats[player2_index].avg_score * (total_matches - 1)) + new_score) / total_matches;
    }
}

// Load player data from players.csv file
// Format: player_id, name, rank, contact, registration_time
bool GameResultManager::loadPlayerData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return false;
    }
    
    std::string line;
    current_player_count = 0;
    
    // Skip header line
    if (std::getline(file, line)) {
        // Process data lines
        while (std::getline(file, line) && current_player_count < max_players) {
            if (line.empty()) continue;
            
            std::string tokens[5];
            splitCSVLine(line, tokens, 5);
            
            // Parse player data
            try {
                int player_id = std::stoi(tokens[0]);
                std::string name = tokens[1];
                std::string rank = tokens[2];
                std::string contact = tokens[3];
                std::string reg_time = tokens[4];
                
                // Create player stats entry
                player_stats[current_player_count] = PlayerStats(player_id, name, rank, contact, reg_time);
                current_player_count++;
            } catch (...) {
                std::cerr << "Warning: Could not parse player data line: " << line << std::endl;
            }
        }
    }
    
    file.close();
    std::cout << "Loaded " << current_player_count << " players from " << filename << std::endl;
    return true;
}

// Load existing matches from matches.csv file
// Format: match_id, stage, group_id, round, player1_id, player2_id, scheduled_time, status, winner_id, score
bool GameResultManager::loadMatchHistory(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return false;
    }
    
    std::string line;
    int matches_loaded = 0;
    
    // Skip header line
    if (std::getline(file, line)) {
        // Process data lines
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            std::string tokens[10];
            splitCSVLine(line, tokens, 10);
            
            // Parse match data
            try {
                int match_id = std::stoi(tokens[0]);
                std::string stage = tokens[1];
                int group_id = std::stoi(tokens[2]);
                int round = std::stoi(tokens[3]);
                int player1_id = std::stoi(tokens[4]);
                int player2_id = std::stoi(tokens[5]);
                std::string scheduled_time = tokens[6];
                std::string status = tokens[7];
                int winner_id = std::stoi(tokens[8]);
                std::string score = tokens[9];
                
                // Create match result
                MatchResult match(match_id, stage, group_id, round, player1_id, player2_id,
                                scheduled_time, status, winner_id, score);
                
                // Add to both queue (for history) and stack (for recent access)
                match_history.enqueue(match);
                recent_matches.push(match);
                
                // Update player statistics
                updatePlayerStats(match);
                
                matches_loaded++;
            } catch (...) {
                std::cerr << "Warning: Could not parse match data line: " << line << std::endl;
            }
        }
    }
    
    file.close();
    std::cout << "Loaded " << matches_loaded << " matches from " << filename << std::endl;
    return true;
}

// Log a new match result and append to matches.csv
bool GameResultManager::logMatchResult(const MatchResult& match, const std::string& filename) {
    // Add to data structures
    recent_matches.push(match);
    match_history.enqueue(match);
    
    // Update player statistics
    updatePlayerStats(match);
    
    // Append to CSV file
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << " for writing" << std::endl;
        return false;
    }
    
    // Write match data in CSV format
    file << match.match_id << "," << match.stage << "," << match.group_id << ","
         << match.round << "," << match.player1_id << "," << match.player2_id << ","
         << match.scheduled_time << "," << match.status << "," << match.winner_id << ","
         << match.score << std::endl;
    
    file.close();
    std::cout << "Match " << match.match_id << " logged successfully" << std::endl;
    return true;
}

// Display the most recent N matches using the Stack data structure
// Stack is ideal here because we want to see the latest matches first (LIFO order)
void GameResultManager::displayRecentMatches(int count) {
    std::cout << "\n=== Recent Matches (Last " << count << ") ===\n";
    std::cout << std::setfill('-') << std::setw(80) << "" << std::setfill(' ') << std::endl;
    
    if (recent_matches.isEmpty()) {
        std::cout << "No matches available.\n";
        return;
    }
    
    int stack_size = recent_matches.size();
    int matches_to_show = (count < stack_size) ? count : stack_size;
    
    for (int i = 0; i < matches_to_show; i++) {
        MatchResult match;
        if (recent_matches.getFromTop(i, match)) {
            // Get player names
            std::string player1_name = "Unknown";
            std::string player2_name = "Unknown";
            std::string winner_name = "Unknown";
            
            int p1_index = findPlayerIndex(match.player1_id);
            int p2_index = findPlayerIndex(match.player2_id);
            int winner_index = findPlayerIndex(match.winner_id);
            
            if (p1_index != -1) player1_name = player_stats[p1_index].name;
            if (p2_index != -1) player2_name = player_stats[p2_index].name;
            if (winner_index != -1) winner_name = player_stats[winner_index].name;
            
            std::cout << "Match ID: " << match.match_id << " | Stage: " << match.stage
                      << " | Round: " << match.round << std::endl;
            std::cout << "Players: " << player1_name << " vs " << player2_name << std::endl;
            std::cout << "Winner: " << winner_name << " | Score: " << match.score
                      << " | Status: " << match.status << std::endl;
            std::cout << std::setfill('-') << std::setw(40) << "" << std::setfill(' ') << std::endl;
        }
    }
}

// Display player performance statistics
void GameResultManager::displayPlayerStats(int player_id) {
    int index = findPlayerIndex(player_id);
    if (index == -1) {
        std::cout << "Player with ID " << player_id << " not found.\n";
        return;
    }
    
    PlayerStats& stats = player_stats[index];
    std::cout << "\n=== Player Statistics ===\n";
    std::cout << "Player ID: " << stats.player_id << std::endl;
    std::cout << "Name: " << stats.name << std::endl;
    std::cout << "Rank: " << stats.rank << std::endl;
    std::cout << "Total Matches: " << stats.total_matches << std::endl;
    std::cout << "Wins: " << stats.wins << std::endl;
    std::cout << "Losses: " << stats.losses << std::endl;
    
    if (stats.total_matches > 0) {
        double win_rate = (double)stats.wins / stats.total_matches * 100;
        std::cout << "Win Rate: " << std::fixed << std::setprecision(1) << win_rate << "%" << std::endl;
    }
    
    std::cout << "Average Score: " << std::fixed << std::setprecision(2) << stats.avg_score << std::endl;
}

// Display all player statistics
void GameResultManager::displayAllPlayerStats() {
    std::cout << "\n=== All Player Statistics ===\n";
    std::cout << std::setfill('-') << std::setw(80) << "" << std::setfill(' ') << std::endl;
    
    for (int i = 0; i < current_player_count; i++) {
        PlayerStats& stats = player_stats[i];
        if (stats.total_matches > 0) {  // Only show players who have played matches
            double win_rate = (stats.total_matches > 0) ? (double)stats.wins / stats.total_matches * 100 : 0;
            
            std::cout << "ID: " << stats.player_id << " | Name: " << std::setw(15) << stats.name
                      << " | Matches: " << std::setw(3) << stats.total_matches
                      << " | W: " << std::setw(2) << stats.wins
                      << " | L: " << std::setw(2) << stats.losses
                      << " | Win Rate: " << std::fixed << std::setprecision(1) << std::setw(5) << win_rate << "%"
                      << " | Avg Score: " << std::fixed << std::setprecision(2) << stats.avg_score << std::endl;
        }
    }
}

// Query matches by player ID using the Queue data structure
// Queue is ideal for this because it maintains chronological order of matches
void GameResultManager::queryMatchesByPlayer(int player_id) {
    std::cout << "\n=== Matches for Player ID " << player_id << " ===\n";
    std::cout << std::setfill('-') << std::setw(60) << "" << std::setfill(' ') << std::endl;
    
    std::string player_name = "Unknown";
    int player_index = findPlayerIndex(player_id);
    if (player_index != -1) {
        player_name = player_stats[player_index].name;
    }
    
    std::cout << "Player: " << player_name << std::endl;
    std::cout << std::setfill('-') << std::setw(60) << "" << std::setfill(' ') << std::endl;
    
    int queue_size = match_history.size();
    int matches_found = 0;
    
    for (int i = 0; i < queue_size; i++) {
        MatchResult match;
        if (match_history.getAt(i, match)) {
            // Check if this match involves the specified player
            if (match.player1_id == player_id || match.player2_id == player_id) {
                // Get opponent name
                int opponent_id = (match.player1_id == player_id) ? match.player2_id : match.player1_id;
                std::string opponent_name = "Unknown";
                int opponent_index = findPlayerIndex(opponent_id);
                if (opponent_index != -1) {
                    opponent_name = player_stats[opponent_index].name;
                }
                
                std::string result = (match.winner_id == player_id) ? "WIN" : "LOSS";
                
                std::cout << "Match " << match.match_id << " | vs " << opponent_name
                          << " | " << result << " | Score: " << match.score
                          << " | Stage: " << match.stage << std::endl;
                matches_found++;
            }
        }
    }
    
    if (matches_found == 0) {
        std::cout << "No matches found for this player.\n";
    } else {
        std::cout << "\nTotal matches found: " << matches_found << std::endl;
    }
}

// Query matches by stage (e.g., "group", "knockout")
void GameResultManager::queryMatchesByStage(const std::string& stage) {
    std::cout << "\n=== Matches in Stage: " << stage << " ===\n";
    std::cout << std::setfill('-') << std::setw(60) << "" << std::setfill(' ') << std::endl;
    
    int queue_size = match_history.size();
    int matches_found = 0;
    
    for (int i = 0; i < queue_size; i++) {
        MatchResult match;
        if (match_history.getAt(i, match)) {
            // Check if this match is in the specified stage
            if (match.stage == stage) {
                // Get player names
                std::string player1_name = "Unknown";
                std::string player2_name = "Unknown";
                std::string winner_name = "Unknown";
                
                int p1_index = findPlayerIndex(match.player1_id);
                int p2_index = findPlayerIndex(match.player2_id);
                int winner_index = findPlayerIndex(match.winner_id);
                
                if (p1_index != -1) player1_name = player_stats[p1_index].name;
                if (p2_index != -1) player2_name = player_stats[p2_index].name;
                if (winner_index != -1) winner_name = player_stats[winner_index].name;
                
                std::cout << "Match " << match.match_id << " | " << player1_name 
                          << " vs " << player2_name << " | Winner: " << winner_name
                          << " | Score: " << match.score << " | Round: " << match.round << std::endl;
                matches_found++;
            }
        }
    }
    
    if (matches_found == 0) {
        std::cout << "No matches found in stage: " << stage << std::endl;
    } else {
        std::cout << "\nTotal matches found: " << matches_found << std::endl;
    }
}

// Get total number of matches in history
int GameResultManager::getTotalMatches() const {
    return match_history.size();
}

// Get total number of players
int GameResultManager::getTotalPlayers() const {
    return current_player_count;
}

// Create sample CSV files for demonstration
void GameResultManager::createSampleFiles() {
    // Create sample players.csv file
    std::ofstream playersFile("players.csv");
    if (playersFile.is_open()) {
        playersFile << "player_id,name,rank,contact,registration_time\n";
        playersFile << "101,Alice Johnson,Pro,alice@email.com,2025-01-15 09:00:00\n";
        playersFile << "102,Bob Smith,Amateur,bob@email.com,2025-01-16 10:30:00\n";
        playersFile << "103,Charlie Brown,Pro,charlie@email.com,2025-01-17 11:15:00\n";
        playersFile << "104,Diana Prince,Expert,diana@email.com,2025-01-18 14:20:00\n";
        playersFile << "105,Eve Wilson,Amateur,eve@email.com,2025-01-19 16:45:00\n";
        playersFile.close();
        std::cout << "✓ Created sample players.csv file\n";
    }
    
    // Create sample matches.csv file with header
    std::ofstream matchesFile("matches.csv");
    if (matchesFile.is_open()) {
        matchesFile << "match_id,stage,group_id,round,player1_id,player2_id,scheduled_time,status,winner_id,score\n";
        matchesFile << "501,group,1,1,101,102,2025-05-20 10:00:00,completed,101,18-15\n";
        matchesFile << "502,group,1,1,103,104,2025-05-20 11:00:00,completed,104,21-17\n";
        matchesFile << "503,group,2,1,105,101,2025-05-21 09:30:00,completed,105,22-20\n";
        matchesFile << "504,knockout,0,2,104,105,2025-05-22 15:00:00,completed,104,25-23\n";
        matchesFile.close();
        std::cout << "✓ Created sample matches.csv file\n";
    }
}

// Function to demonstrate logging a new match
void GameResultManager::demonstrateMatchLogging() {
    std::cout << "\n=== Demonstrating Match Logging ===\n";
    
    // Sample match results to demonstrate the system
    MatchResult sample_matches[] = {
        MatchResult(1001, "group", 1, 1, 101, 102, "2025-05-27 10:00:00", "completed", 101, "21-19"),
        MatchResult(1002, "group", 1, 1, 103, 104, "2025-05-27 11:00:00", "completed", 104, "15-21"),
        MatchResult(1003, "knockout", 0, 2, 101, 104, "2025-05-27 14:00:00", "completed", 101, "25-18")
    };
    
    std::cout << "Logging 3 sample match results...\n";
    
    for (int i = 0; i < 3; i++) {
        if (logMatchResult(sample_matches[i], "matches.csv")) {
            std::cout << "✓ Successfully logged match " << sample_matches[i].match_id << std::endl;
        } else {
            std::cout << "✗ Failed to log match " << sample_matches[i].match_id << std::endl;
        }
    }
    
    std::cout << "\nMatch logging demonstration completed!\n";
}

// Function to display menu options
void GameResultManager::displayMenu() {
    std::cout << "\n=== Esports Championship Management System ===\n";
    std::cout << "=== Game Result Logging & Performance History ===\n";
    std::cout << "1. Display Recent Matches (Last 5)\n";
    std::cout << "2. Display All Player Statistics\n";
    std::cout << "3. Display Specific Player Statistics\n";
    std::cout << "4. Query Matches for a Player\n";
    std::cout << "5. Query Matches by Stage\n";
    std::cout << "6. Log New Match Result\n";
    std::cout << "7. Display System Information\n";
    std::cout << "0. Exit\n";
    std::cout << "Choose an option: ";
}

// Main program execution
void GameResultManager::runProgram() {
    std::cout << "=== Asia Pacific University Esports Championship Management System ===\n";
    std::cout << "=== Task 4: Game Result Logging & Performance History ===\n\n";
    
    // Create sample data files for demonstration
    std::cout << "Creating sample data files...\n";
    createSampleFiles();
    
    // Load player data and match history
    std::cout << "\nLoading existing data...\n";
    if (!loadPlayerData("players.csv")) {
        std::cerr << "Failed to load player data. Creating empty player set.\n";
    }
    
    if (!loadMatchHistory("matches.csv")) {
        std::cerr << "Failed to load match history. Starting with empty history.\n";
    }
    
    // Demonstrate the system with sample data
    std::cout << "\n=== System Demonstration ===\n";
    
    // 1. Display initial system status
    std::cout << "\n1. Initial System Status:\n";
    std::cout << "   Total Players: " << getTotalPlayers() << std::endl;
    std::cout << "   Total Matches: " << getTotalMatches() << std::endl;
    
    // 2. Demonstrate match logging
    std::cout << "\n2. Logging new match results...\n";
    demonstrateMatchLogging();
    
    // 3. Display recent matches (demonstrates Stack usage)
    std::cout << "\n3. Displaying recent matches (Stack demonstration):\n";
    std::cout << "   Using Stack data structure for quick access to latest matches:\n";
    displayRecentMatches(5);
    
    // 4. Display player performance statistics
    std::cout << "\n4. Player Performance Statistics (Array-based structure):\n";
    displayAllPlayerStats();
    
    // 5. Query matches by specific player (demonstrates Queue usage)
    std::cout << "\n5. Querying matches for specific players (Queue demonstration):\n";
    std::cout << "   Using Queue data structure to maintain chronological order:\n";
    queryMatchesByPlayer(101);  // Alice Johnson
    queryMatchesByPlayer(104);  // Diana Prince
    
    // 6. Query matches by stage
    std::cout << "\n6. Querying matches by stage:\n";
    queryMatchesByStage("group");
    queryMatchesByStage("knockout");
    
    // Interactive menu for further exploration
    int choice;
    do {
        displayMenu();
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                displayRecentMatches(5);
                break;
            }
            case 2: {
                displayAllPlayerStats();
                break;
            }
            case 3: {
                int player_id;
                std::cout << "Enter Player ID: ";
                std::cin >> player_id;
                displayPlayerStats(player_id);
                break;
            }
            case 4: {
                int player_id;
                std::cout << "Enter Player ID: ";
                std::cin >> player_id;
                queryMatchesByPlayer(player_id);
                break;
            }
            case 5: {
                std::string stage;
                std::cout << "Enter Stage (group/knockout): ";
                std::cin >> stage;
                queryMatchesByStage(stage);
                break;
            }
            case 6: {
                // Interactive match logging
                MatchResult new_match;
                std::cout << "Enter Match Details:\n";
                std::cout << "Match ID: ";
                std::cin >> new_match.match_id;
                std::cout << "Stage: ";
                std::cin >> new_match.stage;
                std::cout << "Group ID: ";
                std::cin >> new_match.group_id;
                std::cout << "Round: ";
                std::cin >> new_match.round;
                std::cout << "Player 1 ID: ";
                std::cin >> new_match.player1_id;
                std::cout << "Player 2 ID: ";
                std::cin >> new_match.player2_id;
                std::cout << "Winner ID: ";
                std::cin >> new_match.winner_id;
                std::cout << "Score (format: X-Y): ";
                std::cin >> new_match.score;
                
                new_match.scheduled_time = "2025-05-27 12:00:00";
                new_match.status = "completed";
                
                if (logMatchResult(new_match, "matches.csv")) {
                    std::cout << "Match logged successfully!\n";
                } else {
                    std::cout << "Failed to log match.\n";
                }
                break;
            }            
            case 7: {
                std::cout << "\n=== System Information ===\n";
                std::cout << "Data Structures Used:\n";
                std::cout << "• Stack: Stores recent match results for quick access (LIFO)\n";
                std::cout << "  - Justification: Latest matches are most relevant for review\n";
                std::cout << "  - Operations: push(), pop(), peek(), getFromTop()\n";
                std::cout << "• Queue: Maintains chronological match history (FIFO)\n";
                std::cout << "  - Justification: Preserves tournament timeline for analysis\n";
                std::cout << "  - Operations: enqueue(), dequeue(), getAt()\n";
                std::cout << "• Array: Stores player statistics for efficient lookup\n";
                std::cout << "  - Justification: Direct access by player index\n";
                std::cout << "  - Features: Wins, losses, average score tracking\n\n";
                
                std::cout << "File Management:\n";
                std::cout << "• matches.csv: Match results storage and retrieval\n";
                std::cout << "• players.csv: Player data and performance mapping\n\n";
                
                std::cout << "Current Status:\n";
                std::cout << "• Total Players: " << getTotalPlayers() << std::endl;
                std::cout << "• Total Matches: " << getTotalMatches() << std::endl;
                break;
            }
            case 0: {
                std::cout << "Exiting system. Thank you!\n";
                break;
            }
            default: {
                std::cout << "Invalid option. Please try again.\n";
                break;
            }
        }
    } while (choice != 0);
}

// Main function
int main() {
    GameResultManager manager(100);  // Support up to 100 players
    manager.runProgram();
    return 0;
}