#include "core/game.h"

#ifdef WITH_AI
#include "engine/engine.h"
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <string>

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    Game game;

    bool ai_active = false;
    Color ai_side = Color::Black;

#ifdef WITH_AI
    Engine engine;
    std::cout << "Play vs AI? (y/n) ";
    std::string answer;
    if (std::getline(std::cin, answer)) {
        const bool yes = !answer.empty() && (answer[0] == 'y' || answer[0] == 'Y');
        if (yes) {
            std::cout << "Play as (w/b) ? ";
            std::string side;
            std::getline(std::cin, side);
            const Color player =
                (!side.empty() && (side[0] == 'b' || side[0] == 'B')) ? Color::Black : Color::White;
            ai_side = opposite(player);
            ai_active = true;
            std::cout << "AI plays " << (ai_side == Color::White ? "White" : "Black") << ".\n";
        }
    }
#endif

    bool stop = false;
    do {
        game.display();
#ifdef WITH_AI
        if (ai_active && !game.finished() && game.to_move() == ai_side) {
            const Move m = engine.best_move(game.board(), game.to_move());
            stop = game.play_external_move(m);
            continue;
        }
#endif
        (void)ai_active;
        (void)ai_side;  // silence unused-warning in core-only build
        stop = game.play_turn();
    } while (!stop);

    return 0;
}
