#pragma once
#include <string>

// Representa un proceso del sistema. Guarda tanto los datos que vienen
// del archivo de entrada como el estado que se va actualizando mientras
// corre la simulacion (remaining, startTime, completionTime).
class Process {
public:
    std::string label;
    double bt;        // Burst Time original (tiempo total de CPU que necesita)
    double at;        // Arrival Time (momento en que llega al sistema)
    int queue;         // Cola a la que pertenece (1 = mayor prioridad)
    int priority;       // Prioridad dentro de la cola (5 alta, 1 baja)

    double remaining;     // tiempo de CPU que le falta por ejecutar
    double startTime;      // primer instante en que recibio CPU (-1 = aun no)
    double completionTime;  // instante en que termino (-1 = aun no termina)

    Process(std::string label, double bt, double at, int queue, int priority);

    bool finished() const;

    // Metricas (solo validas una vez el proceso termino)
    double waitingTime() const;     // WT = TAT - BT
    double responseTime() const;    // RT = startTime - AT
    double turnaroundTime() const;  // TAT = CT - AT
};
