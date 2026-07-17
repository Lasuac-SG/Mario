#ifndef MARIO_GAMERENDERER_H
#define MARIO_GAMERENDERER_H
#include "common/Type.h"
#include "view/renderer/AssetManager.h"
class GameRenderer {
   public:
    GameRenderer();
    ~GameRenderer() = default;
    void setCamera(const PositionType* cx, const PositionType* cy) noexcept {
        cameraX_ = cx;
        cameraY_ = cy;
    }
    void setPlayerInfo(const PlayerInfo* p) noexcept { playerInfo_ = p; }
    void setTileInfos(const TileInfos* t) noexcept { tileInfos_ = t; }
    void setCoinInfos(const TileInfos* c) noexcept { coinInfos_ = c; }
    void setMushroomInfos(const TileInfos* m) noexcept { mushroomInfos_ = m; }
    void setPlayerBig(const bool* b) noexcept { playerBig_ = b; }
    void setEnemyInfos(const EnemyInfos* e) noexcept { enemyInfos_ = e; }
    void setGoalInfo(const TileInfo* g) noexcept { goalInfo_ = g; }
    void setWon(const bool* w) noexcept { won_ = w; }
    void setGameOver(const bool* g) noexcept { gameOver_ = g; }
    void setHudInfo(const HudInfo* h) noexcept { hudInfo_ = h; }
    void setRestartHovered(bool hovered) noexcept { restartHovered_ = hovered; }
    void setBackToMenuHovered(bool hovered) noexcept { backToMenuHovered_ = hovered; }
    void setGameStarted(const bool* gs) noexcept { gameStarted_ = gs; }
    void setLevel1Hovered(bool h) noexcept { level1Hovered_ = h; }
    void setLevel2Hovered(bool h) noexcept { level2Hovered_ = h; }
    sf::FloatRect restartButtonBounds(const sf::RenderWindow& window) const noexcept;
    sf::FloatRect backToMenuButtonBounds(const sf::RenderWindow& window) const noexcept;
    sf::FloatRect level1Bounds(const sf::RenderWindow& window) const noexcept;
    sf::FloatRect level2Bounds(const sf::RenderWindow& window) const noexcept;
    void render(sf::RenderWindow& window, float dt);
   private:
    void drawHud(sf::RenderWindow& window);
    void drawHudBlock(sf::RenderWindow& window, const std::string& label, const std::string& value, float centerX);
    void drawPixelText(sf::RenderWindow& window, const std::string& text, float x, float y, float scale);
    float measurePixelText(const std::string& text, float scale) const noexcept;
    void drawPlayer(sf::RenderWindow& window, float dt);
    void drawCoin(sf::RenderWindow& window, const TileInfo& coin);
    void drawMushroom(sf::RenderWindow& window, const TileInfo& mushroom);
    void drawEnemy(sf::RenderWindow& window, const EnemyInfo& enemy);
    void drawGoal(sf::RenderWindow& window);
    void drawCastle(sf::RenderWindow& window);
    void drawWinOverlay(sf::RenderWindow& window);
    void drawGameOverOverlay(sf::RenderWindow& window);
    void drawStartMenu(sf::RenderWindow& window);
    void drawTile(sf::RenderWindow& window, const TileInfo& tile);
    void drawFallbackTile(sf::RenderWindow& window, const TileInfo& tile);
    AssetManager assets_;
    const PositionType* cameraX_ = nullptr;
    const PositionType* cameraY_ = nullptr;
    const PlayerInfo* playerInfo_ = nullptr;
    const TileInfos* tileInfos_ = nullptr;
    const TileInfos* coinInfos_ = nullptr;
    const TileInfos* mushroomInfos_ = nullptr;
    const bool* playerBig_ = nullptr;
    const EnemyInfos* enemyInfos_ = nullptr;
    const TileInfo* goalInfo_ = nullptr;
    const bool* won_ = nullptr;
    const bool* gameOver_ = nullptr;
    const HudInfo* hudInfo_ = nullptr;
    float runAnimationTime_ = 0.0f;
    float enemyAnimationTime_ = 0.0f;
    float coinAnimationTime_ = 0.0f;
    float winAnimationTime_ = 0.0f;
    bool restartHovered_ = false;
    bool backToMenuHovered_ = false;
    const bool* gameStarted_ = nullptr;
    bool level1Hovered_ = false;
    bool level2Hovered_ = false;
    MarioState lastPlayerState_ = MarioState::IDLE;
    sf::RectangleShape rect_;
};
#endif  // MARIO_GAMERENDERER_H
