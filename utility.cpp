#include "utility.h"
#include <iostream>
#include <vector>

using namespace std;

void eraseFromVector(int r, int c, vector<Point>& givenVector)
{
    while (true)
    {
        size_t pos;
        for (pos = 0; pos < givenVector.size(); pos++)
            if (givenVector.at(pos).r == r && givenVector.at(pos).c == c)
                break;

        if (pos == givenVector.size())
            break;
        else
        {
            givenVector.at(pos) = givenVector.at(givenVector.size() - 1);
            givenVector.pop_back();
        }
    }
}

