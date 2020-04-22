#include "ParticleSystem.h"

#include "imgui.h"
#include "Particle.h"
#include "Commons.h"

#include <random>

using namespace DirectX;
using DirectX::SimpleMath::Vector3;

ParticleSystem::ParticleSystem(int amountOfParticles,
							   float particleDuration,
							   Transform* systemTransform,
							   Appearance* systemAppearance)
	: _active(true),
	  _amountOfParticles(amountOfParticles),
	  _particleDuration(particleDuration),
	  _systemTransform(systemTransform),
	  _systemAppearance(systemAppearance),
	  _diffuseColour(_systemAppearance->GetMaterial().diffuse),
	  _minParticleDurationOffset(0.0f),
	  _maxParticleDurationOffset(0.0f),
	  _minPosOffset(Vector3::Zero),
	  _maxPosOffset(Vector3::Zero)
{
	InitParticles();
}

ParticleSystem::~ParticleSystem()
{
	DeleteParticles();

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
		AddParticle();
	}
}

void ParticleSystem::AddParticle()
{
	Particle* particle = new Particle(_particleDuration,
									  new Transform(*_systemTransform),
									  new Appearance(*_systemAppearance));

	particle->GetAppearance()->SetDiffuseColour(_diffuseColour);
	_particles.push_back(particle);
}

void ParticleSystem::ResetParticle(Particle* particle)
{
	particle->SetLife(_particleDuration + GenerateRandomFloat(_minParticleDurationOffset, _maxParticleDurationOffset));
	particle->GetParticleModel()->ResetForces();
	particle->GetParticleModel()->SetAcceleration(Vector3::Zero);
	particle->GetParticleModel()->SetVelocty(Vector3::Zero);
	particle->GetTransform()->SetPosition(_systemTransform->GetPosition() + GenerateRandomVec3(_minPosOffset, _maxPosOffset));
}

void ParticleSystem::ResetParticles()
{
	for (auto particle : _particles)
	{
		ResetParticle(particle);
	}
}

void ParticleSystem::DeleteParticles()
{
	for (auto particle : _particles)
	{
		delete particle;
	}
	_particles.clear();
}

void ParticleSystem::ResizeParticlePool()
{
	int difference = _amountOfParticles - static_cast<int>(_particles.size());

	// Too many particles, delete the overflow
	if (difference < 0)
	{
		for (auto it = _particles.end() - 1; it != _particles.end() - abs(difference - 1); --it)
		{
			delete *it;
		}

		_particles.erase(_particles.end() - abs(difference), _particles.end());
	}

	// Too little particles, create more
	else if (difference > 0)
	{
		_particles.reserve(_particles.size() + difference);

		for (; difference > 0; --difference)
		{
			AddParticle();
		}
	}

	ResetParticles();
}

void ParticleSystem::RenderImGUI()
{
	ImGui::Begin("Particle System");

	// Utils
	ImGui::Checkbox("Active", &_active);
	ImGui::SameLine(160.0f);
	if (ImGui::Button("RESET"))
	{
		ResetParticles();
	}

	if (ImGui::InputInt("Amount", &_amountOfParticles))
	{
		if (_amountOfParticles > 0)
		{
			ResizeParticlePool();
		}
		else
		{
			_amountOfParticles = 1;
		}
	}

	if (ImGui::CollapsingHeader("Particle"))
	{
		// Colour
		if (ImGui::ColorEdit3("Colour", &_diffuseColour.x))
		{
			ChangeParticlesColour();
		}

		// Duration
		if (ImGui::InputFloat("Duration", &_particleDuration))
		{
			if (_particleDuration <= 0.0f)
				_particleDuration = 0.1f;
		}

		ImGui::SliderFloat("Min Duration Offset", &_minParticleDurationOffset, 0.0f, _maxParticleDurationOffset);
		ImGui::SliderFloat("Max Duration Offset", &_maxParticleDurationOffset, _minParticleDurationOffset, 50.0f);
	}

	// Position
	if (ImGui::CollapsingHeader("Position"))
	{
		XMFLOAT3 newPos = _systemTransform->GetPosition();
		if (ImGui::DragFloat3("Spawn Pos", &newPos.x, 0.25f, -5.0f, 5.0f))
		{
			_systemTransform->SetPosition(newPos);
		}

		ImGui::SliderFloat3("Min Pos Offset", &_minPosOffset.x, -5.0f, _maxPosOffset.x);
		ImGui::SliderFloat3("Max Pos Offset", &_maxPosOffset.x, -_minPosOffset.x, 5.0f);
	}

	ImGui::End();
}

void ParticleSystem::ChangeParticlesColour()
{
	for (auto particle : _particles)
	{
		particle->GetAppearance()->SetDiffuseColour(_diffuseColour);
	}
}

void ParticleSystem::Update(float deltaTime)
{
	if (_active)
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
}

void ParticleSystem::Render(ID3D11DeviceContext* context,
							ConstantBuffer cb,
							ID3D11Buffer* constantBuffer)
{
	if (_active)
	{
		// Set material
		Material mat = _systemAppearance->GetMaterial();
		cb.surface.AmbientMtrl = mat.ambient;
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
			cb.surface.DiffuseMtrl = particle->GetAppearance()->GetMaterial().diffuse;

			context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

			particle->Render(context);
		}
	}

	RenderImGUI();
}

#pragma region HELPERS
float ParticleSystem::GenerateRandomFloat(float min, float max)
{
	// Abort, boundries not set
	if (min == max) return min;

	static std::random_device seed;
	static std::mt19937 rngGen(seed());
	std::uniform_real_distribution<float> randFloat(min, max);

	return randFloat(rngGen);
}

Vector3 ParticleSystem::GenerateRandomVec3(const Vector3& min,
										   const Vector3& max)
{
	// Abort, boundries not set
	if (min == max) return min;

	static std::random_device seed;
	static std::mt19937 rngGen(seed());
	std::uniform_real_distribution<float> randFloatX(min.x, max.x);
	std::uniform_real_distribution<float> randFloatY(min.y, max.y);
	std::uniform_real_distribution<float> randFloatZ(min.z, max.z);

	return Vector3(randFloatX(rngGen), randFloatY(rngGen), randFloatZ(rngGen));
}
#pragma endregion HELPERS