//
// Created by Amer on 7. 6. 2025..
//

#include "service/Writer.h"
#include <fstream>
#include <iostream>
#include <string>


void Writer::write() {
    throwIfOutIsNotOpen();
    writeModalHeader();
    writeOutVariables();
}

void Writer::throwIfOutIsNotOpen() {
    if (!out.is_open()) {
        std::string message = "Failed to open file for writing: " + baseFilename_ ;
        std::cerr << message << std::endl;
        throw std::exception(message.c_str());
    }
}

void Writer::writeModalHeader(){
    out << "Model [type=NL domain=real eps=1e-6 name=\"PF for " << baseFilename_ << "\"]:\n";
}

void Writer::writeOutVariables() {
    out << "Vars [out=true]:\n";

    out <<"\t//PV Buses\n";
    for(const auto& pv_bus: parser_.pv_buses()) {
        out << "\tv_" << pv_bus.bus_i << "=" << pv_bus.Vm << "\n";
        out << "\tphi_" << pv_bus.bus_i << "=" << pv_bus.Va << "\n";
    }

    out <<"\n\t//PQ Buses\n";
    for(const auto& pq_bus: parser_.pq_buses()) {
        out << "\tv_" << pq_bus.bus_i << "=" << pq_bus.Vm << "\n";
        out << "\tphi_" << pq_bus.bus_i << "=" << pq_bus.Va << "\n";
    }
}




std::string Writer::extractBaseName(const std::string& filepath) {
    size_t lastSlash = filepath.find_last_of("/\\");
    size_t lastDot = filepath.find_last_of('.');

    if (lastSlash == std::string::npos) lastSlash = -1; // So substr starts at 0
    if (lastDot == std::string::npos || lastDot <= lastSlash) lastDot = filepath.length();

    return filepath.substr(lastSlash + 1, lastDot - lastSlash - 1);
}


