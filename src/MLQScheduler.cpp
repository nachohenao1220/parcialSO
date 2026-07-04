#include "MLQScheduler.h"
#include <algorithm>

MLQScheduler::MLQScheduler(std::vector<SchedulerQueue> queues)
    : queues(std::move(queues)) {}

int MLQScheduler::topReadyQueueIndex() const {
    for (size_t i = 0; i < queues.size(); i++) {
        if (queues[i].hasReady()) return (int)i;
    }
    return -1;
}

void MLQScheduler::run(std::vector<Process*>& processes) {
    // Procesos ordenados por llegada, para irlos liberando a sus colas.
    std::vector<Process*> pending = processes;
    std::sort(pending.begin(), pending.end(), [](Process* a, Process* b) {
        if (a->at != b->at) return a->at < b->at;
        return a->label < b->label;
    });

    int totalProcesses = (int)processes.size();
    int completed = 0;
    double time = 0;

    Process* running = nullptr;
    int runningQueueIdx = -1;
    double ticksInQuantum = 0;

    while (completed < totalProcesses) {
        // 1) liberar a sus colas los procesos que ya llegaron
        while (!pending.empty() && pending.front()->at <= time) {
            Process* p = pending.front();
            pending.erase(pending.begin());
            queues[p->queue - 1].add(p);
        }

        // 2) si hay un proceso corriendo, ver si toca expropiarlo
        if (running != nullptr) {
            int topQueue = topReadyQueueIndex();

            bool higherQueueReady = (topQueue != -1 && topQueue < runningQueueIdx);
            bool quantumExpired = (queues[runningQueueIdx].type == PolicyType::RR &&
                                    ticksInQuantum >= queues[runningQueueIdx].quantum);
            bool shorterJobArrived = (queues[runningQueueIdx].type == PolicyType::STCF &&
                                       queues[runningQueueIdx].hasShorterRemaining(running));

            if (higherQueueReady || quantumExpired || shorterJobArrived) {
                queues[runningQueueIdx].requeue(running);
                running = nullptr;
            }
        }

        // 3) si no hay proceso corriendo, elegir uno nuevo
        if (running == nullptr) {
            int topQueue = topReadyQueueIndex();

            if (topQueue == -1) {
                // nada listo todavia: saltar el reloj hasta el proximo arribo
                if (!pending.empty()) {
                    time = pending.front()->at;
                    continue;
                }
                break;  // no deberia pasar si todos los procesos estan contados
            }

            running = queues[topQueue].popNext();
            runningQueueIdx = topQueue;
            ticksInQuantum = 0;
            if (running->startTime < 0) {
                running->startTime = time;
            }
        }

        // 4) ejecutar una unidad de tiempo
        running->remaining -= 1;
        ticksInQuantum += 1;
        time += 1;

        // 5) revisar si termino
        if (running->finished()) {
            running->completionTime = time;
            completed++;
            running = nullptr;
        }
    }
}
