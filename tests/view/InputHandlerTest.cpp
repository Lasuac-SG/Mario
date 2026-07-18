#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include <vector>

#include "common/Type.h"
#include "view/input/InputHandler.h"

namespace {

bool demoModeEnabled() {
    const char* value = std::getenv("TEST_VIEW_DEMO");
    return value && value[0] != '\0' && value[0] != '0';
}

void demoStep(const char* message) {
    if (!demoModeEnabled()) return;
    std::cout << "[demo] " << message << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
}

class RecordingCommand : public ICommandBase {
   public:
    int exec(ICommandParameter* p) override {
        auto* action = dynamic_cast<InputActionParameter*>(p);
        EXPECT_NE(action, nullptr);
        if (action) {
            demoStep("Command recorded from current keyboard state.");
            actions.push_back(action->v_);
        }
        return 0;
    }

    std::vector<InputAction> actions;
};

}  // namespace

TEST(InputHandlerTest, LeftKeyDispatchesMoveLeft) {
    InputHandler handler;
    RecordingCommand command;
    handler.setActionCommand(&command);

    demoStep("Simulate pressing Left.");
    handler.setKeyStateForTest(sf::Keyboard::Key::Left, true);
    demoStep("Dispatch input state.");
    handler.dispatchInput();

    ASSERT_EQ(command.actions.size(), 1u);
    EXPECT_EQ(command.actions[0], InputAction::MOVE_LEFT);
}

TEST(InputHandlerTest, RightKeyDispatchesMoveRight) {
    InputHandler handler;
    RecordingCommand command;
    handler.setActionCommand(&command);

    demoStep("Simulate pressing D / Right.");
    handler.setKeyStateForTest(sf::Keyboard::Key::D, true);
    demoStep("Dispatch input state.");
    handler.dispatchInput();

    ASSERT_EQ(command.actions.size(), 1u);
    EXPECT_EQ(command.actions[0], InputAction::MOVE_RIGHT);
}

TEST(InputHandlerTest, OpposingDirectionsDispatchStop) {
    InputHandler handler;
    RecordingCommand command;
    handler.setActionCommand(&command);

    demoStep("Simulate pressing Left and Right together.");
    handler.setKeyStateForTest(sf::Keyboard::Key::Left, true);
    handler.setKeyStateForTest(sf::Keyboard::Key::Right, true);
    demoStep("Dispatch input state.");
    handler.dispatchInput();

    ASSERT_EQ(command.actions.size(), 1u);
    EXPECT_EQ(command.actions[0], InputAction::STOP);
}

TEST(InputHandlerTest, JumpIsEdgeTriggered) {
    InputHandler handler;
    RecordingCommand command;
    handler.setActionCommand(&command);

    demoStep("Press Jump once.");
    handler.setKeyStateForTest(sf::Keyboard::Key::Space, true);
    demoStep("First dispatch: expect STOP then JUMP.");
    handler.dispatchInput();
    demoStep("Second dispatch while still holding Jump: expect no extra JUMP.");
    handler.dispatchInput();

    ASSERT_EQ(command.actions.size(), 3u);
    EXPECT_EQ(command.actions[0], InputAction::STOP);
    EXPECT_EQ(command.actions[1], InputAction::JUMP);
    EXPECT_EQ(command.actions[2], InputAction::STOP);
}

TEST(InputHandlerTest, RestartIsEdgeTriggered) {
    InputHandler handler;
    RecordingCommand command;
    handler.setActionCommand(&command);

    demoStep("Press Restart once.");
    handler.setKeyStateForTest(sf::Keyboard::Key::R, true);
    demoStep("First dispatch: expect STOP then RESTART.");
    handler.dispatchInput();
    demoStep("Second dispatch while still holding Restart: expect no extra RESTART.");
    handler.dispatchInput();

    ASSERT_EQ(command.actions.size(), 3u);
    EXPECT_EQ(command.actions[0], InputAction::STOP);
    EXPECT_EQ(command.actions[1], InputAction::RESTART);
    EXPECT_EQ(command.actions[2], InputAction::STOP);
}

TEST(InputHandlerTest, ReleasingJumpAllowsAnotherJumpPress) {
    InputHandler handler;
    RecordingCommand command;
    handler.setActionCommand(&command);

    demoStep("Press Jump.");
    handler.setKeyStateForTest(sf::Keyboard::Key::W, true);
    handler.dispatchInput();
    demoStep("Release Jump.");
    handler.setKeyStateForTest(sf::Keyboard::Key::W, false);
    handler.dispatchInput();
    demoStep("Press Jump again after release.");
    handler.setKeyStateForTest(sf::Keyboard::Key::W, true);
    handler.dispatchInput();

    ASSERT_EQ(command.actions.size(), 5u);
    EXPECT_EQ(command.actions[1], InputAction::JUMP);
    EXPECT_EQ(command.actions[4], InputAction::JUMP);
}
