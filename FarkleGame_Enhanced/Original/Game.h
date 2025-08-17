#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include "Player.h"

class Game
{
private:
    std::vector<Player> players;           // List of players
    std::vector<std::string> playerNames;  // Corresponding player names
    int round;                             // Current round number
    bool finalTriggered;                   // Boolean for final round trigger
    int finalScore;                        // Leading score to beat in final round
    int finalWinner;                       // Index of player who triggered final round
    int numPlayers;                        // Total number of players

public:
    Game();

    void displayMenu();  // Show main menu
    void playGame();     // Main game loop
    void displayRules(); // Display game rules from file

    // Save final scores to a file
    void saveScores(const std::vector<Player>& players, const std::vector<std::string>& playerNames);

    // Dice methods
    std::vector<int> rollDice(size_t count);                    // Roll dice
    std::vector<int> countDice(const std::vector<int>& dice);   // Count die values
    int calculateScore(const std::vector<int>& dice);           // Calculate score from roll
    bool isValidScoringDice(const std::vector<int>& selected);  // Check if selected dice are valid
    std::vector<int> getScoringDice(const std::vector<int>& roll); // Get scoring dice indices
    std::vector<int> selectScoringDice(const std::vector<int>& roll, bool skipPrompt = false); // Prompt to select scoring dice
};

#endif