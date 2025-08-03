#include "Game.hpp"

const std::string TITLE = "Art gallery ghost";

const std::uint16_t WIDTH = 1280;
const std::uint16_t HEIGHT = 720;

int main() {
    core::Game game(TITLE, WIDTH, HEIGHT);

    game.Run();
    game.Clear();
    return 0;
}