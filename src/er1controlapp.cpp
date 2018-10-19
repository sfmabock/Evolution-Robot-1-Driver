
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <er1driver/er1driver.hpp>


int main(int argc, char** argv) {
    SDL_Event event;
    bool done = false;
    if (SDL_Init(SDL_INIT_VIDEO)!= 0) {
        return -1;
    }


    SDL_Window *window;

      window = SDL_CreateWindow(
            "An SDL2 window",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640,
            480,
            SDL_WINDOW_OPENGL
    );



    er1driver::SimpleER1Interface er1;
    er1.connect("/dev/serial/by-id/usb-Evolution_Robot_ER1_Control_Module_v1.0_ER1TQEG3-if00-port0");


    int currentDown = 0;

    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN: {
                    auto code = event.key.keysym.sym;
                    if (code == SDLK_UP && currentDown != SDLK_UP) {
                        currentDown = code;
                        er1.move(er1driver::Direction::Forward);
                    } else if (code == SDLK_DOWN && currentDown != SDLK_DOWN) {
                        currentDown = code;
                        er1.move(er1driver::Direction::Forward);
                    } else if (code == SDLK_LEFT && currentDown != SDLK_LEFT) {
                        currentDown = code;
                        er1.turn(er1driver::Orientation::Left);
                    } else if (code == SDLK_RIGHT && currentDown != SDLK_RIGHT){
                        currentDown = code;
                        er1.turn(er1driver::Orientation::Right);
                    }
                    break;
                }
                case SDL_KEYUP:
                    auto code = event.key.keysym.sym;
                    if (code == SDLK_q) {
                        done = true;
                    } else if (code == currentDown) {
                        er1.stop();
                    }
                    currentDown = 0;
                    break;
            }
        }
    }

    er1.disconnect();

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
