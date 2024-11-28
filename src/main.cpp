#include "config.h"
#include "controllers/app.h"

#include "components/camera_component.h"
#include "components/physics_component.h"
#include "components/render_component.h"
#include "components/transform_component.h"
#include "components/light_component.h"

#include "loaders/loader.h"
#include "loaders/loaderRaw.h"
#include "loaders/loaderJSON.h"

int main() {

		App* app = new App();
		Loader* loader = new LoaderRaw();

		app->set_up_opengl();
		app->make_systems();
		
		app->loadModelsAndTextures();
		app->loadEntities();

		app->run();

		delete app;
		return 0;
}