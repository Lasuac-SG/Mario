
#include "app/App.h"

int main() {
    App app;
    app.initialize();
    app.run();
    std::cout << "Hello, Mario!" << std::endl;
    return 0;
}