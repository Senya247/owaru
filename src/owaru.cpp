#include <owaru/commands.hpp>
#include <owaru/owaru.hpp>
#include <string>

namespace Owaru {
std::string Owaru::get_token() { return _token; }

Owaru::Owaru(const std::string &token) : dpp::cluster(token) {}

bool Owaru::is_rconned() { return _is_rconned; }

void Owaru::add_command(std::string name,
                        struct Commands::owaru_command command) {
    _commands.insert({name, command});
}

void Owaru::register_commands() {
    for (auto const &command : _commands) {
        dpp::slashcommand c(command.second.call_name,
                            command.second.description, me.id);

        /* If command has options associated with it*/
        if (!command.second.options.empty())
            for (auto const &option : command.second.options)
                c.add_option(option);

        _slash_commands.push_back(c);
    }

    global_bulk_command_create(_slash_commands);
}

void Owaru::call_command(std::string call_name,
                         const dpp::slashcommand_t &event) {
    for (auto const &command : _commands) {
        if (command.second.call_name == call_name)
            command.second.function(event, *this);
    }
    std::cout << "Called " << call_name << std::endl;
}

void Owaru::rcon_init(std::string host, int port, const std::string &password) {
    if (_is_rconned)
        return;
    _rcon = Rcon::RCON(host, port, password);
    _is_rconned = _rcon.is_connected();
}

std::optional<std::string> Owaru::rcon_send(const std::string &command) {
    return _rcon.send(command);
}

} // namespace Owaru
