//

#ifndef MARIO_VIEWMODEL_H
#define MARIO_VIEWMODEL_H

#include "common/EventTrigger.h"
#include "common/EventId.h"
#include "viewmodel/GameCommands.h"

class GameModel;

class GameViewModel {
public:
    explicit GameViewModel(GameModel* model);
    ~GameViewModel();

    // === Command 引用（View 获取后决定何时 exec）===
    ICommandBase& inputCmd()     { return inputCmd_; }

    // === 游戏驱动 ===
    void tick(float dt);         // → model.update(dt)

    // === 渲染数据查询（View::draw 中调用）===
    const GameModel& gameModel() const { return *model_; }

    // === 事件（View 订阅）===
    EventTrigger vmTrigger;

private:
    void onModelChanged(EventType ev);

    GameModel* model_;
    uintptr_t modelSubId_;

    // 预绑定的命令
    InputCommand inputCmd_;
};

#endif  // MARIO_VIEWMODEL_H