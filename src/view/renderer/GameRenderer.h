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

    void render(sf::RenderWindow& window, float dt);

   private:
    void updateHud(float dt);
    void drawHud(sf::RenderWindow& window);
    void drawHudBlock(sf::RenderWindow& window, const std::string& label, const std::string& value, float centerX);
    void drawPixelText(sf::RenderWindow& window, const std::string& text, float x, float y, float scale);
    float measurePixelText(const std::string& text, float scale) const;
    void drawPlayer(sf::RenderWindow& window, float dt);
    void drawTile(sf::RenderWindow& window, const TileInfo& tile);
    void drawFallbackTile(sf::RenderWindow& window, const TileInfo& tile);

    AssetManager assets_;

    const PositionType* cameraX_ = nullptr;
    const PositionType* cameraY_ = nullptr;
    const PlayerInfo* playerInfo_ = nullptr;
    const TileInfos* tileInfos_ = nullptr;

    float runAnimationTime_ = 0.0f;
    float hudElapsedTime_ = 0.0f;
    MarioState lastPlayerState_ = MarioState::IDLE;
    sf::RectangleShape rect_;

    static constexpr int LOGIC_W = 800;
    static constexpr int LOGIC_H = 600;
    static constexpr int START_TIME = 400;
};

#endif  // MARIO_GAMERENDERER_H
