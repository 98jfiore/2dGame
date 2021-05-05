#ifndef __PARTICLES_H__
#define __PARTICLES_H__

#include <SDL.h>

#include "gfc_vector.h"
#include "gfc_color.h"
#include "gf2d_sprite.h"
#include "gf2d_graphics.h"
#include "gf2d_text.h"
#include "gf2d_shape.h"

enum particleMode {
	PART_MODE_A = 1,
	PART_MODE_B = 2,
};

typedef struct ParticleData_S
{
	Uint8    inuse;          /**<do not touch this*/
	Uint8    loaded;          /**<do not touch this*/
	Uint32   ttl;             /**<time to live in update frames*/
	Uint32   ttl_initial;             /**<time to live in update frames*/
	Vector2D position;      /**<position of the particle*/
	Vector2D start_pos;      /**<start position of the particle*/
	Vector2D velocity;      /**<movement velocity of the particle*/
	Vector2D velocity_initial;      /**<movement velocity of the particle*/
	Vector2D acceleration;  /**<movement acceleration of the particle*/
	Vector2D *base;
	Color color;            /**<rendering color of the particle*/
	Color base_color;            /**<rendering color of the particle*/
	Color colorVector;      /**<rate of change for color over time*/
	Sprite *sprite;
	float size;				/**<size of the particle*/
	float size_change;      /**<rate of change for size over time*/
	//Shape       shape;
	void		*mode;
	Uint8		modeFlag;
	SDL_BlendMode blend_mode;
}ParticleData;

typedef struct ParticleSource_S
{
	Vector2D location;
	Uint32	numPart;
	Uint32	maxPart;
	int		emissionRate;
	ParticleData *particles;
	Uint8		modeFlag;
	float		modedata_a;
	float		modedata_b;
	float		modedata_c;
	float		modedata_d;
	float		mda_var;
	float		mdb_var;
	float		mdc_var;
	float		mdd_var;
	Uint32      ttl;
	Uint32      ttlVariance;
	Vector2D    position;
	Vector2D    positionVariance;
	Vector2D    velocity;
	Vector2D    velocityVariance;
	Vector2D    acceleration;
	Vector2D    accelerationVariance;
	//Shape       shape;
	Sprite		*sprite;
	float		size;
	float		sizeVariance;
	float		size_change;
	float		size_changeVariance;
	Color       color;
	Color       colorVector;
	Color       colorVariance;
	SDL_BlendMode blend_mode;     /**<the render mode for drawing ADD, BLEND*/
}ParticleSource;

typedef struct
{
	float dirX;
	float dirY;
	float rad_acc;
	float tan_acc;
	float dirX_init;
	float dirY_init;
}Particle_modeA;


typedef struct
{
	float angle;
	float degreesPerSecond;
	float radius;
	float radius_change;
	float angle_init;
	float radius_init;
}Particle_modeB;


/**
* @brief Get an allocated particle data
* @param ps the particle source to get from
*/
ParticleData *new_particle_data(ParticleSource *ps);


/**
* @brief free an allocated particle sours
* @param ps the particle source to free
*/
void particle_source_free(ParticleSource *ps);

/**
* @brief update all particles in the provided source
* @param pe the particle source to update
*/
void particle_source_update(ParticleSource *ps);

/**
* @brief draw all particles to current rendering context
* @param pe the particle source to draw
*/
void particle_source_draw(ParticleSource *ps);

/**
* @brief create a new particle source specifying all default values for the source
* @return NULL on error or a pointer to a particle source
*/
ParticleSource *particle_source_new(
	int         maxParticles,
	int         startParticles,
	int		emissionRate,
	Uint32      ttl,
	Uint32      ttlVariance,
	Vector2D    position,
	Vector2D    positionVariance,
	Vector2D    velocity,
	Vector2D    velocityVariance,
	Vector2D    acceleration,
	Vector2D    accelerationVariance,
	Color       color,
	Color       colorVector,
	Color       colorVariance,
	//Shape       shape,
	char		*spriteFile,
	Uint32      frameWidth,
	Uint32      frameHeigh,
	Uint32      framesPerLine,
	float		size,
	float		sizeVariance,
	float		size_change,
	float		size_changeVariance,
	SDL_BlendMode blendmode,
	Uint8		particlemode,
	float		modedata_a,
	float		modedata_b,
	float		modedata_c,
	float		modedata_d,
	float		mda_var,
	float		mdb_var,
	float		mdc_var,
	float		mdd_var);

/**
* @brief Get a seeded, random number
* @param seed Seed for random
*/
float seeded_rand(Uint32 seed);

/**
* @brief Create a number of particles
* @param pe the particle source the particles originates from
* @param n The number of particles to create
*/
void particle_source_create_n_particles(ParticleSource *ps, Uint32 n);

/**
* @brief free an allocated particle data
* @param pe the particle source the particle data originates from
* @param i the index of the particle data to be freed
*/
void particle_sourceData_free(ParticleSource *ps, Uint32 i);

/**
* @brief free an allocated particle data
* @param pe the particle data to free
*/
void particle_data_free(ParticleData *pd);

/**
* @brief Reset an allocated particle data
* @param pe the particle data to free
*/
void particle_data_reset(ParticleData *pd);

/**
* @brief update all particles in the provided data
* @param pe the particle data to update
*/
void particle_data_update(ParticleData *pd);

/**
* @brief draw all particles to current rendering context
* @param pe the particle data to draw
*/
void particle_data_draw(ParticleData *pd);

/**
* @brief create a new particle data specifying all default values for the particle
* @return NULL on error or a pointer to a particle data
*/
ParticleData *particle_data_new(
	ParticleSource *ps,
	Uint32      ttl,
	Vector2D    position,
	Vector2D    velocity,
	Vector2D    acceleration,
	Color       color,
	Color       colorVector,
	float		size,
	float		size_change,
	//Shape		shape,
	SDL_BlendMode blendmode,
	Uint8		particlemode,
	Sprite		*sprite,
	Vector2D	*base,
	float		modeData_a,
	float		modeData_b,
	float		modeData_c,
	float		modeData_d);


#endif