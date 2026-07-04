#pragma once
#include <vector>
#include "Process.h"
#include "SchedulerQueue.h"

// Motor de la simulacion. Recibe las colas ya configuradas (con su
// politica) y la lista de procesos, y corre la simulacion "tick a tick"
// (de a 1 unidad de tiempo), que es facil de leer y de explicar:
// en cada instante se revisa si hay que expropiar al proceso actual,
// se elige quien corre, y se le da 1 unidad de CPU.
//
// Nota: esta version asume tiempos (AT/BT) en numeros enteros, que es
// el caso de todos los archivos de prueba del enunciado.
class MLQScheduler {
public:
    // queues[0] = Cola 1 (mayor prioridad), queues[1] = Cola 2, etc.
    explicit MLQScheduler(std::vector<SchedulerQueue> queues);

    // Corre la simulacion completa sobre "processes" (se modifican in-place:
    // quedan con completionTime, startTime, etc. ya calculados).
    void run(std::vector<Process*>& processes);

private:
    std::vector<SchedulerQueue> queues;

    // Indice (0-based) de la cola de mayor prioridad que tiene algo listo,
    // o -1 si ninguna tiene procesos esperando.
    int topReadyQueueIndex() const;
};
