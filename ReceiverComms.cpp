
#include "ReceiverComms.h"
#include <iostream>
#include "json.hpp"

ReceiverComms::ReceiverComms(asio::io_service& ioService, std::function<void(std::string)> handlerFunc) :
    _socket(ioService, asio::ip::udp::endpoint(asio::ip::udp::v4(), 19443)),
    _receivedEndpoint(false),
    _handlerFunc(handlerFunc)
{
    Receive();
}

void ReceiverComms::Receive()
{
    _socket.async_receive_from(asio::buffer(_buffer, _buffer.size()),
        _remoteEndpoint,
        [this](std::error_code ec, std::size_t bytesRecv)
    {
        //std::cout << "received from " << _remoteEndpoint << "\n";
        if (!ec && bytesRecv > 0)
        {
            _receivedEndpoint = true;
            _handlerFunc(std::string(_buffer.data(), bytesRecv));
        }

        Receive();
    });
}

void ReceiverComms::SendTelemetry(const std::string& str)
{
    if (!_receivedEndpoint)
        return;

    _socket.send_to(asio::buffer(str.c_str(), str.size()), _remoteEndpoint);
}