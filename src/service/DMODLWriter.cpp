//
// Created by Amer on 7. 6. 2025..
//

#include "service/DMODLWriter.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>


void DMODLWriter::write() {
    out << std::fixed << std::setprecision(6);
    throwIfOutIsNotOpen();
    writeModalHeader();
    writeOutVariables();
    writeParams();
    writeNLEs();
    out << "\nend";
}

void DMODLWriter::throwIfOutIsNotOpen() {
    if (!out.is_open()) {
        std::string message = "Failed to open file for writing: " + baseFilename_ ;
        std::cerr << message << std::endl;
        throw std::exception(message.c_str());
    }
}

void DMODLWriter::writeModalHeader(){
    out << "Header:\n"
        << "\tmaxIter=" << maxIter_ << "\n"
        << "\treport=AllDetails\n"
        << "end\n";
    out << "Model [type=NL domain=real eps=" << eps_ << " name=\"PF for " << baseFilename_ << "\"]:\n";
}

void DMODLWriter::writeOutVariables() {
    out << "\nVars [out=true]:\n";

    out <<"\n\t//PV Buses\n";
    SlackBus slack = parser_.slack();
    for(const auto& pv_bus: parser_.pv_buses()) {
        out << "\tv_" << pv_bus.bus_i << "=" << slack.Vm << "\n";
        out << "\tphi_" << pv_bus.bus_i << "=" << slack.Va << "\n";
    }

    out <<"\n\t//PQ Buses\n";
    for(const auto& pq_bus: parser_.pq_buses()) {
        out << "\tv_" << pq_bus.bus_i << "=" << pq_bus.Vm << "\n";
        out << "\tphi_" << pq_bus.bus_i << "=" << pq_bus.Va << "\n";
    }
}

void DMODLWriter::writeParams() {
    out << "\nParams:\n";
    writeSlackParams();
    writeAdmittanceMatrix();
    writePVBusParams();
    writePQBusParams();
}

void DMODLWriter::writeSlackParams() {
    out <<"\n\t//Slack Bus\n";
    out << "\tv_" << parser_.slack().bus_i << "=" << parser_.slack().Vm << " [out=true]\n";
    out << "\tphi_" << parser_.slack().bus_i << "=" << parser_.slack().Va << " [out=true]\n";
}

void DMODLWriter::writeAdmittanceMatrix() {
    out << "\n\t//Admittance Matrix\n";
    for(int i=0; i<parser_.y().size(); i++)
        for (int j=0; j<parser_.y()[i].size(); j++) {
            if(shouldParamBeIncluded(i, j)) {
                out << "\ty_" << i+1 << "_" << j+1 << "=" << std::abs(parser_.y()[i][j]) << "\n";
                out << "\ttheta_" << i+1 << "_" << j+1 << "=" << std::arg(parser_.y()[i][j]) << "\n";
            }
        }
}

bool DMODLWriter::shouldParamBeIncluded(int i, int j) const {
    return std::abs(parser_.y()[i][j]) > eps_;
}


void DMODLWriter::writePVBusParams() {
    out << "\n\t//PV Buses\n";
    for(const auto& pv_bus: parser_.pv_buses()) {
        out << "\tPg_" << pv_bus.bus_i << "=" << pv_bus.Pg << "\n";
        out << "\tPd_" << pv_bus.bus_i << "=" << pv_bus.Pd << "\n";
        out << "\tvsp_" << pv_bus.bus_i << "=" << pv_bus.Vm << "\n";
    }
}

void DMODLWriter::writePQBusParams() {
    out <<"\n\t//PQ Buses\n";
    for(const auto& pq_bus: parser_.pq_buses()) {
        out << "\tPd_" << pq_bus.bus_i << "=" << pq_bus.Pd << "\n";
        out << "\tQd_" << pq_bus.bus_i << "=" << pq_bus.Qd << "\n";
    }
}

void DMODLWriter::writeNLEs() {
    out << "\nNLEs:\n";
    writePVBusNLEs();
    writePQBusNLEs();
}

void DMODLWriter::writePVBusNLEs() {
    out << "\n\t//PV Buses\n";
    for(const auto& pv_bus: parser_.pv_buses()) {
        int i = pv_bus.bus_i;
        writeFP_i_Equation(i);
        rewriteEqualOverPlusSign();
        out << "Pg_" << i << "-Pd_" << i << "\n";
        out << "\tv_" << i << " = vsp_" << i << "\n";
    }
}

void DMODLWriter::writePQBusNLEs() {
    out << "\n\t//PQ Buses\n";
    for(const auto& pq_bus: parser_.pq_buses()) {
        int i = pq_bus.bus_i;
        writeFP_i_Equation(i);
        rewriteEqualOverPlusSign();
        out << "-Pd_" << i << "\n";
        writeFQ_i_Equation(i);
        rewriteEqualOverPlusSign();
        out << "-Qd_" << i << "\n";
    }
}

void DMODLWriter::writeFP_i_Equation(int i) {
    writeEquationWithSlack(i, EquationType::P);
    writeEquationWithPV(i, EquationType::P);
    writeEquationWithPQ(i, EquationType::P);
}

void DMODLWriter::writeFQ_i_Equation(int i) {
    writeEquationWithSlack(i, EquationType::Q);
    writeEquationWithPV(i, EquationType::Q);
    writeEquationWithPQ(i, EquationType::Q);
}

void DMODLWriter::rewriteEqualOverPlusSign() {
    out.seekp(-2, std::ios_base::cur);
    out << "= ";
}

void DMODLWriter::writeEquationWithSlack(int i, EquationType eqType) {
    int j = parser_.slack().bus_i;
    out << "\t";
    if(shouldParamBeIncluded(i-1, j-1))
        writeF_ij_Equation(i,j, TrigFunction(eqType));
}

void DMODLWriter::writeEquationWithPV(int i, EquationType eqType) {
    for (const auto& nested_pv_bus: parser_.pv_buses()) {
        int j = nested_pv_bus.bus_i;
        if(shouldParamBeIncluded(i-1, j-1))
            writeF_ij_Equation(i, j, TrigFunction(eqType));
    }
}

void DMODLWriter::writeEquationWithPQ(int i, EquationType eqType) {
    for (const auto& nested_pq_bus: parser_.pq_buses()) {
        int j = nested_pq_bus.bus_i;
        if(shouldParamBeIncluded(i-1, j-1))
            writeF_ij_Equation(i, j, TrigFunction(eqType));
    }
}

void DMODLWriter::writeF_ij_Equation(int i, int j, TrigFunction trig_function) {
    out << "v_" << i << "*y_" << i << "_" << j << "*v_" << j
            << "*" << trig_function << "(phi_" << i << "-theta_" << i << "_" << j << "-phi_" << j <<") + ";
}

std::ostream &operator<<(std::ostream &out, DMODLWriter::TrigFunction trig_function){
    switch(trig_function) {
        case DMODLWriter::TrigFunction::COS: out << "cos"; break;
        case DMODLWriter::TrigFunction::SIN: out << "sin"; break;
    }
    return out;
}

std::string DMODLWriter::extractBaseName(const std::string& filepath) {
    size_t lastSlash = filepath.find_last_of("/\\");
    size_t lastDot = filepath.find_last_of('.');

    if (lastSlash == std::string::npos) lastSlash = -1; // So substr starts at 0
    if (lastDot == std::string::npos || lastDot <= lastSlash) lastDot = filepath.length();

    return filepath.substr(lastSlash + 1, lastDot - lastSlash - 1);
}


