#pragma once

#include <dpp/dpp.h>
#include <owaru/command/command.hpp>
#include <vector>

namespace Owaru::Owaru_Commands {

class Ping : public Command::Command {
  public:
    Ping();
    void operator()(const dpp::slashcommand_t &event) override;
};
} // namespace Owaru::Owaru_Commands
