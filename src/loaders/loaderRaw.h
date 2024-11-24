#pragma once
#include "loader.h"

class LoaderRaw : public Loader {
	private :
		void loadQuilles(App* app, CameraComponent* camera) const override;
		void loadBall(App* app, CameraComponent* camera) const override;
};