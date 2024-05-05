#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include "utility.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <vector>

using namespace std;

class GameImpl
{
    int g_rows, g_cols;                     // Rows and columns of game

    struct Ship                             // Tracks all attributes of each ship
    {
        int len;
        char symbol;
        string name;
    };
    vector<Ship*> ships;                    // FVector containing all ships

public:
    GameImpl(int nRows, int nCols);
    ~GameImpl();
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols) : g_rows(nRows), g_cols(nCols), ships()
{}

GameImpl::~GameImpl()
{
    // Destroy each ship pointer

    if (ships.size() > 0)
    {
        for (size_t i = (ships.size() - 1); i > 0; i--)
        {
            Ship* tempShip = ships.at(i);
            ships.pop_back();
            delete tempShip;
        }
        delete ships.at(0);
    }
}

int GameImpl::rows() const
{
    return g_rows;
}

int GameImpl::cols() const
{
    return g_cols;
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0 && p.r < rows() && p.c >= 0 && p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
    // Initialize new ship pointer, record attributes, add to ships vector

    Ship* added_ship = new Ship;

    added_ship->len = length;
    added_ship->name = name;
    added_ship->symbol = symbol;

    ships.push_back(added_ship);

    return true;        // Ship successfully added
}

int GameImpl::nShips() const
{
    return static_cast<int>(ships.size());
}

int GameImpl::shipLength(int shipId) const
{
    // If valid shipId, return ship length
    if (shipId < ships.size() && shipId >= 0)
        return ships.at(shipId)->len;
    return 0;
}

char GameImpl::shipSymbol(int shipId) const
{
    // If valid shipId, return ship symbol
    if (shipId < ships.size() && shipId >= 0)
        return ships.at(shipId)->symbol;
    return '.';
}

string GameImpl::shipName(int shipId) const
{
    // If valid shipId, return ship name
    if (shipId < ships.size() && shipId >= 0)
        return ships.at(shipId)->name;
    return " ";
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause = true)
{
    // Place ships on board

    if (p1->isHuman())
        cout << p1->name() << " must place " << nShips() << " ships." << endl;
    if (!p1->placeShips(b1))
        return nullptr;

    if (p2->isHuman())
        cout << p2->name() << " must place " << nShips() << " ships." << endl;
    if (!p2->placeShips(b2))
        return nullptr;

    // Game starts

    bool validShot = false;
    bool shotHit = false;
    bool shipDestroyed = false;
    int shipId = -1;

    while (true)            // While game hasn't been ended
    {
        // First player's turn

        Point p;
        cout << p1->name() << "'s turn.  Board for " << p2->name() << ':' << endl;

        // Display second player's board
        if (p1->isHuman())
            b2.display(true);
        else
            b2.display(false);

        p = p1->recommendAttack();                          // Choose attack position

        // Attack at chosen position and record results

        validShot = b2.attack(p, shotHit, shipDestroyed, shipId);               // Attack and record if attack hit a previously attacked location

        p1->recordAttackResult(p, validShot, shotHit, shipDestroyed, shipId);

        // If attack hit a previously attacked location
        if (!validShot)
            cout << p1->name() << " wasted a shot at (" << p.r << ',' << p.c << ")." << endl;

        else
        {
            // Display results of attack

            if (shotHit && !shipDestroyed)
                cout << p1->name() << " attacked (" << p.r << ',' << p.c << ") and hit something, resulting in:" << endl;
            else if (!shotHit)
                cout << p1->name() << " attacked (" << p.r << ',' << p.c << ") and missed, resulting in:" << endl;
            else if (shotHit && shipDestroyed)
                cout << p1->name() << " attacked (" << p.r << ',' << p.c << ") and destroyed the " << shipName(shipId) << ", resulting in:" << endl;
            if (b2.allShipsDestroyed())
            {
                b2.display(false);
                cout << p1->name() << " wins!" << endl;
                return p1;
            }
            if (p1->isHuman())
                b2.display(true);
            else
                b2.display(false);
        }


        // If applicable, pause game until user hits enter
        if (shouldPause)
        {
            waitForEnter();
        }

        // Second player's turn

        cout << p2->name() << "'s turn.  Board for " << p1->name() << ':' << endl;
        if (p2->isHuman())
            b1.display(true);
        else
            b1.display(false);

        p = p2->recommendAttack();                          // Choose attack position

        // Attack at chosen position and record results

        validShot = b1.attack(p, shotHit, shipDestroyed, shipId);                   // Attack and record if attack hits a previously attacked location
        p2->recordAttackResult(p, validShot, shotHit, shipDestroyed, shipId);


        // If attack hit a previously attacked location
        if (!validShot)
            cout << p2->name() << " wasted a shot at (" << p.r << ',' << p.c << ")." << endl;

        else
        {
            // Display results of attack

            if (shotHit && !shipDestroyed)
                cout << p2->name() << " attacked (" << p.r << ',' << p.c << ") and hit something, resulting in:" << endl;
            else if (!shotHit)
                cout << p2->name() << " attacked (" << p.r << ',' << p.c << ") and missed, resulting in:" << endl;
            else if (shotHit && shipDestroyed)
                cout << p2->name() << " attacked (" << p.r << ',' << p.c << ") and destroyed the " << shipName(shipId) << ", resulting in:" << endl;
            if (b1.allShipsDestroyed())
            {
                b1.display(false);
                cout << p2->name() << " wins!" << endl;
                return p2;
            }
            if (p2->isHuman())
                b1.display(true);
            else
                b1.display(false);
        }

        // If applicable, pause game until user hits enter
        if (shouldPause)
            waitForEnter();


    }
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1 || nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1 || nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows() && length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
            << endl;
        return false;
    }
    if (!isascii(symbol) || !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
            << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X' || symbol == '.' || symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
            << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0 && shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0 && shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0 && shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr || p2 == nullptr || nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

