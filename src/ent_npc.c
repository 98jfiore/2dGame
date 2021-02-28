#include "simple_logger.h"

#include "gf2d_sprite.h"
#include "gfc_types.h"
#include "gfc_vector.h"

#include "ent_npc.h"
#include "shapes.h"


Entity *npc_spawn(Vector2D position)
{
	Entity *ent;

	ent = entity_new();
	if (ent == NULL)
	{
		slog("Failed to create entity");
		return NULL;
	}

	vector2d_copy(ent->position, position);

	ent->update = npc_update;

	return ent;
}

void npc_update(Entity *self)
{
	if (self->think)
	{
		self->think(self);
	}

	vector2d_add(self->position, self->position, self->velocity);
	//Move in a direction based on random
	/*rand_move = ((int)(gfc_random() * 4.0)) % 4;
	if (rand_move == 0) new_vel = vector2d(10, 0);
	else if (rand_move == 1) new_vel = vector2d(-10, 0);
	else if (rand_move == 2) new_vel = vector2d(0, 10);
	else new_vel = vector2d(0, -10);
	self->velocity = new_vel;*/

}


/*eol@eof*/