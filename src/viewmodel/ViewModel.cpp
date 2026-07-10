//
#include "viewmodel/ViewModel.h"
#include "common/Type.h"

ViewModel::ViewModel(GameModel* model) : model_(model), actionCmd_(model) {
    funct_calback_Index_ = model_->modelTrigger.add_notification([this](EventType ev) { onModelChanged(ev); });
}

ViewModel::~ViewModel() { model_->modelTrigger.remove_notification(funct_calback_Index_); }

void ViewModel::onModelChanged(EventType /*ev*/) {
    vmTrigger.fire(static_cast<uint32_t>(ViewModelEvent::RENDER_UPDATE));
}

void ViewModel::tick(float dt) { model_->update(dt); }

void ViewModel::addNotification(Notify_Funtion func) { vmTrigger.add_notification(std::move(func)); }
