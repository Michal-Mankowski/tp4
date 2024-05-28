#pragma once

#include <memory>

#include <SDL.h>
#include <SDL2_gfx/SDL2_gfxPrimitives.h>

#include "planar_quadrotor.h"

class PlanarQuadrotorVisualizer {
private:
    PlanarQuadrotor *quadrotor_ptr;
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
public:
    PlanarQuadrotorVisualizer(PlanarQuadrotor *quadrotor_ptr, const int& SCREEN_WIDTH, const int& SCREEN_HEIGHT);
    void render(std::shared_ptr<SDL_Renderer> &gRenderer);
};