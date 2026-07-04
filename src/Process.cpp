#include "Process.h"

Process::Process(std::string label, double bt, double at, int queue, int priority)
    : label(std::move(label)), bt(bt), at(at), queue(queue), priority(priority),
      remaining(bt), startTime(-1), completionTime(-1) {}

bool Process::finished() const {
    return remaining <= 0.0;
}

double Process::turnaroundTime() const {
    return completionTime - at;
}

double Process::waitingTime() const {
    return turnaroundTime() - bt;
}

double Process::responseTime() const {
    return startTime - at;
}
