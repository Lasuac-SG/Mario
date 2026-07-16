#include "view/audio/AudioManager.h"

// ── BGM ────────────────────────────────────────────────────────────

bool AudioManager::playBGM(const std::string& path, float volume) {
    if (!bgm_.openFromFile(path)) return false;
    bgm_.setLooping(true);
    bgmVolume_ = volume;
    bgm_.setVolume(volume);
    bgm_.play();
    return true;
}

void AudioManager::stopBGM() { bgm_.stop(); }

void AudioManager::setBGMVolume(float volume) {
    bgmVolume_ = volume;
    bgm_.setVolume(volume);
}

bool AudioManager::isBGMPlaying() const { return bgm_.getStatus() == sf::SoundSource::Status::Playing; }

// ── SFX ────────────────────────────────────────────────────────────

bool AudioManager::loadSFX(const std::string& name, const std::string& path) {
    auto& buffer = buffers_[name];
    return buffer.loadFromFile(path);
}

void AudioManager::playSFX(const std::string& name) {
    auto it = buffers_.find(name);
    if (it == buffers_.end()) return;

    // 清理已停止的 Sound（原地删除，不需要移动语义）
    activeSounds_.remove_if([](const sf::Sound& s) {
        return s.getStatus() == sf::SoundSource::Status::Stopped;
    });

    // 原地构造新 Sound → 播放
    activeSounds_.emplace_back(it->second);
    activeSounds_.back().setVolume(sfxVolume_);
    activeSounds_.back().play();
}

void AudioManager::setSFXVolume(float volume) { sfxVolume_ = volume; }

void AudioManager::stopAllSFX() {
    for (auto& s : activeSounds_) s.stop();
    activeSounds_.clear();
}

// ── Event notification ─────────────────────────────────────────────

Notify_Funtion AudioManager::getNotification() {
    return [this](EventType ev) { onEvent(ev); };
}

void AudioManager::onEvent(EventType ev) {
    const auto& map = eventSFXMap();
    auto it = map.find(ev);
    if (it != map.end()) playSFX(it->second);
}

const std::unordered_map<EventType, std::string>& AudioManager::eventSFXMap() {
    static const std::unordered_map<EventType, std::string> map = {
        {static_cast<EventType>(Event::COIN_COLLECTED),     "coin"},
        {static_cast<EventType>(Event::MUSHROOM_COLLECTED), "mushroom"},
        {static_cast<EventType>(Event::MARIO_GROWN),        "grow"},
        {static_cast<EventType>(Event::MARIO_SHRUNK),       "hurt"},
        {static_cast<EventType>(Event::ENEMY_STOMPED),      "stomp"},
        {static_cast<EventType>(Event::MARIO_HURT),         "hurt"},
        {static_cast<EventType>(Event::MARIO_DIED),          "die"},
        {static_cast<EventType>(Event::BLOCK_BUMPED),       "bump"},
        {static_cast<EventType>(Event::BRICK_BROKEN),       "brick"},
        {static_cast<EventType>(Event::GOAL_REACHED),       "goal"},
    };
    return map;
}
