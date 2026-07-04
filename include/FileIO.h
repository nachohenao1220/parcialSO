#pragma once
#include <string>
#include <vector>
#include "Process.h"

namespace FileIO {

// Lee un archivo de entrada con lineas "etiqueta;BT;AT;Q;Prioridad".
// Ignora lineas vacias y las que empiezan con '#'.
std::vector<Process*> readProcesses(const std::string& path);

// Escribe el archivo de salida con el formato del enunciado:
//   # archivo: mlq001.txt
//   # etiqueta; BT; AT; Q; Pr; WT; CT; RT; TAT
//   A;6; 0; 1; 5; 3; 9; 0; 9
//   ...
//   # WT=..; CT=..; RT=..; TAT=..;
void writeResults(const std::string& path,
                   const std::vector<Process*>& processes,
                   const std::string& sourceFilename);

}  // namespace FileIO
