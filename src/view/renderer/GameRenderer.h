#ifndef MARIO_GAMERENDERER_H
#define MARIO_GAMERENDERER_H

#include "common/Type.h"
#include "view/renderer/AssetManager.h"

class GameRenderer {
   public:
    GameRenderer();
    ~GameRenderer() = default;

    void setCamera(const PositionType* cx, const PositionType* cy) {
        cameraX_ = cx;
        cameraY_ = cy;
    }
    void setPlayerInfo(const PlayerInfo* p) { playerInfo_ = p; }
    void setTileInfos(const TileInfos* t) { tileInfos_ = t; }
    void setEnemyInfos(const EnemyInfos* e) { enemyInfos_ = e; }
    void setHudInfo(const HudInfo* h) { hudInfo_ = h; }

    void render(sf::RenderWindow& window, float dt);

   private:
    void drawHud(sf::RenderWindow& window);
    void drawHudBlock(sf::RenderWindow& window, const std::string& label, const std::string& value, float centerX);
    void drawPixelText(sf::RenderWindow& window, const std::string& text, float x, float y, float scale);
    float measurePixelText(const std::string& text, float scale) const;
    void drawPlayer(sf::RenderWindow& window, float dt);
    void drawEnemy(sf::RenderWindow& window, const EnemyInfo& enemy);
    void drawTile(sf::RenderWindow& window, const TileInfo& tile);
    void drawFallbackTile(sf::RenderWindow& window, const TileInfo& tile);

    AssetManager assets_;

    const PositionType* cameraX_ = nullptr;
    const PositionType* cameraY_ = nullptr;
    const PlayerInfo* playerInfo_ = nullptr;
    const TileInfos* tileInfos_ = nullptr;
    const EnemyInfos* enemyInfos_ = nullptr;
    const HudInfo* hudInfo_ = nullptr;

    float runAnimationTime_ = 0.0f;
    float enemyAnimationTime_ = 0.0f;
    MarioState lastPlayerState_ = MarioState::IDLE;
    sf::RectangleShape rect_;
};

#endif  // MARIO_GAMERENDERER_H
