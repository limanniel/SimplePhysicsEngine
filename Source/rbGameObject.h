#pragma once

#include "GameObject.h"

class rbGameObject : public GameObject
{
private:

public:
	rbGameObject();
	~rbGameObject();

	void Update(double deltaTime) override;
};

