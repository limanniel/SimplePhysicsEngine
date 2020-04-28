#pragma once

#include "ParticleModel.h"
#include "Quaternion.h"

class RigidBody : public ParticleModel
{
private:
	float _angularDamping;
	DirectX::SimpleMath::Vector3    _angularVelocity;
	DirectX::SimpleMath::Vector3    _angularAcceleration;
	DirectX::SimpleMath::Vector3    _torqueAccumulator;
	Quaternion	_orientation;
	DirectX::SimpleMath::Matrix     _inverseInertiaTensor;
	DirectX::SimpleMath::Matrix     _transformMatrix;

private:
	void Integrate(float deltaTime) override;
	void CalculateTransformation();
	void ResetForces() override;

public:
	RigidBody(Transform& transform,
			  const DirectX::SimpleMath::Vector3& initialVelocity,
			  const DirectX::SimpleMath::Vector3& initialAcceleration);
	~RigidBody();

	void AddForce(const DirectX::SimpleMath::Vector3& force,
				  const DirectX::SimpleMath::Vector3& point);

	void AddTorque(const DirectX::SimpleMath::Vector3& force);

	// Angular Damping
	inline void SetAngularDamping(float damping) { _angularDamping = damping; }
	inline float GetAngularDamping() const { return _angularDamping; }

	// Angular Velocity
	inline void SetAngularVelocity(const DirectX::SimpleMath::Vector3& velocity) { _angularVelocity = velocity; }
	inline DirectX::SimpleMath::Vector3 GetAngularVelocity() const { return _angularVelocity; }

	// Angular Acceleration
	inline void SetAngularAcceleration(const DirectX::SimpleMath::Vector3& acceleration) { _angularAcceleration = acceleration; }
	inline DirectX::SimpleMath::Vector3 GetAngularAcceleration() const { return _angularAcceleration; }

	// Inertia Tensor
	inline void SetInertiaTensor(const DirectX::SimpleMath::Matrix& tensor) { _inverseInertiaTensor = XMMatrixInverse(nullptr, tensor); }
	inline DirectX::SimpleMath::Matrix GetInertiaTensor() const { return XMMatrixInverse(nullptr, _inverseInertiaTensor); }

	// Inverse Inertia Tensor
	inline void SetInverseInertiaTensor(const DirectX::SimpleMath::Matrix& inverseTensor) { _inverseInertiaTensor = inverseTensor; }
	inline DirectX::SimpleMath::Matrix GetInverseInertiaTensor() const { return _inverseInertiaTensor; }

	// Get rigid bodie's transform matrix (rotation * position)
	inline DirectX::SimpleMath::Matrix GetTransformMatrix() const { return _transformMatrix; }
};