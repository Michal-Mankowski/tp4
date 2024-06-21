#include "planar_quadrotor_visualizer.h"

namespace {
    constexpr float bodyWidth = 80.0f;
    constexpr float bodyHeight = 20.0f;
    constexpr float armHeight = 15.0f;
    constexpr float armWidth = 3.0f;
    constexpr float prop_r = 10.0f;
}

PlanarQuadrotorVisualizer::PlanarQuadrotorVisualizer(PlanarQuadrotor *quadrotor_ptr, const int& SCREEN_WIDTH, const int& SCREEN_HEIGHT,
const double& scale): quadrotor_ptr(quadrotor_ptr), xOffset(SCREEN_WIDTH / 2), yOffset(SCREEN_HEIGHT/2), scale(scale) {}
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
    q_x = state[0] * this->scale + this->xOffset;
    q_y = state[1] * this->scale + this->yOffset;
    q_theta = state[2];
    SDL_Texture* quadrotorTexture = SDL_CreateTexture(gRenderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bodyWidth, bodyHeight);
    if (!quadrotorTexture) {
        return;
    }
    SDL_Rect body;
    body.x = q_x - bodyWidth / 2;
    body.y = q_y - bodyHeight / 2;
    body.w = bodyWidth;
    body.h = bodyHeight;
    SDL_Rect leftArm;
    leftArm.x = q_x - bodyWidth / 2;
    leftArm.y = -armHeight + q_y - bodyHeight / 2;
    leftArm.w = armWidth;
    leftArm.h = armHeight + bodyHeight/2;
    SDL_Rect rightArm;
    rightArm.x = -armWidth + q_x + bodyWidth / 2;
    rightArm.y = -armHeight + q_y - bodyHeight / 2;
    rightArm.w = armWidth;
    rightArm.h = armHeight + bodyHeight/2;
    SDL_Point rotationCenter = {static_cast<int>(q_x), static_cast<int>(q_y)};
    SDL_RenderCopyEx(gRenderer.get(), quadrotorTexture, nullptr, &body, q_theta * this->scale/100, &rotationCenter, SDL_FLIP_NONE);
    SDL_RenderCopyEx(gRenderer.get(), quadrotorTexture, nullptr, &leftArm, q_theta * this->scale/100, &rotationCenter, SDL_FLIP_NONE);
    SDL_RenderCopyEx(gRenderer.get(), quadrotorTexture, nullptr, &rightArm, q_theta * this->scale/100, &rotationCenter, SDL_FLIP_NONE);
    if (target[0] != -1 && target[1] != -1) {
        SDL_SetRenderDrawColor(gRenderer.get(), 0xFF, 0x00, 0x00, 0xFF);
        filledCircleColor(gRenderer.get(), this->target[0], this->target[1], 3, 0xFF0000FF);
    }
    if (prop_change == 0) {
        filledEllipseRGBA(gRenderer.get(), leftArm.x, leftArm.y, prop_r * prop_change, 1, 0x00, 0x00, 0x00, 0xFF);
        filledEllipseRGBA(gRenderer.get(), rightArm.x, rightArm.y, prop_r * prop_change, 1, 0x00, 0x00, 0x00, 0xFF);
    } else if (prop_change == 1){
        filledEllipseRGBA(gRenderer.get(), leftArm.x, leftArm.y, prop_r * prop_change, 1, 0x00, 0x00, 0x00, 0xFF);
        filledEllipseRGBA(gRenderer.get(), rightArm.x, rightArm.y, prop_r * prop_change, 1, 0x00, 0x00, 0x00, 0xFF);
    } else {
        filledEllipseRGBA(gRenderer.get(), leftArm.x, leftArm.y, prop_r * prop_change, 1, 0x00, 0x00, 0x00, 0xFF);
        filledEllipseRGBA(gRenderer.get(), rightArm.x, rightArm.y, prop_r * prop_change, 1, 0x00, 0x00, 0x00, 0xFF);
        prop_change = 0;
    }
    prop_change = !prop_change;
    SDL_DestroyTexture(quadrotorTexture);
}

std::pair<double, double> PlanarQuadrotorVisualizer::GetOffset() {
    return {this->xOffset, this->yOffset};
}
