#pragma once
#include <cstdint>
#include <memory>

class Server {
public:
    Server();
    ~Server();
    void run(uint16_t port, size_t max_clients);
    void stop();

private:
    bool m_running;
    void processNetwork();

    struct Impl;
    std::unique_ptr<Impl> m_enet_impl;
};