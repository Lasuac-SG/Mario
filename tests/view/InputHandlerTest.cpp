#include <gtest/gtest.h>

#include <vector>

#include "common/Type.h"
#include "view/input/InputHandler.h"

namespace {

class RecordingCommand : public ICommandBase {
   public:
    int exec(ICommandParameter* p) override {
        auto* action = dynamic_cast<InputActionParameter*>(p);
        EXPECT_NE(action, nullptr);
        if (action) {
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

    handler.setKeyStateForTest(sf::Keyboard::Key::Left, true);
    handler.dispatchInput();

    ASSERT_EQ(command.actions.size(), 1u);
    EXPECT_EQ(command.actions[0], InputAction::MOVE_LEFT);
}

TEST(InputHandlerTest, RightKeyDispatchesMoveRight) {
    InputHandler handler;
    RecordingCommand command;
    handler.setActionCommand(&command);

    handler.setKeyStateForTest(sf::Keyboard::Key::D, true);
    handler.dispatchInput();

    ASSERT_EQ(command.actions.size(), 1u);
    EXPECT_EQ(command.actions[0], InputAction::MOVE_RIGHT);
}

TEST(InputHandlerTest, OpposingDirectionsDispatchStop) {
    InputHandler handler;
    RecordingCommand command;
    handler.setActionCommand(&command);

    handler.setKeyStateForTest(sf::Keyboard::Key::Left, true);
    handler.setKeyStateForTest(sf::Keyboard::Key::Right, true);
    handler.dispatchInput();

    ASSERT_EQ(command.actions.size(), 1u);
    EXPECT_EQ(command.actions[0], InputAction::STOP);
}

TEST(InputHandlerTest, JumpIsEdgeTriggered) {
    InputHandler handler;
    RecordingCommand command;
    handler.setActionCommand(&command);

    handler.setKeyStateForTest(sf::Keyboard::Key::Space, true);
    handler.dispatchInput();
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

    handler.setKeyStateForTest(sf::Keyboard::Key::R, true);
    handler.dispatchInput();
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

    handler.setKeyStateForTest(sf::Keyboard::Key::W, true);
    handler.dispatchInput();
    handler.setKeyStateForTest(sf::Keyboard::Key::W, false);
    handler.dispatchInput();
    handler.setKeyStateForTest(sf::Keyboard::Key::W, true);
    handler.dispatchInput();

    ASSERT_EQ(command.actions.size(), 5u);
    EXPECT_EQ(command.actions[1], InputAction::JUMP);
    EXPECT_EQ(command.actions[4], InputAction::JUMP);
}
