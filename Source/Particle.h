#pragma once

#include "pmGameObject.h"

class Particle : public pmGameObject
{
private:
	float _life;

public:
	Particle(float life,
			 Transform* transform,
			 Appearance* appearance);

	~Particle();

	void Update(float deltaTime) override;

	inline void SetLife(float life) { _life = life; }
	inline float GetLife() const { return _life; }
	inline bool IsExpired() const { return _life <= 0.0f; }
};

