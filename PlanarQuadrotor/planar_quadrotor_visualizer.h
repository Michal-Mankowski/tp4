#pragma once

#include <memory>
#include <utility>

#include <SDL.h>
#include <SDL2_gfx/SDL2_gfxPrimitives.h>

#include "planar_quadrotor.h"

class PlanarQuadrotorVisualizer {
private:
    PlanarQuadrotor *quadrotor_ptr;
    int x_offset;
    int y_offset;
    int scale;
public:
    PlanarQuadrotorVisualizer(PlanarQuadrotor *quadrotor_ptr, const int& SCREEN_WIDTH, const int& SCREEN_HEIGHT,
     const int& scale);
    void render(std::shared_ptr<SDL_Renderer> &gRenderer);
    std::pair<int, int> GetOffset();
};