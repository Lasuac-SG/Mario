//

#ifndef MARIO_COMMANDPARAMETER_H
#define MARIO_COMMANDPARAMETER_H
#include "ICommand.h"

template <typename T>
class TypeParameter : public ICommandParameter {
 public:
  T v;
};
// TODO: 来自Book的示例, 需要使用command进行命令绑定吗?
typedef TypeParameter<double> PriceParameter;

#endif  // MARIO_COMMANDPARAMETER_H
