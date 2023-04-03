#include <csignal>
#include <dotenv.h>
#include <dpp/dpp.h>
#include <filesystem>
#include <fmt/core.h>
#include <owaru/command/command.hpp>
#include <owaru/owaru/commands/ping.hpp>
#include <owaru/owaru/owaru.hpp>
#include <unordered_map>

Owaru::Owaru *ptr_owaru = NULL;

void signal_handler(int sig) {
    if (sig == SIGHUP)
        return;

    ptr_owaru->show_notice(
        fmt::format("Received {}, exiting...", strsignal(sig)));

    if (ptr_owaru == NULL)
        exit(1);

    ptr_owaru->shutdown();
    ptr_owaru->show_notice("Shut down gracefully");
    exit(0);
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    dotenv::env.load_dotenv();
    const std::string BOT_TOKEN = dotenv::env["TOKEN"];
    const std::string RCON_PASS = dotenv::env["RCON_PASS"];
    const std::string AUDIO_PATH = dotenv::env["AUDIO_PATH"];

    if (BOT_TOKEN.empty()) {
        std::cerr << "Could not find TOKEN variable in .env" << std::endl;
        return 1;
    }

    Owaru::Owaru owaru(BOT_TOKEN);
    ptr_owaru = &owaru;
    if (!AUDIO_PATH.empty())
        owaru.set_audio_path(AUDIO_PATH);

    std::vector<Owaru::Command::Command> coms;
    {
        using namespace Owaru::Owaru_Commands;
        coms = {Ping()};
    }

    for (auto &command : coms) {
        command.set_instance(&owaru);
        owaru.add_command(command);
    }

    owaru.rcon_init("100.67.113.112", 25575, RCON_PASS);
    if (!owaru.is_rconned()) {
        std::cerr << "Could not initialize RCON" << std::endl;
        return 1;
    }

    owaru.on_log(dpp::utility::cout_logger());

    owaru.on_slashcommand([&owaru](const dpp::slashcommand_t &event) {
        owaru.call_command(event.command.get_command_name(), event);
    });

    owaru.on_ready([&owaru](const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            owaru.register_commands();
        }
        owaru.show_notice("Ready");
    });
    owaru.start(dpp::st_wait);
}
