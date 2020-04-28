#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include <vector>

#include "ForceRegistry.h"

class Particle;
class Transform;
class Appearance;
struct ConstantBuffer;

class ParticleSystem
{
private:
	bool  _active;
	int   _amountOfParticles;
	float _particleDuration;

	Transform*  _systemTransform;
	Appearance* _systemAppearance;

	DirectX::XMFLOAT4 _diffuseColour;
	float _minParticleDurationOffset;
	float _maxParticleDurationOffset;
	DirectX::SimpleMath::Vector3 _minPosOffset;
	DirectX::SimpleMath::Vector3 _maxPosOffset;

	ForceRegistry*         _forceRegister;

	GravityGenerator*  _gravGen;
	DragGenerator*     _dragGen;
	BuoyancyGenerator* _buoyGen;

	std::vector<Particle*> _particles;

private:
	void InitParticles();
	void RegisterForceGenerators();
	void AddParticle();

	void ResetParticle(Particle* particle);
	void ResetParticles();

	void RegisterParticleWithForceGen(Particle* particle);
	void UnregisterParticleFromForceGen(Particle* particle);

	void DeleteParticles();
	void ResizeParticlePool();

	void ParticleImGUI();
	void UtilsImGUI();
	void PositionImGUI();
	void ForceGeneratorsImGUI();

	void RenderImGUI();
	void ChangeParticlesColour();

	static float GenerateRandomFloat(float min, float max);
	static DirectX::SimpleMath::Vector3 GenerateRandomVec3(const DirectX::SimpleMath::Vector3& min,
														   const DirectX::SimpleMath::Vector3& max);

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

