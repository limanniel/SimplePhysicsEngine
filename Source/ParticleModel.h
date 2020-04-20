#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>

class Transform;

class ParticleModel
{
protected:
	Transform& _transform;

	DirectX::SimpleMath::Vector3 _velocity;
	DirectX::SimpleMath::Vector3 _acceleration;
	DirectX::SimpleMath::Vector3 _forceAccumulator;

	float _inverseMass;
	float _linearDamping;

protected:
	virtual void Integrate(float deltaTime);
	virtual void ResetForces();

public:
	ParticleModel(Transform& transform,
				  const DirectX::SimpleMath::Vector3& initialVelocity,
				  const DirectX::SimpleMath::Vector3& initialAcceleration);

	virtual ~ParticleModel();

	void Update(float deltaTime);

	void AddForce(const DirectX::SimpleMath::Vector3& force);

	void MoveConstVelocity(float deltaTime);
	void MoveConstAcceleration(float deltaTime);

	// Velocity
	inline void SetVelocty(const DirectX::SimpleMath::Vector3& velocity) { _velocity = velocity; }
	inline DirectX::SimpleMath::Vector3 GetVelocity() const { return _velocity; }

	// Acceleration
	inline void SetAcceleration(const DirectX::SimpleMath::Vector3& acceleration) { _acceleration = acceleration; }
	inline DirectX::SimpleMath::Vector3 GetAcceleration() const { return _acceleration; }

	// Linear Damping
	inline void SetLinearDamping(float damping) { _linearDamping = damping; }
	inline float GetLinearDamping() const { return _linearDamping; }

	// Mass
	inline void SetMass(float mass) { _inverseMass = 1.0f / mass; }
	inline float GetMass() const { return 1.0f / _inverseMass; }

	// Inverse Mass
	inline void SetInverseMass(float inverseMass) { _inverseMass = inverseMass; }
	inline float GetInverseMass() const { return _inverseMass; }

	inline bool HasFiniteMass() const { return _inverseMass > 0.0f; }
};

