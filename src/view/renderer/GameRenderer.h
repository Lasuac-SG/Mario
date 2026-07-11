//

#ifndef MARIO_GAMERENDERER_H
#define MARIO_GAMERENDERER_H

#include "common/Type.h"
#include "view/renderer/AssetManager.h"

class GameRenderer {
   public:
    GameRenderer();
    ~GameRenderer() = default;

    // 数据绑定
    void setCamera(const PositionType* cx, const PositionType* cy) {
        cameraX_ = cx;
        cameraY_ = cy;
    }
    void setPlayerInfo(const PlayerInfo* p) { playerInfo_ = p; }
    void setTileInfos(const TileInfos* t) { tileInfos_ = t; }

    // 渲染一帧：clear → setView → draw tiles → draw player → display
    // dt 由外部（App 驱动时钟）经 View 下推，用于动画计时；渲染器不再自持时钟。
    void render(sf::RenderWindow& window, float dt);

   private:
    void drawPlayer(sf::RenderWindow& window, float dt);
    void drawTile(sf::RenderWindow& window, const TileInfo& tile);
    void drawFallbackTile(sf::RenderWindow& window, const TileInfo& tile);

    AssetManager assets_;

    const PositionType* cameraX_ = nullptr;
    const PositionType* cameraY_ = nullptr;
    const PlayerInfo* playerInfo_ = nullptr;
    const TileInfos* tileInfos_ = nullptr;

    // 玩家动画状态（动画计时由外部 dt 累加，不再自持 sf::Clock）
    float runAnimationTime_ = 0.0f;
    MarioState lastPlayerState_ = MarioState::IDLE;
    sf::RectangleShape rect_;

    static constexpr int LOGIC_W = 800;
    static constexpr int LOGIC_H = 600;
};

#endif  // MARIO_GAMERENDERER_H
