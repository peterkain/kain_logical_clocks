#include "lamport_clock.h"

#define ASIO_STANDALONE
#include "asio.hpp"
#include "CLI11.hpp"
#include "spdlog/spdlog.h"

#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;
using namespace asio::ip;
using namespace chrono_literals;

void print_message(std::string msg, LamportClock& clock,
                   lamport_clock_t time = 0, bool receive = false) {
    static mutex m;
    lock_guard guard{m};
    if (!receive) {
        clock.update();
        spdlog::info("{} / Lamport Time: {}", msg, clock.get_time());
    }
    else {
        clock.receive(time);
        spdlog::info("Received Message: {} / Lamport Time: {}",
                     msg, clock.get_time());
    }
}

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
        print_message(msg, clock, time, true);
    }
}

void connect1(unsigned short connect, unsigned short port, LamportClock& clock) {
    tcp::iostream strm{"localhost", to_string(connect)};
    if (strm) {
        print_message("One event...", clock);
        this_thread::sleep_for(3s);
        print_message("Another event...", clock);
        print_message("Sending a message!", clock);
        strm << "Hello from " << port << endl;
        strm << clock.get_time() << endl;
    }
    else {
        spdlog::error("Failed to connect");
    }
}

void connect2(unsigned short connect, unsigned short port, LamportClock& clock) {
    tcp::iostream strm{"localhost", to_string(connect)};
    if (strm) {
        print_message("Yet another event...", clock);
        this_thread::sleep_for(1s);
        print_message("Sending a message!", clock);
        strm << "Bye from " << port << endl;
        strm << clock.get_time() << endl;
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

    c1.join();
    c2.join();
    t.join();
}