#include <cmath>
#include <memory>
#include <utility>
#include <vector>

#include "src/model/GameModel.h"
#include "src/view/View.h"
#include "src/viewmodel/ViewModel.h"

namespace {

class AutoPlayController {
   public:
    explicit AutoPlayController(ViewModel* viewModel)
        : actionCommand_(viewModel->getActionCommand()),
          startGame_(viewModel->getStartGameCommand()),
          jumpRelease_(viewModel->getJumpReleaseCommand()),
          started_(viewModel->getGameStarted()),
          won_(viewModel->getWon()),
          gameOver_(viewModel->getGameOver()) {}

    void tick(float dt) {
        totalElapsed_ += dt;

        if (!started_ || !(*started_)) {
            handleMenu(dt);
            return;
        }

        if ((won_ && *won_) || (gameOver_ && *gameOver_)) {
            applyMovement(InputAction::STOP);
            releaseJump();
            return;
        }

        playElapsed_ += dt;
        runScript();
        releaseJumpWhenReady();
    }

   private:
    void handleMenu(float dt) {
        menuElapsed_ += dt;
        playElapsed_ = 0.0f;
        cycleIndex_ = -1;
        jumpEventIndex_ = 0;
        applyMovement(InputAction::STOP);
        releaseJump();

        if (!startTriggered_ && menuElapsed_ >= 2.2f) {
            startGame_(1);
            startTriggered_ = true;
            menuElapsed_ = 0.0f;
        }
    }

    void runScript() {
        constexpr float kCycleDuration = 18.0f;
        const int newCycleIndex = static_cast<int>(std::floor(playElapsed_ / kCycleDuration));
        if (newCycleIndex != cycleIndex_) {
            cycleIndex_ = newCycleIndex;
            jumpEventIndex_ = 0;
        }

        const float cycleTime = std::fmod(playElapsed_, kCycleDuration);
        applyMovement(scriptedMovement(cycleTime));
        triggerScheduledJumps(cycleTime);
    }

    InputAction scriptedMovement(float cycleTime) const {
        if (cycleTime < 3.8f) return InputAction::MOVE_RIGHT;
        if (cycleTime < 4.6f) return InputAction::STOP;
        if (cycleTime < 6.8f) return InputAction::MOVE_LEFT;
        if (cycleTime < 7.6f) return InputAction::STOP;
        if (cycleTime < 12.4f) return InputAction::MOVE_RIGHT;
        if (cycleTime < 13.0f) return InputAction::STOP;
        if (cycleTime < 15.5f) return InputAction::MOVE_RIGHT;
        return InputAction::MOVE_LEFT;
    }

    void triggerScheduledJumps(float cycleTime) {
        static const std::vector<float> kJumpMoments = {1.4f, 5.2f, 9.5f, 14.1f};
        while (jumpEventIndex_ < static_cast<int>(kJumpMoments.size()) &&
               cycleTime >= kJumpMoments[jumpEventIndex_]) {
            pressJump();
            ++jumpEventIndex_;
        }
    }

    void pressJump() {
        InputActionParameter param(InputAction::JUMP);
        actionCommand_->exec(&param);
        jumpHeld_ = true;
        jumpHoldElapsed_ = 0.0f;
    }

    void releaseJumpWhenReady() {
        if (!jumpHeld_) return;

        jumpHoldElapsed_ += kFixedStep_;
        if (jumpHoldElapsed_ >= 0.16f) {
            releaseJump();
        }
    }

    void releaseJump() {
        if (!jumpHeld_) return;
        jumpRelease_();
        jumpHeld_ = false;
        jumpHoldElapsed_ = 0.0f;
    }

    void applyMovement(InputAction action) {
        if (action == currentMovement_) return;
        InputActionParameter param(action);
        actionCommand_->exec(&param);
        currentMovement_ = action;
    }

    static constexpr float kFixedStep_ = 1.0f / 60.0f;

    ICommandBase* actionCommand_ = nullptr;
    std::function<void(int)> startGame_;
    std::function<void()> jumpRelease_;
    const bool* started_ = nullptr;
    const bool* won_ = nullptr;
    const bool* gameOver_ = nullptr;

    float totalElapsed_ = 0.0f;
    float menuElapsed_ = 0.0f;
    float playElapsed_ = 0.0f;
    float jumpHoldElapsed_ = 0.0f;
    bool startTriggered_ = false;
    bool jumpHeld_ = false;
    int cycleIndex_ = -1;
    int jumpEventIndex_ = 0;
    InputAction currentMovement_ = InputAction::STOP;
};

}  // namespace

int main() {
    auto model = std::make_unique<GameModel>();
    auto viewModel = std::make_unique<ViewModel>(model.get());
    auto view = std::make_unique<GameView>();

    view->setCamera(viewModel->getCameraX(), viewModel->getCameraY());
    view->setPlayerInfo(viewModel->getPlayerInfo());
    view->setTileInfos(viewModel->getTileInfos());
    view->setCoinInfos(viewModel->getCoinInfos());
    view->setMushroomInfos(viewModel->getMushroomInfos());
    view->setPlayerBig(viewModel->getPlayerBig());
    view->setEnemyInfos(viewModel->getEnemyInfos());
    view->setGoalInfo(viewModel->getGoalInfo());
    view->setWon(viewModel->getWon());
    view->setGameOver(viewModel->getGameOver());
    view->setGameStarted(viewModel->getGameStarted());
    view->setHudInfo(viewModel->getHudInfo());

    auto modelStep = viewModel->getNextStepCommand();
    auto controller = std::make_shared<AutoPlayController>(viewModel.get());
    view->setNextStepCommand([controller, modelStep = std::move(modelStep)](float dt) mutable {
        controller->tick(dt);
        modelStep(dt);
    });

    view->setResizeCommand(viewModel->getResizeCommand());
    view->setStartGameCommand(viewModel->getStartGameCommand());
    view->setJumpReleaseCommand(viewModel->getJumpReleaseCommand());
    view->setBackToMenuCommand(viewModel->getBackToMenuCommand());
    view->getInputHandler()->setActionCommand(viewModel->getActionCommand());

    viewModel->addNotification(view->getRenderNotification());
    viewModel->addNotification(view->getAudioNotification());
    view->pushCurrentViewportSize();
    view->run();
    return 0;
}
