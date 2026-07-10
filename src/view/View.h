//

#ifndef MARIO_VIEW_H
#define MARIO_VIEW_H

#include "common/Type.h"
#include "view/input/InputHandler.h"
#include "view/renderer/GameRenderer.h"

class GameView {
   public:
    GameView();
    ~GameView() = default;

    // === 生命周期 ===
    bool isOpen() const { return window_.isOpen(); }

    // ★ 唯一的帧入口（等价 FLTK timeout_cb → m_next_step_command(turn)）
    void NextStep(float dt);

    // === 通知回调（等价 FLTK get_notification） ===
    Notify_Funtion getRenderNotification();

    // === 属性绑定 —— 委托给组件 ===
    void setActionCommand(ICommandBase* cmd) { input_.setActionCommand(cmd); }
    void setCamera(const PositionType* cx, const PositionType* cy) { renderer_.setCamera(cx, cy); }
    void setPlayerInfo(const PlayerInfo* p) { renderer_.setPlayerInfo(p); }
    void setTileInfos(const TileInfos* t) { renderer_.setTileInfos(t); }

    // === next_step command ===
    void setNextStepCommand(std::function<void(float)>&& cmd) { nextStepCommand_ = std::move(cmd); }

   private:
    sf::RenderWindow window_;
    InputHandler input_;
    GameRenderer renderer_;
    std::function<void(float)> nextStepCommand_;
};

#endif  // MARIO_VIEW_H
