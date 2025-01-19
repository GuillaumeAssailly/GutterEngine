#pragma once
#include "loader.h"

class LoaderRaw : public Loader {
	private :
		void loadPins(App* app, CameraComponent* camera) const override;
		void loadBall(App* app, CameraComponent* camera) const override;
		void loadLights(App* app, CameraComponent* camera) const override;
		void loadCamera(App* app, CameraComponent* camera) const override;
		void loadLane(App* app, CameraComponent* camera)const override;

};