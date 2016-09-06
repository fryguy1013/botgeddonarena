#pragma once

#include <array>
#include <asio.hpp>
#include <functional>

class ReceiverComms
{
public:
    ReceiverComms(asio::io_service& ioService, std::function<void(std::string)> handlerFunc);
    void Receive();
    void SendTelemetry(const std::string& data);

private:
    asio::ip::udp::socket _socket;
    asio::ip::udp::endpoint _remoteEndpoint;
    bool _receivedEndpoint;
    std::array<char, 1024> _buffer;
    std::function<void(std::string)> _handlerFunc;
};
