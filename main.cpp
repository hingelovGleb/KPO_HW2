#include "infrastructure/console.hpp"
#include "infrastructure/infrastructure.hpp"
#include <iostream>
#include <exception>

int main() {
    try {
        pos::AppContainer container("pos_data.txt");
        runConsole(container.app);
    } catch (const std::exception& error) {
        std::cerr << "Startup error: " << error.what() << '\n';
        return 1;
    }
}
