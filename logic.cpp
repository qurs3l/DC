#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    //if file is invalid return nullptr
    ifstream ifs;
    ifs.open(fileName);
    if(!ifs.is_open())
    {
        return nullptr;
    }
    //also check if the input stream works
    ifs >> maxRow;
    if(ifs.fail())
    {
        return nullptr;
    }
    ifs >> maxCol;
    if(ifs.fail())
    {
        return nullptr;
    }
    //check for valid dimensions
    if(maxRow <= 0 || maxCol <= 0)
    {
        return nullptr;
    }
    if(maxRow > (INT32_MAX/ maxCol))
    {
        return nullptr;
    }
    ifs >> player.row;
    if(ifs.fail())
    {
        return nullptr;
    }
    ifs >> player.col;
    if(ifs.fail())
    {
        return nullptr;
    }
    //if the player column is bigger than the max or is negative, same with row
    if(player.row >= maxRow || player.col >= maxCol || player.row < 0 || player.col < 0)
    {
        return nullptr;
    }
    //if correct behavior call create map
    char** map = createMap(maxRow, maxCol);
    //represnets tile to be copied from map
    bool door = false;
    bool exitExists = false;
	for(int r = 0; r < maxRow; ++r)
    {
        for(int c = 0; c < maxCol; ++c)
        {
            //makes player's spot
            //check for invalid characters
            ifs >> map[r][c];
            if (ifs.fail())
            {
                deleteMap(map, maxRow);
				return nullptr;
            }
            if(map[r][c] != TILE_AMULET 
            && map[r][c] != TILE_DOOR 
            && map[r][c] != TILE_EXIT 
            && map[r][c] != TILE_MONSTER 
            && map[r][c] != TILE_OPEN
            && map[r][c] != TILE_PILLAR 
            && map[r][c] != TILE_TREASURE)
            {
                deleteMap(map, maxRow);
                return nullptr;
            }
            if(map[r][c] == TILE_DOOR)
            {
                door = true;
            }
            if(map[r][c] == TILE_EXIT)
            {
                exitExists = true;
            }
        }
    }
    if(door == false && exitExists == false)
    {
        deleteMap(map,maxRow);
        return nullptr;
    }
    //check if there are too many values in the file
    char checkMany;
    ifs >> checkMany;
    if(!ifs.fail())
    {
        deleteMap(map,maxRow);
        return nullptr;
    }
    map[player.row][player.col] = TILE_PLAYER;
    ifs.close();
    return map;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    
    if(input == MOVE_DOWN)
    {
        nextRow += 1;
    }
    else if(input == MOVE_UP)
    {
        nextRow -= 1;
    }
    else if(input == MOVE_LEFT)
    {
        nextCol -= 1;
    }
    else if (input == MOVE_RIGHT)
    {
        nextCol += 1;
    }
    
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    //if input values are invalid return nullptr
    //invalid if maxRow and maxCol are 0/negative
    //r = row, c = col
    if(maxRow <= 0 || maxCol <= 0)
    {
        return nullptr;
    }
    //else, create dynamically allocated 2D Char array
    char** map = new char*[size_t(maxRow)];
    for(size_t r = 0; r < maxRow; ++r)
    {
        //adds columns per row
        map[r] = new char[size_t(maxCol)];   
    }
    //initialize each array to TILE_OPEN
    for(size_t r = 0; r < maxRow; ++r)
    {
        for(size_t c = 0; c < maxCol; ++c)

        {
            map[r][c] = TILE_OPEN;  
        }
    }
    //return pointer
    return map;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    //loop through row and delete then delete entire map
    //first check if the map is null
    if(map != nullptr)
    {
        for(int r = 0; r < maxRow; ++r)
        {
            delete[] map[r];   
        }
        delete[] map;
    }
    maxRow = 0;
    map = nullptr; 
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    int deleteRow = maxRow;
    if(map == nullptr)
    {
        return nullptr;
    }
    //check if dimensions are valid
    if(maxRow <= 0 || maxCol <= 0)
    {
        return nullptr;
    }
    if(maxRow>=INT32_MAX/2 || maxCol>=INT32_MAX/2)
    {
        return nullptr;
    }
    //new dimensions for map are twice the size
    char** newMap = createMap((2 * maxRow),(2 * maxCol));
    //need to copy old map to new map
    //adventurer only copied into submap A, not B,C, or D
    //replace adventurer with TILE_OPEN for maps that are not A
    for(int r = 0; r < maxRow; ++r)
    {
        for(int c = 0; c < maxCol; ++c)
        {
            if(map[r][c] == TILE_PLAYER)
            {
                newMap[r][c] = map[r][c];
                newMap[r][c+maxCol] = TILE_OPEN;
            }
            else
            {
                newMap[r][c] = map[r][c];
                newMap[r][c+maxCol] = map[r][c];
            }
        }

    }
    //replaces C and D
    //from max row to max row *2
    for(int r = maxRow; r < maxRow*2; ++r)
    {
        for(int c = 0; c < maxCol; ++c)
        {
            //if it's a player tile set it to open
            if(map[r-maxRow][c] == TILE_PLAYER)
            {
                newMap[r][c] = TILE_OPEN;
                newMap[r][c+maxCol] = TILE_OPEN;
            }
            //otherwise copy data from the old map
            else
            {
                newMap[r][c] = map[r-maxRow][c];
                newMap[r][c+maxCol] = map[r-maxRow][c];
            }
        }

    }
    //delete the old map and double row and columns
    deleteMap(map, deleteRow);
    maxRow*=2;
    maxCol*=2;

    return newMap;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    //update player column and row based on nextCol and nextRow
    //first check if the nextRow and nextColumn are in bounds
    if(nextRow >= maxRow || nextCol >= maxCol || nextRow < 0 || nextCol < 0)
    {
        return STATUS_STAY;
    }
    //check if map at next row and column is a monster or pillar
    else if(map[nextRow][nextCol] == TILE_MONSTER || map[nextRow][nextCol] == TILE_PILLAR)
    {
        return STATUS_STAY;
    }
    //if the adventurer has no treasure they do not move
    else if(map[nextRow][nextCol] == TILE_EXIT && player.treasure < 1)
    {
        return STATUS_STAY;
    }
    else
    {
        //player is now able to move
        //set players's current position to an open tile and update their position by setting it to next row/col
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        //if player moves onto empty tile their status is move
        if(map[nextRow][nextCol] == TILE_OPEN)
        {
            map[player.row][player.col] = TILE_PLAYER;
            return STATUS_MOVE;
        }
        //if player moves on treasure tile increment treasure and status is treasure
        else if(map[player.row][player.col] == TILE_TREASURE)
        {
            map[player.row][player.col] = TILE_PLAYER;
            player.treasure++;
            return STATUS_TREASURE;
        }
        //if on amulet tile status is amulet
        else if(map[player.row][player.col] == TILE_AMULET)
        {
            map[player.row][player.col] = TILE_PLAYER;
            return STATUS_AMULET;
        }
        //if position is on a door status is leave
        else if(map[player.row][player.col] == TILE_DOOR)
        {
            map[player.row][player.col] = TILE_PLAYER;
            return STATUS_LEAVE;
        }
        //if player is at exit and has one piece of treasure they escape
        else if(map[player.row][player.col] == TILE_EXIT && player.treasure >= 1)
        {
            map[player.row][player.col] = TILE_PLAYER;
            return STATUS_ESCAPE;
        }

    }
   return 0;
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    //check if everything above, below, to the left, and right of player
    //create tile value to check if tile is monster
    bool eaten = false;
    //check up start at one above player row until row 0 is hit
    for(int r = player.row - 1; r >= 0; --r)
    {
        //if monster hits a pillar, breaks
        if(map[r][player.col] == TILE_PILLAR)
        {
            break;
        }
        //else if above is the monster, move the monster down one row and set prev row to empty
        //moving it down moves it closer to the adventurer
        else if(map[r][player.col] == TILE_MONSTER)
        {
            //if the above row is a player, then the monster got the player
            if(map[r+1][player.col] == TILE_PLAYER)
            {
                eaten = true;
            }
            //update
            map[r][player.col] = TILE_OPEN;
            map[r+1][player.col] = TILE_MONSTER;
        }

    }
    //check down, start one below player and move down
    //same as up, but move monster up one row
    for(int r = player.row + 1; r < maxRow; ++r)
    {
        if(map[r][player.col] == TILE_PILLAR)
        {
            break;
        }
        else if(map[r][player.col] == TILE_MONSTER)
        {
            if(map[r-1][player.col] == TILE_PLAYER)
            {
                eaten = true;
            }
            map[r][player.col] = TILE_OPEN;
            map[r-1][player.col] = TILE_MONSTER;
        }
    }
    //check left
    for(int c = player.col - 1; c >= 0; --c)
    {
        if(map[player.row][c] == TILE_PILLAR)
        {
            break;
        }
        else if(map[player.row][c] == TILE_MONSTER)
        {
            //if the above row is a player, then the monster got the player
            if(map[player.row][c+1] == TILE_PLAYER)
            {
                eaten = true;
            }
            //update
            map[player.row][c] = TILE_OPEN;
            map[player.row][c+1] = TILE_MONSTER;
        }

    }
    //check right 
    for(int c = player.col + 1; c < maxCol; ++c)
    {
        if(map[player.row][c] == TILE_PILLAR)
        {
            break;
        }
        else if(map[player.row][c] == TILE_MONSTER)
        {
            //if the above row is a player, then the monster got the player
            if(map[player.row][c-1] == TILE_PLAYER)
            {
                eaten = true;
            }
            //update
            map[player.row][c] = TILE_OPEN;
            map[player.row][c-1] = TILE_MONSTER;
        }

    }
   
    return eaten;
}
