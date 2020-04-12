#include "lamport_clock.h"

#define ASIO_STANDALONE
#include "asio.hpp"
#include "CLI11.hpp"
#include "spdlog/spdlog.h"

#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace asio::ip;
using namespace chrono_literals;

void listen_func(unsigned short port, LamportClock& clock) {
    asio::io_context ctx;
    tcp::endpoint ep{tcp::v4(), port};
    tcp::acceptor acc{ctx, ep};
    acc.listen();
    string msg;
    lamport_clock_t time;
    while (true) {
        tcp::iostream strm{acc.accept()};
        getline(strm, msg);
        strm >> time;
        clock.receive(time);
        spdlog::info("Received Message: {} / Lamport Time: {}", msg, clock.get_time());
    }
}

void connect1(unsigned short connect, unsigned short port, LamportClock& clock) {
    tcp::iostream strm{"localhost", to_string(connect)};
    if (strm) {
        clock.update();
        spdlog::info("One event... / Lamport Time: {}", clock.get_time());
        this_thread::sleep_for(3s);
        clock.update();
        spdlog::info("Another event... / Lamport Time: {}", clock.get_time());
        clock.update();
        strm << "Hello from " << port << endl;
        strm << clock.get_time() << endl;
        spdlog::info("Sent a message. / Lamport Time: {}", clock.get_time());
    }
    else {
        spdlog::error("Failed to connect");
    }
}

void connect2(unsigned short connect, unsigned short port, LamportClock& clock) {
    tcp::iostream strm{"localhost", to_string(connect)};
    if (strm) {
        clock.update();
        spdlog::info("Yet another event... / Lamport Time: {}", clock.get_time());
        this_thread::sleep_for(1s);
        clock.update();
        strm << "Bye from " << port << endl;
        strm << clock.get_time() << endl;
        spdlog::info("Sent a message. / Lamport Time: {}", clock.get_time());
    }
    else {
        spdlog::error("Failed to connect");
    }
}

int main(int argc, char** argv) {
    CLI::App app{"Starts a process to simulate a lamport clock"};

    unsigned short port;
    app.add_option("-p,--port", port, "The port to listen on")->required();

    vector<unsigned short> connects;
    app.add_option("-c,--connects", connects, "The ports to connect to")->required()->expected(2);

    CLI11_PARSE(app, argc, argv);

    LamportClock clock;

    thread t{listen_func, port, ref(clock)};

    cout << "Listening. Press any key to connect..." << endl;
    cin.get();

    thread c1{connect1, connects[0], port, ref(clock)};
    thread c2{connect2, connects[1], port, ref(clock)};

    t.join();
}