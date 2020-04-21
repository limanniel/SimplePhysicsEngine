#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include <vector>

class Particle;
class Transform;
class Appearance;
struct ConstantBuffer;

class ParticleSystem
{
private:
	int _amountOfParticles;
	float _particleDuration;

	Transform*  _systemTransform;
	Appearance* _systemAppearance;

	std::vector<Particle*> _particles;

private:
	void InitParticles();
	void ResetParticle(Particle* particle);

public:
	ParticleSystem(int amountOfParticles,
				   float particleDuration,
				   Transform* systemTransform,
				   Appearance* systemAppearance);

	~ParticleSystem();

	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* context,
				ConstantBuffer cb,
				ID3D11Buffer* constantBuffer);
};

