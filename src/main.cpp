// main.cpp
//
// Uso:
//   ./mlq_simulator inputs/mlq001.txt
//   ./mlq_simulator inputs/mlq001.txt inputs/mlq002.txt
//   ./mlq_simulator inputs/mlq001.txt --scheme RR3,RR5,PRIORITY
//
// Esquemas disponibles para --scheme (separados por coma, uno por cola):
//   RR<n>      -> Round Robin con quantum n, ej: RR1, RR3, RR5
//   SJF        -> Shortest Job First (no expropiativa)
//   STCF       -> Shortest Time-to-Completion First (expropiativa)
//   PRIORITY   -> Prioridad no expropiativa (5 > 1)
//
// Por defecto se usa "RR3,RR5,PRIORITY", que es el esquema que reproduce
// exactamente el ejemplo del enunciado.

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include "Process.h"
#include "SchedulerQueue.h"
#include "MLQScheduler.h"
#include "FileIO.h"
#include <algorithm>

namespace fs = std::filesystem;

const std::string DEFAULT_SCHEME = "RR3,RR5,PRIORITY";

bool isValidScheme(const std::string& scheme) {
    // Se normaliza removiendo espacios si los hubiera
    std::string cleanScheme = scheme;
    cleanScheme.erase(std::remove(cleanScheme.begin(), cleanScheme.end(), ' '), cleanScheme.end());
    
    // Esquemas oficiales del parcial
    return (cleanScheme == "RR1,RR3,SJF" || 
            cleanScheme == "RR3,RR5,PRIORITY" || 
            cleanScheme == "RR2,RR3,STCF");
}

// Convierte un texto como "RR3" o "SJF" en una SchedulerQueue.
SchedulerQueue parsePolicy(const std::string& spec) {
    if (spec.size() >= 2 && spec[0] == 'R' && spec[1] == 'R') {
        double quantum = std::stod(spec.substr(2));
        return SchedulerQueue(PolicyType::RR, quantum);
    }
    if (spec == "SJF") return SchedulerQueue(PolicyType::SJF);
    if (spec == "STCF") return SchedulerQueue(PolicyType::STCF);
    if (spec == "PRIORITY") return SchedulerQueue(PolicyType::PRIORITY);
    throw std::runtime_error("Politica desconocida: " + spec);
}

std::vector<SchedulerQueue> buildQueues(const std::string& scheme) {
    std::vector<SchedulerQueue> queues;
    std::stringstream ss(scheme);
    std::string spec;
    while (std::getline(ss, spec, ',')) {
        queues.push_back(parsePolicy(spec));
    }
    return queues;
}

void processFile(const std::string& inputPath, const std::string& scheme, const std::string& outDir) {
    std::vector<Process*> processes = FileIO::readProcesses(inputPath);
    std::vector<SchedulerQueue> queues = buildQueues(scheme);

    MLQScheduler scheduler(queues);
    scheduler.run(processes);

    fs::path in(inputPath);
    std::string base = in.stem().string();
    std::string ext = in.extension().string();
    fs::path outPath = fs::path(outDir) / (base + "_out" + ext);

    FileIO::writeResults(outPath.string(), processes, in.filename().string());

    std::cout << "[OK] " << inputPath << " -> " << outPath.string()
              << "  (esquema: " << scheme << ")\n";

    for (Process* p : processes) delete p;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " archivo1.txt [archivo2.txt ...] [--scheme RR3,RR5,PRIORITY]\n";
        return 1;
    }

    std::vector<std::string> inputFiles;
    std::string scheme = DEFAULT_SCHEME;
    std::string outDir = "outputs";

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--scheme" && i + 1 < argc) {
            scheme = argv[++i];
        } else if (arg == "--outdir" && i + 1 < argc) {
            outDir = argv[++i];
        } else {
            inputFiles.push_back(arg);
        }
    }

    if (!isValidScheme(scheme)) {
        std::cerr << "[FATAL] Esquema invalido: '" << scheme << "'\n";
        std::cerr << "Los esquemas permitidos por la guia son:\n"
                  << "  - RR1,RR3,SJF\n"
                  << "  - RR3,RR5,PRIORITY\n"
                  << "  - RR2,RR3,STCF\n";
        return 1;
    }

    fs::create_directories(outDir);

    for (const auto& file : inputFiles) {
        try {
            processFile(file, scheme, outDir);
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] " << file << ": " << e.what() << "\n";
        }
    }

    return 0;
}
