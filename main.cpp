#include <dotenv.h>
#include <dpp/dpp.h>
#include <owaru/commands.hpp>
#include <owaru/owaru.hpp>
#include <unordered_map>

using namespace dotenv;

const static std::unordered_map<std::string,
                                struct Owaru::Commands::owaru_command>
    owaru_commands{
        {"ping", {"ping", "Standard ping command", Owaru::Commands::ping}},
    };

int main() {
    env.load_dotenv();
    const std::string BOT_TOKEN = env["TOKEN"];
    if (BOT_TOKEN.empty()) {
        std::cerr << "Could not find TOKEN variable in .env" << std::endl;
        return 1;
    }

    Owaru::Owaru owaru(BOT_TOKEN);
    for (auto const &command : owaru_commands) {
        owaru.add_command(command.second.call_name, command.second);
    }

    owaru.on_log(dpp::utility::cout_logger());

    owaru.on_slashcommand([&owaru](const dpp::slashcommand_t &event) {
        owaru.call_command(event.command.get_command_name(), event);
    });

    owaru.on_ready([&owaru](const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            owaru.register_commands();
        }
    });
    owaru.start(dpp::st_wait);
}
