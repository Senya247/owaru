#pragma once
#include <dpp/dpp.h>
#include <filesystem>
#include <functional>
#include <mpg123.h>
#include <owaru/commands.hpp>
#include <owaru/rcon.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace Owaru {

class Owaru : public dpp::cluster {
  private:
    std::string _token;
    Rcon::RCON _rcon;

    /* whether rcon has been initialized */
    bool _is_rconned = false;

    /* Map of commands */
    std::unordered_map<std::string, struct Commands::owaru_command> _commands;
    /* Used in dpp::global_bulk_command_create*/
    std::vector<dpp::slashcommand> _slash_commands;

    /* Directory for music/whatever audio and vector of those paths*/
    fs::path _audio_path;
    std::vector<fs::path> _audio_samples;

  public:
    Owaru(const std::string &token);
    ~Owaru();

    mpg123_handle *_mpg_handle;
    int _mpg_err;

    std::string get_token();

    /* get/set path to get audio sampled from*/
    void set_audio_path(const fs::path path);
    fs::path get_audio_path();

    const std::vector<fs::path> &get_audio_samples();

    void rcon_init(std::string host, int port, const std::string &password);
    void rcon_close();
    bool is_rconned();
    std::optional<std::string> rcon_command(const std::string &command);

    /* Add command to commands map (_commands) */
    void add_command(std::string name, struct Commands::owaru_command command);

    /* Register all commands in _commands map with discord, for the slash
     * autocomplete thing*/
    void register_commands(void);

    /* Call a discord command in a given context*/
    void call_command(std::string call_name, const dpp::slashcommand_t &event);

    std::optional<std::string> rcon_send(const std::string &command);
};

} // namespace Owaru
