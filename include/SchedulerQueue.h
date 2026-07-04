#pragma once
#include <vector>
#include "Process.h"

// Tipos de politica que puede tener una cola.
enum class PolicyType { RR, SJF, STCF, PRIORITY };

// Representa UNA cola del MLQ (por ejemplo la Cola 1). Sabe agregar
// procesos que llegan, y sabe elegir cual debe correr segun su propia
// politica (RR / SJF / STCF / Priority). Cada funcion es pequena y hace
// una sola cosa, para que sea facil de explicar y de mantener.
class SchedulerQueue {
public:
    PolicyType type;
    double quantum;  // solo se usa si type == RR

    SchedulerQueue(PolicyType type, double quantum = 0);

    void add(Process* p);       // un proceso nuevo llega a la cola
    void requeue(Process* p);    // un proceso expropiado vuelve a la cola

    bool hasReady() const;        // hay al menos un proceso esperando?
    Process* popNext();          // elige y saca de la lista al siguiente a correr

    // Solo tiene sentido para STCF: hay en la lista de espera un proceso
    // con menos tiempo restante que "current"? (para decidir si se expropia)
    bool hasShorterRemaining(const Process* current) const;

private:
    std::vector<Process*> ready;

    int indexOfShortestBurst() const;      // para SJF (BT original)
    int indexOfShortestRemaining() const;   // para STCF (tiempo restante)
    int indexOfHighestPriority() const;    // para PRIORITY
};
