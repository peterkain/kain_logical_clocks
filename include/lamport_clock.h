#pragma once
#include <atomic>
#include <algorithm>

using lamport_clock_t = unsigned long long;

class LamportClock {
  public:
    LamportClock(lamport_clock_t start = 0) {
        counter.store(start);
    }

    lamport_clock_t get_time() const {
        return counter.load();
    }

    void update() {
        counter.store(counter.load() + 1);
    }

    void receive(lamport_clock_t time) {
        counter.store(std::max(time, counter.load()) + 1);
    }
    
  private:
    std::atomic<lamport_clock_t> counter;
};