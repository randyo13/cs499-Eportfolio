/*
Title: Game Class Implementation
Name: Randy Ortiz
Course: IT - 312 Software Development w / C++.NET
Date: June 22, 2025
Project Number: 7-1
*/

/*
Description:

    This file contains the implementation of the Game class, which manages the core logic
of the Farkle game. It includes methods for displaying the menu and rules, rolling and
scoring dice, handling player turns, managing game rounds, triggering the final round,
and saving scores to a file.

    The class supports gameplay for two or more players and enforces Farkle rules such as
entering the game with 1000 points, hot dice rerolls, and banking points. It also interacts
with the Player class to track player scores and game status.

    While testing, an issue was discovered where input was required before the prompt to
select scoring dice was displayed. My attempts to resolve this timing behavior were
unsuccessful, but functionality still works. After inputing dice the total score is 
printed and game continues.
*/


#include "GameEnhanced.h"  // ENHANCEMENT: ---------------------------------------------------------------- switched from "Game.h" to "GameEnhanced.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <set>

using namespace std;

// ENHANCEMENT:---------------------------------------------------------------------------------   clears leftover input after cin >> to avoid skipping getline()
void clearInputBuffer()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Constructor: Initializes round, flags, and seeds RNG
Game::Game() : round(1), finalTriggered(false), finalScore(0), finalWinner(-1)
{
    srand(static_cast<unsigned int>(time(0))); 
}

// Displays menu options and handles selection
void Game::displayMenu()
{
    int choice;
    do
    {
        // Menu UI
        cout << "\n=== Farkle Game Menu ===" << endl;
        cout << "1. Start Game" << endl;
        cout << "2. View Rules" << endl;
        cout << "3. Quit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        clearInputBuffer();  // ENHANCEMENT: ---------------------------------------------------- fix input sync bug cuased by leftover newline. Prevents skipped input prompt.

        // Handle choice
        switch (choice)
        {
        case 1:
            playGame(); // Start game loop
            break;
        case 2:
            displayRules(); // Show rules
            break;
        case 3:
            cout << "Exiting..." << endl; // Exit game
            break;
        default:
            cout << "Invalid choice. Try again." << endl;
        }
    } while (choice != 3); // Repeat until user quits
}

// Loads rules from external text file and prints to console
void Game::displayRules()
{
    ifstream file("farkle_rules.txt");
    string line;
    if (file.is_open())
    {
        cout << "\n===== FARKLE RULES =====\n\n";
        while (getline(file, line)) // Print each line
        {
            cout << line << endl;
        }
        file.close();
    }
    else
    {
        cout << "Error: Could not open rules file.\n";
    }
}

// Appends final scores to a text file
void Game::saveScores(const vector<Player>& players, const vector<string>& names)
{
    ofstream file("scores.txt", ios::app);
    if (file.is_open())
    {
        file << "\nFinal Scores:\n===============\n";
        for (size_t i = 0; i < players.size(); i++)
        {
            file << names[i] << ": " << players[i].getScore() << "\n";
        }
        file << "===========================\n";
        file.close();
    }
}

// Rolls 'diceCount' dice and returns result
vector<int> Game::rollDice(size_t diceCount)
{
    vector<int> dice;
    for (size_t i = 0; i < diceCount; i++)
    {
        dice.push_back(rand() % 6 + 1); // Random 1-6
    }
    return dice;
}

// Tallies occurrences of each die face (1-6)
vector<int> Game::countDice(const vector<int>& dice)
{
    vector<int> count(7, 0); // 0 index unused
    for (int val : dice)
    {
        count[val]++;
    }
    return count;
}

// ENHANCEMENT: -------------------------------------------------------------------------------- Refactored the scoring logic to improve modularity and readability. 
// Separated the triplet and single-die scoring into two distinct helper functions.

// Calculates score for triplets (three or more of a kind)
int Game::scoreTriplets(vector<int>& counts)
{
    int score = 0;

    // Iterate through die faces 1–6
    for (int i = 1; i <= 6; i++)
    {
        // Check if there are at least three of a kind
        if (counts[i] >= 3)
        {
            // Triple 1s are worth 1000 points, others are worth face value × 100
            score += (i == 1) ? 1000 : i * 100;

            // Subtract the triplet from the count for remaining single scoring
            counts[i] -= 3;
        }
    }

    return score;
}

// Calculates score for single 1s and 5s after triplets are removed
int Game::scoreSingles(const vector<int>& counts)
{
    int score = 0;

    // Single 1s are worth 100 points each
    score += counts[1] * 100;

    // Single 5s are worth 50 points each
    score += counts[5] * 50;
    return score;
}

// Scoring function using modular helpers to compute total score for a roll
int Game::calculateScore(const vector<int>& dice)
{
    // Count occurrences of each die face (1–6)
    vector<int> counts = countDice(dice);
    int score = 0;

    // Add score from triplets
    score += scoreTriplets(counts);

    // Add score from remaining single 1s and 5s
    score += scoreSingles(counts);

    return score;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   OLD SCORING LOGIC
// Calculates score using Farkle rules (1s, 5s, Triples)
//int Game::calculateScore(const vector<int>& dice)
//{
//    vector<int> counts = countDice(dice);
//    int score = 0;

    // Triplets
//    for (int i = 1; i <= 6; i++)
//    {
//        if (counts[i] >= 3)
//        {
//            score += (i == 1) ? 1000 : i * 100;
//            counts[i] -= 3;
//        }
//    }

    // Remaining 1s and 5s
//    score += counts[1] * 100;
//    score += counts[5] * 50;

//    return score;
//}

// Returns true if selected dice result in valid points
bool Game::isValidScoringDice(const vector<int>& selected)
{
    return calculateScore(selected) > 0;
}

// Finds indices of dice that contribute to score
vector<int> Game::getScoringDice(const vector<int>& roll)
{
    vector<int> scoringDice;
    vector<int> counts = countDice(roll);

    // Add triplets
    for (int i = 1; i <= 6; i++)
    {
        if (counts[i] >= 3)
        {
            for (size_t j = 0; j < roll.size(); j++)
            {
                if (roll[j] == i)
                {
                    scoringDice.push_back((int)j);
                }
            }
        }
    }

    // Add 1s and 5s
    for (size_t j = 0; j < roll.size(); j++)
    {
        if (roll[j] == 1 || roll[j] == 5)
        {
            scoringDice.push_back((int)j);
        }
    }

    // Remove duplicates
    set<int> unique(scoringDice.begin(), scoringDice.end());
    return vector<int>(unique.begin(), unique.end());
}

// Prompts user to select dice to keep based on scoring dice
vector<int> Game::selectScoringDice(const vector<int>& roll, bool skipPrompt)
{
    vector<int> selected;
    set<int> usedIndexes;
    string input;

    // Get valid scoring indexes
    set<int> validScoringIndexes;
    vector<int> scoringIndexes = getScoringDice(roll);
    for (int idx : scoringIndexes)
    {
        validScoringIndexes.insert(idx + 1);
    }

    while (true)
    {
        if (!skipPrompt)
        {
            cout << "Select dice to keep by entering their positions (e.g., 1 3 5): ";
        }

        getline(cin, input);
        stringstream ss(input);
        int index;
        selected.clear();
        usedIndexes.clear();
        bool valid = true;

        while (ss >> index)
        {
            if (index >= 1 && (size_t)index <= roll.size() && usedIndexes.find(index) == usedIndexes.end())
            {
                if (validScoringIndexes.count(index) == 0)
                {
                    valid = false;
                    break;
                }
                selected.push_back(roll[index - 1]);
                usedIndexes.insert(index);
            }
        }

        if (valid && !selected.empty() && isValidScoringDice(selected))
        {
            break;
        }
        else
        {
            cout << "Invalid selection. Only choose scoring dice.\n";
        }
    }

    return selected;
}

// Core game loop: setup players, manage turns, handle scoring
void Game::playGame()
{
    int numPlayers = 0;
    cout << "Enter number of players: ";
    cin >> numPlayers;

    // Enforce min player count
    while (numPlayers < 2)
    {
        cout << "You need at least 2 players. Try again: ";
        cin >> numPlayers;
        clearInputBuffer();  // ENHANCEMENT:  ------------------------------------------------------------   fixes leftover newline before getline
    }

    playerNames.resize(numPlayers);
    players.resize(numPlayers);

    // Input player names
    cout << "Enter the names of the players:\n";
    for (int i = 0; i < numPlayers; i++)
    {
        cout << "Player " << (i + 1) << ": ";
        cin >> playerNames[i];
        clearInputBuffer();// ENHANCEMENT: ---------------------------------------------------------------  fixes leftover newline before getline
    }

    // Save to file
    ofstream outFile("scores.txt");
    outFile << "Players\n=======\n";
    for (int i = 0; i < numPlayers; i++)
    {
        outFile << "Player #" << (i + 1) << ": " << playerNames[i] << "\n";
    }
    outFile << "== End of Player List ==\n\n";
    outFile.close();

    // Regular round loop
    while (true)
    {
        ofstream roundFile("scores.txt", ios::app);
        roundFile << "Round " << round << "\n--------\n";

        for (int i = 0; i < numPlayers; i++)
        {
            cout << "\n=============================\n";
            cout << playerNames[i] << "'s turn.\n";
            cout << "=============================\n";

            int turnScore = 0;
            size_t diceRemaining = 6;
            size_t turnDiceUsed = 0;
            bool farkle = false;

            // Individual turn loop
            while (true)
            {
                vector<int> roll = rollDice(diceRemaining);

                // Display roll
                cout << "\nRolled: ";
                for (size_t j = 0; j < roll.size(); j++)
                {
                    cout << "[" << j + 1 << "]" << roll[j] << " ";
                }
                cout << endl;

                // Show scoring positions
                vector<int> scoringIndexes = getScoringDice(roll);
                cout << "Scoring dice at positions: ";
                for (int idx : scoringIndexes)
                {
                    cout << "[" << idx + 1 << "]" << roll[idx] << " ";
                }
                cout << endl << endl;

                // Check for Farkle
                if (calculateScore(roll) == 0)
                {
                    cout << "Farkle! No points this turn." << endl;
                    farkle = true;
                    break;
                }

                // Player selects scoring dice
                vector<int> selected = selectScoringDice(roll);
                int score = calculateScore(selected);
                turnScore += score;
                diceRemaining -= selected.size();
                turnDiceUsed += selected.size();

                cout << "Turn score: " << turnScore << endl;

                // Handle hot dice
                if (diceRemaining == 0 && turnDiceUsed == 6)
                {
                    cout << "Hot dice! Rerolling all 6.\n";
                    diceRemaining = 6;
                    turnDiceUsed = 0;
                    continue;
                }

                // Prompt to roll or bank
                char choice;
                cout << "Roll again (r) or bank points (b)? ";
                cin >> choice;
                clearInputBuffer(); // ENHANCEMENT: ---------------------------------------------------------------- FIX: fixes leftover newline before getline

                if (choice == 'b')
                {
                    break;
                }
            }

            // Post-turn logic
            if (!farkle)
            {
                if (!players[i].hasEntered() && turnScore >= 1000)
                {
                    players[i].enterGame();
                    players[i].addScore(turnScore);
                    cout << playerNames[i] << " has entered the game!" << endl;
                }
                else if (players[i].hasEntered())
                {
                    players[i].addScore(turnScore);
                }
                else
                {
                    cout << "Need 1000+ points in one turn to enter the game." << endl;
                }

                cout << "Total score: " << players[i].getScore() << endl;
                roundFile << playerNames[i] << ": " << turnScore << " (Total: " << players[i].getScore() << ")\n";

                // Trigger final round
                if (!finalTriggered && players[i].getScore() >= 10000)
                {
                    cout << "\n" << playerNames[i] << " reached 10,000! Final round triggered!\n";
                    finalTriggered = true;
                    finalScore = players[i].getScore();
                    finalWinner = i;
                }
            }
        }

        // End round summary
        roundFile << "== End of Round " << round++ << " ==\n===========================\n\n";
        roundFile.close();

        if (finalTriggered)
        {
            break;
        }
    }

    // Final round for others
    cout << "\nFinal round begins!\n";
    for (int i = 0; i < players.size(); i++)
    {
        if (i == finalWinner)
        {
            continue;
        }

        cout << "\n" << playerNames[i] << "'s final turn:\n";

        int turnScore = 0;
        size_t diceRemaining = 6;

        // Final turn loop
        while (true)
        {
            vector<int> roll = rollDice(diceRemaining);

            cout << "Rolled: ";
            for (size_t j = 0; j < roll.size(); j++)
            {
                cout << "[" << j + 1 << "]" << roll[j] << " ";
            }
            cout << endl;

            vector<int> scoringIndexes = getScoringDice(roll);
            cout << "Scoring dice at positions: ";
            for (int idx : scoringIndexes)
            {
                cout << "[" << idx + 1 << "]" << roll[idx] << " ";
            }
            cout << endl << endl;

            if (calculateScore(roll) == 0)
            {
                cout << "Farkle! No score." << endl;
                break;
            }

            vector<int> selected = selectScoringDice(roll);
            int score = calculateScore(selected);
            turnScore += score;
            diceRemaining -= selected.size();

            char choice;
            cout << "Roll again (r) or bank (b)? ";
            cin >> choice;
            clearInputBuffer(); // ENHANCEMENT: --------------------------------------------------------------- fixes leftover newline before getline

            if (choice == 'b')
            {
                break;
            }

            // Hot dice check
            if (diceRemaining == 0)
            {
                diceRemaining = 6;
            }
        }

        // Update final scores
        if (players[i].hasEntered())
        {
            players[i].addScore(turnScore);
            if (players[i].getScore() > finalScore)
            {
                finalScore = players[i].getScore();
                finalWinner = i;
            }
        }
    }

    // Announce winner and save to file
    cout << "\nWinner: " << playerNames[finalWinner] << " with " << players[finalWinner].getScore() << " points!\n";
    saveScores(players, playerNames);
}
