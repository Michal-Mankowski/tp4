#include "planar_quadrotor_visualizer.h"

PlanarQuadrotorVisualizer::PlanarQuadrotorVisualizer(PlanarQuadrotor *quadrotor_ptr, const int& SCREEN_WIDTH, const int& SCREEN_HEIGHT,
const double& scale): quadrotor_ptr(quadrotor_ptr), x_offset(SCREEN_WIDTH / 2), y_offset(SCREEN_HEIGHT/2), scale(scale) {}
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
    const float body_width = 80.0f;
    const float body_height = 20.0f;
    SDL_Texture* quadrotorTexture = SDL_CreateTexture(gRenderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, body_width, body_height);
    SDL_Rect body;
    body.x = static_cast<int>(q_x - body_width / 2);
    body.y = static_cast<int>(q_y - body_height / 2);
    body.w = static_cast<int>(body_width);
    body.h = static_cast<int>(body_height);
    SDL_Point rotationCenter = {static_cast<int>(q_x), static_cast<int>(q_y)};
    SDL_RenderCopyEx(gRenderer.get(), quadrotorTexture, nullptr, &body, q_theta * this->scale/100, &rotationCenter, SDL_FLIP_NONE);
}

std::pair<double, double> PlanarQuadrotorVisualizer::GetOffset() {
    return {this->x_offset, this->y_offset};
}