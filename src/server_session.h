#ifndef MODBUS_TESTER_SERVER_SESSION_H
#define MODBUS_TESTER_SERVER_SESSION_H

#include "session_base.h"
#include <thread>

class server_session : public session_base
{
    Q_OBJECT
public:
    server_session(int port);
    ~server_session() override;

    void start_server();
    void stop_server();

    modbus_mapping_t* mb_map {nullptr};

    static constexpr size_t buffer_size_bits = 1024;
    static constexpr size_t buffer_size_coils = 1024;
    static constexpr size_t buffer_size_holding_register = 1024;
    static constexpr size_t buffer_size_input_registers = 1024;

private:
    void task();
    int server_reply(const std::vector<uint8_t>& query);

    int sock_listen {-1};
    int sock_accept {-1};
    std::thread thr;
    bool do_run {false};

signals:
    void message(const QString& msg);
    void error_message(const QString& msg);
};

#endif //MODBUS_TESTER_SERVER_SESSION_H
