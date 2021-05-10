#ifndef __LEVELSAVE_H__
#define __LEVELSAVE_H__

#include "SDL_stdinc.h"
#include "simple_json.h"
#include "simple_json_array.h"

#include "gfc_vector.h"

#include "level.h"


/**
* @brief Save a level as a json to a save file
* @param level The level object we want to encode as a json
* @param saveFile The name of the file we want to save the level to
*/
void save_level(Level *level, char *saveFile);

/**
* @brief Clears the enemiesjs SJson
*/
void clear_enemies_js();

/**
* @brief Add an enemy to the custom level
* @param enemyCode The code identifying the type of enemy
* @param xpos The x position of the enemy
* @param ypos The y position of the enemy
* @param dir The start direction of the enemy
*/
void add_enemy_customLevel(int enemyCode, int xpos, int ypos, const char *dir);

#endif