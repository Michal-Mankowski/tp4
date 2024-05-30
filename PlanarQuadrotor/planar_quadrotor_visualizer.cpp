#include "planar_quadrotor_visualizer.h"

PlanarQuadrotorVisualizer::PlanarQuadrotorVisualizer(PlanarQuadrotor *quadrotor_ptr, const int& SCREEN_WIDTH, const int& SCREEN_HEIGHT,
const int& scale): quadrotor_ptr(quadrotor_ptr), x_offset(SCREEN_WIDTH / 2), y_offset(SCREEN_HEIGHT/2), scale(scale) {}
/**
 * TODO: Improve visualizetion
 * 1. Transform coordinates from quadrotor frame to image frame so the circle is in the middle of the screen
 * 2. Use more shapes to represent quadrotor (e.x. try replicate http://underactuated.mit.edu/acrobot.html#section3 or do something prettier)
 * 3. Animate proppelers (extra points)
 */
void PlanarQuadrotorVisualizer::render(std::shared_ptr<SDL_Renderer> &gRenderer) {
    Eigen::VectorXf state = this->quadrotor_ptr->GetState();
    float q_x, q_y, q_theta;
    /* x, y, theta coordinates */
    q_x = state[0] * this->scale + this->x_offset;
    q_y = state[1] * this->scale + this->y_offset;
    q_theta = state[2];

    SDL_SetRenderDrawColor(gRenderer.get(), 0xFF, 0x00, 0x00, 0xFF);
    filledCircleColor(gRenderer.get(), q_x, q_y, 30, 0xFF0000FF);
}

std::pair<int, int> PlanarQuadrotorVisualizer::GetOffset() {
    return {this->x_offset, this->y_offset};
}