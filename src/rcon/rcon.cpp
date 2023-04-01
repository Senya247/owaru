#include <arpa/inet.h>
#include <cerrno>
#include <cinttypes>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <optional>
#include <owaru/rcon.hpp>
#include <sys/socket.h>
#include <unistd.h>

namespace Owaru {
namespace Rcon {

size_t write_all(int s, const void *buf, size_t len) {
    size_t total = 0;       // how many bytes we’ve sent
    size_t bytesleft = len; // how many we have left to send
    size_t n;
    while (total < len) {
        n = write(s, (char *)buf + total, bytesleft);
        if (n == -1) {
            perror("write_all");
            break;
        }
        total += n;
        bytesleft -= n;
    }
    return n == -1 ? -1
                   : total; // return -1 on failure, bytes written on success
}

size_t read_all(int s, void *buf, size_t len) {
    size_t total = 0;       // how many bytes we've sent
    size_t bytesleft = len; // how many we have left to send
    size_t n;

    while (total < len) {
        n = read(s, (char *)buf + total, bytesleft);
        if (n == -1 || n == 0) {
            perror("read_all");
            break;
        }
        total += n;
        bytesleft -= n;
    }

    return n == -1 ? -1 : total; // return -1 on failure, bytes read on success
}

RCON::RCON(const std::string &host, int port, const std::string &password)
    : _host(host), _port(port), _password(password) {
    if (_is_connected)
        return;

    if (_connect() == -1)
        return;
    _is_connected = true;

    if (!password.empty()) {
        if (_authenticate() == -1)
            return;
        _is_authenticaed = true;
    }
}

RCON::RCON() {}

bool RCON::is_connected() { return _is_connected; }
bool RCON::is_authenticated() { return _is_authenticaed; }

int RCON::_authenticate() {
    if (!_is_connected)
        return -1;

    int ret;
    struct rc_packet packet;

    if (_packet_build(packet, RCON_PID, RCON_AUTHENTICATE, _password) != 0)
        return -1;

    ret = _send_packet(packet);
    if (ret == -1)
        return -1;

    /* Can reuse packet to receive, I guess*/
    if (_receive_packet(packet) != 0) {
        return -1;
    }

    ret = packet.id;

    // TODO read protocol to see if auth was successfull
    // return 1 if authentication OK
    return ret == -1 ? -1 : 0;
}

int RCON::_connect(void) {
    int fd;
    struct sockaddr_in addr;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    if (inet_pton(AF_INET, _host.c_str(), &addr.sin_addr) <= 0)
        return -1;

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        return -1;
    _fd = fd;

    return 0;
}

/* Try to send packet, return -1 on any error */
int RCON::_send_packet(const struct rc_packet &packet) {
    if (!_is_connected)
        return -1;

    int ret, len;
    len = packet.size + sizeof(int);

    ret = write_all(_fd, &packet, len);

    return ret == -1 || ret != len ? -1 : 0;
}

int RCON::_receive_packet(struct rc_packet &packet) {
    int psize, ret;
    memset(&packet, 0, sizeof(packet));

    /* receiving the 'size' member of rcon packet, i guess */
    /*     int ret = read_all(_fd, &psize, sizeof(psize)); */
    ret = read_all(_fd, &psize, sizeof(psize));

    if (ret == -1 || ret != sizeof(int))
        return -1;

    // NOTE(Tiiffi): This should fail if size is out of spec!
    if (psize < 10 || psize > DATA_BUFFSIZE) {
        _clean_incoming(psize);
        return -1;
    }

    packet.size = psize;

    ret = read_all(_fd, (char *)&packet + sizeof(int), psize);

    return ret == -1 || ret != psize ? -1 : 0;
}

int RCON::_clean_incoming(size_t size) {
    char tmp[size];

    int ret = read_all(_fd, tmp, size);
    return ret;
}

int RCON::_packet_build(struct rc_packet &packet, int id, int cmd,
                        const std::string &s1) {

    memset(&packet, 0, sizeof(packet));
    if (s1.length() >= DATA_BUFFSIZE)
        return -1;

    packet.size = sizeof(int) * 2 + s1.length() + 2;
    packet.id = id;
    packet.cmd = cmd;
    strncpy(packet.data, s1.c_str(), DATA_BUFFSIZE - 1);

    return 0;
}

std::optional<std::string> RCON::send(const std::string &string) {
    struct rc_packet packet;
    _packet_build(packet, RCON_PID, RCON_EXEC_COMMAND, string);

    if (_send_packet(packet) != 0)
        return std::nullopt;

    /* reuse packet struct*/
    if (_receive_packet(packet) != 0)
        return std::nullopt;
    if (packet.id != RCON_PID)
        return std::nullopt;

    return packet.data;
}

int RCON::disconnect() { return close(_fd); }

} // namespace Rcon
} // namespace Owaru
