#include <filesystem>
#include <fmt/core.h>
#include <mpg123.h>
#include <owaru/command/command.hpp>
#include <owaru/owaru/owaru.hpp>
#include <rang/rang.hpp>
#include <string>

namespace fs = std::filesystem;

namespace Owaru {
std::string Owaru::get_token() { return _token; }

Owaru::Owaru(const std::string &token) : dpp::cluster(token) {}
Owaru::~Owaru() { _rcon.disconnect(); }

void Owaru::show_warning(const std::string &message) const {
    using namespace rang;
    std::cerr << style::bold << fg::yellow << "[WARNING] " << fg::reset
              << style::reset;
    std::cerr << message << std::endl;
}

void Owaru::show_error(const std::string &message) const {
    using namespace rang;
    std::cerr << style::bold << fg::red << "[ERROR] " << fg::reset
              << style::reset;
    std::cerr << message << std::endl;
}
void Owaru::show_notice(const std::string &message) const {
    using namespace rang;
    std::cerr << style::bold << fg::green << "[NOTICE] " << fg::reset
              << style::reset;
    std::cerr << message << std::endl;
}

fs::path Owaru::get_audio_path() { return _audio_path; }

void Owaru::set_audio_path(const std::string &path_str) {
    auto path = fs::path(path_str);
    if (!fs::exists(path)) {
        show_warning(
            fmt::format("Path {} for audio samples does not exist", path_str));
        return;
    }

    _audio_samples.clear();
    _audio_path = path;

    for (const auto &entry : fs::directory_iterator(path))
        _audio_samples.push_back(entry.path());
}

const std::vector<fs::path> &Owaru::get_audio_samples() {
    return _audio_samples;
}

void Owaru::add_command(const Command::Command &command) {
    if (command.get_instance() == NULL)
    {
        show_error(fmt::format("Command {} must contain a pointer to Owaru class", command.get_call_name()));
        std::exit(1);
    }
    _commands.push_back(command);
}

void Owaru::register_commands() {
    for (auto const &command : _commands) {
        dpp::slashcommand slash_com(command.get_call_name(),
                                    command.get_description(), me.id);

        /* If command has options associated with it*/
        auto opts = command.get_options();
        if (!opts.empty())
            for (auto const &option : opts)
                slash_com.add_option(option);

        _slash_commands.push_back(slash_com);
    }

    global_bulk_command_create(_slash_commands);
}

void Owaru::call_command(std::string call_name,
                         const dpp::slashcommand_t &event) {
    show_notice(fmt::format("Called {}", call_name));
    for (auto &command : _commands) {
        if (command.get_call_name() == call_name)
            command.call(event);
    }
}

bool Owaru::is_rconned() { return _is_rconned; }

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
