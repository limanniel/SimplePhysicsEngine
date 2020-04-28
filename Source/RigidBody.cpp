#include "RigidBody.h"
#include "Transform.h"
#include "spdlog.h"

using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;

RigidBody::RigidBody(Transform& transform,
					 const Vector3& initialVelocity,
					 const Vector3& initialAcceleration)
	: ParticleModel(transform, initialVelocity, initialAcceleration),
	  _angularDamping(0.65f),
	  _angularVelocity(Vector3::Zero),
	  _angularAcceleration(Vector3::Zero),
	  _torqueAccumulator(Vector3::Zero),
	  _orientation(Quaternion()),
	  _inverseInertiaTensor(Matrix::Identity),
	  _transformMatrix(Matrix::Identity)
{

}

RigidBody::~RigidBody()
{

}

// Verlet Velocity Integration
void RigidBody::Integrate(float deltaTime)
{
	// Skip integration, if particle has infinite mass
	if (_inverseMass == 0.0f) return;


	// Calculate new accelerations (current)
	Vector3 newAcceleration = _forceAccumulator * _inverseMass;
	Vector3 newAngularAcceleration = XMVector3Transform(_torqueAccumulator, _inverseInertiaTensor);

	// Calculate and update new velocities, from previous and currect accelerations
	_velocity += (_acceleration + newAcceleration) * (deltaTime * 0.5f);
	_angularVelocity += (_angularAcceleration + newAngularAcceleration) * (deltaTime * 0.5f);

	// Apply dampening
	_velocity *= powf(_linearDamping, deltaTime);
	_angularVelocity *= powf(_angularDamping, deltaTime);

	// Update Position
	Vector3 newPosition = _transform.GetPosition();
	newPosition += _velocity * deltaTime;
	newPosition += _acceleration * (deltaTime * deltaTime);

	_transform.SetPosition(newPosition);

	// Update Orientation
	_orientation.addScaledVector(_angularVelocity, deltaTime);

	// Update old Accelerations to current
	_acceleration = newAcceleration;
	_angularAcceleration = newAngularAcceleration;


	CalculateTransformation();

	ResetForces();
}

void RigidBody::CalculateTransformation()
{
	_orientation.normalise();
	CalculateTransformMatrixColumnMajor((XMMATRIX&)_transformMatrix, _transform.GetPosition(), _orientation);
}

void RigidBody::ResetForces()
{
	_forceAccumulator = Vector3::Zero;
	_torqueAccumulator = Vector3::Zero;
}

void RigidBody::AddForce(const DirectX::SimpleMath::Vector3& force,
						 const DirectX::SimpleMath::Vector3& point)
{
	_forceAccumulator += force;
	_torqueAccumulator += point.Cross(force);
}

void RigidBody::AddTorque(const Vector3& force)
{
	_torqueAccumulator += force;
}