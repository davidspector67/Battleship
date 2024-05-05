#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include "utility.h"
#include <iostream>
#include <string>
#include <stack>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
public:
    AwfulPlayer(string nm, const Game& g);
    ~AwfulPlayer() {}
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
        bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
    : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
    // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if (!b.placeShip(Point(k, 0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
    bool /* shotHit */, bool /* shipDestroyed */,
    int /* shipId */)
{
    // AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
    // AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class HumanPlayer : public Player
{
public:
    HumanPlayer(string nm, const Game& g);
    ~HumanPlayer();
    virtual bool isHuman() const { return true; }
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
        bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    Point m_attackCell;
    Point m_placeShip;
    int current_shipId;
    Direction current_dir;
    bool hasOwnShip[MAXROWS][MAXCOLS];          // Record if player has previously placed a ship in a given position on the board
    vector <Point> chosen_points;               // Stores previously chosen points player has attacked
};

HumanPlayer::HumanPlayer(string nm, const Game& g)
    : Player(nm, g), m_attackCell(0, 0), m_placeShip(0, 0), current_shipId(-1), current_dir(HORIZONTAL), hasOwnShip()
{}

HumanPlayer::~HumanPlayer()
{}

bool HumanPlayer::placeShips(Board& b)
{
    // Walk human player through placing their ships

    int r, c, dir;
    Direction direction;


    // Check if there's enough area in the board to place all ships

    int total_ship_area = 0;
    for (int id = 0; id < game().nShips(); id++)
    {
        total_ship_area += game().shipLength(id);
        if (game().shipLength(id) >= game().rows() && game().shipLength(id) >= game().cols())
        {
            cout << "It is not possible for current ships to be placed on current board." << endl;
            return false;
        }
    }

    int total_board_area = game().rows() * game().cols();

    if (total_board_area < total_ship_area)
    {
        cout << "It is not possible for current ships to be placed on current board." << endl;
        return false;
    }


    for (int id = 0; id < game().nShips(); id++)
    {
        string dir;
        Direction direction;
        b.display(false);
        while (true)
        {
            cout << "Enter h or v for direction of " << game().shipName(id) << " (length " << game().shipLength(id) << ") : ";
            getline(cin, dir);
            if (dir != "h" && dir != "v")
            {
                cout << "Direction must be h or v." << endl;
            }
            if (dir == "h" || dir == "v")
                break;
        }


        while (true)
        {
            if (dir == "h")
            {
                cout << "Emter row and column of leftmost cell (e.g., 3 5): ";
                direction = HORIZONTAL;
            }
            if (dir == "v")
            {
                cout << "Enter row and column of topmost cell (e.g., 3 5): ";
                direction = VERTICAL;
            }
            int r, c;
            bool integers = getLineWithTwoIntegers(r, c);
            if (!integers)
                cout << "You must enter two integers." << endl;
            else
            {
                if (game().isValid(Point(r, c)) && b.placeShip(Point(r, c), id, direction))
                    break;
                else
                    cout << "The ship can not be placed there." << endl;
            }
        }
    }

    return true;


}

Point HumanPlayer::recommendAttack()
{
    // Prompt human for attack position

    cout << "Enter the row andd column to attack (e.g., 3 5): ";
    int r, c;
    bool integer = getLineWithTwoIntegers(r, c);



    // If input is not made of 2 integerss
    if (!integer)
    {
        cout << "You must enter two integers. " << endl;
        return recommendAttack();
    }

    chosen_points.push_back(Point(r, c));           // Record attack at inputted position

    return Point(r, c);                             // Return valid inputted position
}

void HumanPlayer::recordAttackResult(Point /* p */, bool /* validShot */, bool /* shotHit */, bool /* shipDestroyed */, int /* shipId */)
{
    // does nothing
}

void HumanPlayer::recordAttackByOpponent(Point /* p */)
{
    // HumanPlayer completely ignores what the opponent does
}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class MediocrePlayer : public Player
{
public:
    MediocrePlayer(string nm, const Game& g);
    ~MediocrePlayer() {}
    // Determine where to place each ship
    bool placeShip(Point p, int shipId, Board& b, int tries);
    // If current ship configuration doesn't work, unplace all previously placed ships 
    bool backTrack(int current_shipId, size_t index, Board& b);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
        bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
private:
    Point m_attackCell;
    int state;
    bool hasHit[MAXROWS][MAXCOLS];                  // Record if ship has been hit at each position on board
    vector <Point> close_points;                    // Store current list of points within 4 steps of hit point both vertically and horizontally
    vector <Point> unChosen_coordinates;            // Store all points on board not yet attacked
    vector <Point> Chosen_coordinates;              // Store all points on board previously attacked
    vector <Point> unused_coordinates;              // Store all points without a placed ship
    vector <Point> used_coordinates;                // Store all points with a placed ship
    Point start_point;                              // Record hit location for close_points to reference

};

MediocrePlayer::MediocrePlayer(string nm, const Game& g) :Player(nm, g), state(0), close_points(), unChosen_coordinates()
{
    // Record each point in the board as unChosen, unused, and not hit
    for (int r = 0; r < game().rows(); r++)
        for (int c = 0; c < game().cols(); c++)
        {
            unChosen_coordinates.push_back(Point(r, c));
            unused_coordinates.push_back(Point(r, c));
            hasHit[r][c] = false;
        }
}

bool MediocrePlayer::backTrack(int current_shipId, size_t index, Board& b)
{
    // If we've examined all used coordinates, return false
    if (index >= used_coordinates.size())
        return false;

    // If all ships are removed, return true
    if (current_shipId == -1)
        return true;

    // If inputted ship can be unplaced at current position, move to next ship and next location
    if (b.unplaceShip(used_coordinates.at(index), current_shipId, VERTICAL) || b.unplaceShip(used_coordinates.at(index), current_shipId, HORIZONTAL))
        return backTrack(current_shipId - 1, index + 1, b);

    // If inputted ship cannot be unplaced at current position, try again at next location
    return backTrack(current_shipId, index + 1, b);
}

bool MediocrePlayer::placeShip(Point p, int shipId, Board& b, int tries)
{

    // If all ships have been placed, return true
    if (shipId == game().nShips())
        return true;

    // If the last unused coordinate has been reached
    if (unused_coordinates.size() == 0)
    {
        // If no ships have been placed, return false
        if (shipId == 0)
            return false;

        // If we've used all unused coordinates 50 times, return false
        if (tries >= 50)
            return false;

        // If we backTrack fails, return false
        if (!backTrack(shipId, 0, b))
            return false;

        // All positions in board are now unused and no positions are used
        for (size_t i = 0; i < used_coordinates.size(); i++)
            unused_coordinates.push_back(used_coordinates.at(i));
        used_coordinates.clear();

        // Pick a random coordinate to start placing ships at again from scratch, but record the try that has taken place
        int index = randInt(static_cast<int>(unused_coordinates.size()));
        return placeShip(unused_coordinates.at(index), shipId, b, tries + 1);
    }

    if (b.placeShip(p, shipId, VERTICAL) || b.placeShip(p, shipId, HORIZONTAL))
    {
        // Free up all positions in the board to attempt to place the next ship
        for (size_t i = 0; i < used_coordinates.size(); i++)
            unused_coordinates.push_back(used_coordinates.at(i));
        used_coordinates.clear();

        // If possible, place ship at inputted location and proceed to place next ship
        int rand_index = randInt(static_cast<int>(unused_coordinates.size()));
        return placeShip(unused_coordinates.at(rand_index), shipId + 1, b, tries);
    }

    // Coordinate has now been used
    used_coordinates.push_back(p);
    eraseFromVector(p.r, p.c, unused_coordinates);

    if (unused_coordinates.size() > 0)
    {
        // If ship could not be placed, try a different coordinate
        int index = randInt(static_cast<int>(unused_coordinates.size()));
        return placeShip(unused_coordinates.at(index), shipId, b, tries);
    }

    // If the function has used the last unused coordinate on the board
    else
        return placeShip(Point(-1, -1), shipId, b, tries);



}

bool MediocrePlayer::placeShips(Board& b)
{
    // Make sure there's enough area in the board to place all ships
    int total_area_ships = 0;
    for (int id = 0; id < game().nShips(); id++)
        total_area_ships += game().shipLength(id);

    int total_area_blockedBoard = int(0.5 * game().rows() * game().cols());
    if (total_area_blockedBoard < total_area_ships)
        return false;

    for (int i = 0; i < game().nShips(); i++)
        if (game().shipLength(i) >= game().cols() && game().shipLength(i) >= game().rows())
            return false;

    b.block();                          // Block half the cells on the board


    // Start the placeShip function at a random cell on the board
    int index = randInt(static_cast<int>(unused_coordinates.size()));
    bool set = placeShip(unused_coordinates.at(index), 0, b, 0);

    b.unblock();                        // Unblock all blocked cells on the board

    return set;
}

Point MediocrePlayer::recommendAttack()
{
    if (state == 0)
    {
        // If ship hasn't been hit without destroying ship, return a random unchosen coordinate
        int index = randInt(static_cast<int>(unChosen_coordinates.size()));
        Point current(unChosen_coordinates.at(index));
        Chosen_coordinates.push_back(current);
        eraseFromVector(current.r, current.c, unChosen_coordinates);
        return current;
    }

    else
    {
        // If ship has been hit and a ship hasn't been destroyed, return a random unchosen coordinate within 4 steps of original hit
        int index = randInt(static_cast<int>(close_points.size()));
        Point current = close_points.at(index);
        Chosen_coordinates.push_back(current);
        eraseFromVector(current.r, current.c, close_points);
        eraseFromVector(current.r, current.c, unChosen_coordinates);
        return current;
    }

}


void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
    bool shipDestroyed, int /* shipId */)
{
    // If attack uhas an invalid coordinate, stop the function
    if (!validShot)
        return;

    if (state == 0)
    {
        // For all cases where shot was hit, record the event
        if (shotHit)
            hasHit[p.r][p.c] = true;


        if (shotHit && !shipDestroyed)
        {
            // If shot hit but ship wasn't destroyed, record point and make list of 
            // coordinates within 4 steps of inputted location
            start_point = p;
            close_points.clear();

            for (int i = p.r - 4; i <= (p.r + 4); i++)
                if (game().isValid(Point(i, p.c)))
                    close_points.push_back(Point(i, p.c));

            for (int i = p.c - 4; i <= (p.c + 4); i++)
                if (game().isValid(Point(p.r, i)))
                    close_points.push_back(Point(p.r, i));

            for (size_t i = 0; i < Chosen_coordinates.size(); i++)
                for (size_t j = 0; j < close_points.size(); j++)
                    if (Chosen_coordinates.at(i).r == close_points.at(j).r && Chosen_coordinates.at(i).c == close_points.at(j).c)
                        eraseFromVector(close_points.at(j).r, close_points.at(j).c, close_points);

            state = 1;              // Record change in state
        }
    }

    else
    {
        if (shotHit && !shipDestroyed)
        {
            hasHit[p.r][p.c] = true;            // Record hit shot
        }
        if (shotHit && shipDestroyed)
        {
            hasHit[p.r][p.c] = true;
            state = 0;
        }
        if (close_points.size() == 0)           // All close points have been attacked
            state = 0;

    }
}


void MediocrePlayer::recordAttackByOpponent(Point /* p */)
{
    // This function does nothing
}



//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class GoodPlayer : public Player
{
public:
    GoodPlayer(string nm, const Game& g);
    ~GoodPlayer() {}                                            // delete player subclass destructors next
    bool placeShipsRestricted(int shipId, Board& b, int tries); // Back-up to placeRestOfShips
    bool placeRestOfShips(int shipId, Board& b, int tries);     // Place all ships so that none neighbour each other
    int num_possible_ships(int r, int c) const;                 // Count number of ship location possibilities at each location
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    Point chooseNextFree();                                     // Return unchosen position with most ship possibilities
    Point chooseClose(int r, int c, int dir);                   // Return correct location close to previously hit but not destroyed ship
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
        bool shipDestroyed, int shipId);
    virtual void recordAttackByOpponent(Point p);
    void eraseNeighbouringPoints(Point start, int shipId, Direction dir);       // Remove all points containing or neighbouring a ship from unused_coordinates vector
private:
    int state;
    bool hasOwnShip[MAXROWS][MAXCOLS];
    bool hasHit[MAXROWS][MAXCOLS];
    bool hasMissed[MAXROWS][MAXCOLS];
    vector<Point> unused_coordinates;               // Store all points without a ship and not neighbouring a placed ship
    vector<Point> used_coordinates;                 // Store all points either containing a ship or neighbouring a ship
    vector<Point> unChosen_coordinates;
    int closeDirections;
    Point start_point;
    int r_cur, c_cur;                               // First hit but not destroyed position to anchor chooseClose outcomes
};

GoodPlayer::GoodPlayer(string nm, const Game& g) : Player(nm, g), state(0), closeDirections(0), r_cur(-1), c_cur(-1)
{
    // Initialize each cell in the board as empty without any history
    for (int r = 0; r < game().rows(); r++)
        for (int c = 0; c < game().cols(); c++)
        {
            hasOwnShip[r][c] = false;
            unused_coordinates.push_back(Point(r, c));
            unChosen_coordinates.push_back(Point(r, c));
            hasMissed[r][c] = false;
            hasHit[r][c] = false;
        }
}

bool GoodPlayer::placeShipsRestricted(int shipId, Board& b, int tries)
{
    // If ships cannot be placed without neighbours, place them randomly in the board

    // Function has placed all the ships
    if (shipId == game().nShips())
        return true;

    // All coordinates have ships on them
    if (unused_coordinates.size() == 0)
        return false;

    // Function has unsuccessfully tried to place ships too many times
    if (tries >= MAXROWS * MAXCOLS * 2)
        return false;

    // Choose a random shipless point
    int random = randInt(static_cast<int>(unused_coordinates.size()));
    Point random_point = unused_coordinates.at(random);

    // Place ship horizontally or vertically if possible

    if (/*shipVertical && */ b.placeShip(random_point, shipId, VERTICAL))
    {
        for (int r = random_point.r; r < (random_point.r + game().shipLength(shipId)); r++)
            eraseFromVector(r, random_point.c, unused_coordinates);
        return placeShipsRestricted(shipId + 1, b, tries);
    }
    if (/*shipHorizontal &&*/ b.placeShip(random_point, shipId, HORIZONTAL))
    {
        for (int c = random_point.c; c < (random_point.c + game().shipLength(shipId)); c++)
            eraseFromVector(random_point.r, c, unused_coordinates);
        return placeShipsRestricted(shipId + 1, b, tries);
    }

    // If ship was not placed, try again with a different coordinate
    return placeRestOfShips(shipId, b, tries + 1);

}

bool GoodPlayer::placeRestOfShips(int shipId, Board& b, int tries)
{
    // All ships have been placed on the board
    if (shipId == game().nShips())
        return true;

    // All unused coordinates have been used
    if (unused_coordinates.size() == 0)
        return false;

    // We've unsuccessfully tried to place ships too many times
    if (tries >= MAXROWS * MAXCOLS * 2)
    {
        b.clear();
        used_coordinates.clear();
        unused_coordinates.clear();
        for (int r = 0; r < game().rows(); r++)
            for (int c = 0; c < game().cols(); c++)
                unused_coordinates.push_back(Point(r, c));
        return placeShipsRestricted(0, b, 0);
    }

    // Pick a random point that doesn't have a ship on it
    int random = randInt(static_cast<int>(unused_coordinates.size()));
    Point random_point = unused_coordinates.at(random);

    // Determine if current ship can be placed at random point vertically
    bool shipVertical = true;
    bool shipHorizontal = true;

    for (int r = random_point.r - 1; r <= random_point.r + game().shipLength(shipId); r++)
        for (size_t i = 0; i < used_coordinates.size(); i++)
            if (used_coordinates.at(i).r == r && used_coordinates.at(i).c == random_point.c)
            {
                shipVertical = false;
            }

    // Determine if current ship can be placed at random point horizontally

    for (int c = random_point.c - 1; c <= random_point.c + game().shipLength(shipId); c++)
        for (size_t i = 0; i < used_coordinates.size(); i++)
            if (used_coordinates.at(i).r == random_point.r && used_coordinates.at(i).c == c)
            {
                shipHorizontal = false;
            }

    // Place ship horizontally or vertically if possible

    bool placedVertical = false;
    bool placedHorizontal = false;

    if ((shipVertical && b.placeShip(random_point, shipId, VERTICAL)))
    {
        eraseNeighbouringPoints(random_point, shipId, VERTICAL);
        placedVertical = true;
    }
    if (shipHorizontal && b.placeShip(random_point, shipId, HORIZONTAL))
    {
        eraseNeighbouringPoints(random_point, shipId, HORIZONTAL);
        placedHorizontal = true;
    }

    // If ship was placed, move on to next ship
    if (placedHorizontal || placedVertical)
        return placeRestOfShips(shipId + 1, b, tries + 1);

    // If ship was not placed, try again with a different coordinate
    return placeRestOfShips(shipId, b, tries + 1);

}

void GoodPlayer::eraseNeighbouringPoints(Point start, int shipId, Direction dir)
{
    // Erase all points containing or neighbouring inputted ship at inputted location

    if (dir == HORIZONTAL)
    {
        for (int i = start.c; i < (start.c + game().shipLength(shipId)); i++)
        {
            hasOwnShip[start.r][i] = true;
            used_coordinates.push_back(Point(start.r, i));
            used_coordinates.push_back(Point(start.r + 1, i));
            used_coordinates.push_back(Point(start.r - 1, i));
            eraseFromVector(start.r, i, unused_coordinates);
            eraseFromVector(start.r + 1, i, unused_coordinates);
            eraseFromVector(start.r - 1, i, unused_coordinates);
        }
        used_coordinates.push_back(Point(start.r, start.c - 1));
        used_coordinates.push_back(Point(start.r, start.c + game().shipLength(shipId)));
        eraseFromVector(start.r, start.c - 1, unused_coordinates);
        eraseFromVector(start.r, start.c + game().shipLength(shipId), unused_coordinates);
    }
    else
    {
        for (int i = start.r; i < (start.r + game().shipLength(shipId)); i++)
        {
            hasOwnShip[i][start.c] = true;
            used_coordinates.push_back(Point(i, start.c));
            used_coordinates.push_back(Point(i, start.c + 1));
            used_coordinates.push_back(Point(i, start.c - 1));
            eraseFromVector(i, start.c, unused_coordinates);
            eraseFromVector(i, start.c + 1, unused_coordinates);
            eraseFromVector(i, start.c - 1, unused_coordinates);
        }
        used_coordinates.push_back(Point(start.r - 1, start.c));
        used_coordinates.push_back(Point(start.r + game().shipLength(shipId), start.c));
        eraseFromVector(start.r - 1, start.c, unused_coordinates);
        eraseFromVector(start.r + game().shipLength(shipId), start.c, unused_coordinates);
    }
}

bool GoodPlayer::placeShips(Board& b)
{
    // If no ships to place, return true
    if (game().nShips() <= 0)
        return true;

    // If one ship doesn't fit in board, return false
    for (int i = 0; i < game().nShips(); i++)
        if (game().shipLength(i) >= game().cols() && game().shipLength(i) >= game().rows())
            return false;

    // If ships have a larger area than board, return false

    int total_area_ships = 0;
    for (int id = 0; id < game().nShips(); id++)
        total_area_ships += game().shipLength(id);

    int total_area_board = int(game().rows() * game().cols());
    if (total_area_board < total_area_ships)
        return false;

    // Place first ship on random side of board

    // Let top, right, bottom, left = 0,1,2,3
    int side = randInt(4);
    int col, row;
    switch (side)
    {
    case 0:         // top          
        col = randInt(game().cols() - game().shipLength(0));
        if (b.placeShip(Point(0, col), 0, HORIZONTAL))
        {
            eraseNeighbouringPoints(Point(0, col), 0, HORIZONTAL);
            break;
        }
        else
        {
            ;
        }
    case 1:         // right
        row = randInt(game().rows() - game().shipLength(0));
        if (b.placeShip(Point(row, 0), 0, VERTICAL))
        {
            eraseNeighbouringPoints(Point(row, 0), 0, VERTICAL);
            break;
        }
        else
        {
            ;
        }
    case 2:         // bottom
        col = randInt(game().cols() - game().shipLength(0));
        if (b.placeShip(Point(game().rows() - 1, col), 0, HORIZONTAL))
        {
            eraseNeighbouringPoints(Point(game().rows() - 1, col), 0, HORIZONTAL);
            break;
        }
        else
        {
            ;
        }
    case 3:         // left
        row = randInt(game().rows() - game().shipLength(0));
        if (b.placeShip(Point(row, game().cols() - 1), 0, VERTICAL))
        {
            eraseNeighbouringPoints(Point(row, game().cols() - 1), 0, VERTICAL);
            break;
        }
        else
        {
            col = randInt(game().cols() - game().shipLength(0));
            if (b.placeShip(Point(0, col), 0, HORIZONTAL))
            {
                eraseNeighbouringPoints(Point(0, col), 0, HORIZONTAL);
                break;
            }
            else
            {
                ;
            }
        }
    }

    // If more ships to play
    if (game().nShips() > 1)
        return placeRestOfShips(1, b, 0);

    return true;        // Ships successfully placed
}


int GoodPlayer::num_possible_ships(int r, int c) const
{
    // Count number of possible ship configurations at inputted location while treating hit ships as misses

    // For each ship, count the number of horizontal and vertical possibilities and add them together

    int combination_count = 0;

    for (int shipId = 0; shipId < game().nShips(); shipId++)
    {
        for (int cur_row = (r - game().shipLength(shipId) + 1); cur_row <= r; cur_row++)
        {
            if (cur_row >= 0 && cur_row < game().rows() && (cur_row + game().shipLength(shipId) - 1) < game().rows())
            {
                ++combination_count;
                for (int row = cur_row; row < (cur_row + game().shipLength(shipId)); row++)
                    if (hasMissed[row][c] || hasHit[row][c])
                    {
                        --combination_count;
                        break;
                    }
            }
        }

        for (int cur_col = (c - game().shipLength(shipId) + 1); cur_col <= c; cur_col++)
        {
            if (cur_col >= 0 && cur_col < game().cols() && (cur_col + game().shipLength(shipId) - 1) < game().cols())
            {
                ++combination_count;
                for (int col = cur_col; col < (cur_col + game().shipLength(shipId)); col++)
                    if (hasMissed[r][col] || hasHit[r][col])
                    {
                        --combination_count;
                        break;
                    }
            }
        }
    }
    return combination_count;       // Return total number of possibilities for inputted location
}

Point GoodPlayer::chooseNextFree()
{
    // Iterate through each point on the board and find the location with the largest ship possibilities

    int max_possibilities = 0;
    Point max(-1, -1);

    for (size_t i = 0; i < unChosen_coordinates.size(); i++)
    {
        int possibilities = num_possible_ships(unChosen_coordinates.at(i).r, unChosen_coordinates.at(i).c);
        if (possibilities > max_possibilities)
        {
            max_possibilities = possibilities;
            max.r = unChosen_coordinates.at(i).r;
            max.c = unChosen_coordinates.at(i).c;
        }
    }

    // Special case of few spaces left
    if (max_possibilities == 0)
    {
        int random_point = randInt(static_cast<int>(unChosen_coordinates.size()));
        return unChosen_coordinates.at(random_point);
    }

    return max;             // Retrun the location with the largest amount of ship possibilities

}

Point GoodPlayer::chooseClose(int r, int c, int dir)
{
    // For inputted direction, keep attacking until a ship is no longer hit
    Point current;
    switch (dir)
    {
    case 4:                             // Left
        current = Point(r, c - 1);
        if (game().isValid(current) && !hasMissed[r][c - 1])
        {
            // If neighbouring position has been hit, re-run function with next position in the left direction
            if (hasHit[r][c - 1])
                return chooseClose(r, c - 1, closeDirections);
            else
                return current;
        }
        else
        {
            --closeDirections;
        }
    case 3:                             // Right
        current = Point(r, c + 1);
        if (game().isValid(current) && !hasMissed[r][c + 1])
        {
            // If neighbouring position has been hit, re-run function with next position in the right direction
            if (hasHit[r][c + 1])
                return chooseClose(r, c + 1, closeDirections);
            else
                return current;
        }
        else
        {
            --closeDirections;
        }
    case 2:                             // Up
        current = Point(r - 1, c);
        if (game().isValid(current) && !hasMissed[r - 1][c])
        {
            // If neighbouring position has been hit, re-run function with next position in the upwards direction
            if (hasHit[r - 1][c])
                return chooseClose(r - 1, c, closeDirections);
            else
                return current;
        }
        else
        {
            --closeDirections;
        }
    case 1:                             // Down
        current = Point(r + 1, c);
        if (game().isValid(current) && !hasMissed[r + 1][c])
        {
            // If neighbouring position has been hit, re-run function with next position in the downwards direction
            if ((hasHit[r + 1][c]))
                return chooseClose(r + 1, c, closeDirections);
            else
                return current;
        }
        else
        {
            ;
        }
    }

    // added shit to else statements
    state = 0;
    return recommendAttack();
}

Point GoodPlayer::recommendAttack()
{
    // Ship was just hit
    if (state == 0)
        return chooseNextFree();

    // Ship was either not just hit or was just destroyed
    else
        return chooseClose(r_cur, c_cur, closeDirections);
}


void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
    bool shipDestroyed, int /* shipId */)
{
    if (!validShot)
        return;

    eraseFromVector(p.r, p.c, unChosen_coordinates);        // Record inputted position

    if (state == 0)
    {
        // Record events of all possible outcomes

        if (!shotHit)
            hasMissed[p.r][p.c] = true;

        if (shotHit && !shipDestroyed)
        {
            state = 1;
            start_point = p;
            hasHit[p.r][p.c] = true;
            closeDirections = 4;        // Represents 4 currently unexplored directions from new anchor point
            r_cur = p.r;                // chooseClose anchor point
            c_cur = p.c;
        }

        if (shipDestroyed)
        {
            hasHit[p.r][p.c] = true;
        }

    }

    if (state == 1)
    {
        // Record events of all possible outcomes

        if (shotHit && shipDestroyed)
        {
            state = 0;
            hasHit[p.r][p.c] = true;
        }

        if (!shotHit && closeDirections == 0)
        {
            --closeDirections;                  // No more directions to explore. Back to state 0
            hasMissed[p.r][p.c] = true;
            state = 0;
        }

        if (!shotHit && closeDirections > 0)
        {
            hasMissed[p.r][p.c] = true;
            --closeDirections;                  // Time to explore next direction
            r_cur = start_point.r;
            c_cur = start_point.c;
            state = 1;
        }

        if (shotHit && !shipDestroyed)
            hasHit[p.r][p.c] = true;
    }
}

void GoodPlayer::recordAttackByOpponent(Point /* p */)
{
    // do nothing
}



//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };

    int pos;
    for (pos = 0; pos != sizeof(types) / sizeof(types[0]) && type != types[pos]; pos++)
        ;
    switch (pos)
    {
    case 0:  return new HumanPlayer(nm, g);
    case 1:  return new AwfulPlayer(nm, g);
    case 2:  return new MediocrePlayer(nm, g);
    case 3:  return new GoodPlayer(nm, g);
    default: return nullptr;
    }
}
