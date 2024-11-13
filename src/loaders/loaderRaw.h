#pragma once
#include "loader.h"

class LoaderRaw : public Loader {
	void loadQuilles(App *app, TransformComponent transform, 
		PhysicsComponent physics, RenderComponent render) const override;
};