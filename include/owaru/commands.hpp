#pragma once

#include <dpp/dpp.h>
#include <functional>
#include <string>
#include <unordered_map>

namespace Owaru {
namespace Commands {

struct owaru_command {
    std::string call_name;
    std::string description;

    std::function<void(const dpp::slashcommand_t &event)> function;
};

/* Owaru ping command */
void ping(const dpp::slashcommand_t &event);
} // namespace Commands

} // namespace Owaru
