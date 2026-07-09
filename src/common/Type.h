//

#ifndef MARIO_TYPE_H
#define MARIO_TYPE_H

#include "common/ICommand.h"

using PositionType = float;
using EventType = uint32_t;
using Notify_Funtion = std::function<void(EventType)>;

template <typename T>
class TypeParameter : public ICommandParameter {
public:
  T v_;
};

#endif // MARIO_TYPE_H
