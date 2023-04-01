#include <owaru/command/command.hpp>
#include <vector>

namespace Owaru::Owaru_Commands {

/* Owaru ping command */
void ping(const dpp::slashcommand_t &event, Owaru &instance);

void rcon_send(const dpp::slashcommand_t &event, Owaru &instance);

void test(const dpp::slashcommand_t &event, Owaru &instance);

void vc(const dpp::slashcommand_t &event, Owaru &instance);

void vc_play(const dpp::slashcommand_t &event, Owaru &instance);

extern std::vector<Command::Command> owaru_commands;

} // namespace Owaru::Owaru_Commands
