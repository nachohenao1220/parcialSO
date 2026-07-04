#include "SchedulerQueue.h"
#include <algorithm>

SchedulerQueue::SchedulerQueue(PolicyType type, double quantum)
    : type(type), quantum(quantum) {}

void SchedulerQueue::add(Process* p) {
    ready.push_back(p);
}

void SchedulerQueue::requeue(Process* p) {
    // Un proceso expropiado simplemente vuelve a la lista de espera;
    // cada politica decide despues, en popNext(), a quien le toca.
    ready.push_back(p);
}

bool SchedulerQueue::hasReady() const {
    return !ready.empty();
}

// Desempate estandar usado en todas las politicas: el que llego primero
// (menor AT); si hay empate, primero por prioridad, luego por etiqueta, 
//para que el resultado sea siempre el mismo (determinista).
static bool arrivedBefore(const Process* a, const Process* b) {
    if (a->at != b->at) return a->at < b->at;
    if (a->priority != b->priority) {
        return a->priority > b->priority;
    }
    return a->label < b->label;
}

int SchedulerQueue::indexOfShortestBurst() const {
    int best = 0;
    for (size_t i = 1; i < ready.size(); i++) {
        if (ready[i]->bt < ready[best]->bt ||
            (ready[i]->bt == ready[best]->bt && arrivedBefore(ready[i], ready[best]))) {
            best = (int)i;
        }
    }
    return best;
}

int SchedulerQueue::indexOfShortestRemaining() const {
    int best = 0;
    for (size_t i = 1; i < ready.size(); i++) {
        if (ready[i]->remaining < ready[best]->remaining ||
            (ready[i]->remaining == ready[best]->remaining && arrivedBefore(ready[i], ready[best]))) {
            best = (int)i;
        }
    }
    return best;
}

int SchedulerQueue::indexOfHighestPriority() const {
    int best = 0;
    for (size_t i = 1; i < ready.size(); i++) {
        if (ready[i]->priority > ready[best]->priority ||
            (ready[i]->priority == ready[best]->priority && arrivedBefore(ready[i], ready[best]))) {
            best = (int)i;
        }
    }
    return best;
}

Process* SchedulerQueue::popNext() {
    int idx = 0;
    switch (type) {
        case PolicyType::RR:
            idx = 0;  // FIFO: siempre el primero de la fila
            break;
        case PolicyType::SJF:
            idx = indexOfShortestBurst();
            break;
        case PolicyType::STCF:
            idx = indexOfShortestRemaining();
            break;
        case PolicyType::PRIORITY:
            idx = indexOfHighestPriority();
            break;
    }
    Process* chosen = ready[idx];
    ready.erase(ready.begin() + idx);
    return chosen;
}

bool SchedulerQueue::hasShorterRemaining(const Process* current) const {
    if (ready.empty()) return false;
    int idx = indexOfShortestRemaining();
    return ready[idx]->remaining < current->remaining;
}
