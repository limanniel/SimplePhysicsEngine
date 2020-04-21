#include "Particle.h"

Particle::Particle(float life,
                   Transform* transform,
                   Appearance* appearance)
    : pmGameObject(transform, appearance),
      _life(life)
{
    _particle->SetMass(1.0f);
}

Particle::~Particle()
{
    delete _transform;
}

void Particle::Update(float deltaTime)
{
    _life -= deltaTime;

    pmGameObject::Update(deltaTime);
}
