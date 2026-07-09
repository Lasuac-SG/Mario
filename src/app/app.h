//

#ifndef MARIO_APP_H
#define MARIO_APP_H

#include "model/GameModel.h"
#include "viewmodel/ViewModel.h"
#include "view/View.h"
#include "view/SimpleRenderer.h"

class App {
public:
    App() = default;

    void initialize();

    void run();

private:
    std::unique_ptr<GameModel>      model_;
    std::unique_ptr<GameViewModel>  vm_;
    std::unique_ptr<SimpleRenderer> renderer_;
    std::unique_ptr<GameView>       view_;
};

#endif  // MARIO_APP_H
