#include "CollisionResponse.h"

#include "RigidBody.h"
#include "rbGameObject.h"

using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Matrix;

void CollisionResponse::ResolveCollision(const CollisionManifold& manifold)
{
    Vector3 relativeVelocity = manifold.body[1]->GetVelocity() - manifold.body[0]->GetVelocity();

    Vector3 relativeNormal = manifold.normal;

    // Early-out if objects are moving apart
    if (relativeVelocity.Dot(relativeNormal) > 0.0f) return;

	float restituion = 0.9f;
    float numerator = (-(1.0f + restituion)) * relativeVelocity.Dot(relativeNormal);
    float denominator = manifold.body[0]->GetInverseMass() + manifold.body[1]->GetInverseMass();

    Vector3 pt1 = manifold.contacts[0] - manifold.body[0]->GetTransformRef().GetPosition();
    Vector3 pt2 = manifold.contacts[0] - manifold.body[1]->GetTransformRef().GetPosition();

    Vector3 r1 = pt1.Cross(relativeNormal);
    r1 = Vector3::Transform(r1, manifold.body[0]->GetInverseInertiaTensor());
    r1 = r1.Cross(pt1);

	Vector3 r2 = pt2.Cross(relativeNormal);
	r2 = Vector3::Transform(r2, manifold.body[1]->GetInverseInertiaTensor());
	r2 = r2.Cross(pt2);

    denominator += relativeNormal.Dot(r1 + r2);

    float j = numerator / denominator;

    Vector3 impulse = relativeNormal * j;

    Vector3 newVelocity = manifold.body[0]->GetVelocity();
    newVelocity -= impulse * manifold.body[0]->GetInverseMass();
   manifold.body[0]->SetVelocty(newVelocity);

    newVelocity = manifold.body[1]->GetVelocity();
    newVelocity += impulse * manifold.body[1]->GetInverseMass();
    manifold.body[1]->SetVelocty(newVelocity);

    Vector3 newAngVelocity = manifold.body[0]->GetAngularVelocity();
    newAngVelocity -= XMVector3Transform(pt1.Cross(impulse), manifold.body[0]->GetInverseInertiaTensor() * 100000.0f);
    manifold.body[0]->SetAngularVelocity(newAngVelocity);

	newAngVelocity = manifold.body[1]->GetAngularVelocity();
	newAngVelocity += XMVector3Transform(pt2.Cross(impulse), manifold.body[1]->GetInverseInertiaTensor() * 100000.0f);
	manifold.body[1]->SetAngularVelocity(newAngVelocity);
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
