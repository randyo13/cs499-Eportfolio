#ifndef PLAYER_H
#define PLAYER_H

class Player
{
private:
    int score;     // Total score for the player
    bool entered;  // True if player has scored 1000+ in one turn

public:
    Player();               // Constructor to initialize score and entry flag
    int getScore() const;   // Returns player's score
    bool hasEntered() const;// Returns true if player is eligible to bank points
    void addScore(int points); // Adds points to current score
    void enterGame();       // Marks player as entered into the game
};

#endif
