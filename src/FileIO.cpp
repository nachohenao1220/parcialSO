#include "FileIO.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <chrono>    // <<-- Personalización: Para manejo del tiempo del sistema
#include <iomanip>   // <<-- Personalización: Para dar formato al timestamp

namespace {

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::vector<std::string> splitBySemicolon(const std::string& line) {
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ';')) {
        parts.push_back(trim(item));
    }
    return parts;
}

std::string fmt(double value) {
    if (std::abs(value - std::round(value)) < 1e-9) {
        return std::to_string((long long)std::round(value));
    }
    std::ostringstream out;
    out.precision(2);
    out << std::fixed << value;
    return out.str();
}

// <<-- Personalización: Función auxiliar para obtener el timestamp actual
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

}  // namespace

namespace FileIO {

std::vector<Process*> readProcesses(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo: " + path);
    }

    std::vector<Process*> processes;
    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;

        std::vector<std::string> parts = splitBySemicolon(trimmed);
        if (parts.size() < 5) {
            throw std::runtime_error("Linea con formato invalido en " + path + ": " + line);
        }

        try {
            std::string label = parts[0];
            double bt = std::stod(parts[1]);
            double at = std::stod(parts[2]);
            int q = std::stoi(parts[3]);
            int pr = std::stoi(parts[4]);

            if (pr < 1 || pr > 5) {
                throw std::out_of_range("La prioridad debe estar entre 1 y 5");
            }

            processes.push_back(new Process(label, bt, at, q, pr));
        } 
        catch (const std::exception& e) {
            throw std::runtime_error("Error de casteo/validación");
        }
    }
    return processes;
}

void writeResults(const std::string& originalPath,
                  const std::vector<Process*>& processes,
                  const std::string& sourceFilename) {
    
    // <<-- Personalización: Modificación del path para inyectar el timestamp único
    std::string actualPath = originalPath;
    size_t dotPos = actualPath.find_last_of('.');
    std::string timestamp = getCurrentTimestamp();
    
    if (dotPos != std::string::npos) {
        // Inserta el timestamp justo antes de la extensión (.txt)
        actualPath.insert(dotPos, "_" + timestamp);
    } else {
        actualPath += "_" + timestamp;
    }

    std::vector<Process*> ordered = processes;
    std::sort(ordered.begin(), ordered.end(), [](Process* a, Process* b) {
        if (a->at != b->at) return a->at < b->at;
        return a->label < b->label;
    });

    std::ofstream out(actualPath); // Abre el flujo con la ruta única generada
    if (!out.is_open()) {
        throw std::runtime_error("No se pudo crear el archivo de resultados en: " + actualPath);
    }

    out << "# archivo: " << sourceFilename << "\n";
    out << "# etiqueta; BT; AT; Q; Pr; WT; CT; RT; TAT\n";

    double totalWT = 0, totalCT = 0, totalRT = 0, totalTAT = 0;
    for (Process* p : ordered) {
        double wt = p->waitingTime();
        double ct = p->completionTime;
        double rt = p->responseTime();
        double tat = p->turnaroundTime();
        totalWT += wt; totalCT += ct; totalRT += rt; totalTAT += tat;

        out << p->label << ";" << fmt(p->bt) << "; " << fmt(p->at) << "; "
            << p->queue << "; " << p->priority << "; "
            << fmt(wt) << "; " << fmt(ct) << "; " << fmt(rt) << "; " << fmt(tat) << "\n";
    }

    size_t n = ordered.size();
    out.precision(2);
    out << std::fixed;
    out << "# WT=" << (totalWT / n) << "; CT=" << (totalCT / n)
        << "; RT=" << (totalRT / n) << "; TAT=" << (totalTAT / n) << ";\n";
        
    // Feedback visual por consola con la ruta real utilizada
    std::cout << "[INFO] Resultados guardados en: " << actualPath << "\n";
}

}  // namespace FileIO
