#pragma once
#include "loader.h"

class LoaderRaw : public Loader {
public :
	void loadQuilles(App *app, CameraComponent *camera) const override;
	void loadEntities(App* app) const override;
};