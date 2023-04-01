#include <dpp.h>
#include <owaru/command/command.hpp>
#include <vector>

namespace Owaru {
class Owaru;
namespace Command {

Command::Command(std::string name, std::string call_name,
                 std::string description,
                 std::vector<dpp::command_option> options,
                 owaru_function_type function)
    : _name(name), _call_name(call_name), _description(description),
      _options(options), _function(function) {}

void Command::set_instance(Owaru *instance) { _instance = instance; }
const Owaru * Command::get_instance() const {return _instance;}

std::string Command::get_name() const { return _name; }

std::string Command::get_call_name() const { return _call_name; }

std::string Command::get_description() const { return _description; }

void Command::add_option(const dpp::command_option &option) {
    _options.push_back(option);
}

const std::vector<dpp::command_option> &Command::get_options() const {
    return _options;
}

void Command::call(const dpp::slashcommand_t &event) {
    _function(event, *_instance);
}

} // namespace Command
} // namespace Owaru
