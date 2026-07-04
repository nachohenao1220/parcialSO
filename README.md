# Simulador MLQ (Multilevel Queue)

Implementación en C++17, orientada a objetos, del algoritmo MLQ para el
Parcial No. 1 de Sistemas Operativos.

## Estructura

```
mlq_cpp/
├── include/
│   ├── Process.h          # clase Process (datos + métricas)
│   ├── SchedulerQueue.h    # una cola con su política (RR/SJF/STCF/Priority)
│   └── MLQScheduler.h     # motor de simulación
├── src/
│   ├── Process.cpp
│   ├── SchedulerQueue.cpp
│   ├── MLQScheduler.cpp
│   ├── FileIO.cpp         # lectura/escritura de archivos
│   └── main.cpp           # programa principal (CLI)
├── inputs/                # mlq001.txt (del enunciado), mlq002.txt, mlq003.txt
├── outputs/                # se generan al ejecutar
├── Makefile
└── README.md
```

## Cómo funciona

- **`Process`**: guarda los datos del proceso y calcula sus métricas
  (`waitingTime`, `responseTime`, `turnaroundTime`) a partir de
  `startTime` y `completionTime`.

- **`SchedulerQueue`**: representa una sola cola del MLQ con su política.
  Tiene funciones cortas y directas:
  - `add` / `requeue`: meten un proceso a la lista de espera.
  - `popNext`: según el tipo de política (`RR`, `SJF`, `STCF`,
    `PRIORITY`), elige y saca de la lista al proceso que debe correr.
  - `hasShorterRemaining`: solo la usa STCF, para saber si conviene
    expropiar al proceso actual por uno con menos tiempo restante.

- **`MLQScheduler::run`**: simulación **tick a tick** (de a 1 unidad de
  tiempo):
  1. Libera a sus colas los procesos que ya llegaron.
  2. Si hay un proceso corriendo, revisa si toca expropiarlo (llegó algo
     a una cola de mayor prioridad, se acabó el quantum, o llegó un
     proceso más corto en una cola STCF).
  3. Si no hay nadie corriendo, elige uno nuevo (la cola de mayor
     prioridad que tenga procesos listos).
  4. Le da 1 unidad de CPU.
  5. Si terminó, registra su `completionTime`.

  La prioridad entre colas es **estricta**: la Cola 1 siempre gana sobre
  la Cola 2, y esta sobre la Cola 3, sin importar la política interna de
  cada una.

> Esta versión asume tiempos (AT/BT) en números enteros — es el caso de
> todos los archivos de prueba del enunciado — porque simplifica mucho
> el código sin perder generalidad para este ejercicio.

## Esquemas de colas

| Esquema | Cola 1 | Cola 2 | Cola 3 |
|---|---|---|---|
| 1 | RR1 | RR3 | SJF |
| 2 (por defecto) | RR3 | RR5 | PRIORITY |
| 3 | RR2 | RR3 | STCF |

> Se validó que el esquema por defecto (`RR3,RR5,PRIORITY`) reproduce
> **exactamente** los valores de WT/CT/RT/TAT del ejemplo del enunciado
> para `mlq001.txt`.

## Compilar y ejecutar

```bash
make

# un archivo con el esquema por defecto
./mlq_simulator inputs/mlq001.txt

# varios archivos
./mlq_simulator inputs/mlq001.txt inputs/mlq002.txt inputs/mlq003.txt

# otro esquema
./mlq_simulator inputs/mlq001.txt --scheme RR1,RR3,SJF
./mlq_simulator inputs/mlq001.txt --scheme RR2,RR3,STCF

# otra carpeta de salida
./mlq_simulator inputs/mlq001.txt --outdir mis_resultados
```# parcialSO
