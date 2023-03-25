#pragma once

#include <cerrno>
#include <cinttypes>
#include <csignal>
#include <cstring>
#include <optional>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

namespace Owaru {
namespace Rcon {

#define RCON_EXEC_COMMAND 2
#define RCON_AUTHENTICATE 3
#define RCON_RESPONSEVALUE 0
#define RCON_AUTH_RESPONSE 2
#define RCON_PID 0xBADC0DE

#define DATA_BUFFSIZE 4096

size_t write_all(int s, const void *buf, size_t len);
size_t read_all(int s, const void *buf, size_t len);

class RCON {
  private:
    struct rc_packet {
        int size;
        int id;
        int cmd;
        char data[DATA_BUFFSIZE];
        // ignoring string2 for now
    };

    int _fd = -1;
    int _port = -1;
    std::string _host;
    std::string _password;

    bool _is_connected = false;
    bool _is_authenticaed = false;

    int _connect(void);
    int _authenticate(void);
    int _send_packet(const struct rc_packet &packet);
    int _receive_packet(struct rc_packet &packet);
    int _clean_incoming(size_t size);
    int _packet_build(struct rc_packet &packet, int id, int cmd,
                      const std::string &s1);

  public:
    /* Connect, and if password is specified, authenticate to host*/
    RCON(const std::string &host, int port, const std::string &password = "");
    RCON();

    bool is_connected();
    bool is_authenticated();

    /* send command, returns response as string*/
    std::optional<std::string> send(const std::string &string);

    int disconnect();
};
} // namespace Rcon
} // namespace Owaru
