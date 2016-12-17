#include "SDL2/SDL.h"
#include "../frontend.h"
#include "../../window.h"
#include "../../display.h"
#include "../../input.h"
#include "../../gbe.h"

#define WINDOW_HEIGHT 144
#define WINDOW_WIDTH 160
#define SDL_CHECK_CYCLES_NUM 256

SDL_Window* window = NULL;
SDL_Texture* texture = NULL;
SDL_Renderer* renderer = NULL;

input local_input = {};

// Swap buffers
void frontend_swap_buffers() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

// Return current window size
void frontend_get_window_size(windowSize *size) {
    int width = 0, height = 0;
    SDL_GetRendererOutputSize(renderer, &width, &height);
    size->width = width;
    size->height = height;
}

// Get texture ready to be displayed
void displayOnWindow(uint8 *frameBuffer) {
    // Convert framebuffer to texture to display
    SDL_Surface* frame = SDL_CreateRGBSurfaceFrom(frameBuffer, DISPLAY_WIDTH, DISPLAY_HEIGHT, 32, DISPLAY_WIDTH * 4, 0, 0, 0, 0);
    // If texture isn't loaded yet, create one; else update existing texture.
    if (texture == NULL) {
        texture = SDL_CreateTextureFromSurface(renderer, frame);
    } else {
        SDL_UpdateTexture(texture, NULL, frame->pixels, frame->pitch);
    }
    SDL_FreeSurface(frame);
    frontend_swap_buffers();
}

// Get current input
void getInput(input *current_input) {
    // Send local input to the enulator
    current_input->start    = local_input.start;
    current_input->select   = local_input.select;
    current_input->a        = local_input.a;
    current_input->b        = local_input.b;
    current_input->up       = local_input.up;
    current_input->down     = local_input.down;
    current_input->left     = local_input.left;
    current_input->right    = local_input.right;
}

// Start SDL window
void startDisplay() {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("GBE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
}

// Stop SDL window and free up resources.
void stopDisplay() {
     // Remove screen, texture, render, and window
     SDL_DestroyTexture(texture);
     SDL_DestroyRenderer(renderer);
     SDL_DestroyWindow(window);
     texture = NULL;
     renderer = NULL;
     window = NULL;
     // Quit SDL
     SDL_Quit();
}

// Update window size
static void resizeWindow(int width, int height) {
    SDL_RenderSetLogicalSize(renderer, width, height);
}

// Handle key presses and releases
void handeKeyEvent(SDL_Event *event) {
    // Send interrupt if new key pressed
    if (event->type == SDL_KEYDOWN) {
        emulatorInterrupt(EMULATOR_INTER_JOYPAD);
    }
    // Set wether the key is pressed or not.
    if (event->key.keysym.sym == SDLK_RETURN) {
        local_input.start   = (event->type == SDL_KEYDOWN);
    } else if (event->key.keysym.sym == SDLK_RSHIFT) {
        local_input.select  = (event->type == SDL_KEYDOWN);
    } else if (event->key.keysym.sym == SDLK_z) {
        local_input.a       = (event->type == SDL_KEYDOWN);
    } else if (event->key.keysym.sym == SDLK_x) {
        local_input.b       = (event->type == SDL_KEYDOWN);
    } else if (event->key.keysym.sym == SDLK_UP) {
        local_input.up      = (event->type == SDL_KEYDOWN);
    } else if (event->key.keysym.sym == SDLK_DOWN) {
        local_input.down    = (event->type == SDL_KEYDOWN);
    } else if (event->key.keysym.sym == SDLK_LEFT) {
        local_input.left    = (event->type == SDL_KEYDOWN);
    } else if (event->key.keysym.sym == SDLK_RIGHT) {
        local_input.right   = (event->type == SDL_KEYDOWN);
    }
}

// Run emulator from this method.
int main(int argc, char *argv[]) {
    startDisplay();
    int out = startEmulator(argc, argv);
    SDL_Event event;
    // Emulator loop. Run until error
    while (!out) {
        // Check events
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            } else if (event.type == SDL_WINDOWEVENT_SIZE_CHANGED) {
                // Update rendering window size
                resizeWindow(event.window.data1, event.window.data2);
            } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                // Handle input
                handeKeyEvent(&event);
            }
        }
        // Too much overhead to check input for every cycle.
        for (int i = 0; i < SDL_CHECK_CYCLES_NUM; i++) {
            // Cycle emulator one instruction
            out = cycleEmulator();
            // Quit if error returned
            if (out) {
                break;
            }
        }
    }
    // End the program
    stopEmulator();
    stopDisplay();
    return out;
}

