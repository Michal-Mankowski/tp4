#pragma once

#include <memory>
#include <utility>

#include <SDL.h>
#include <SDL2_gfx/SDL2_gfxPrimitives.h>

#include "planar_quadrotor.h"

class PlanarQuadrotorVisualizer {
private:
    PlanarQuadrotor *quadrotor_ptr;
    int xOffset;
    int yOffset;
    double scale;
    int prop_change = 0;
public:
    int target[2] = {-1};
    PlanarQuadrotorVisualizer(PlanarQuadrotor *quadrotor_ptr, const int& SCREEN_WIDTH, const int& SCREEN_HEIGHT,
     const double& scale);
    void render(std::shared_ptr<SDL_Renderer> &gRenderer);
    void DrawPtr(const int& x, const int& y);
    std::pair<double, double> GetOffset();
};