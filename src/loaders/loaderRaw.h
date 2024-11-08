#pragma once
#include "loader.h"

class LoaderRaw : public Loader {
	void load(glm::vec3* vectors) const override;
};