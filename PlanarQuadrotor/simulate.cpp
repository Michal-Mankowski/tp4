/**
 * SDL window creation adapted from https://github.com/isJuhn/DoublePendulum
*/
#include "simulate.h"

Eigen::MatrixXf LQR(PlanarQuadrotor &quadrotor, float dt) {
    /* Calculate LQR gain matrix */
    Eigen::MatrixXf Eye = Eigen::MatrixXf::Identity(6, 6);
    Eigen::MatrixXf A = Eigen::MatrixXf::Zero(6, 6);
    Eigen::MatrixXf A_discrete = Eigen::MatrixXf::Zero(6, 6);
    Eigen::MatrixXf B(6, 2);
    Eigen::MatrixXf B_discrete(6, 2);
    Eigen::MatrixXf Q = Eigen::MatrixXf::Identity(6, 6);
    Eigen::MatrixXf R = Eigen::MatrixXf::Identity(2, 2);
    Eigen::MatrixXf K = Eigen::MatrixXf::Zero(6, 6);
    Eigen::Vector2f input = quadrotor.GravityCompInput();

    Q.diagonal() << 10, 10, 10, 1, 10, 0.25 / 2 / M_PI;
    R.row(0) << 0.1, 0.05;
    R.row(1) << 0.05, 0.1;

    std::tie(A, B) = quadrotor.Linearize();
    A_discrete = Eye + dt * A;
    B_discrete = dt * B;
    
    return LQR(A_discrete, B_discrete, Q, R);
}

void control(PlanarQuadrotor &quadrotor, const Eigen::MatrixXf &K) {
    Eigen::Vector2f input = quadrotor.GravityCompInput();
    quadrotor.SetInput(input - K * quadrotor.GetControlState());
}

namespace {
    constexpr int SCREEN_WIDTH = 1280;
    constexpr int SCREEN_HEIGHT = 720;
    constexpr double scale = 10000000;
}
namespace mp = matplot;
int main(int argc, char* args[])
{
    std::shared_ptr<SDL_Window> gWindow = nullptr;
    std::shared_ptr<SDL_Renderer> gRenderer = nullptr;
    /**
     * TODO: Extend simulation
     * 1. Set goal state of the mouse when clicking left mouse button (transform the coordinates to the quadrotor world! see visualizer TODO list)
     *    [x, y, 0, 0, 0, 0]
     * 2. Update PlanarQuadrotor from simulation when goal is changed
    */
    Eigen::VectorXf initial_state = Eigen::VectorXf::Zero(6);
    PlanarQuadrotor quadrotor(initial_state);
    std::shared_ptr<PlanarQuadrotor> quadrotor_ptr = std::make_shared<PlanarQuadrotor>(quadrotor);
    PlanarQuadrotorVisualizer quadrotor_visualizer(&quadrotor, SCREEN_WIDTH, SCREEN_HEIGHT, scale);
    /**
     * Goal pose for the quadrotor
     * [x, y, theta, x_dot, y_dot, theta_dot]
     * For implemented LQR controller, it has to be [x, y, 0, 0, 0, 0]
    */
    Eigen::VectorXf goal_state = Eigen::VectorXf::Zero(6);
    goal_state << 0, 0 , 0, 0, 0, 0;
    quadrotor.SetGoal(goal_state);
    /* Timestep for the simulation */
    const float dt = 0.001;
    Eigen::MatrixXf K = LQR(quadrotor, dt);
    Eigen::Vector2f input = Eigen::Vector2f::Zero(2);

    /**
     * TODO: Plot x, y, theta over time
     * 1. Update x, y, theta history vectors to store trajectory of the quadrotor
     * 2. Plot trajectory using matplot++ when key 'p' is clicked
    */
    std::vector<double> x_history;
    std::vector<double> y_history;
    std::vector<double> theta_history;

    if (init(gWindow, gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT) >= 0)
    {
        SDL_Event e;
        bool quit = false;
        float delay;
        int x, y;
        Eigen::VectorXf state = Eigen::VectorXf::Zero(6);

        while (!quit)
        {
            //events
            while (SDL_PollEvent(&e) != 0)
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
                else if (e.type == SDL_MOUSEMOTION)
                {
                    SDL_GetMouseState(&x, &y);
                    std::cout << "Mouse position: (" << x << ", " << y << ")" << std::endl;
                }
                else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                {
                    SDL_GetMouseState(&x, &y);
                    quadrotor_visualizer.target[0] = x;
                    quadrotor_visualizer.target[1] = y;
                    std::cout << "Left button clicked!\n";
                    Eigen::VectorXf goal_state = Eigen::VectorXf::Zero(6);
                    std::pair<double, double> offset = quadrotor_visualizer.GetOffset();
                    double x_offset = offset.first;
                    double y_offset = offset.second;
                    goal_state << (x - x_offset) / scale, (y - y_offset) / scale, 0, 0, 0, 0;
                    quadrotor.SetGoal(goal_state);
                }
                else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_p) {
                    std::cout << "Key 'p' pressed!\n";
                    std::vector<double> t = mp::linspace(0, x_history.size(), x_history.size());
                    mp::plot(t, x_history, t, y_history, "--", t, theta_history, ":");
                    mp::xlabel("Time [ms]");
                    mp::show();
                }
            }

            SDL_Delay((int) dt * 1000);
            std::pair<double, double> offset = quadrotor_visualizer.GetOffset();
            double x_offset = offset.first;
            double y_offset = offset.second;
            Eigen::VectorXf state = quadrotor.GetState();
            x_history.push_back(state[0] * scale + x_offset);
            y_history.push_back(-(state[1] * scale + y_offset));
            theta_history.push_back(state[2] * scale / 100);
            SDL_SetRenderDrawColor(gRenderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer.get());

            /* Quadrotor rendering step */
            quadrotor_visualizer.render(gRenderer);

            SDL_RenderPresent(gRenderer.get());

            /* Simulate quadrotor forward in time */
            control(quadrotor, K);
            quadrotor.Update(dt);
        }
    }
    SDL_Quit();
    return 0;
}

int init(std::shared_ptr<SDL_Window>& gWindow, std::shared_ptr<SDL_Renderer>& gRenderer, const int SCREEN_WIDTH, const int SCREEN_HEIGHT)
{
    if (SDL_Init(SDL_INIT_VIDEO) >= 0)
    {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        gWindow = std::shared_ptr<SDL_Window>(SDL_CreateWindow("Planar Quadrotor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN), SDL_DestroyWindow);
        gRenderer = std::shared_ptr<SDL_Renderer>(SDL_CreateRenderer(gWindow.get(), -1, SDL_RENDERER_ACCELERATED), SDL_DestroyRenderer);
        SDL_SetRenderDrawColor(gRenderer.get(), 0xFF, 0xFF, 0xFF, 0xFF);
    }
    else
    {
        std::cout << "SDL_ERROR: " << SDL_GetError() << std::endl;
        return -1;
    }
    return 0;
}