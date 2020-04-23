#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include <vector>

#include "Quaternion.h"
#include "rbGameObject.h"

using DirectX::SimpleMath::Matrix;

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

	static Vector3 GetPositionFromMatrix(DirectX::SimpleMath::Matrix* transMat)
	{
		Vector3 vec = Vector3::Zero;
		vec.x = transMat->_31;
		vec.y = transMat->_32;
		vec.z = transMat->_33;

		return vec;
	}

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

		Vector3 r1 = collision->contacts[0] - GetPositionFromMatrix(&collision->body[0]->GetTransformMatrix());
		Vector3 r2 = collision->contacts[0] - GetPositionFromMatrix(&collision->body[1]->GetTransformMatrix());

		Matrix i1 = collision->body[0]->GetInverseInertiaTensor();
		Matrix i2 = collision->body[1]->GetInverseInertiaTensor();

		Vector3 relativeVelocity = (collision->body[1]->GetVelocity() + collision->body[1]->GetAngularVelocity().Cross(r2)) - (collision->body[0]->GetVelocity() + collision->body[0]->GetAngularVelocity().Cross(r1));

		Vector3 relativeNormal = collision->normal;
		relativeNormal.Normalize();

		// Objects moving apart, abort
		if (relativeVelocity.Dot(relativeNormal) > 0.0f) return;

		float coef = 1.0f;
		float numerator = (-(1.0f + coef) * relativeVelocity.Dot(relativeNormal));

		float d1 = invMassSum;

		Vector3 d2 = static_cast<Vector3>((XMVector3Transform(r1.Cross(relativeNormal), i1))).Cross(r1);
		Vector3 d3 = static_cast<Vector3>((XMVector3Transform(r2.Cross(relativeNormal), i2))).Cross(r2);

		float denominator = d1 + relativeNormal.Dot(d2 + d3);
		float j = (denominator == 0.0f) ? 0.0f : numerator / denominator;

		if (!collision->contacts.empty() && j != 0.0f)
		{
			j /= (float)collision->contacts.size();
		}

		Vector3 impulse = relativeNormal * j;
		collision->body[0]->SetVelocty(collision->body[0]->GetVelocity() - impulse * invMass1);
		collision->body[0]->SetAngularVelocity(collision->body[0]->GetAngularVelocity() - (Vector3)XMVector3Transform(r1.Cross(impulse), i1));

		collision->body[1]->SetVelocty(collision->body[1]->GetVelocity() + impulse * invMass2);
		collision->body[1]->SetAngularVelocity(collision->body[1]->GetAngularVelocity() - (Vector3)XMVector3Transform(r2.Cross(impulse), i2));
	}
}