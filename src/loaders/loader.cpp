#include "loader.h"

void Loader::loadEntities(App* app) const {
    CameraComponent camera;

    // Lane
    loadLane(app, &camera);

    // Ball
    loadBall(app, &camera);

    // Pins
    loadPins(app, &camera);

    // Lights
    loadLights(app, &camera);

    // Camera
    loadCamera(app, &camera);
}