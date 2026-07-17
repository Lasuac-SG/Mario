#include "view/audio/AudioManager.h"

#include <exception>
#include <set>

// ── BGM ────────────────────────────────────────────────────────────

bool AudioManager::playBGM(const std::string& path, float volume) {
    try {
        if (!bgm_.openFromFile(path)) return false;
        bgm_.setLooping(true);
        bgmVolume_ = volume;
        bgm_.setVolume(volume);
        bgm_.play();
        return true;
    } catch (const std::exception&) {
        return false;
    } catch (...) {
        return false;
    }
}

void AudioManager::stopBGM() { bgm_.stop(); }

void AudioManager::setBGMVolume(float volume) {
    bgmVolume_ = volume;
    bgm_.setVolume(volume);
}

bool AudioManager::isBGMPlaying() const { return bgm_.getStatus() == sf::SoundSource::Status::Playing; }

// ── SFX ────────────────────────────────────────────────────────────

bool AudioManager::loadSFX(const std::string& name, const std::string& path) {
    try {
        auto& buffer = buffers_[name];
        return buffer.loadFromFile(path);
    } catch (const std::exception&) {
        return false;
    } catch (...) {
        return false;
    }
}

void AudioManager::loadAllSFX() {
    const auto& map = eventSFXMap();
    std::set<std::string> loaded;  // 去重（多个 Event 可共用同一 SFX 文件）
    for (const auto& [ev, name] : map) {
        if (loaded.insert(name).second) {
            loadSFX(name, "./assets/audio/sound/" + name + ".ogg");
        }
    }
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
    // SFX name 与 audio/sound/ 下的 .ogg 文件名一致（不含扩展名）
    // 路径格式: ./audio/sound/<name>.ogg
    static const std::unordered_map<EventType, std::string> map = {
        {static_cast<EventType>(Event::MARIO_JUMPED),       "MARIO_JUMPED"},
        {static_cast<EventType>(Event::COIN_COLLECTED),     "COIN_COLLECTED"},
        {static_cast<EventType>(Event::MUSHROOM_COLLECTED), "MUSHROOM_COLLECTED"},
        {static_cast<EventType>(Event::MARIO_GROWN),        "MUSHROOM_COLLECTED"},  // 吃蘑菇长大
        {static_cast<EventType>(Event::MARIO_SHRUNK),       "MARIO_SHRUNK"},
        {static_cast<EventType>(Event::ENEMY_STOMPED),      "ENEMY_STOMPED"},
        {static_cast<EventType>(Event::MARIO_HURT),         "MARIO_SHRUNK"},         // 受伤变小
        {static_cast<EventType>(Event::MARIO_DIED),         "MARIO_DIED"},
        {static_cast<EventType>(Event::BLOCK_BUMPED),       "BLOCK_BUMPED"},
        {static_cast<EventType>(Event::BRICK_BROKEN),       "BRICK_BROKEN"},
        {static_cast<EventType>(Event::GOAL_REACHED),       "GOAL_REACHED"},
    };
    return map;
}
