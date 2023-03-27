#include <dotenv.h>
#include <dpp/dpp.h>
#include <filesystem>
#include <owaru/commands.hpp>
#include <owaru/owaru.hpp>
#include <unordered_map>

using namespace dotenv;

int main() {
    env.load_dotenv();
    const std::string BOT_TOKEN = env["TOKEN"];
    const std::string RCON_PASS = env["RCON_PASS"];
    const std::string AUDIO_PATH = env["AUDIO_PATH"];

    if (BOT_TOKEN.empty()) {
        std::cerr << "Could not find TOKEN variable in .env" << std::endl;
        return 1;
    }

    Owaru::Owaru owaru(BOT_TOKEN);
    if (!AUDIO_PATH.empty())
        owaru.set_audio_path(AUDIO_PATH);
    /* I'm sorry for this*/
    const static std::unordered_map<std::string,
                                    struct Owaru::Commands::owaru_command>
        owaru_commands{
            {"Ping",
             {"ping",
              "Standard ping command",
              {},
              owaru,
              Owaru::Commands::ping}},
            {"RCON Send",
             {"send",
              "Send command through RCON",
              {dpp::command_option(dpp::co_string, "command", "Command to run",
                                   true)},
              owaru,
              Owaru::Commands::rcon_send}},
            {"Testing command",
             {"test", "Testing only", {}, owaru, Owaru::Commands::test}},
            {"Join Voice Channel",
             {"vc",
              "Join the voice channel of the calling user",
              {},
              owaru,
              Owaru::Commands::vc_join}},
            {"Play audio",
             {"play",
              "Play audio",
              {dpp::command_option(dpp::co_string, "name",
                                   "Audio sample to play", true)},
              owaru,
              Owaru::Commands::vc_play}}};
    for (auto const &command : owaru_commands) {
        owaru.add_command(command.second.call_name, command.second);
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
        std::cout << "Ready" << std::endl;
    });
    owaru.start(dpp::st_wait);
}
