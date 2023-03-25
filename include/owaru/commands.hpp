#pragma once

#include <dpp/dpp.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Owaru {
class Owaru;
namespace Commands {

struct owaru_command {
    std::string call_name;
    std::string description;
    std::vector<dpp::command_option> options;
    Owaru &instance;

    std::function<void(const dpp::slashcommand_t &event, Owaru &instance)>
        function;
};

/* Owaru ping command */
void ping(const dpp::slashcommand_t &event, Owaru &instance);

void rcon_send(const dpp::slashcommand_t &event, Owaru &instance);
} // namespace Commands

} // namespace Owaru
