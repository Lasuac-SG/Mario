#include <memory>

#include "model/GameModel.h"
#include "view/SimpleRenderer.h"
#include "view/View.h"
#include "viewmodel/ViewModel.h"

int main() {
    auto model = std::make_unique<GameModel>();
    auto viewModel = std::make_unique<GameViewModel>(model.get());
    auto renderer = std::make_unique<SimpleRenderer>();

    GameView view(viewModel.get(), renderer.get());
    view.run();
    return 0;
}
