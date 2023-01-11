#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#include <vector>

// Constants for the board size and number of pieces
const int BOARD_SIZE = 24;
const int NUM_PIECES = 15;

// Enum for the two players
enum class Player { WHITE, BLACK };

// Overload << operator for enum class player 
std::ostream& operator<<(std::ostream& stream, const Player& currentPlayer)
{
    stream << (currentPlayer == Player::WHITE ? "WHITE" : "BLACK");
    return stream;
}

// Struct for dice roll
struct diceRoll
{
    // To hold each dice (each dice is the equivalent of a possible move)
    std::vector<int> diceNums;

    // Rolls dice
    void rollDice()
    {
        diceNums.push_back(rand() % 6 + 1);
        diceNums.push_back(rand() % 6 + 1);

        // If there are doubles add two more "moves" to diceNums 
        if (diceNums[0] == diceNums[1])
        {
            diceNums.push_back(diceNums[0]);
            diceNums.push_back(diceNums[0]);
        }
    }

    // Swap dice order
    void swap()
    {
        // If there are more than two items in diceNums that means "moves" are doubles and all "moves" are the same number
        if (diceNums.size() > 2)
            return;
        int temp = diceNums[1];
        diceNums[1] = diceNums[0];
        diceNums[0] = temp;
    }
};

//Struct to hold current move (from point, to point)
struct Move
{
    int from;
    int to;
};

// Struct for the game state
struct GameState
{
    // The board is represented as an array of integers, with
    // each index representing a point on the board. The value
    // of each element is the number of pieces on that point.
    int board[25];

    // The number of pieces each player has on the bar (in the center of the board)
    int whiteBar, blackBar;

    // The number of pieces each player has in their home quadrant 
    int whiteGoal, blackGoal;

    // The current player
    Player currentPlayer;

    // The current move
    Move currentMove;

    // Struct to hold dice (moves) information 
    diceRoll dice;

    // Message for player after each turn 
    std::string Message;

    // Returns players ability to bear off, they should have all their pieces in the home quadrant or have already scored at least once 
    // For the points in a player's home, sum the number of pieces, return if that value equals NUM_PIECES or 15 or check if the player has already scored at least one 
    bool canBearOff()
    {
        int total = 0;
        if (currentPlayer == Player::WHITE)
        {
            if (whiteGoal > 0)
                return true;
            for (int i = 0; i < 6; i++)
            {
                if (board[i] < 0)
                    total += abs(board[i]);
            }
        }
        else
        {
            if (blackGoal > 0)
                return true;
            for (int i = 18; i < 24; i++)
            {
                if (board[i] > 0)
                    total += board[i];
            }
        }
        return total == NUM_PIECES;
    }

    // Member function to check player's move 
    bool checkMove()
    {
        // If the player has barred pieces but didn't try to move them 
        if (currentMove.from != -1 && (currentPlayer == Player::WHITE ? whiteBar : blackBar))
        {
            Message = " has at least one barred checker";
            return false;
        }

        // If the player doesn't have barred pieces but tried to move them
        if (currentMove.from == -1 && (currentPlayer == Player::WHITE ? !whiteBar : !blackBar))
        {
            Message = " does not have any barred pieces";
            return false;
        }

        // If the player is trying to bear off but can't
        if (currentMove.to == 24 && !canBearOff())
        {
            Message = " cannot bear off"; //problem with this function 
            return false;
        }

        // If the player is trying to move from bar but can't ->
        if (currentPlayer == Player::WHITE && currentMove.from == -1)
            // If the point "to" is occupied by more than one opponent piece or the move is not allowed using current dice->
            if (board[currentMove.to] > 1 || BOARD_SIZE - currentMove.to != dice.diceNums[0])
            {
                Message = " cannot move from bar to point: " + std::to_string(currentMove.to + 1);
                return false;
            }
        if (currentPlayer == Player::BLACK && currentMove.from == -1)
            if (board[currentMove.to] < -1 || currentMove.to + 1 != dice.diceNums[0])
            {
                Message = " cannot move from bar to point: " + std::to_string(currentMove.to + 1);
                return false;
            }

        // If the player is trying to bear off and can't ->
        if (currentPlayer == Player::WHITE && currentMove.to == 24)
            // If the move is not allowed using current dice or there is no piece at the point "from" ->
            if (currentMove.from + 1 > dice.diceNums[0] || board[currentMove.from] > -1)
            {
                Message = " cannot bear off from here, the piece is too far from home or you have no pieces at point: " + std::to_string(currentMove.from + 1);
                return false;
            }
            // If the move is allowed using current dice ->
            else if (currentMove.from + 1 < dice.diceNums[0])
            {
                // For all points preceding "from" in home quadrant 
                for (int point = currentMove.from + 1; point < 6; point++)
                    // If there is a piece at point "point" -> 
                    if (board[point] < 0)
                    {
                        Message = " cannot bear off from point: " + std::to_string(currentMove.from + 1) + ", there are pieces in higher positions in your home";
                        return false;
                    }
            }
        if (currentPlayer == Player::BLACK && currentMove.to == 24)
            if (24 - currentMove.from > dice.diceNums[0] || board[currentMove.from] < 1)
            {
                Message = " cannot bear off from here, the piece is too far from home or you have no pieces at point: " + std::to_string(currentMove.from + 1);
                return false;
            }
            else if (24 - currentMove.from < dice.diceNums[0])
            {
                for (int point = currentMove.from - 1; point > 17; point--)
                    if (board[point] > 0)
                    {
                        Message = " cannot bear off from point: " + std::to_string(currentMove.from + 1) + ", there are pieces in higher positions in your home";
                        return false;
                    }
            }

        // If the player is trying to move (not from the bar and not bearing off) and can't ->
        if (currentMove.from != -1 && currentMove.to != 24)
        {
            // If the current player is WHITE -> 
            if (currentPlayer == Player::WHITE)
                // If there are no pieces at the point "from" or there is more than one opponent piece at point "to" or the move is not allowed using current dice ->
                if (board[currentMove.from] > -1 || board[currentMove.to] > 1 || currentMove.from - currentMove.to != dice.diceNums[0])
                {
                    Message = " cannot move from point: " + std::to_string(currentMove.from + 1) + " to point: " + std::to_string(currentMove.to + 1);
                    return false;
                }
            if (currentPlayer == Player::BLACK)
                if (board[currentMove.from] < 1 || board[currentMove.to] < -1 || currentMove.from - currentMove.to != dice.diceNums[0] * -1)
                {
                    Message = " cannot move from point: " + std::to_string(currentMove.from + 1) + " to point: " + std::to_string(currentMove.to + 1);
                    return false;
                }
        }

        // If the function has not returned the move is valid, check if there is a hit if the player is not bearing off 
        if (currentMove.to != 24)
            // If the point "to" has one opponent piece -> 
            if (board[currentMove.to] == (currentPlayer == Player::WHITE ? 1 : -1))
            {
                currentPlayer == Player::WHITE ? blackBar++ : whiteBar++;
                currentPlayer == Player::WHITE ? board[currentMove.to]-- : board[currentMove.to]++;
                std::cout << currentPlayer << " hit at point: " << currentMove.to+1 << std::endl;
            }

        // Make move
        // If the player is moving from bar 
        if (currentMove.from == -1)
        {
            currentPlayer == Player::WHITE ? whiteBar-- : blackBar--;
            currentPlayer == Player::WHITE ? board[currentMove.to]-- : board[currentMove.to]++;
            Message = " player moved from bar to point: " + std::to_string(currentMove.to+1);
        }
        // If the player is bearing off 
        else if (currentMove.to == 24)
        {
            currentPlayer == Player::WHITE ? whiteGoal++ : blackGoal++;
            currentPlayer == Player::WHITE ? board[currentMove.from]++ : board[currentMove.from]--;
            Message = " player beared off from point: " + std::to_string(currentMove.from+1);
        }
        // Otherwise it is a normal move 
        else
        {
            currentPlayer == Player::WHITE ? board[currentMove.from]++ : board[currentMove.from]--;
            currentPlayer == Player::WHITE ? board[currentMove.to]-- : board[currentMove.to]++;
            Message = " player moved from point: " + std::to_string(currentMove.from+1) + " to point: " + std::to_string(currentMove.to+1);
        }

        return true;
    }

    // Adjust dice based on available moves, called before the player enters their move 
    void adjustDice()
    {
        // If the player cannot enter their piece(s) from the bar ->
        if (currentPlayer == Player::WHITE && whiteBar)
        {
            // If the player cannot enter their piece from the bar with dice one ->
            if (board[BOARD_SIZE - dice.diceNums[0]] >= 2)
            {
                // If the player only has one dice or the dice is doubles->
                if (dice.diceNums.size() == 1 || dice.diceNums.size() > 2)
                {
                    Message += "\n~ has no valid moves, voiding turn, diceOne: " + std::to_string(dice.diceNums[0]) + " diceTwo: " + std::to_string(dice.diceNums[1]);
                    dice.diceNums.clear();
                    return;
                }
                // If the player cannot move from the bar with the dice two either ->
                else if (board[BOARD_SIZE - dice.diceNums[1]] >= 2)
                {
                    Message += "\n~ has no valid moves, voiding turn, diceOne: " + std::to_string(dice.diceNums[0]) + " diceTwo: " + std::to_string(dice.diceNums[1]);
                    dice.diceNums.clear();
                    return;
                }
                // Otherwise the player will have to use dice two to move first 
                else
                {
                    dice.swap();
                    return;
                }
            }
            // Otherwise the player can move with dice one 
            return;
        }
        // If the player cannot enter their piece(s) from the bar (BLACK)-> 
        if (currentPlayer == Player::BLACK && blackBar)
        {
            if (board[dice.diceNums[0] - 1] <= -2)
            {
                if (dice.diceNums.size() == 1 || dice.diceNums.size() > 2)
                {
                    Message += "\n~ has no valid moves, voiding turn, diceOne: " + std::to_string(dice.diceNums[0]) + " diceTwo: " + std::to_string(dice.diceNums[1]);
                    dice.diceNums.clear();
                    return;
                }
                else if (board[dice.diceNums[0] - 1] <= -2)
                {
                    Message += "\n~ has no valid moves, voiding turn, diceOne: " + std::to_string(dice.diceNums[0]) + " diceTwo: " + std::to_string(dice.diceNums[1]);
                    dice.diceNums.clear();
                    return;
                }
                else
                {
                    dice.swap();
                    return;
                }
            }
            return;
        }

        // If the player has no barred pieces but can't move any of their pieces (excluding bearing off) 
        if (currentPlayer == Player::WHITE && !canBearOff())
        {
            // Find all points player can move from 
            std::vector<int> froms;
            for (int i = 0; i < BOARD_SIZE; i++)
                if (board[i] < 0)
                    froms.push_back(i);
            // For the points that the player can move from 
            for (int i : froms)
            {
                // If moving from point "i" using dice one is out of range of the board continue
                if (i - dice.diceNums[0] < 0)
                    continue;
                // Else check to see if point from "i" using dice one is not occupied by more than one of an opponent's pieces 
                else
                    if (board[i - dice.diceNums[0]] < 2)
                        return;
            }

            // If we didn't return that means there are no valid moves using dice one
            // If there is only one dice or the dice is doubles->
            if (dice.diceNums.size() == 1 || dice.diceNums.size() > 2)
            {
                Message += "\n~ has no valid moves, voiding turn, diceOne: " + std::to_string(dice.diceNums[0]) + " diceTwo: " + std::to_string(dice.diceNums[1]);
                dice.diceNums.clear();
                return;
            }
            // If there is more than one dice check if there are valid moves using dice two, if there is, swap() the player will have to use dice two first  
            for (int i : froms)
            {
                // If moving from point "i" using dice two is out of range of the board continue
                if (i - dice.diceNums[1] < 0)
                    continue;
                // Else check to see if point from "i" using dice two is not occupied by more than one of an opponent's pieces 
                else
                    if (board[i - dice.diceNums[1]] < 2)
                    {
                        dice.swap();
                        return;
                    }
            }
            // If we didn't return that means there are no valid moves using dice one or dice two 
            Message += "\n~ has no valid moves, voiding turn, diceOne: " + std::to_string(dice.diceNums[0]) + " diceTwo: " + std::to_string(dice.diceNums[1]);
            dice.diceNums.clear();
            return;
        }
        // If the player has no barred pieces but can't move any of their pieces (BLACK)
        if (currentPlayer == Player::BLACK && !canBearOff())
        {
            std::vector<int> froms;
            for (int i = 0; i < BOARD_SIZE; i++)
                if (board[i] > 0)
                    froms.push_back(i);
            for (int i : froms)
            {
                if (i + dice.diceNums[0] > 23)
                    continue;
                else
                    if (board[i + dice.diceNums[0]] > -2)
                        return;
            }
            if (dice.diceNums.size() == 1)
            {
                Message += "\n~ has no valid moves, voiding turn, diceOne: " + std::to_string(dice.diceNums[0]) + " diceTwo: " + std::to_string(dice.diceNums[1]);
                dice.diceNums.clear();
                return;
            }
            for (int i : froms)
            {
                if (i + dice.diceNums[0] > 23)
                    continue;
                else
                    if (board[i + dice.diceNums[0]] > -2)
                    {
                        dice.swap();
                        return;
                    }
            }
            Message += "\n~ has no valid moves, voiding turn, diceOne: " + std::to_string(dice.diceNums[0]) + " diceTwo: " + std::to_string(dice.diceNums[1]);
            dice.diceNums.clear();
            return;
        }
        return;
    }
};

//function prototypes
void initGame(GameState& state);
void printBoard(const GameState& state);
void playGame(GameState& state);
bool isGameOver(const GameState& state);
Player getWinner(const GameState& state);

// Main function
int main()
{
    // Seed the random number generator
    srand(time(NULL));

    // Create the game state
    GameState state;
    initGame(state);

    // Play the game
    playGame(state);

    return 0;
}

// Initializes the game state
void initGame(GameState& state)
{
    // Clear the board
    for (int i = 0; i < BOARD_SIZE; i++)
        state.board[i] = 0;

    // Place the pieces on the board
    state.board[0] = 2;
    state.board[11] = 5;
    state.board[16] = 3;
    state.board[18] = 5;

    state.board[23] = -2;
    state.board[12] = -5;
    state.board[7] = -3;
    state.board[5] = -5;

    // Set the initial number of pieces on the bar and in the home quadrant
    state.whiteBar = 0;
    state.blackBar = 0;
    state.whiteGoal = 0;
    state.blackGoal = 0;

    // Set the current player to white
    state.currentPlayer = Player::WHITE;
}

// Prints the current board state
void printBoard(const GameState& state)
{
    std::cout << "=====================================================" << std::endl;

    // Show current dice (moves) 
    int id = 1;
    for (int i : state.dice.diceNums)
    {
        std::cout << "Dice " + std::to_string(id) + ": " << i << std::endl;
        id++;
    }

    std::string tempBoardTop = "";
    std::string tempBoardBottom = "";
    int count = 0;

    for (int index = 12; index < BOARD_SIZE; index++)
    {
        if (count == 6)
        {
            tempBoardTop.append("| | ");
            tempBoardBottom.append("| | ");
        }
        tempBoardTop.append("(" + std::to_string(state.board[index]) + ") ");
        tempBoardBottom.append("(" + std::to_string(state.board[23 - index]) + ") ");
        count++;
    }

    std::cout << "\n" << tempBoardTop << "\n\n" << std::endl;
    std::cout << tempBoardBottom << "\n\n" << std::endl;
    std::cout << "White Bar: " << state.whiteBar << std::endl;
    std::cout << "Black Bar: " << state.blackBar << "\n\n";

    if (state.whiteGoal || state.blackGoal)
    {
        std::cout << "White Goal: " << state.whiteGoal << std::endl;
        std::cout << "Black Goal: " << state.blackGoal << "\n\n";
    }
    std::cout << "=====================================================" << std::endl;
}

// Plays a game of backgammon
void playGame(GameState& state)
{
    // Display general information
    std::cout << "Welcome to the wonderful game of backgammon!\n1. from = 0 to move from bar\n2. to = 25 to bear off\n3. from = -2 to swap dice\n\n";
    // While the game is not over keep switching players 
    while (!isGameOver(state))
    {
        // Roll the dice 
        state.dice.rollDice();

        // Check to make sure the player has possible moves 
        state.adjustDice();

        // While there is still a move available and the game is not over 
        while (state.dice.diceNums.size() > 0 && !isGameOver(state))
        {
            // Print the board
            printBoard(state);

            // Variables to hold players turn 
            int from, to;

            // Get player's move, loop if the user's inputs are out of range, check if the player is swapping the dice or asking for general info 
            do
            {
                std::cout << "~ from = -3 to view game information again" << std::endl;
                std::cout << "Player " << (state.currentPlayer == Player::WHITE ? "White" : "Black") << ", enter your move (from point *enter*, to point *enter*): " << std::endl;
                std::cin >> from >> to;
                if (from == -2)
                {
                    state.dice.swap();
                    std::cout << " Dice swapped please enter a move" << std::endl;
                }
                else if (from == -3)
                    std::cout<< "1. from = 0 to move from bar\n2.to = 25 to bear off\n3.from = -2 to swap dice\nPlease enter a move\n\n";
                from--;
                to--;
            } while (from < -1 || from > 23 || to < 0 || to > 25);

            // Update current move 
            state.currentMove.from = from;
            state.currentMove.to = to;

            // If the move is valid, remove dice (move) from diceNums and check to make sure the player still has a valid move 
            if (state.checkMove())
            {
                state.dice.diceNums.erase(state.dice.diceNums.begin());
                if (!state.dice.diceNums.empty())
                    state.adjustDice();
            }

            // Display result of turn, clear message 
            std::cout << "\n" << state.currentPlayer << state.Message << "\n\n";
            state.Message.clear();
        }
        // Display result of turn if adjustDice found there is no available moves for player 
        if (!state.Message.empty())
            std::cout << state.currentPlayer << state.Message << "\n\n";

        // Switch players
        state.currentPlayer = (state.currentPlayer == Player::WHITE ? Player::BLACK : Player::WHITE);
    }

    // Print the board one last time
    printBoard(state);

    // Announce the winner
    std::cout << "Player " << (getWinner(state) == Player::WHITE ? "White" : "Black") << " wins!" << std::endl;
}

// Returns true if the game is over, false otherwise
bool isGameOver(const GameState& state)
{
    return state.whiteGoal == NUM_PIECES || state.blackGoal == NUM_PIECES;
}

// Returns the winner of the game
Player getWinner(const GameState& state)
{
    return state.whiteGoal == NUM_PIECES ? Player::WHITE : Player::BLACK;
}
