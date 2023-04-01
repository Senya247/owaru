#pragma once

#include <dpp/dpp.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Owaru {
class Owaru;
namespace Command {

typedef std::function<void(const dpp::slashcommand_t &event, Owaru &instance)>
    owaru_function_type;

class Command {
  private:
    std::string _name;
    std::string _call_name;

    std::string _description;
    std::vector<dpp::command_option> _options;

    owaru_function_type _function;
    Owaru *_instance = NULL;

  public:
    explicit Command(std::string name, std::string call_name,
                     std::string description,
                     std::vector<dpp::command_option> options,
                     owaru_function_type function); /* TODO */

    void set_instance(Owaru *instance);
    const Owaru * get_instance() const;

    std::string get_name() const;
    std::string get_call_name() const;
    std::string get_description() const;

    void add_option(const dpp::command_option &option);
    const std::vector<dpp::command_option> &get_options() const;

    void call(const dpp::slashcommand_t &event);
};

} // namespace Command

} // namespace Owaru
