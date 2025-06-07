//
// Created by Amer on 7. 6. 2025..
//

#include "service/Writer.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>


void Writer::write() {
    out << std::fixed << std::setprecision(6);
    throwIfOutIsNotOpen();
    writeModalHeader();
    writeOutVariables();
    writeParams();
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
    out << "\nVars [out=true]:\n";

    out <<"\n\t//PV Buses\n";
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

void Writer::writeParams() {
    out << "\nParams:\n";
    writeSlackParams();
    writeAdmittanceMatrix();
}

void Writer::writeSlackParams() {
    out <<"\n\t//Slack Buses\n";
    out << "\tv_" << parser_.slack().bus_i << "=" << parser_.slack().Vm << "\n";
    out << "\tphi_" << parser_.slack().bus_i << "=" << parser_.slack().Va << "\n";
}

void Writer::writeAdmittanceMatrix() {
    out << "\n\t//Admittance Matrix\n";
    for(int i=0; i<parser_.y().size(); i++)
        for (int j=0; j<parser_.y()[i].size(); j++) {
            out << "\ty_" << i+1 << j+1 << "=" << std::abs(parser_.y()[i][j]) << "\n";
            out << "\ttheta_" << i+1 << j+1 << "=" << std::arg(parser_.y()[i][j]) << "\n";
        }
}





std::string Writer::extractBaseName(const std::string& filepath) {
    size_t lastSlash = filepath.find_last_of("/\\");
    size_t lastDot = filepath.find_last_of('.');

    if (lastSlash == std::string::npos) lastSlash = -1; // So substr starts at 0
    if (lastDot == std::string::npos || lastDot <= lastSlash) lastDot = filepath.length();

    return filepath.substr(lastSlash + 1, lastDot - lastSlash - 1);
}


