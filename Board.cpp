#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <vector>

using namespace std;

class BoardImpl
{
public:
    BoardImpl(const Game& g);           // class constructor
    ~BoardImpl();                       // class destructor

    // Mutators
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);

    // Accessors
    void display(bool shotsOnly) const;
    bool allShipsDestroyed() const;
private:
    const Game& m_game;                 // current game instance
    vector<bool> ship_occured;          // vector keeping track of whether or not each ship has been placed

    struct cell                         // stores all information at each position in the board
    {
        char symbol;
        bool has_attacked;
        bool has_hit;
        int shipId;
        bool ship_destroyed;
    };

    cell* g_board[MAXROWS][MAXCOLS];    // grid of cells which will become the board

};

BoardImpl::BoardImpl(const Game& g)
    : m_game(g), g_board()
{
    // All ships have not been placed yet
    for (size_t i = 0; i < m_game.nShips(); i++)
        ship_occured.push_back(false);

    // Initialize an empty cell at each point in the board
    for (size_t r = 0; r < m_game.rows(); r++)
        for (size_t c = 0; c < m_game.cols(); c++)
        {
            g_board[r][c] = new cell;
            g_board[r][c]->symbol = '.';
            g_board[r][c]->has_attacked = false;
            g_board[r][c]->has_hit = false;
            g_board[r][c]->shipId = -1;
            g_board[r][c]->ship_destroyed = true;
        }
}

BoardImpl::~BoardImpl()
{
    // Delete each cell in the board
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
            delete g_board[r][c];
        }
}

void BoardImpl::clear()
{
    // Clear the board
    // Delete each cell in the old board and replace them with newly initialized ones
    for (size_t r = 0; r < m_game.rows(); r++)
        for (size_t c = 0; c < m_game.cols(); c++)
        {
            delete g_board[r][c];
            g_board[r][c] = new cell;
            g_board[r][c]->symbol = '.';
            g_board[r][c]->has_attacked = false;
            g_board[r][c]->has_hit = false;
            g_board[r][c]->shipId = -1;
            g_board[r][c]->ship_destroyed = true;
        }

    // All ships have not been replaced yet
    for (size_t i = 0; i < m_game.nShips(); i++)
        if (i < ship_occured.size())
            ship_occured.at(i) = false;
}

void BoardImpl::block()     // check
{
    // Block half the cells on the board
    int count = 0;
    int num_cells = m_game.rows() * m_game.cols();
    while (count != (num_cells / 2))
    {
        Point current = m_game.randomPoint();
        if (g_board[current.r][current.c]->symbol != 'X')
        {
            g_board[current.r][current.c]->symbol = 'X';
            ++count;
        }

    }
}

void BoardImpl::unblock()   // check
{
    // Unblock all currently blocked cells on the board
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
            if (g_board[r][c]->symbol == 'X')
                g_board[r][c]->symbol = '.';

}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    // If a shipId that doesn't exist is inputted, return false
    if (shipId >= m_game.nShips() || shipId < 0)
        return false;

    // If the inputted ship has already been placed, return false
    if (ship_occured.at(shipId))
        return false;

    if (dir == VERTICAL)                // Place the ship vertically
    {
        // If placing the inputted ship at the inputted position isn't possible, return false
        if ((topOrLeft.r + m_game.shipLength(shipId) - 1) >= m_game.rows() || topOrLeft.c >= m_game.cols() || topOrLeft.r < 0 || topOrLeft.c < 0)
            return false;

        // If an unempty cell exists anywhere where we're trying to place our ship, return false
        for (int i = topOrLeft.r; i < (topOrLeft.r + m_game.shipLength(shipId)); i++)
            if (g_board[i][topOrLeft.c]->symbol != '.')
                return false;

        // Place our ship and update corresponding cells in board
        for (int i = topOrLeft.r; i < (topOrLeft.r + m_game.shipLength(shipId)); i++)
        {
            g_board[i][topOrLeft.c]->symbol = m_game.shipSymbol(shipId);
            g_board[i][topOrLeft.c]->shipId = shipId;
            g_board[i][topOrLeft.c]->ship_destroyed = false;
        }

        ship_occured.at(shipId) = true;                 // Inputted ship has been placed

        return true;                // Ship was successfully placed      
    }


    else                            // Place the ship horizontally
    {
        // If placing the inputted ship at the inputted position isn't possible, return false 
        if (topOrLeft.r >= m_game.rows() || (topOrLeft.c + m_game.shipLength(shipId) - 1) >= m_game.cols() || topOrLeft.r < 0 || topOrLeft.c < 0)
            return false;

        // If an unempty cell exists anywhere where we're trying to place our ship, return false
        for (int i = topOrLeft.c; i < (topOrLeft.c + m_game.shipLength(shipId)); i++)
            if (g_board[topOrLeft.r][i]->symbol != '.')
                return false;

        // Place our ship and update corresponding cells in board
        for (int i = topOrLeft.c; i < (topOrLeft.c + m_game.shipLength(shipId)); i++)
        {
            g_board[topOrLeft.r][i]->symbol = m_game.shipSymbol(shipId);
            g_board[topOrLeft.r][i]->shipId = shipId;
            g_board[topOrLeft.r][i]->ship_destroyed = false;
        }

        ship_occured.at(shipId) = true;                 // Inputted ship has been placed

        return true;                // Ship was successfully placed
    }
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    // Remove inputted ship from board

    if (shipId >= m_game.nShips() || shipId < 0)        // If shipId isn't valid, return false
        return false;

    // If inputted start position isn't valid, return false
    if (topOrLeft.r >= m_game.rows() || topOrLeft.c >= m_game.cols() || topOrLeft.r < 0 || topOrLeft.c < 0)
        return false;

    // If inputted position doesn't contain inputted ship, return false
    if (g_board[topOrLeft.r][topOrLeft.c]->symbol != m_game.shipSymbol(shipId))
        return false;

    if (dir == VERTICAL)                    // Ship is placed vertically
    {
        // If the ship's location is inaccurate, return false
        for (int i = (topOrLeft.r + 1); i < (topOrLeft.r + m_game.shipLength(shipId)); i++)
            if (g_board[i][topOrLeft.c]->symbol != m_game.shipSymbol(shipId) || i >= m_game.rows())
                return false;

        // Clear all cells containing the inputted ship
        for (int i = topOrLeft.r; i < (topOrLeft.r + m_game.shipLength(shipId) + 1); i++)
        {
            g_board[i][topOrLeft.c]->symbol = '.';
            g_board[i][topOrLeft.c]->shipId = -1;
            g_board[i][topOrLeft.c]->ship_destroyed = true;
        }

        ship_occured.at(shipId) = false;                // Ship no longer occurs on board

        return true;                                    // Ship removal was successful
    }


    else                                                // Ship was placed horizontally
    {
        // If the ship's location is inaccurate, return false
        for (int i = (topOrLeft.c + 1); i < (topOrLeft.c + m_game.shipLength(shipId)); i++)
            if (g_board[i][topOrLeft.c]->symbol != m_game.shipSymbol(shipId) || i >= m_game.rows())
                return false;

        // Clear all cells containing the inputted ship
        for (int i = topOrLeft.c; i < (topOrLeft.c + m_game.shipLength(shipId) + 1); i++)
        {
            g_board[topOrLeft.r][i]->symbol = '.';
            g_board[topOrLeft.r][i]->shipId = -1;
            g_board[topOrLeft.c][i]->ship_destroyed = true;
        }

        ship_occured.at(shipId) = false;                // Ship no longer occurs on board

        return true;                                    // Ship removal was successful 
    }
}

void BoardImpl::display(bool shotsOnly) const
{
    // Display current board

    // Display column labels
    cout << "  ";
    for (int c = 0; c < m_game.cols(); c++)
        cout << c;
    cout << endl;

    for (int r = 0; r < m_game.rows(); r++)
    {
        cout << r << ' ';               // Display row label

        // For each column value, if attacked, display result. If not, display either true symbol or empty symbol

        for (int c = 0; c < m_game.cols(); c++)
        {
            if (g_board[r][c]->has_attacked)
            {
                if (g_board[r][c]->has_hit)
                    cout << 'X';
                else
                    cout << 'o';
            }
            else if (!shotsOnly)
                cout << g_board[r][c]->symbol;
            else
                cout << '.';
        }
        cout << '\n';
    }

}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    // If inputted position isn't on board, return false
    if (!m_game.isValid(p))
        return false;

    // If inputted position has already been attacked, return false
    if (g_board[p.r][p.c]->has_attacked)
        return false;

    g_board[p.r][p.c]->has_attacked = true;         // Inputted position has now been attacked


    if (g_board[p.r][p.c]->symbol != '.' && g_board[p.r][p.c]->has_hit == false)        // If attack hits a ship
    {
        // Position has now been hit
        g_board[p.r][p.c]->has_hit = true;
        shotHit = true;
        int current_shipId = g_board[p.r][p.c]->shipId;
        shipDestroyed = true;

        // If another part of hit ship hasn't been hit yet, ship is not yet destroyed
        for (int r = 0; r < m_game.rows(); r++)
            for (int c = 0; c < m_game.cols(); c++)
                if (g_board[r][c]->shipId == current_shipId && !g_board[r][c]->has_hit)
                    shipDestroyed = false;

        if (shipDestroyed)
        {
            // If ship was destroyed, record this result
            shipId = g_board[p.r][p.c]->shipId;
            for (int r = 0; r < m_game.rows(); r++)
                for (int c = 0; c < m_game.cols(); c++)
                    if (g_board[r][c]->shipId == shipId)
                        g_board[r][c]->ship_destroyed = true;
        }

    }
    else                            // attack missed
        shotHit = false;

    return true;                    // attack was successfully executed
}

bool BoardImpl::allShipsDestroyed() const
{
    // If all ships have been destroyed, return true. Otherwise, return false
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
            if (g_board[r][c]->ship_destroyed == false && g_board[r][c]->shipId != -1)
                return false;
    return true;
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
