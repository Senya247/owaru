#include <owaru/commands.hpp>
#include <owaru/owaru.hpp>
#include <string>

namespace Owaru {
std::string Owaru::get_token() { return _token; }

Owaru::Owaru(const std::string &token) : dpp::cluster(token) {}

void Owaru::add_command(std::string name,
                        struct Commands::owaru_command command) {
    _commands.insert({name, command});
}

void Owaru::register_commands() {
    for (auto const &command : _commands) {
        global_command_create(dpp::slashcommand(
            command.second.call_name, command.second.description, me.id));
    }
}

void Owaru::call_command(std::string call_name,
                         const dpp::slashcommand_t &event) {
    for (auto const &command : _commands) {
        if (command.second.call_name == call_name)
            command.second.function(event);
    }
}

} // namespace Owaru
