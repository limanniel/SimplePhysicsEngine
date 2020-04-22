#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include <vector>

#include "rbGameObject.h"

namespace CollisionResponse
{
	struct CollisionManifold
	{
		float depth{ FLT_MAX };
		/* @ 0 index - 1st RigidBody
		   @ 1 index - 2nd RigidBody */
		RigidBody* body[2]{ nullptr, nullptr };
		DirectX::SimpleMath::Vector3 normal{ Vector3::Zero };
		std::vector< DirectX::SimpleMath::Vector3> contacts;
	};

	static CollisionManifold FindCollisionsFeatures(const rbGameObject& object1,
													const rbGameObject& object2)
	{
		CollisionManifold result;

		float radius = object1.GetBoundingSphereRadius() + object2.GetBoundingSphereRadius();
		Vector3 distance = object2.GetTransform()->GetPosition() - object1.GetTransform()->GetPosition();

		// Check if two speheres collide, if not abort (return empty)
		if (distance.LengthSquared() - (radius * radius) > 0
			|| distance.LengthSquared() == 0.0f)
		{
			return result;
		}

		distance.Normalize();

		result.body[0] = object1.GetRigidBody();
		result.body[1] = object2.GetRigidBody();
		result.normal = distance;
		result.depth = fabsf((distance.Length() - radius) * 0.5f);

		float distanceToIntersection = object1.GetBoundingSphereRadius() - result.depth;
		Vector3 point = object1.GetTransform()->GetPosition() + distance * distanceToIntersection;
		result.contacts.push_back(point);

		return result;
	}

	static void ResolveCollisionManifold(CollisionManifold* collision)
	{
		float invMass1 = collision->body[0]->GetInverseMass();
		float invMass2 = collision->body[1]->GetInverseMass();
		float invMassSum = invMass1 + invMass2;

		// Abort, if detected infinite mass
		if (invMassSum <= 0.0f) return;

		Vector3 relativeVelocity = collision->body[1]->GetVelocity() - collision->body[0]->GetVelocity();

		Vector3 relativeNormal = collision->normal;
		relativeNormal.Normalize();

		// Objects moving apart, abort
		if (relativeVelocity.Dot(relativeNormal) > 0.0f) return;

		float coef = 1.0f;
		float numerator = (-(1.0f + coef) * relativeVelocity.Dot(relativeNormal));

		float impulseMagnitude = numerator / invMassSum;
		if (!collision->contacts.empty() && impulseMagnitude != 0.0f)
		{
			impulseMagnitude /= (float)collision->contacts.size();
		}

		// Calculate and apply impulse
		Vector3 impulse = relativeNormal * impulseMagnitude;
		collision->body[0]->SetVelocty(collision->body[0]->GetVelocity() - impulse * invMass1);
		collision->body[1]->SetVelocty(collision->body[1]->GetVelocity() + impulse * invMass2);
	}
}