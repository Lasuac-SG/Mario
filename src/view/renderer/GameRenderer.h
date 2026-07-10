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
    void render(sf::RenderWindow& window);

   private:
    void drawPlayer(sf::RenderWindow& window);
    void drawTile(sf::RenderWindow& window, const TileInfo& tile);
    void drawFallbackTile(sf::RenderWindow& window, const TileInfo& tile);

    AssetManager assets_;

    const PositionType* cameraX_ = nullptr;
    const PositionType* cameraY_ = nullptr;
    const PlayerInfo* playerInfo_ = nullptr;
    const TileInfos* tileInfos_ = nullptr;

    // 玩家动画状态
    float runAnimationTime_ = 0.0f;
    MarioState lastPlayerState_ = MarioState::IDLE;
    sf::Clock animationClock_;
    sf::RectangleShape rect_;

    static constexpr int LOGIC_W = 800;
    static constexpr int LOGIC_H = 600;
};

#endif  // MARIO_GAMERENDERER_H
