#include "CollisionResponse.h"

#include "RigidBody.h"
#include "rbGameObject.h"

using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;

Vector3 CollisionResponse::s_hardConstraints[6] = {
    Vector3(0.0f, 1.0f, 0.0f),
    Vector3(0.0f, -1.0f, 0.0f),
    Vector3(1.0f, 0.0f, 0.0f),
    Vector3(-1.0f, 0.0f, 0.0f),
    Vector3(0.0f, 0.0f, 1.0f),
    Vector3(0.0f, 0.0f, -1.0f)
};

CollisionResponse::CollisionResponse()
	: relativeVelocity(Vector3::Zero),
	  relativeNormal(Vector3::Zero),
	  pt1(Vector3::Zero),
	  pt2(Vector3::Zero),
	  invMassSum(0.0f)
{
}

CollisionResponse::~CollisionResponse() = default;

void CollisionResponse::Update(rbGameObject* obj1, rbGameObject* obj2)
{
	// Early-out, comparing the same object
	if (obj1 == obj2) return;

	CollisionManifold manifold;

#pragma region HARD_CONSTRAINTS

	// Hard constraint - FLOOR
	if (obj1->GetTransform()->GetPosition().y < 1.0f)
	{
		manifold = CreateCollisionManifold(*obj1, HARD_CONSTRAINTS::FLOOR);
	}

	// Hard constraint - CEILING
	if (obj1->GetTransform()->GetPosition().y > 9.0f)
	{
		manifold = CreateCollisionManifold(*obj1, HARD_CONSTRAINTS::CEILING);
	}

	// Hard constraint - RIGHT WALL
	if (obj1->GetTransform()->GetPosition().x > 9.0f)
	{
		manifold = CreateCollisionManifold(*obj1, HARD_CONSTRAINTS::RIGT_WALL);
	}

	// Hard constraint - LEFT WALL
	if (obj1->GetTransform()->GetPosition().x < -9.0f)
	{
		manifold = CreateCollisionManifold(*obj1, HARD_CONSTRAINTS::LEFT_WALL);
	}

	// Hard constraint - BACK WALL
	if (obj1->GetTransform()->GetPosition().z > 12.0f)
	{
		manifold = CreateCollisionManifold(*obj1, HARD_CONSTRAINTS::BACK_WALL);
	}

	// Hard constraint - FRONT WALL
	if (obj1->GetTransform()->GetPosition().z < -2.0f)
	{
		manifold = CreateCollisionManifold(*obj1, HARD_CONSTRAINTS::FRONT_WALL);
	}

#pragma endregion HARD_CONSTRAINTS

	// Check if both objects contain, spherical bounding volume
	if (obj1->GetBoundingSphereRadius() > 0.0f &&
		obj2->GetBoundingSphereRadius() > 0.0f)
	{
		manifold = CreateCollisionManifold(*obj1, *obj2);
	}

	// Resolve collision, if there's contact point
	if (!manifold.contacts.empty())
	{
		ResolveCollision(manifold);
		ResolveInterpenetration(manifold);
	}
}


////////////////////////////////////////////////////////////////////////////////
//								[Collision Resolving]                         //
////////////////////////////////////////////////////////////////////////////////

void CollisionResponse::ResolveCollision(const CollisionManifold& manifold)
{
    relativeVelocity = CalculateRelativeVelocity(manifold);
    relativeNormal = manifold.normal;

    // Early-out if objects are moving apart
    if (relativeVelocity.Dot(relativeNormal) > 0.0f) return;

    invMassSum = CalculateInverseMassSum(manifold);
    // Early-out if inverse mass sum is infinite
    if (invMassSum == 0.0f) return;

    Vector3 impulse = CalculateImpulse(manifold);
    // Flip sign, if creating impulse for hard constraint (no second object)
    if (manifold.body[1] == nullptr)
        impulse = -impulse;

    ApplyLinearImpulse(impulse, manifold);
    ApplyAngularImpulse(impulse, manifold);
}

void CollisionResponse::ResolveInterpenetration(const CollisionManifold& manifold)
{
    float penetration = fmaxf(manifold.penetration, 0.0f);

    // Percentage of penetration that needs to be applied, to each object (distributed)
    float scalar = penetration * invMassSum;

    Vector3 scaledPenetration = manifold.normal * scalar;

    // Flip sign, if creating correction for hard constraint (no second object)
    if (manifold.body[1] == nullptr)
        scaledPenetration = -scaledPenetration;

    Vector3 newPos = manifold.body[0]->GetTransformRef().GetPosition() - scaledPenetration * manifold.body[0]->GetInverseMass();
    manifold.body[0]->GetTransformRef().SetPosition(newPos);

    if (manifold.body[1])
    {
		newPos = manifold.body[1]->GetTransformRef().GetPosition() + scaledPenetration * manifold.body[0]->GetInverseMass();
		manifold.body[1]->GetTransformRef().SetPosition(newPos);
    }
}


////////////////////////////////////////////////////////////////////////////////
//								[Calculate Functions]                         //
////////////////////////////////////////////////////////////////////////////////

Vector3 CollisionResponse::CalculateRelativeVelocity(const CollisionManifold& manifold)
{
    Vector3 rv;

	if (manifold.body[1])
		rv = manifold.body[1]->GetVelocity() - manifold.body[0]->GetVelocity();

	else
		rv = manifold.body[0]->GetVelocity();

    return rv;
}

float CollisionResponse::CalculateInverseMassSum(const CollisionManifold& manifold)
{
    float massSum = 0.0f;

    massSum = manifold.body[0]->GetInverseMass();

	if (manifold.body[1])
        massSum += manifold.body[1]->GetInverseMass();

    return massSum;
}

Vector3 CollisionResponse::CalculateImpulse(const CollisionManifold& manifold)
{
	float restituion = 0.9f;
	float numerator = (-(1.0f + restituion)) * relativeVelocity.Dot(relativeNormal);

	pt1 = manifold.contacts[0] - manifold.body[0]->GetTransformRef().GetPosition();

	Vector3 r1 = pt1.Cross(relativeNormal);
	r1 = Vector3::Transform(r1, manifold.body[0]->GetInverseInertiaTensor());
	r1 = r1.Cross(pt1);

	Vector3 product = r1;

	if (manifold.body[1])
	{
		pt2 = manifold.contacts[0] - manifold.body[1]->GetTransformRef().GetPosition();

		Vector3 r2 = pt2.Cross(relativeNormal);
		r2 = Vector3::Transform(r2, manifold.body[1]->GetInverseInertiaTensor());
		r2 = r2.Cross(pt2);

		product += r2;
	}

	float denominator = invMassSum + relativeNormal.Dot(product);

	float impulseMag = numerator / denominator;

    return Vector3(relativeNormal * impulseMag);
}


////////////////////////////////////////////////////////////////////////////////
//								[Impulse Functions]                           //
////////////////////////////////////////////////////////////////////////////////

void CollisionResponse::ApplyLinearImpulse(const Vector3& impulse,
                                           const CollisionManifold& manifold)
{
	// Apply Linear impulse to first object
	Vector3 newVelocity = manifold.body[0]->GetVelocity();
	newVelocity -= impulse * manifold.body[0]->GetInverseMass();
	manifold.body[0]->SetVelocty(newVelocity);

    if (manifold.body[1])
    {
		// Apply Linear impulse to second object
		newVelocity = manifold.body[1]->GetVelocity();
		newVelocity += impulse * manifold.body[1]->GetInverseMass();
		manifold.body[1]->SetVelocty(newVelocity);
    }
}

void CollisionResponse::ApplyAngularImpulse(const Vector3& impulse,
                                            const CollisionManifold& manifold)
{
	// Apply Angular impulse to first object
	Vector3 newAngVelocity = manifold.body[0]->GetAngularVelocity();
	newAngVelocity -= XMVector3Transform(pt1.Cross(impulse), manifold.body[0]->GetInverseInertiaTensor() * 100000.0f);
	manifold.body[0]->SetAngularVelocity(newAngVelocity);

	if (manifold.body[1])
	{
		// Apply Angular impulse to second object
		newAngVelocity = manifold.body[1]->GetAngularVelocity();
		newAngVelocity += XMVector3Transform(pt2.Cross(impulse), manifold.body[1]->GetInverseInertiaTensor() * 100000.0f);
		manifold.body[1]->SetAngularVelocity(newAngVelocity);
	}
}


////////////////////////////////////////////////////////////////////////////////
//								[Manifold Creation]                           //
////////////////////////////////////////////////////////////////////////////////

CollisionManifold CollisionResponse::CreateCollisionManifold(const rbGameObject& obj1,
                                                             const rbGameObject& obj2)
{
    CollisionManifold result;

    float radius = obj1.GetBoundingSphereRadius() + obj2.GetBoundingSphereRadius();
    Vector3 distance = obj2.GetTransform()->GetPosition() - obj1.GetTransform()->GetPosition();

    // Early-out, if sphere are not colliding
    if (distance.LengthSquared() - (radius * radius) > 0.0f)
        return result;

    distance.Normalize();

    // Fill-out manifold
    result.body[0] = obj1.GetRigidBody();
    result.body[1] = obj2.GetRigidBody();

    result.normal = distance;
    result.penetration = fabsf(distance.Length() - radius) * 0.5f;

    float distanceToIntersection = obj1.GetBoundingSphereRadius() - result.penetration;
    Vector3 contactPoint = obj1.GetTransform()->GetPosition() + (distance * distanceToIntersection);

    result.contacts.push_back(contactPoint);
    return result;
}

CollisionManifold CollisionResponse::CreateCollisionManifold(const rbGameObject& obj,
                                                             const HARD_CONSTRAINTS& constraint)
{
    CollisionManifold result;

    Vector3 position = obj.GetTransform()->GetPosition();

    Vector3 normal = s_hardConstraints[static_cast<int>(constraint)];
    normal.Normalize();

    float distance = normal.Dot(position) - obj.GetBoundingSphereRadius();

    if (distance >= 0) 
        return result;

    // Fill-out manifold
    result.body[0] = obj.GetRigidBody();
    result.body[1] = nullptr;

    result.normal = normal;
    result.penetration = -distance;

    Vector3 contactPoint = position - normal * (distance + obj.GetBoundingSphereRadius());

    result.contacts.push_back(contactPoint);
    return result;
}