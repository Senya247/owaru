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
  protected:
    Command(const std::string &name, const std::string &call_name,
            const std::string &description,
            const std::vector<dpp::command_option> &options);

    std::string _name;
    std::string _call_name;

    std::string _description;
    std::vector<dpp::command_option> _options;

    Owaru *_instance = NULL;

  public:
    virtual void operator()(const dpp::slashcommand_t &event);

    void set_instance(Owaru *instance);
    const Owaru *get_instance() const;

    std::string get_name() const;
    std::string get_call_name() const;
    std::string get_description() const;

    void add_option(const dpp::command_option &option);
    const std::vector<dpp::command_option> &get_options() const;
};

} // namespace Command

} // namespace Owaru
