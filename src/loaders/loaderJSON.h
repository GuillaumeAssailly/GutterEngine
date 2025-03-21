#pragma once
#include "loader.h"
#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

class LoaderJSON : public Loader {
	private :
		std::string fileName;
		json jsonData;
		void loadTransforms() override;
		virtual void loadRenders() override;
		virtual void loadCameras() override;
		virtual void loadLights() override;
		virtual void loadEntitiesNames() override;
		virtual void loadPhysics() override;
		virtual void loadEntities();
	public :
		LoaderJSON(std::string fileName, EntityManager *entityManager);
};