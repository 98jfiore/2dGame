#include <math.h>
#include "simple_logger.h"

#include "particles.h"



union Random_Union
{
	Uint32 d;
	float f;
};

float seeded_rand(Uint32 seed)
{
	union Random_Union rand_union;
	seed = seed * 134775813 + 1;
	rand_union.d = (((Uint32)(seed)& 0x7fff) << 8) | 0x40000000;
	return rand_union.f - 3.0f;
}


void particle_source_free(ParticleSource *ps)
{
	if (ps->particles != NULL)
	{
		free(ps->particles);
	}
	memset(ps, 0, sizeof(ParticleSource));
}

void particle_source_update(ParticleSource *ps)
{
	int i;
	if (ps == NULL)return;
	for (i = 0; i < ps->maxPart; i++)
	{
		if (ps->particles[i].inuse == 0 || ps->particles[i].loaded == 0) continue;
		particle_data_update(&ps->particles[i]);
		if (ps->particles[i].loaded == 0) ps->numPart--;
	}
	//Emit some number of new particles
	particle_source_create_n_particles(ps, min(ps->emissionRate, ps->maxPart - ps->numPart));
}

void particle_source_draw(ParticleSource *ps)
{
	int i;
	if (!ps)return;
	for (i = 0; i < ps->numPart; i++)
	{
		if (ps->particles[i].inuse == 0 || ps->particles[i].loaded == 0) continue;
		particle_data_draw(&ps->particles[i]);
	}
}

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
	float		modedata_d)
{
	ParticleSource *ps;

	ps = (ParticleSource *)malloc(sizeof(ParticleSource));
	if (ps == NULL) return NULL;
	ps->maxPart = maxParticles;
	ps->numPart = 0;
	ps->particles = (ParticleData *)gfc_allocate_array(sizeof(ParticleData), maxParticles);
	ps->modeFlag = particlemode;
	ps->ttl = ttl;
	ps->ttlVariance = ttlVariance;
	vector2d_copy(ps->position, position);
	vector2d_copy(ps->positionVariance, positionVariance);
	vector2d_copy(ps->velocity, velocity);
	vector2d_copy(ps->velocityVariance, velocityVariance);
	vector2d_copy(ps->acceleration, acceleration);
	vector2d_copy(ps->accelerationVariance, accelerationVariance);
	//gf2d_shape_copy(&ps->shape, shape);
	ps->sprite = gf2d_sprite_load_all(spriteFile, frameWidth, frameHeigh, framesPerLine);
	ps->size = size;
	ps->sizeVariance = sizeVariance;
	ps->size_change = size_change;
	ps->size_changeVariance = size_changeVariance;
	gfc_color_copy(ps->color, color);
	gfc_color_copy(ps->colorVector, colorVector);
	gfc_color_copy(ps->colorVariance, colorVariance);
	ps->blend_mode = blendmode;
	ps->emissionRate = emissionRate;

	ps->modedata_a = modedata_a;
	ps->modedata_b = modedata_b;
	ps->modedata_c = modedata_c;
	ps->modedata_d = modedata_d;

	//CREATE startParticles NUMBER OF PARTICLES
	particle_source_create_n_particles(ps, startParticles);
	return ps;
}

void particle_source_create_n_particles(ParticleSource *ps, Uint32 n)
{
	Uint32 RANDSEED;
	ParticleData *pd;

	int i;
	Uint32 ttl;
	float size, size_change;
	Vector2D pos, vel, acc;
	Color color;
	for (i = 0; i < ps->maxPart; ++i)
	{
		if (n <= 0) break;
		if (ps->particles[i].inuse == 1 && ps->particles[i].loaded == 0)
		{
			ps->particles[i].loaded = 1;
			//slog("%.6f, %.6f", ps->particles[i].position.x, ps->particles[i].position.y);
			n--;
		}
	}

	//life
	for (i = 0; i < n; ++i)
	{
		RANDSEED = rand();
		ttl = ps->ttl + ps->ttlVariance * seeded_rand(RANDSEED);
		pos.x = ps->position.x + ps->positionVariance.x * seeded_rand(RANDSEED);
		pos.y = ps->position.y + ps->positionVariance.y * seeded_rand(RANDSEED);
		vel.x = ps->velocity.x + ps->velocityVariance.x * seeded_rand(RANDSEED);
		vel.y = ps->velocity.y + ps->velocityVariance.y * seeded_rand(RANDSEED);
		acc.x = ps->acceleration.x + ps->accelerationVariance.x * seeded_rand(RANDSEED);
		acc.y = ps->acceleration.y + ps->accelerationVariance.y * seeded_rand(RANDSEED);
		color.r = ps->color.r + ps->colorVariance.r * seeded_rand(RANDSEED);
		color.b = ps->color.b + ps->colorVariance.b * seeded_rand(RANDSEED);
		color.g = ps->color.g + ps->colorVariance.g * seeded_rand(RANDSEED);
		color.a = ps->color.a + ps->colorVariance.a * seeded_rand(RANDSEED);
		size = ps->size + ps->sizeVariance * seeded_rand(RANDSEED);
		size_change = ps->size_change + ps->size_changeVariance * seeded_rand(RANDSEED);

		pd = particle_data_new(ps,ttl, pos, vel, acc, color, ps->colorVector, size, size_change, 
			ps->blend_mode, ps->modeFlag, ps->sprite, ps->modedata_a, ps->modedata_b, ps->modedata_c, ps->modedata_d);
		if (pd == NULL) break;
	}
}


void particle_sourceData_free(ParticleSource *ps, Uint32 i)
{
	if (i >= ps->maxPart) return;
	particle_data_free(&ps->particles[i]);
}

ParticleData *new_particle_data(ParticleSource *ps)
{
	int i;
	for (i = 0; i < ps->maxPart; i++)
	{
		if (ps->particles[i].inuse == 0)
		{
			if (ps->particles[i].loaded == 1) continue;
			ps->numPart++;
			return &ps->particles[i];
		}
		else if (ps->particles[i].loaded == 0)
		{
			ps->particles[i].loaded = 1;
			return &ps->particles[i];

		}
	}
	return NULL;
}

void particle_data_free(ParticleData *pd)
{
	if (pd == NULL) return;
	if (pd->mode != NULL)
	{
		free(pd->mode);
	}
	pd->inuse = 0;
	memset(pd, 0, sizeof(ParticleData));
}

void particle_data_reset(ParticleData *pd)
{
	Particle_modeA *ma;
	Particle_modeB *mb;
	if (pd == NULL) return;
	pd->loaded = 1;
	vector2d_copy(pd->position, pd->start_pos);
	vector2d_copy(pd->velocity, pd->velocity_initial);
	if (pd->modeFlag & PART_MODE_A)
	{
		ma = (Particle_modeA *)pd->mode;
		ma->dirX = ma->dirX_init;
		ma->dirY = ma->dirY_init;
	}
	else if (pd->modeFlag & PART_MODE_B)
	{
		mb = (Particle_modeB *)pd->mode;
		mb->angle = mb->angle_init;
		mb->radius = mb->radius_init;
	}
	pd->ttl = pd->ttl_initial;
}

void particle_data_update(ParticleData *pd)
{
	float n;
	float dt = 1.0 / 25;
	if (pd == NULL) return;
	if (pd->loaded == 0) return;
	if (pd->inuse == 0) return;
	Vector2D tmp, radial, tangential;
	Particle_modeA *ma;
	Particle_modeB *mb;

	pd->ttl--;
	if (pd->ttl <= 0)
	{
		particle_data_reset(pd);
		return;
	}

	if (pd->modeFlag & PART_MODE_A)
	{
		radial = vector2d(0, 0);
		
		// radial acceleration
		if (pd->position.x || pd->position.y)
		{
			n = pd->position.x * pd->position.x + pd->position.y * pd->position.y;
			// Already normalized.
			if (n != 1.0f)
			{
				n = sqrt(n);
				// Too close to zero.
				if (n >= 1e-5)
				{
					n = 1.0f / n;
					radial.x = pd->position.x * n;
					radial.y = pd->position.y * n;
				}
			}
		}
		vector2d_copy(tangential, radial);
		ma = (Particle_modeA *)pd->mode;
		radial.x *= ma->rad_acc;
		radial.y *= ma->rad_acc;

		// tangential acceleration
		n = tangential.x;
		tangential.x = tangential.y;
		tangential.y = n;

		tangential.x *= (-1 * ma->tan_acc);
		tangential.y *= (-1 * ma->tan_acc);

		// (gravity + radial + tangential) * dt
		tmp.x = radial.x + tangential.x + 10;
		tmp.y = radial.y + tangential.y + 10;
		tmp.x *= dt;
		tmp.y *= dt;

		ma->dirX += tmp.x;
		ma->dirY += tmp.y;
		
		tmp.x = ma->dirX * dt * 1;
		tmp.y = ma->dirY * dt * 1;
		pd->position.x += tmp.x;
		pd->position.y += tmp.y;
	}
	else if(pd->modeFlag & PART_MODE_B)
	{
		mb = (Particle_modeB *)pd->mode;
		mb->angle += mb->degreesPerSecond * dt;
		mb->radius += mb->radius_change * dt;

		pd->position.x = -cosf(mb->angle) * mb->radius;
		pd->position.y = -sinf(mb->angle) * mb->radius * 1;
	}

	pd->color.a += pd->colorVector.a * dt;
	pd->color.b += pd->colorVector.b * dt;
	pd->color.g += pd->colorVector.g * dt;
	pd->color.r += pd->colorVector.r * dt;
	pd->size += pd->size_change * dt;
	pd->size = max(0, pd->size);
}


void particle_data_draw(ParticleData *pd)
{
	Vector4D color;
	if ((!pd) || (pd->inuse == 0) || (pd->loaded == 0))return;
	SDL_SetRenderDrawBlendMode(gf2d_graphics_get_renderer(), pd->blend_mode);
	//gf2d_shape_move_to(&pd->shape, pd->position);
	//gf2d_shape_draw(pd->shape, pd->color);
	color = gfc_color_to_vector4(pd->color);
	gf2d_sprite_draw(
		pd->sprite,
		pd->position,
		NULL,
		NULL,
		NULL,
		NULL,
		&color,
		0);
	SDL_SetRenderDrawBlendMode(gf2d_graphics_get_renderer(), SDL_BLENDMODE_BLEND);
	
}


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
	float		modeData_a,
	float		modeData_b,
	float		modeData_c,
	float		modeData_d)
{
	ParticleData *p;
	Particle_modeA *ma;
	Particle_modeB *mb;

	p = new_particle_data(ps);
	if (p == NULL) return NULL;
	if (p->inuse == 1)
	{
		p->loaded = 1;
		return p;
	}
	p->inuse = 1;
	p->ttl = ttl;
	p->ttl_initial = ttl;
	vector2d_copy(p->position, position);
	vector2d_copy(p->start_pos, position);
	vector2d_copy(p->velocity, velocity);
	vector2d_copy(p->acceleration, acceleration);
	vector2d_copy(p->velocity_initial, velocity);
	//p->shape = shape;
	//gf2d_shape_copy(&p->shape, shape);
	p->sprite = sprite;
	p->size = size;
	p->size_change = size_change;
	gfc_color_copy(p->color, color);
	gfc_color_copy(p->colorVector, colorVector);
	p->blend_mode = blendmode;

	if (particlemode & PART_MODE_A)
	{
		ma = (Particle_modeA *)malloc(sizeof(Particle_modeA));
		ma->dirX = modeData_a;
		ma->dirY = modeData_b;
		ma->dirX_init = modeData_a;
		ma->dirY_init = modeData_b;
		ma->rad_acc = modeData_c;
		ma->tan_acc = modeData_d;
		p->mode = ma;
	}
	else if (particlemode & PART_MODE_B)
	{
		mb = (Particle_modeB *)malloc(sizeof(Particle_modeB));
		mb->angle = modeData_a;
		mb->degreesPerSecond = modeData_b;
		mb->radius = modeData_c;
		mb->radius_change = modeData_d;
		mb->angle_init = modeData_a;
		mb->radius_init = modeData_c;
		p->mode = mb;
	}
	p->modeFlag = particlemode;
	p->loaded = 1;
	return p;
}

/*eol@eof*/