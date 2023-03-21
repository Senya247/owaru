#include <dpp/dpp.h>
#include <functional>
#include <owaru/commands.hpp>
#include <owaru/owaru.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace Owaru {
namespace Commands {

void ping(const dpp::slashcommand_t &event) { event.reply("Pong"); }

} // namespace Commands
} // namespace Owaru
