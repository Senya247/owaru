#pragma once
#include <dpp/dpp.h>
#include <functional>
#include <owaru/commands.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace Owaru {

class Owaru : public dpp::cluster {
  private:
    std::string _token;

    /* Map of commands */
    std::unordered_map<std::string, struct Commands::owaru_command> _commands;

  public:
    Owaru(const std::string &token);

    std::string get_token();

    /* Add command to commands map (_commands) */
    void add_command(std::string name, struct Commands::owaru_command command);

    /* Register all commands in _commands map with discord, for the slash
     * autocomplete thing*/
    void register_commands(void);

    /* Call a command in a given context*/
    void call_command(std::string call_name, const dpp::slashcommand_t &event);
};

} // namespace Owaru
