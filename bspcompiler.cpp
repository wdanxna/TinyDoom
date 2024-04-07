#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cassert>
#include <SDL2/SDL.h>


uint32_t pack_color(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 255) {
    return r + (g << 8) + (b << 16) + (a << 24);
}

void unpack_color(uint32_t c, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) {
    r = uint8_t(c & 255);
    g = uint8_t((c >> 8) & 255);
    b = uint8_t((c >> 16) & 255);
    a = uint8_t((c >> 24) & 255);
}

int main() {
    const size_t win_w = 640;
    const size_t win_h = 480;
    std::vector<uint32_t> framebuffer(win_w*win_h, pack_color(100, 0, 100));

    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;


    if (SDL_CreateWindowAndRenderer(win_w, win_h, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS, &window, &renderer)) {
        std::cerr << "Failed to create window and renderer: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Texture *framebuffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, win_w, win_h);
    if (!framebuffer_texture) {
        std::cerr << "Failed to create SDL texture: " << SDL_GetError() << std::endl;
        return -1;
    }

    using namespace std::chrono_literals;
    bool game_will_stop = false;
    auto last_time = std::chrono::high_resolution_clock::now();

    while (!game_will_stop) {
        auto curr_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed_time = curr_time - last_time;
        if (elapsed_time.count() < 33) {
            std::this_thread::sleep_for(33ms - elapsed_time);
            continue;
        }
        last_time = curr_time;

        float dt = elapsed_time.count() / 1000.0f;

        {
            SDL_Event event;
            if (SDL_PollEvent(&event)) {
                if (SDL_QUIT==event.type || (SDL_KEYDOWN==event.type && SDLK_ESCAPE==event.key.keysym.sym)) {game_will_stop = true;};
                if (SDL_KEYUP==event.type) {
                    // if ('a'==event.key.keysym.sym || 'd'==event.key.keysym.sym) player_turn = 0;
                    // if ('w'==event.key.keysym.sym || 's'==event.key.keysym.sym) player_walk = 0;
                }
                if (SDL_KEYDOWN==event.type) {
                    // if ('a'==event.key.keysym.sym) player_turn = -1;
                    // if ('d'==event.key.keysym.sym) player_turn =  1;
                    // if ('w'==event.key.keysym.sym) player_walk =  1;
                    // if ('s'==event.key.keysym.sym) player_walk = -1;
                }
            }
        }
        

        SDL_UpdateTexture(framebuffer_texture, NULL, reinterpret_cast<void*>(framebuffer.data()), win_w*4);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, framebuffer_texture, NULL, NULL);
        SDL_RenderPresent(renderer);

    }


    SDL_DestroyTexture(framebuffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;


}