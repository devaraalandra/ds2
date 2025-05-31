#include "task4.hpp"
#include <iomanip>
#include <ctime>

// ========================= STACK IMPLEMENTATION =========================

bool Stack::push(const MatchResult& match) {
    // Check if stack is full
    if (top_index >= MAX_CAPACITY - 1) {
        std::cerr << "Warning: Stack is full, cannot add more matches\n";
        return false;
    }
    
    // Add match to top of stack
    data[++top_index] = match;
    return true;
}

bool Stack::getFromTop(int index, MatchResult& out) const {
    // Validate index (0 = topmost, 1 = second from top, etc.)
    if (index < 0 || index > top_index) {
        return false;
    }
    
    // Retrieve match at specified index from top
    out = data[top_index - index];
    return true;
}

// ========================= QUEUE IMPLEMENTATION =========================

bool Queue::enqueue(const MatchResult& match) {
    // Check if queue is full
    if (current_size >= MAX_CAPACITY) {
        std::cerr << "Warning: Queue is full, cannot add more matches\n";
        return false;
    }
    
    // Add match to rear of queue (circular array implementation)
    rear_index = (rear_index + 1) % MAX_CAPACITY;
    data[rear_index] = match;
    current_size++;
    return true;
}

bool Queue::getAt(int index, MatchResult& out) const {
    // Validate index
    if (index < 0 || index >= current_size) {
        return false;
    }
    
    // Calculate actual array index (circular array)
    int actual_index = (front_index + index) % MAX_CAPACITY;
    out = data[actual_index];
    return true;
}

// ===================== GAME RESULT MANAGER IMPLEMENTATION =====================

GameResultManager::GameResultManager(int max_players) 
    : max_players(max_players), current_player_count(0), next_match_id(1) {
    // Allocate dynamic array for player statistics
    player_stats = new PlayerStats[max_players];
}

GameResultManager::~GameResultManager() {
    // Deallocate dynamic memory
    delete[] player_stats;
}

bool GameResultManager::loadPlayerData(const std::string& filename) {
    std::ifstream file(filename);
    
    // Check if file can be opened
    if (!file.is_open()) {
        std::cerr << "Warning: Cannot open players file '" << filename << "'\n";
        return false;
    }
    
    std::string line;
    int loaded_count = 0;
    bool header_skipped = false;
    
    // Read file line by line
    while (std::getline(file, line) && current_player_count < max_players) {
        // Skip empty lines
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }
        
        // Skip header line (first non-empty line)
        if (!header_skipped) {
            header_skipped = true;
            continue;
        }
        
        // Parse CSV line into tokens - now expecting 6 fields
        std::string tokens[6];
        splitCSVLine(line, tokens, 6);
        
        // Validate and parse player ID
        if (tokens[0].empty()) {
            std::cerr << "Warning: Empty player ID in line: " << line << "\n";
            continue;
        }
        
        int player_id;
        try {
            player_id = std::stoi(tokens[0]);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid player ID '" << tokens[0] << "' in line: " << line << "\n";
            continue;
        }
        
        // Check for duplicate player ID
        if (findPlayerIndex(player_id) != -1) {
            std::cerr << "Warning: Duplicate player ID " << player_id << " found, skipping\n";
            continue;
        }
        
        // Create and store player data with updated field mapping:
        // tokens[0] = Player ID, tokens[1] = Player Name, tokens[2] = Registration Type, 
        // tokens[3] = Email, tokens[4] = Rank, tokens[5] = Check-In
        player_stats[current_player_count] = PlayerStats(
            player_id, tokens[1], tokens[4], tokens[3], tokens[2]
        );
        current_player_count++;
        loaded_count++;
    }
    
    file.close();
    std::cout << "Loaded " << loaded_count << " players from " << filename << "\n";
    return true;
}

bool GameResultManager::loadMatchHistory(const std::string& filename) {
    std::ifstream file(filename);
    
    // Check if file can be opened
    if (!file.is_open()) {
        std::cerr << "Warning: Cannot open matches file '" << filename << "'\n";
        return false;
    }
    
    std::string line;
    int loaded_count = 0;
    bool header_skipped = false;
    
    // Read file line by line
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }
        
        // Skip header line
        if (!header_skipped) {
            header_skipped = true;
            continue;
        }
        
        // Parse CSV line into tokens
        std::string tokens[10];
        splitCSVLine(line, tokens, 10);
        
        // Validate required fields
        if (tokens[0].empty() || tokens[4].empty() || tokens[5].empty()) {
            std::cerr << "Warning: Missing required fields in line: " << line << "\n";
            continue;
        }
        
        // Parse and validate numeric fields
        int match_id, group_id = 0, round = 0, player1_id, player2_id, winner_id = 0;
        
        try {
            match_id = std::stoi(tokens[0]);
            if (!tokens[2].empty()) group_id = std::stoi(tokens[2]);
            if (!tokens[3].empty()) round = std::stoi(tokens[3]);
            player1_id = std::stoi(tokens[4]);
            player2_id = std::stoi(tokens[5]);
            if (!tokens[8].empty()) winner_id = std::stoi(tokens[8]);
            
            // Update next_match_id to be higher than any existing match ID
            if (match_id >= next_match_id) {
                next_match_id = match_id + 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid numeric data in line: " << line << "\n";
            continue;
        }
        
        // Validate player IDs exist
        if (findPlayerIndex(player1_id) == -1) {
            std::cerr << "Warning: Player ID " << player1_id << " not found, skipping match\n";
            continue;
        }
        if (findPlayerIndex(player2_id) == -1) {
            std::cerr << "Warning: Player ID " << player2_id << " not found, skipping match\n";
            continue;
        }
        
        // Create match result
        MatchResult match(match_id, tokens[1], group_id, round, player1_id, 
                         player2_id, tokens[6], tokens[7], winner_id, tokens[9]);
        
        // Add to both data structures
        match_history.enqueue(match);
        recent_matches.push(match);
        
        // Update player statistics
        if (winner_id != 0) {
            updatePlayerStats(player1_id, (winner_id == player1_id), parseScore(tokens[9]));
            updatePlayerStats(player2_id, (winner_id == player2_id), parseScore(tokens[9]));
        }
        
        loaded_count++;
    }
    
    file.close();
    std::cout << "Loaded " << loaded_count << " matches from " << filename << "\n";
    std::cout << "Next available match ID: " << next_match_id << "\n";
    return true;
}

void GameResultManager::displayRecentMatches(int count) {
    std::cout << "\n" << std::string(100, '=') << "\n";
    std::cout << "                               RECENT MATCHES\n";
    std::cout << std::string(100, '=') << "\n";
    
    if (recent_matches.isEmpty()) {
        std::cout << "│" << std::setw(98) << std::left << " No recent matches available." << "│\n";
        std::cout << std::string(100, '=') << "\n";
        return;
    }
    
    int display_count = std::min(count, recent_matches.size());
    std::cout << "Showing last " << display_count << " matches:\n";
    std::cout << std::string(100, '-') << "\n";
    
    // Table header
    std::cout << "│" << std::left << std::setw(8) << " Match" << "│" 
              << std::setw(12) << " Stage" << "│"
              << std::setw(25) << " Players" << "│"
              << std::setw(15) << " Winner" << "│"
              << std::setw(12) << " Score" << "│"
              << std::setw(12) << " Date" << "│"
              << std::setw(12) << " Status" << "│\n";
    std::cout << "│" << std::string(7, '-') << "│" 
              << std::string(11, '-') << "│"
              << std::string(24, '-') << "│"
              << std::string(14, '-') << "│"
              << std::string(11, '-') << "│"
              << std::string(11, '-') << "│"
              << std::string(11, '-') << "│\n";
    
    // Display matches from most recent to oldest
    for (int i = 0; i < display_count; i++) {
        MatchResult match;
        if (recent_matches.getFromTop(i, match)) {
            // Get player names
            int p1_index = findPlayerIndex(match.player1_id);
            int p2_index = findPlayerIndex(match.player2_id);
            std::string p1_name = (p1_index != -1) ? player_stats[p1_index].name : "ID:" + std::to_string(match.player1_id);
            std::string p2_name = (p2_index != -1) ? player_stats[p2_index].name : "ID:" + std::to_string(match.player2_id);
            
            // Truncate long names
            if (p1_name.length() > 10) p1_name = p1_name.substr(0, 9) + ".";
            if (p2_name.length() > 10) p2_name = p2_name.substr(0, 9) + ".";
            
            std::string players = p1_name + " vs " + p2_name;
            if (players.length() > 24) players = players.substr(0, 21) + "...";
            
            // Determine winner name
            std::string winner_name = "Draw";
            if (match.winner_id != 0) {
                int winner_index = findPlayerIndex(match.winner_id);
                winner_name = (winner_index != -1) ? player_stats[winner_index].name : "ID:" + std::to_string(match.winner_id);
                if (winner_name.length() > 14) winner_name = winner_name.substr(0, 11) + "...";
            }
            
            // Extract date
            std::string date = extractDateFromScheduledTime(match.scheduled_time);
            if (date.empty()) date = "Unknown";
            
            // Truncate fields to fit table
            std::string stage = match.stage.length() > 11 ? match.stage.substr(0, 8) + "..." : match.stage;
            std::string score = match.score.length() > 11 ? match.score.substr(0, 8) + "..." : match.score;
            std::string status = match.status.length() > 11 ? match.status.substr(0, 8) + "..." : match.status;
            
            std::cout << "│" << std::left << std::setw(8) << (" " + std::to_string(match.match_id)) << "│" 
                      << std::setw(12) << (" " + stage) << "│"
                      << std::setw(25) << (" " + players) << "│"
                      << std::setw(15) << (" " + winner_name) << "│"
                      << std::setw(12) << (" " + score) << "│"
                      << std::setw(12) << (" " + date) << "│"
                      << std::setw(12) << (" " + status) << "│\n";
        }
    }
    
    std::cout << std::string(100, '=') << "\n";
}

void GameResultManager::displayPlayerStats(int player_id) {
    int index = findPlayerIndex(player_id);
    if (index == -1) {
        std::cout << "\n┌" << std::string(48, '-') << "┐\n";
        std::cout << "│" << std::setw(48) << std::left << " Player not found." << "│\n";
        std::cout << "└" << std::string(48, '-') << "┘\n";
        return;
    }
    
    const PlayerStats& player = player_stats[index];
    
    std::cout << "\n┌" << std::string(60, '-') << "┐\n";
    std::cout << "│" << std::setw(22) << " " << "PLAYER STATISTICS" << std::setw(21) << " " << "│\n";
    std::cout << "├" << std::string(60, '-') << "┤\n";
    
    // Player basic info
    std::cout << "│ " << std::left << std::setw(18) << "Player ID:" << "│ " << std::setw(38) << std::to_string(player.player_id) << "│\n";
    std::cout << "│ " << std::left << std::setw(18) << "Name:" << "│ " << std::setw(38) << player.name << "│\n";
    std::cout << "│ " << std::left << std::setw(18) << "Rank:" << "│ " << std::setw(38) << player.rank << "│\n";
    std::cout << "│ " << std::left << std::setw(18) << "Contact:" << "│ " << std::setw(38) << player.contact << "│\n";
    std::cout << "│ " << std::left << std::setw(18) << "Registration:" << "│ " << std::setw(38) << player.registration_time << "│\n";
    
    std::cout << "├" << std::string(60, '-') << "┤\n";
    
    // Performance statistics
    std::cout << "│ " << std::left << std::setw(18) << "Total Matches:" << "│ " << std::setw(38) << std::to_string(player.total_matches) << "│\n";
    std::cout << "│ " << std::left << std::setw(18) << "Wins:" << "│ " << std::setw(38) << std::to_string(player.wins) << "│\n";
    std::cout << "│ " << std::left << std::setw(18) << "Losses:" << "│ " << std::setw(38) << std::to_string(player.losses) << "│\n";
    
    // Calculate and display win rate
    std::string win_rate_str;
    if (player.total_matches > 0) {
        double win_rate = (static_cast<double>(player.wins) / player.total_matches) * 100.0;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << win_rate << "%";
        win_rate_str = oss.str();
    } else {
        win_rate_str = "N/A";
    }
    
    std::cout << "│ " << std::left << std::setw(18) << "Win Rate:" << "│ " << std::setw(38) << win_rate_str << "│\n";
    
    std::ostringstream avg_score_oss;
    avg_score_oss << std::fixed << std::setprecision(2) << player.avg_score;
    std::cout << "│ " << std::left << std::setw(18) << "Average Score:" << "│ " << std::setw(38) << avg_score_oss.str() << "│\n";
    
    std::cout << "└" << std::string(60, '-') << "┘\n";
}

void GameResultManager::displayAllPlayerStats() {
    std::cout << "\n" << std::string(95, '=') << "\n";
    std::cout << "                                ALL PLAYER STATISTICS\n";
    std::cout << std::string(95, '=') << "\n";
    
    if (current_player_count == 0) {
        std::cout << "│" << std::setw(93) << std::left << " No players loaded." << "│\n";
        std::cout << std::string(95, '=') << "\n";
        return;
    }
    
    // Display header
    std::cout << "┌" << std::string(5, '-') << "┬" << std::string(18, '-') << "┬" 
              << std::string(12, '-') << "┬" << std::string(10, '-') << "┬"
              << std::string(8, '-') << "┬" << std::string(10, '-') << "┬"
              << std::string(12, '-') << "┬" << std::string(12, '-') << "┐\n";
    
    std::cout << "│" << std::left << std::setw(5) << " ID" << "│" 
              << std::setw(18) << " Name" << "│"
              << std::setw(12) << " Rank" << "│" 
              << std::setw(10) << " Matches" << "│"
              << std::setw(8) << " Wins" << "│" 
              << std::setw(10) << " Losses" << "│"
              << std::setw(12) << " Win Rate" << "│" 
              << std::setw(12) << " Avg Score" << "│\n";
    
    std::cout << "├" << std::string(5, '-') << "┼" << std::string(18, '-') << "┼" 
              << std::string(12, '-') << "┼" << std::string(10, '-') << "┼"
              << std::string(8, '-') << "┼" << std::string(10, '-') << "┼"
              << std::string(12, '-') << "┼" << std::string(12, '-') << "┤\n";
    
    // Display each player's statistics
    for (int i = 0; i < current_player_count; i++) {
        const PlayerStats& player = player_stats[i];
        
        // Calculate win rate
        std::string win_rate_str = "N/A";
        if (player.total_matches > 0) {
            double win_rate = (static_cast<double>(player.wins) / player.total_matches) * 100.0;
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << win_rate << "%";
            win_rate_str = oss.str();
        }
        
        // Truncate long names and ranks
        std::string display_name = player.name.length() > 17 ? player.name.substr(0, 14) + "..." : player.name;
        std::string display_rank = player.rank.length() > 11 ? player.rank.substr(0, 8) + "..." : player.rank;
        
        std::ostringstream avg_score_oss;
        avg_score_oss << std::fixed << std::setprecision(2) << player.avg_score;
        
        std::cout << "│" << std::left << std::setw(5) << (" " + std::to_string(player.player_id)) << "│" 
                  << std::setw(18) << (" " + display_name) << "│"
                  << std::setw(12) << (" " + display_rank) << "│"
                  << std::setw(10) << (" " + std::to_string(player.total_matches)) << "│"
                  << std::setw(8) << (" " + std::to_string(player.wins)) << "│"
                  << std::setw(10) << (" " + std::to_string(player.losses)) << "│"
                  << std::setw(12) << (" " + win_rate_str) << "│"
                  << std::setw(12) << (" " + avg_score_oss.str()) << "│\n";
    }
    
    std::cout << "└" << std::string(5, '-') << "┴" << std::string(18, '-') << "┴" 
              << std::string(12, '-') << "┴" << std::string(10, '-') << "┴"
              << std::string(8, '-') << "┴" << std::string(10, '-') << "┴"
              << std::string(12, '-') << "┴" << std::string(12, '-') << "┘\n";
}

void GameResultManager::queryMatchesByPlayer(int player_id) {
    int player_index = findPlayerIndex(player_id);
    if (player_index == -1) {
        std::cout << "\n┌" << std::string(48, '-') << "┐\n";
        std::cout << "│" << std::setw(48) << std::left << " Player not found." << "│\n";
        std::cout << "└" << std::string(48, '-') << "┘\n";
        return;
    }
    
    std::cout << "\n" << std::string(90, '=') << "\n";
    std::cout << "                           MATCHES FOR PLAYER " << player_id << "\n";
    std::cout << "                           Player: " << player_stats[player_index].name << "\n";
    std::cout << std::string(90, '=') << "\n";
    
    bool found_matches = false;
    
    // Table header
    std::cout << "┌" << std::string(8, '-') << "┬" << std::string(18, '-') << "┬" 
              << std::string(12, '-') << "┬" << std::string(10, '-') << "┬"
              << std::string(12, '-') << "┬" << std::string(12, '-') << "┬"
              << std::string(12, '-') << "┐\n";
    
    std::cout << "│" << std::left << std::setw(8) << " Match" << "│" 
              << std::setw(18) << " Opponent" << "│"
              << std::setw(12) << " Stage" << "│" 
              << std::setw(10) << " Result" << "│"
              << std::setw(12) << " Score" << "│" 
              << std::setw(12) << " Date" << "│"
              << std::setw(12) << " Round" << "│\n";
    
    std::cout << "├" << std::string(8, '-') << "┼" << std::string(18, '-') << "┼" 
              << std::string(12, '-') << "┼" << std::string(10, '-') << "┼"
              << std::string(12, '-') << "┼" << std::string(12, '-') << "┼"
              << std::string(12, '-') << "┤\n";
    
    // Search through match history
    for (int i = 0; i < match_history.size(); i++) {
        MatchResult match;
        if (match_history.getAt(i, match)) {
            // Check if player is involved in this match
            if (match.player1_id == player_id || match.player2_id == player_id) {
                found_matches = true;
                
                // Determine opponent
                int opponent_id = (match.player1_id == player_id) ? match.player2_id : match.player1_id;
                int opponent_index = findPlayerIndex(opponent_id);
                std::string opponent_name = (opponent_index != -1) ? player_stats[opponent_index].name : "ID:" + std::to_string(opponent_id);
                
                // Truncate long opponent names
                if (opponent_name.length() > 17) opponent_name = opponent_name.substr(0, 14) + "...";
                
                // Determine result
                std::string result = "DRAW";
                if (match.winner_id == player_id) {
                    result = "WIN";
                } else if (match.winner_id != 0 && match.winner_id != player_id) {
                    result = "LOSS";
                }
                
                // Extract date from scheduled time
                std::string date = extractDateFromScheduledTime(match.scheduled_time);
                if (date.empty()) date = "Unknown";
                
                // Truncate fields to fit table
                std::string stage = match.stage.length() > 11 ? match.stage.substr(0, 8) + "..." : match.stage;
                std::string score = match.score.length() > 11 ? match.score.substr(0, 8) + "..." : match.score;
                
                std::cout << "│" << std::left << std::setw(8) << (" " + std::to_string(match.match_id)) << "│" 
                          << std::setw(18) << (" " + opponent_name) << "│"
                          << std::setw(12) << (" " + stage) << "│"
                          << std::setw(10) << (" " + result) << "│"
                          << std::setw(12) << (" " + score) << "│"
                          << std::setw(12) << (" " + date) << "│"
                          << std::setw(12) << (" " + std::to_string(match.round)) << "│\n";
            }
        }
    }
    
    if (!found_matches) {
        std::cout << "│" << std::setw(86) << std::left << " No matches found for this player." << "│\n";
    }
    
    std::cout << "└" << std::string(8, '-') << "┴" << std::string(18, '-') << "┴" 
              << std::string(12, '-') << "┴" << std::string(10, '-') << "┴"
              << std::string(12, '-') << "┴" << std::string(12, '-') << "┴"
              << std::string(12, '-') << "┘\n";
}

void GameResultManager::queryMatchesByStage(const std::string& stage) {
    std::cout << "\n" << std::string(90, '=') << "\n";
    std::cout << "                           MATCHES IN STAGE: " << stage << "\n";
    std::cout << std::string(90, '=') << "\n";
    
    bool found_matches = false;
    
    // Table header
    std::cout << "┌" << std::string(8, '-') << "┬" << std::string(25, '-') << "┬" 
              << std::string(15, '-') << "┬" << std::string(12, '-') << "┬"
              << std::string(12, '-') << "┬" << std::string(12, '-') << "┐\n";
    
    std::cout << "│" << std::left << std::setw(8) << " Match" << "│" 
              << std::setw(25) << " Players" << "│"
              << std::setw(15) << " Winner" << "│" 
              << std::setw(12) << " Score" << "│"
              << std::setw(12) << " Round" << "│" 
              << std::setw(12) << " Date" << "│\n";
    
    std::cout << "├" << std::string(8, '-') << "┼" << std::string(25, '-') << "┼" 
              << std::string(15, '-') << "┼" << std::string(12, '-') << "┼"
              << std::string(12, '-') << "┼" << std::string(12, '-') << "┤\n";
    
    // Search through match history
    for (int i = 0; i < match_history.size(); i++) {
        MatchResult match;
        if (match_history.getAt(i, match)) {
            // Check if match is in the specified stage
            if (match.stage == stage) {
                found_matches = true;
                
                // Get player names
                int p1_index = findPlayerIndex(match.player1_id);
                int p2_index = findPlayerIndex(match.player2_id);
                std::string p1_name = (p1_index != -1) ? player_stats[p1_index].name : "ID:" + std::to_string(match.player1_id);
                std::string p2_name = (p2_index != -1) ? player_stats[p2_index].name : "ID:" + std::to_string(match.player2_id);
                
                // Truncate long names
                if (p1_name.length() > 10) p1_name = p1_name.substr(0, 9) + ".";
                if (p2_name.length() > 10) p2_name = p2_name.substr(0, 9) + ".";
                
                std::string players = p1_name + " vs " + p2_name;
                if (players.length() > 24) players = players.substr(0, 21) + "...";
                
                // Determine winner name
                std::string winner_name = "Draw/None";
                if (match.winner_id != 0) {
                    int winner_index = findPlayerIndex(match.winner_id);
                    winner_name = (winner_index != -1) ? player_stats[winner_index].name : "ID:" + std::to_string(match.winner_id);
                    if (winner_name.length() > 14) winner_name = winner_name.substr(0, 11) + "...";
                }
                
                // Extract date from scheduled time
                std::string date = extractDateFromScheduledTime(match.scheduled_time);
                if (date.empty()) date = "Unknown";
                
                // Truncate score if too long
                std::string score = match.score.length() > 11 ? match.score.substr(0, 8) + "..." : match.score;
                
                std::cout << "│" << std::left << std::setw(8) << (" " + std::to_string(match.match_id)) << "│" 
                          << std::setw(25) << (" " + players) << "│"
                          << std::setw(15) << (" " + winner_name) << "│"
                          << std::setw(12) << (" " + score) << "│"
                          << std::setw(12) << (" " + std::to_string(match.round)) << "│"
                          << std::setw(12) << (" " + date) << "│\n";
            }
        }
    }
    
    if (!found_matches) {
        std::cout << "│" << std::setw(86) << std::left << " No matches found for this stage." << "│\n";
    }
    
    std::cout << "└" << std::string(8, '-') << "┴" << std::string(25, '-') << "┴" 
              << std::string(15, '-') << "┴" << std::string(12, '-') << "┴"
              << std::string(12, '-') << "┴" << std::string(12, '-') << "┘\n";
}

void GameResultManager::displayMenu() {
    std::cout << "\n┌" << std::string(60, '=') << "┐\n";
    std::cout << "│" << std::setw(18) << " " << "APUEC MANAGEMENT SYSTEM" << std::setw(19) << " " << "│\n";
    std::cout << "│" << std::setw(10) << " " << "Game Result Logging & Performance History" << std::setw(9) << " " << "│\n";
    std::cout << "├" << std::string(60, '=') << "┤\n";
    std::cout << "│ 1. Display Recent Matches (Last 5)                         │\n";
    std::cout << "│ 2. Display All Player Statistics                           │\n";
    std::cout << "│ 3. Display Specific Player Statistics                      │\n";
    std::cout << "│ 4. Query Matches by Player                                 │\n";
    std::cout << "│ 5. Query Matches by Stage                                  │\n";
    std::cout << "│ 6. Show Data Structure Justification                       │\n";
    std::cout << "│ 0. Exit                                                    │\n";
    std::cout << "└" << std::string(60, '=') << "┘\n";
    std::cout << "Enter your choice: ";
}

void GameResultManager::runProgram() {
    // Load initial data
    std::cout << "=== APUEC Management System Starting ===\n";
    std::cout << "Loading player data...\n";
    loadPlayerData("players.csv");
    std::cout << "Loading match history...\n";
    loadMatchHistory("matches.csv");
    std::cout << "System ready!\n";
    
    int choice;
    
    // Main program loop
    while (true) {
        displayMenu();
        
        // Input validation for menu choice
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        
        switch (choice) {
            case 1: {
                std::cin.ignore(); // Clear newline character
                displayRecentMatches();
                break;
            }
            
            case 2: {
                std::cin.ignore(); // Clear newline character
                displayAllPlayerStats();
                break;
            }
            
            case 3: {
                std::cout << "Enter player ID: ";
                int player_id;
                if (std::cin >> player_id) {
                    std::cin.ignore(); // Clear newline character after reading player ID
                    displayPlayerStats(player_id);
                } else {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid player ID format.\n";
                }
                break;
            }
            
            case 4: {
                std::cout << "Enter player ID: ";
                int player_id;
                if (std::cin >> player_id) {
                    std::cin.ignore(); // Clear newline character after reading player ID
                    queryMatchesByPlayer(player_id);
                } else {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid player ID format.\n";
                }
                break;
            }
            
            case 5: {
                std::cin.ignore(); // Clear newline character first
                std::cout << "Enter stage name (e.g., group, knockout): ";
                std::string stage;
                std::getline(std::cin, stage);
                queryMatchesByStage(stage);
                break;
            }
            
            case 6: {
                std::cin.ignore(); // Clear newline character
                std::cout << "\n┌" << std::string(70, '=') << "┐\n";
                std::cout << "│" << std::setw(22) << " " << "DATA STRUCTURE JUSTIFICATION" << std::setw(20) << " " << "│\n";
                std::cout << "├" << std::string(70, '=') << "┤\n";
                std::cout << "│ This system uses three carefully chosen data structures:      │\n";
                std::cout << "├" << std::string(70, '-') << "┤\n";
                std::cout << "│ 1. STACK for Recent Matches (LIFO - Last In, First Out):     │\n";
                std::cout << "│    • Provides quick access to the most recent match results   │\n";
                std::cout << "│    • Perfect for displaying latest tournament activities      │\n";
                std::cout << "│    • Tournament organizers frequently need recent results     │\n";
                std::cout << "│    • O(1) insertion and top access for efficiency            │\n";
                std::cout << "├" << std::string(70, '-') << "┤\n";
                std::cout << "│ 2. QUEUE for Match History (FIFO - First In, First Out):     │\n";
                std::cout << "│    • Maintains chronological order of all matches            │\n";
                std::cout << "│    • Essential for historical analysis and trend tracking    │\n";
                std::cout << "│    • Supports comprehensive tournament record keeping        │\n";
                std::cout << "│    • Enables time-based queries and performance evolution    │\n";
                std::cout << "├" << std::string(70, '-') << "┤\n";
                std::cout << "│ 3. DYNAMIC ARRAY for Player Statistics:                      │\n";
                std::cout << "│    • Simple, efficient storage with direct index access      │\n";
                std::cout << "│    • O(1) access time for player lookups by index           │\n";
                std::cout << "│    • Memory-efficient with predictable allocation           │\n";
                std::cout << "│    • Suitable for relatively stable player roster           │\n";
                std::cout << "├" << std::string(70, '-') << "┤\n";
                std::cout << "│ These structures work together to provide both immediate      │\n";
                std::cout << "│ access to recent results and comprehensive historical        │\n";
                std::cout << "│ analysis capabilities.                                       │\n";
                std::cout << "└" << std::string(70, '=') << "┘\n";
                break;
            }
            
            case 0: {
                std::cout << "Thank you for using APUEC Management System!\n";
                return;
            }
            
            default: {
                std::cin.ignore(); // Clear newline character
                std::cout << "Invalid choice. Please select from 0-6.\n";
                break;
            }
        }
        
        // Pause before showing menu again
        std::cout << "\n--- Please press Enter to return to the main menu ---";
        std::cin.get();
    }
}

// ===================== HELPER FUNCTIONS =====================

std::string GameResultManager::extractDateFromScheduledTime(const std::string& scheduled_time) {
    // Expected format: "YYYY-MM-DD HH:MM:SS"
    if (scheduled_time.length() < 10) {
        return "";
    }
    
    // Check if the first 10 characters match date format pattern
    std::string date_part = scheduled_time.substr(0, 10);
    
    // Basic validation: check for proper format YYYY-MM-DD
    if (date_part.length() == 10 && date_part[4] == '-' && date_part[7] == '-') {
        // Additional validation: check if year, month, day are numeric
        std::string year = date_part.substr(0, 4);
        std::string month = date_part.substr(5, 2);
        std::string day = date_part.substr(8, 2);
        
        try {
            int y = std::stoi(year);
            int m = std::stoi(month);
            int d = std::stoi(day);
            
            // Basic range validation
            if (y >= 2000 && y <= 2100 && m >= 1 && m <= 12 && d >= 1 && d <= 31) {
                return date_part;
            }
        } catch (const std::exception& e) {
            return "";
        }
    }
    
    return "";
}

void GameResultManager::splitCSVLine(const std::string& line, std::string tokens[], int max_tokens) {
    std::istringstream stream(line);
    std::string token;
    int count = 0;
    
    // Parse CSV line, handling basic comma separation
    while (std::getline(stream, token, ',') && count < max_tokens) {
        // Trim whitespace from token
        size_t start = token.find_first_not_of(" \t\r\n");
        size_t end = token.find_last_not_of(" \t\r\n");
        
        if (start != std::string::npos && end != std::string::npos) {
            tokens[count] = token.substr(start, end - start + 1);
        } else {
            tokens[count] = "";
        }
        count++;
    }
    
    // Fill remaining tokens with empty strings
    while (count < max_tokens) {
        tokens[count] = "";
        count++;
    }
}

int GameResultManager::findPlayerIndex(int player_id) {
    // Linear search through player array
    for (int i = 0; i < current_player_count; i++) {
        if (player_stats[i].player_id == player_id) {
            return i;
        }
    }
    return -1; // Player not found
}

double GameResultManager::parseScore(const std::string& score_str) {
    // Find the dash separator
    size_t dash_pos = score_str.find('-');
    if (dash_pos == std::string::npos) {
        return 0.0;
    }
    
    // Extract first score (before dash)
    std::string first_score = score_str.substr(0, dash_pos);
    
    // Trim whitespace
    size_t start = first_score.find_first_not_of(" \t");
    size_t end = first_score.find_last_not_of(" \t");
    
    if (start != std::string::npos && end != std::string::npos) {
        first_score = first_score.substr(start, end - start + 1);
    }
    
    // Convert to double
    try {
        return std::stod(first_score);
    } catch (const std::exception& e) {
        return 0.0;
    }
}

void GameResultManager::updatePlayerStats(int player_id, bool is_winner, double score) {
    int index = findPlayerIndex(player_id);
    if (index == -1) {
        return; // Player not found
    }
    
    PlayerStats& player = player_stats[index];
    
    // Update match count
    player.total_matches++;
    
    // Update wins/losses
    if (is_winner) {
        player.wins++;
    } else {
        player.losses++;
    }
    
    // Update average score using incremental formula
    // new_avg = ((old_avg * (n-1)) + new_score) / n
    if (player.total_matches == 1) {
        player.avg_score = score;
    } else {
        player.avg_score = ((player.avg_score * (player.total_matches - 1)) + score) / player.total_matches;
    }
}

// ===================== MAIN FUNCTION =====================

int main() {
    // Initialize the game result manager with capacity for 100 players
    GameResultManager manager(100);
    
    // Run the main program
    manager.runProgram();
    
    return 0;
}