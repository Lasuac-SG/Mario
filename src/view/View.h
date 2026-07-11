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
    // 主循环 + 驱动时钟：View 自持 sf::Clock，while(窗口打开){ 算 dt → NextStep(dt) }。
    void run();

    // === 通知回调（等价 FLTK get_notification） ===
    Notify_Funtion getRenderNotification();

    // === 属性绑定 —— 委托给组件 ===
    void setCamera(const PositionType* cx, const PositionType* cy) { renderer_.setCamera(cx, cy); }
    void setPlayerInfo(const PlayerInfo* p) { renderer_.setPlayerInfo(p); }
    void setTileInfos(const TileInfos* t) { renderer_.setTileInfos(t); }

    // === next_step command ===
    void setNextStepCommand(std::function<void(float)>&& cmd) { nextStepCommand_ = std::move(cmd); }
    InputHandler* getInputHandler() { return &input_; }

   private:
    // 单帧步进（run 内部调用）：poll 输入 → nextStepCommand_(dt) → 通知触发 render。
    void NextStep(float dt);

    sf::RenderWindow window_;
    InputHandler input_;
    GameRenderer renderer_;
    std::function<void(float)> nextStepCommand_;
    float lastDt_ = 0.0f;  // 当前帧 dt（App 外部时钟下推），供通知触发的 render 使用
};

#endif  // MARIO_VIEW_H
