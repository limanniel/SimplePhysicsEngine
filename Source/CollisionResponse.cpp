#include "CollisionResponse.h"

#include "RigidBody.h"
#include "rbGameObject.h"

using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;

void CollisionResponse::ResolveCollision(const CollisionManifold& manifold)
{
    float invMass1 = manifold.body[0]->GetInverseMass();
    float invMass2 = manifold.body[1]->GetInverseMass();
    float invMassSum = invMass1 + invMass2;

    // Early-out, if combined mass is infinite
    if (invMassSum == 0.0f) return;

    Vector3 relativePointOfContact1 = manifold.contacts[0] - manifold.body[0]->GetTransformRef().GetPosition();
    Vector3 relativePointOfContact2 = manifold.contacts[0] - manifold.body[1]->GetTransformRef().GetPosition();

    Matrix invInertia1 = manifold.body[0]->GetInverseInertiaTensor();
    Matrix invInertia2 = manifold.body[1]->GetInverseInertiaTensor();

    // Relative Velocity
    Vector3 relativeVelocity = manifold.body[1]->GetVelocity() + manifold.body[1]->GetAngularVelocity().Cross(relativePointOfContact2);
    relativeVelocity -= manifold.body[0]->GetVelocity() + manifold.body[0]->GetAngularVelocity().Cross(relativePointOfContact1);

    // Relative Normal
    Vector3 relativeNormal = manifold.normal;
    relativeNormal.Normalize();

    // Early-out, if objects are moving apart
    if (relativeVelocity.Dot(relativeNormal) > 0.0f) return;

    // Impulse Magnitude
    float restitution = 0.9f;
    float numerator = (-(1.0f + restitution) * relativeVelocity.Dot(relativeNormal));

    float d1 = invMassSum;

    Vector3 d2 = Vector3::Transform(relativePointOfContact1.Cross(relativeNormal), invInertia1).Cross(relativePointOfContact1);
    Vector3 d3 = Vector3::Transform(relativePointOfContact2.Cross(relativeNormal), invInertia2).Cross(relativePointOfContact2);

    float denominator = d1 + relativeNormal.Dot(d2 + d3);

    float impulseMag = (denominator == 0.0f) ? 0.0f : numerator / denominator;
    Vector3 impulse = relativeNormal * impulseMag;

    manifold.body[0]->SetVelocty(manifold.body[0]->GetVelocity() - impulse * invMass1);
    manifold.body[1]->SetVelocty(manifold.body[1]->GetVelocity() + impulse * invMass2);

    manifold.body[0]->AddForce(-impulse, relativePointOfContact1);
    manifold.body[1]->AddForce(-impulse, relativePointOfContact2);
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
