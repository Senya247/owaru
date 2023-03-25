#include <dpp/dpp.h>
#include <functional>
#include <owaru/commands.hpp>
#include <owaru/owaru.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace Owaru {
class Owaru;
namespace Commands {

void ping(const dpp::slashcommand_t &event, Owaru &instance) {
    event.reply("Pong");
}

void rcon_send(const dpp::slashcommand_t &event, Owaru &instance) {
    if (!instance.is_rconned()) {
        event.reply("RCON has not been initialized");
        return;
    }

    std::string command = std::get<std::string>(event.get_parameter("command"));
    if (command.empty())
        return;

    std::optional<std::string> response = instance.rcon_send(command);
    if (response)
        event.reply(response.value());
    else
        event.reply("Sent");
}

} // namespace Commands
} // namespace Owaru
