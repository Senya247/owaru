#include <dpp.h>
#include <owaru/command/command.hpp>
#include <vector>

namespace Owaru {
class Owaru;
namespace Command {

Command::Command(const std::string &name, const std::string &call_name,
                 const std::string &description,
                 const std::vector<dpp::command_option> &options)
    : _name(name), _call_name(call_name), _description(description),
      _options(options) {}
void Command::operator()(const dpp::slashcommand_t &event) {}

void Command::set_instance(Owaru *instance) { _instance = instance; }
const Owaru *Command::get_instance() const { return _instance; }

std::string Command::get_name() const { return _name; }

std::string Command::get_call_name() const { return _call_name; }

std::string Command::get_description() const { return _description; }

void Command::add_option(const dpp::command_option &option) {
    _options.push_back(option);
}

const std::vector<dpp::command_option> &Command::get_options() const {
    return _options;
}

} // namespace Command
} // namespace Owaru
