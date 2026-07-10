//
#include "viewmodel/ViewModel.h"

#include "common/Type.h"

ViewModel::ViewModel(GameModel* model) : model_(model), actionCmd_(model) {
    funct_callback_Index_ = model_->modelTrigger.add_notification([this](EventType ev) { onModelChanged(ev); });
    syncFromModel();  // 初始化时同步一次
}

ViewModel::~ViewModel() { model_->modelTrigger.remove_notification(funct_callback_Index_); }

void ViewModel::onModelChanged(EventType /*ev*/) {
    syncFromModel();
    vmTrigger.fire(static_cast<uint32_t>(ViewModelEvent::RENDER_UPDATE));
}

void ViewModel::syncFromModel() {
    // 同步玩家数据
    player_info_.x = model_->playerX();
    player_info_.y = model_->playerY();
    player_info_.width = model_->playerW();
    player_info_.height = model_->playerH();
    player_info_.state = model_->playerState();
    player_info_.direction = model_->playerFacing();

    // 同步瓦片数据
    tile_infos_.clear();
    for (const auto& tile : model_->tiles()) {
        tile_infos_.push_back({tile.x, tile.y, tile.w, tile.h, tile.type});
    }
}

void ViewModel::tick(float dt) { model_->update(dt); }

void ViewModel::addNotification(Notify_Funtion func) { vmTrigger.add_notification(std::move(func)); }

int ViewModel::act_Command(InputActionParameter& param) noexcept { return actionCmd_.exec(&param); }
