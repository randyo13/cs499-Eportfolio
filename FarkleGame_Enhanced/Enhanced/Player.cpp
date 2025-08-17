/*
Title: Player Class Implementation
Name: Randy Ortiz
Course: IT - 312 Software Development w / C++.NET
Date: June 22, 2025
Project Number: 7-1
*/

/*
Description:

    This file implements the Player class, which is used to store and manage player
data for the Farkle game. Each Player object tracks its score and whether the player has
officially entered the game by scoring 1000 points or more in a single turn.

    The class provides methods to get and update the player’s score and entry status. These
methods are used by the Game class to determine turn outcomes, manage scoring, and enforce
game entry rules.

    During testing, the class functioned correctly as a simple data container. No logical errors
were observed, and integration with the Game class worked without issues.
*/



#include "Player.h"

// Constructor: Initializes score to 0 and entry status to false
Player::Player()
{
    score = 0;
    entered = false;
}

// Returns the player's current score
int Player::getScore() const
{
    return score;
}

// Returns true if player has entered the game (scored 1000+ in a turn)
bool Player::hasEntered() const
{
    return entered;
}

// Adds points to the player's score
void Player::addScore(int points)
{
    score += points;
}

// Sets entry status to true (player is now in the game)
void Player::enterGame()
{
    entered = true;
}
