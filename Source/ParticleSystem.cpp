#include "ParticleSystem.h"

#include "Particle.h"
#include "Commons.h"

using namespace DirectX;
using DirectX::SimpleMath::Vector3;

ParticleSystem::ParticleSystem(int amountOfParticles,
							   float particleDuration,
							   Transform* systemTransform,
							   Appearance* systemAppearance)
	: _amountOfParticles(amountOfParticles),
	  _particleDuration(particleDuration),
	  _systemTransform(systemTransform),
	  _systemAppearance(systemAppearance)
{
	InitParticles();
}

ParticleSystem::~ParticleSystem()
{
	delete _systemTransform;
	_systemTransform = nullptr;

	delete _systemAppearance;
	_systemAppearance = nullptr;
}

void ParticleSystem::InitParticles()
{
	_particles.reserve(_amountOfParticles);

	for (int i = 0; i < _amountOfParticles; ++i)
	{
		Particle* particle = new Particle(_particleDuration,
										  new Transform(*_systemTransform),
										  _systemAppearance);

		_particles.push_back(particle);
	}
}

void ParticleSystem::ResetParticle(Particle* particle)
{
	particle->SetLife(_particleDuration);
	particle->GetParticleModel()->ResetForces();
	particle->GetParticleModel()->SetAcceleration(Vector3::Zero);
	particle->GetParticleModel()->SetVelocty(Vector3::Zero);
	particle->GetTransform()->SetPosition(_systemTransform->GetPosition());
}

void ParticleSystem::Update(float deltaTime)
{
	for (auto particle : _particles)
	{
		if (particle->IsExpired())
		{
			ResetParticle(particle);
			continue;
		}

		particle->GetParticleModel()->AddForce(Vector3(0.0f, 2.0f, 0.0f));
		particle->Update(deltaTime);
	}
}

void ParticleSystem::Render(ID3D11DeviceContext* context,
							ConstantBuffer cb,
							ID3D11Buffer* constantBuffer)
{
	// Set material
	Material mat = _systemAppearance->GetMaterial();
	cb.surface.AmbientMtrl = mat.ambient;
	cb.surface.DiffuseMtrl = mat.diffuse;
	cb.surface.SpecularMtrl = mat.specular;

	// Set texture
	if (_systemAppearance->HasTexture())
		cb.HasTexture = 1.0f;
	else
		cb.HasTexture = 0.0f;

	// Render Particles
	for (auto particle : _particles)
	{
		cb.World = XMMatrixTranspose(particle->GetWorldMatrix());

		context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

		particle->Render(context);
	}
}
