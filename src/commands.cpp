#include <dpp/dpp.h>
#include <filesystem>
#include <fmt/core.h>
#include <functional>
#include <inttypes.h>
#include <mpg123.h>
#include <owaru/commands.hpp>
#include <owaru/owaru.hpp>
#include <rapidfuzz/fuzz.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;
namespace fuzz = rapidfuzz::fuzz;

namespace Owaru {
class Owaru;
namespace Commands {

/* Convenience function, send message to channel*/
auto _message(const std::string &content, const dpp::slashcommand_t &event,
              Owaru &instance) {
    return instance.message_create(
        dpp::message(event.command.channel_id, content));
}

/* return max element from map from A to B on the basis of B*/
template <typename A, typename B> auto _max(const std::map<A, B> &map) {
    return std::max_element(
        std::begin(map), std::end(map),
        [](const std::pair<A, B> &p1, const std::pair<A, B> &p2) {
            return p1.second < p2.second;
        });
}

void _stream_audio(dpp::voiceconn &voiceconn, const fs::path &path) {
    int err = 0;
    size_t buffer_size, done;
    int channels, encoding;
    long rate;

    uint8_t *buffer;
    std::vector<uint8_t> pcm_data;

    if (!fs::exists(path)) {
        std::cerr << "Path " << fs::canonical(path) << " does not exist"
                  << std::endl;
        return;
    }
    std::cout << "Playing " << fs::canonical(path) << std::endl;

    mpg123_handle *handle = mpg123_new(NULL, &err);
    mpg123_param(handle, MPG123_FORCE_RATE, 48000, 48000.0); /* Force 48Mhz*/

    buffer_size = mpg123_outblock(handle);
    buffer = new uint8_t[buffer_size];

    mpg123_open(handle, fs::canonical(path).c_str());
    mpg123_getformat(handle, &rate, &channels, &encoding);

    unsigned int counter = 0;
    for (int total = 0;
         mpg123_read(handle, buffer, buffer_size, &done) == MPG123_OK;) {
        for (auto i = 0; i < buffer_size; i++)
            pcm_data.push_back(buffer[i]);
        counter += buffer_size;
        total += done;
    }
    delete buffer;

    mpg123_close(handle);
    mpg123_delete(handle);

    if (voiceconn.voiceclient->is_ready())
        voiceconn.voiceclient->send_audio_raw((uint16_t *)pcm_data.data(),
                                              pcm_data.size());
}

#define _MESSAGE(CONTENT) _message(CONTENT, event, instance)

void ping(const dpp::slashcommand_t &event, Owaru &instance) {
    event.reply("Pong");
}

void rcon_send(const dpp::slashcommand_t &event, Owaru &instance) {
    if (!instance.is_rconned()) {
        event.reply("RCON has not been initialized");
        return;
    }

    std::string command = std::get<std::string>(event.get_parameter("command"));
    if (command.empty())
        return;

    std::optional<std::string> response = instance.rcon_send(command);
    if (response)
        event.reply(response.value());
    else
        event.reply("Sent");
}

void test(const dpp::slashcommand_t &event, Owaru &instance) {
    dpp::guild guild = event.command.get_guild();
    dpp::user author = event.command.get_issuing_user();

    event.reply(std::to_string(author.id));
}

void vc_join(const dpp::slashcommand_t &event, Owaru &instance) {
    dpp::guild guild = event.command.get_guild();
    dpp::user author = event.command.get_issuing_user();

    dpp::voiceconn *current_vc = event.from->get_voice(guild.id);

    /* Map from user id to voicecon class*/
    auto voice_members_map = guild.voice_members;

    /* voice channel of author, std::pair of snowflake and
dpp::voicestate*/
    auto author_vc = voice_members_map.find(author.id);

    dpp::snowflake author_vc_snowflake = author_vc->second.channel_id;

    /* if user not in vc
            you dont seem to be in a vc
            return

    # User definitely in some vc now
    if we're in vc
            if same vc as user
                    disconnect
            else # not in same vc as user
                    disconnect from current vc
                    join_vc = true
    else # we're not in vc
            join_vc = true

    if (join_vc == true)
            join vc */

    /* So discord doesn't say "Applicatino didn't respond"*/

    bool join_vc = false;
    if (author_vc == voice_members_map.end()) {
        event.reply("You don't seem to be connected to a voice chat");
        return;
    }
    if (current_vc) {
        if (current_vc->channel_id == author_vc->second.channel_id) {
            event.reply(
                fmt::format("Disconnecting from <#{}>", author_vc_snowflake));
            event.from->disconnect_voice(guild.id);
        } else {
            event.from->disconnect_voice(guild.id);
            join_vc = true;
        }
    } else {
        join_vc = true;
    }
    if (join_vc) {
        if (guild.connect_member_voice(author.id))
            event.reply(fmt::format("Joining <#{}>", author_vc_snowflake));
        else
            event.reply(
                fmt::format("Could not join <#{}>", author_vc_snowflake));
    }
}

void vc_play(const dpp::slashcommand_t &event, Owaru &instance) {
    static std::map<fs::path, double> audio_score_map;

    dpp::guild guild = event.command.get_guild();
    dpp::user author = event.command.get_issuing_user();

    dpp::voiceconn *current_vc = event.from->get_voice(guild.id);

    /* Map from user id to voicecon class*/
    auto voice_members_map = guild.voice_members;

    /* voice channel of author, std::pair of snowflake and
dpp::voicestate*/
    auto author_vc = voice_members_map.find(author.id);

    /* So discord doesn't say "Applicatino didn't respond"*/

    const std::string audio_name =
        std::get<std::string>(event.get_parameter("name"));
    if (audio_name.empty()) {
        event.reply("You must specify an audio name");
        return;
    }

    if (current_vc) {
        if (author_vc != voice_members_map.end()) {

            if (instance.get_audio_samples().empty()) {
                event.reply("Audio samples have not been initialized");
                return;
            }

            audio_score_map.clear();
            for (const auto &sample : instance.get_audio_samples()) {
                audio_score_map.emplace(
                    sample, fuzz::partial_ratio(audio_name,
                                                sample.filename().string()));
            }
            auto t = _max(audio_score_map);
            event.reply(t->first.filename());

            _stream_audio(*event.from->get_voice(guild.id), t->first);

        } else {
            event.reply("Call /vc first");
        }
    } else {
        event.reply("Call /vc first");
    }
}

} // namespace Commands
} // namespace Owaru
