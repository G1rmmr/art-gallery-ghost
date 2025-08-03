#include "Game.hpp"

const std::string TITLE = "Art gallery ghost";

const std::uint16_t WIDTH = 1920;
const std::uint16_t HEIGHT = 1080;

int main() {
    core::Game game(TITLE, WIDTH, HEIGHT);

    game.Run();
    game.Clear();
    return 0;
}