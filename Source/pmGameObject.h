#pragma once

#include "GameObject.h"

class pmGameObject : public GameObject
{
private:

public:
	pmGameObject(const DirectX::SimpleMath::Vector3& position,
				 const DirectX::SimpleMath::Vector3& rotation,
				 const DirectX::SimpleMath::Vector3& scale,
				 const Geometry& geometry,
				 const Material& material);
	~pmGameObject();

	void Update(double deltaTime) override;
};

