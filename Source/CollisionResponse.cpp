#include "CollisionResponse.h"

#include "RigidBody.h"
#include "rbGameObject.h"

using DirectX::SimpleMath::Vector3;

void CollisionResponse::ResolveCollision(const CollisionManifold& manifold)
{
    // Linear Velocity
    float invMass1 = manifold.body[0]->GetInverseMass();
    float invMass2 = manifold.body[1]->GetInverseMass();
    float invMassSum = invMass1 + invMass2;

    // Early-out, if the collision objects have infinite mass
    if (invMassSum == 0.0f) return;

    Vector3 relativeVelocity = manifold.body[1]->GetVelocity() - manifold.body[0]->GetVelocity();
    Vector3 relativeNormal = manifold.normal;
    relativeNormal.Normalize();

    // Early-out, if the collision objects are moving apart
    if (relativeVelocity.Dot(relativeNormal) > 0.0f) return;

    float coeffecient = 0.0f;
    float numerator = (-(1.0f + coeffecient) * relativeVelocity.Dot(relativeNormal));

    float impulseMag = numerator / invMassSum;

    Vector3 linearImpulse = relativeNormal * impulseMag;
    
    Vector3 newVelocity = manifold.body[0]->GetVelocity() - (linearImpulse * invMass1);
    manifold.body[0]->SetVelocty(newVelocity);

    newVelocity = manifold.body[1]->GetVelocity() + (linearImpulse * invMass2);
    manifold.body[1]->SetVelocty(newVelocity);
}

CollisionManifold CollisionResponse::CreateCollisionManifold(const rbGameObject& obj1,
                                                             const rbGameObject& obj2)
{
    CollisionManifold result;

    float radius = obj1.GetBoundingSphereRadius() + obj2.GetBoundingSphereRadius();
    Vector3 distance = obj2.GetTransform()->GetPosition() - obj1.GetTransform()->GetPosition();

    // Early-out, if sphere are not colliding
    if (distance.LengthSquared() - (radius * radius) >= 0.0f)
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

void CollisionResponse::Update(rbGameObject* obj1, rbGameObject* obj2)
{
    // Early-out, comparing the same object
    if (obj1 == obj2) return;

    // Check if both objects contain, spherical bounding volume
    if (obj1->GetBoundingSphereRadius() > 0.0f &&
        obj2->GetBoundingSphereRadius() > 0.0f)
    {
        CollisionManifold manifold = CreateCollisionManifold(*obj1, *obj2);

        // Resolve collision, if there's contact point
        if (!manifold.contacts.empty())
            ResolveCollision(manifold);
    }
}
