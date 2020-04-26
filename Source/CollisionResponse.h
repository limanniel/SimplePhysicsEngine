#pragma once

#include <vector>
#include <d3d11_1.h>
#include <SimpleMath.h>

class RigidBody;
class rbGameObject;

struct CollisionManifold
{
	/* @ body[0] - Rigid Body of the first object
	   @ body[1] - Rigid Body of the second object */
	RigidBody* body[2]{ nullptr, nullptr };

	float penetration{ FLT_MAX };
	DirectX::SimpleMath::Vector3 normal{ DirectX::SimpleMath::Vector3::Zero };

	std::vector<DirectX::SimpleMath::Vector3> contacts;
};

enum class HARD_CONSTRAINTS
{
	FLOOR = 0,
	CEILING,
	LEFT_WALL,
	RIGT_WALL,
	FRONT_WALL,
	BACK_WALL
};

class CollisionResponse
{
private:
	static DirectX::SimpleMath::Vector3 s_hardConstraints[6];

private:
	void ResolveCollision(const CollisionManifold& manifold);
	void ResolveInterpenetration(const CollisionManifold& manifold);
	CollisionManifold CreateCollisionManifold(const rbGameObject& obj1,
											  const rbGameObject& obj2);

	CollisionManifold CreateCollisionManifold(const rbGameObject& obj,
											  const HARD_CONSTRAINTS& constraint);

public:
	void Update(rbGameObject* obj1, rbGameObject* obj2);
};