#include "CollisionSystem.h"

#include "RigidBody.h"
#include "Quaternion.h"
#include "rbGameObject.h"

using DirectX::SimpleMath::Vector3;

CollisionSystem::CollisionSystem()
	: _coeffecient(1.0f)
{
}

CollisionSystem::~CollisionSystem()
{
}

void CollisionSystem::Update(const rbGameObject& obj1,
							 const rbGameObject& obj2)
{
	// Abort, both are the same object
	if (&obj1 == &obj2) return;

	// Check if both objects contains spherical bounding volume
	if (obj1.GetBoundingSphereRadius() > 0.0f && obj2.GetBoundingSphereRadius() > 0.0f)
	{
		Contact collision = GenerateContact(obj1, obj2);

		// Check if there was any contact point between two objects provided
		if (!collision.contacts.empty())
		{
			// Resolve collision and apply impulse
			ResolveLinearCollision(&collision);
		}
	}
}

CollisionSystem::Contact CollisionSystem::GenerateContact(const rbGameObject& obj1,
														  const rbGameObject& obj2) const
{
	Contact result;

	float radius = obj1.GetBoundingSphereRadius() + obj2.GetBoundingSphereRadius();
	Vector3 distance = obj2.GetTransform()->GetPosition() - obj1.GetTransform()->GetPosition();

	// Early-out if, two spheres haven't collided
	if (distance.LengthSquared() - (radius * radius) > 0.0f ||
		distance.LengthSquared() == 0.0f)
	{
		return result;
	}

	distance.Normalize();

	// Prepare Contact
	result.depth = fabsf((distance.Length() - radius) * 0.5f);
	result.body[0] = obj1.GetRigidBody();
	result.body[1] = obj2.GetRigidBody();
	result.normal = distance;

	// Calculate intersection point
	float distanceToIntersection = obj1.GetBoundingSphereRadius() - result.depth;
	Vector3 point = obj1.GetTransform()->GetPosition() + (distance * distanceToIntersection);
	result.contacts.push_back(point);

	return result;
}

void CollisionSystem::ResolveLinearCollision(Contact* collision) const
{
	float invMass1 = collision->body[0]->GetInverseMass();
	float invMass2 = collision->body[1]->GetInverseMass();
	float invMassSum = invMass1 + invMass2;

	// Early-out, when contact occured between two infinite masses
	if (invMassSum <= 0.0f) return;

	Vector3 relativeVelocity = collision->body[1]->GetVelocity() - collision->body[0]->GetVelocity();

	Vector3 relativeNormal = collision->normal;
	relativeNormal.Normalize();

	float rDn = relativeVelocity.Dot(relativeNormal);

	// Early-out, when objects are moving apart
	if (rDn > 0.0f) return;

	float numerator = (-(1.0f + _coeffecient)) * rDn;
	float impulseMagnitude = numerator / invMassSum;

	// Scale impulse strength by amount of contacts
	if (impulseMagnitude != 0.0f)
		impulseMagnitude /= static_cast<float>(collision->contacts.size());

	// Calculate and apply Impulse
	Vector3 impulse = relativeNormal * impulseMagnitude;
	collision->body[0]->SetVelocty(collision->body[0]->GetVelocity() - (impulse * invMass1));
	collision->body[1]->SetVelocty(collision->body[1]->GetVelocity() + (impulse * invMass2));
}