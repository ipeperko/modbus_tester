#include "server_session.h"
#include <sys/socket.h>
#include <sstream>
#include <QDebug>
#include <unistd.h>

server_session::server_session(int port)
{
    mb_map = modbus_mapping_new(buffer_size_coils, buffer_size_coils, buffer_size_holding_register, buffer_size_input_registers);

    ctx = modbus_new_tcp("0.0.0.0", port);
    if (!ctx) {
        on_error("Cannot create new Modbus tcp connection");
    }
}

server_session::~server_session()
{
    stop_server();

    if (mb_map) {
        modbus_mapping_free(mb_map);
    }
}

void server_session::start_server()
{
    // Stop server if running
    stop_server();

    // Enable task execution
    do_run = true;

    // Start thread
    thr = std::thread([this]() {
        try {
            task();
        }
        catch (std::exception& e) {
            emit message_emitter.error_message(e.what());
        }
    });
}

void server_session::stop_server()
{
    // Disable task execution
    do_run = false;

    // Shutdown listener socket
    if (sock_listen >= 0) {
        shutdown(sock_listen, SHUT_RDWR);
        sock_listen = -1;
    }

    // Shutdown accept socket
    if (sock_accept >= 0) {
        shutdown(sock_accept, SHUT_RDWR);
        sock_accept = -1;
    }

    // Join thread
    if (thr.joinable()) {
        thr.join();
    }
}

void server_session::task()
{
    qDebug() << "Server task started";

    sock_listen = modbus_tcp_listen(ctx, 1);
    if (sock_listen < 0) {
        on_error("Modbus tcp server socket failed");
    }

    while (do_run) {
        qDebug() << "Server accepting...";
        int rc = modbus_tcp_accept(ctx, &sock_listen);
        qDebug() << "Server accepted - code : " << rc;

        if (rc < 0) {
            qWarning() << "TCP accept failed";
            std::this_thread::yield();
            continue;
        }

        sock_accept = rc;

        int nsend;

        do {
            std::vector<uint8_t> query(MODBUS_TCP_MAX_ADU_LENGTH, 0);

            qDebug() << "Server receiving...";
            int nrcv = modbus_receive(ctx, &query[0]);
            qDebug() << "Server received " << nrcv;
            if (nrcv < 10) {
                qWarning() << "Modbus server received < 10 bytes (" << nrcv << ") - break";
                close(sock_accept);
                sock_accept = -1;
                break;
            }

            query.resize(nrcv);
            nsend = server_reply(query);

            if (nsend < 0) {
                qWarning() << "Modbus sending error - %s" << modbus_strerror(errno);
            } else {
                qDebug() << "Replying to request bytes : " << nsend;
            }

            close(sock_accept);
            sock_accept = -1;

        } while(nsend > 0);
    }

    qDebug() << "Server task finished";
}

int server_session::server_reply(const std::vector<uint8_t>& query)
{
    unsigned unitid = query[6];
    unsigned cmd = query[7];

    {
        std::ostringstream os;
        os << "Received cmd " << cmd << " (0x" << std::hex << cmd << std::dec <<
            ") from device id 0x" << std::hex << unitid << std::dec <<
            " (num bytes : " << query.size() << ") :";

        for (auto it : query) {
            os << " " << std::hex << static_cast<int>(it) << std::dec;
        }

        emit message_emitter.message(os.str().c_str());
    }

    return modbus_reply(ctx, &query[0], query.size(), mb_map);
}
