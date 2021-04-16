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

#endif