#ifndef MARIO_AUDIOMANAGER_H
#define MARIO_AUDIOMANAGER_H

#include <list>

#include "common/Type.h"

class AudioManager {
   public:
    AudioManager() = default;
    ~AudioManager() = default;

    // ── BGM ──
    bool playBGM(const std::string& path, float volume = 30.f);
    void stopBGM();
    void setBGMVolume(float volume);
    bool isBGMPlaying() const;

    // ── SFX ──
    /// @return false if file not found or format unsupported
    bool loadSFX(const std::string& name, const std::string& path);
    void playSFX(const std::string& name);
    void setSFXVolume(float volume);
    void stopAllSFX();

    // ── Event notification ──
    /// returns a callable wrapping onEvent(), suitable for EventTrigger::add_notification
    Notify_Funtion getNotification();

   private:
    /// 事件入口：查 eventSFXMap() → playSFX()
    void onEvent(EventType ev);

    /// 硬编码 Event → SFX name 映射
    static const std::unordered_map<EventType, std::string>& eventSFXMap();

    sf::Music bgm_;
    float bgmVolume_ = 30.f;
    float sfxVolume_ = 100.f;

    std::unordered_map<std::string, sf::SoundBuffer> buffers_;
    std::list<sf::Sound> activeSounds_;
};

#endif  // MARIO_AUDIOMANAGER_H
