#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include <vector>

class RigidBody;
class Quaternion;
class rbGameObject;

class CollisionSystem
{
	struct Contact
	{
		float depth{ FLT_MAX };
		RigidBody* body[2]{ nullptr, nullptr };
		DirectX::SimpleMath::Vector3 normal{ DirectX::SimpleMath::Vector3::Zero };
		std::vector<DirectX::SimpleMath::Vector3> contacts;
	};

private:
	float _coeffecient;

private:
	Contact GenerateContact(const rbGameObject& obj1,
							const rbGameObject& obj2) const;

	void ResolveLinearCollision(Contact* collision) const;

public:
	CollisionSystem();
	~CollisionSystem();

	void Update(const rbGameObject& obj1,
				const rbGameObject& obj2);
};

