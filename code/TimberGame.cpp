#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <ctime>
#include <vector>

// Constants
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int PLAYER_POSITION_LEFT = 600;
const int PLAYER_POSITION_RIGHT = 1200;
const int TREE_POSITION_X = 860;

enum class Side { LEFT, RIGHT, NONE };

struct Branch {
    SDL_Rect rect;
    Side side = Side::NONE;
};

// Initialize SDL
bool init(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    *window = SDL_CreateWindow("Timber SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!*window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

// Load texture
SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        std::cerr << "Unable to load texture: " << path << " SDL_Error: " << SDL_GetError() << std::endl;
    }
    return texture;
}

// Update branches
void updateBranches(std::vector<Branch>& branches, SDL_Renderer* renderer) {
    for (int i = branches.size() - 1; i > 0; --i) {
        branches[i].side = branches[i - 1].side;
    }

    int r = rand() % 5;
    branches[0].side = (r == 0) ? Side::LEFT : (r == 1) ? Side::RIGHT : Side::NONE;
}

int main(int argc, char* args[]) {
    srand(static_cast<unsigned int>(time(0)));
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!init(&window, &renderer)) {
        return -1;
    }

    SDL_Texture* background = loadTexture("./../graphics/background.png", renderer);
    SDL_Texture* tree = loadTexture("./../graphics/tree.png", renderer);
    SDL_Texture* player = loadTexture("./../graphics/player.png", renderer);
    SDL_Texture* branch = loadTexture("./../graphics/branch.png", renderer);

    if (!background || !tree || !player || !branch) {
        return -1;
    }

    bool quit = false;
    SDL_Event e;
    Side playerSide = Side::LEFT;
    std::vector<Branch> branches(6, { {0, 0, 200, 100}, Side::NONE });

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_LEFT) {
                    playerSide = Side::LEFT;
                    updateBranches(branches, renderer);
                } else if (e.key.keysym.sym == SDLK_RIGHT) {
                    playerSide = Side::RIGHT;
                    updateBranches(branches, renderer);
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background, nullptr, nullptr);
        SDL_Rect treeRect = { TREE_POSITION_X, 0, 200, 1080 };
        SDL_RenderCopy(renderer, tree, nullptr, &treeRect);

        for (size_t i = 0; i < branches.size(); ++i) {
            if (branches[i].side != Side::NONE) {
                branches[i].rect.y = i * 150;
                branches[i].rect.x = (branches[i].side == Side::LEFT) ? 400 : 1300;
                SDL_RenderCopy(renderer, branch, nullptr, &branches[i].rect);
            }
        }

        SDL_Rect playerRect = { (playerSide == Side::LEFT) ? PLAYER_POSITION_LEFT : PLAYER_POSITION_RIGHT, 800, 200, 300 };
        SDL_RenderCopy(renderer, player, nullptr, &playerRect);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(tree);
    SDL_DestroyTexture(player);
    SDL_DestroyTexture(branch);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
