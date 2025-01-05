#include "loader.h"

void Loader::loadEntities(App* app) const {
    CameraComponent camera;

    // Lane
    loadLane(app, &camera);

    // Ball
    loadBall(app, &camera);

    // Pins
    loadQuilles(app, &camera);

    // Lights
    loadLight(app, &camera);

    // Camera
    loadCamera(app, &camera);
}