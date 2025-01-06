#pragma once
#include "loader.h"

class LoaderRaw : public Loader {
	private :
		void loadQuilles(App* app, CameraComponent* camera) const override;
		void loadBall(App* app, CameraComponent* camera) const override;
		void loadLight(App* app, CameraComponent* camera) const override;
		void loadCamera(App* app, CameraComponent* camera) const override;
		void loadLane(App* app, CameraComponent* camera)const override;

};