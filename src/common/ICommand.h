// Command Interface

#ifndef MARIO_ICOMMAND_H
#define MARIO_ICOMMAND_H

class ICommandParameter {
 public:
  virtual ~ICommandParameter() noexcept {}
};

class ICommandBase {
 public:
  virtual int exec(ICommandParameter* p) = 0;
};

#endif  // MARIO_ICOMMAND_H
