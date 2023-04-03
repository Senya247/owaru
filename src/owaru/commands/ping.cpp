#include <dpp/dpp.h>
#include <owaru/command/command.hpp>
#include <owaru/owaru/commands/ping.hpp>

namespace Owaru::Owaru_Commands {
Ping::Ping() : Command::Command("Ping", "ping", "Standard ping command", {}){};
void Ping::operator()(const dpp::slashcommand_t &event) { event.reply("Pong"); }

} // namespace Owaru::Owaru_Commands
