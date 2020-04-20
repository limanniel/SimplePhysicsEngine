#include "RigidBody.h"
#include "Transform.h"

using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;

RigidBody::RigidBody(Transform& transform,
					 const Vector3& initialVelocity,
					 const Vector3& initialAcceleration)
	: ParticleModel(transform, initialVelocity, initialAcceleration),
	  _angularDamping(0.99f),
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

void RigidBody::Integrate(float deltaTime)
{
	// Skip integration, particle has infinite mass
	if (_inverseMass <= 0.0f) return;

	// Calculate Acceleration
	_acceleration = _forceAccumulator * _inverseMass;
	_angularAcceleration = XMVector3Transform(_torqueAccumulator, _inverseInertiaTensor);

	// Calculate Velocity
	_velocity += _acceleration * deltaTime;
	_angularVelocity += _angularAcceleration * deltaTime;

	// Apply damping
	_velocity *= powf(_linearDamping, deltaTime);
	_angularVelocity *= powf(_angularDamping, deltaTime);

	// Update Position & Orientation
	_transform.SetPosition(_transform.GetPosition() + (_velocity * deltaTime));
	_orientation.addScaledVector(_angularVelocity, deltaTime);

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
	//_forceAccumulator += force;
	_torqueAccumulator += point.Cross(force);
}

void RigidBody::AddTorque(const Vector3& force)
{
	_torqueAccumulator += force;
}