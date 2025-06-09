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
    writeNLEs();
    out << "\nend";
}

void Writer::throwIfOutIsNotOpen() {
    if (!out.is_open()) {
        std::string message = "Failed to open file for writing: " + baseFilename_ ;
        std::cerr << message << std::endl;
        throw std::exception(message.c_str());
    }
}

void Writer::writeModalHeader(){
    out << "Header:\n"
        << "\tmaxIter=500\n"
        << "\treport=AllDetails\n"
        << "end\n";
    out << "Model [type=NL domain=real eps=1e-6 name=\"PF for " << baseFilename_ << "\"]:\n";
}

void Writer::writeOutVariables() {
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

void Writer::writeParams() {
    out << "\nParams:\n";
    writeSlackParams();
    writeAdmittanceMatrix();
    writePVBusParams();
    writePQBusParams();
}

void Writer::writeSlackParams() {
    out <<"\n\t//Slack Bus\n";
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

void Writer::writePVBusParams() {
    out << "\n\t//PV Buses\n";
    for(const auto& pv_bus: parser_.pv_buses()) {
        out << "\tPg_" << pv_bus.bus_i << "=" << pv_bus.Pg << "\n";
        out << "\tPd_" << pv_bus.bus_i << "=" << pv_bus.Pd << "\n";
        out << "\tvsp_" << pv_bus.bus_i << "=" << pv_bus.Vm << "\n";
    }
}

void Writer::writePQBusParams() {
    out <<"\n\t//PQ Buses\n";
    for(const auto& pq_bus: parser_.pq_buses()) {
        out << "\tPd_" << pq_bus.bus_i << "=" << pq_bus.Pd << "\n";
        out << "\tQd_" << pq_bus.bus_i << "=" << pq_bus.Qd << "\n";
    }
}

void Writer::writeNLEs() {
    out << "\nNLEs:\n";
    writePVBusNLEs();
    writePQBusNLEs();
}

void Writer::writePVBusNLEs() {
    out << "\n\t//PV Buses\n";
    for(const auto& pv_bus: parser_.pv_buses()) {
        int i = pv_bus.bus_i;
        writeFP_i_Equation(i);
        out << "=Pg_" << i << "-Pd_" << i << "\n";
        out << "\tv_" << i << "=vsp_" << i << "\n";
    }
}

void Writer::writePQBusNLEs() {
    out << "\n\t//PQ Buses\n";
    for(const auto& pq_bus: parser_.pq_buses()) {
        int i = pq_bus.bus_i;
        writeFP_i_Equation(i);
        out << "=-Pd_" << i << "\n";
        writeFQ_i_Equation(i);
        out << "=-Qd_" << i << "\n";
    }
}


void Writer::writeFP_i_Equation(int i) {
    //SLACK
    int j = parser_.slack().bus_i;
    out << "\t";
    writeFP_ij_Equation(i,j);

    //PV Buses
    for (const auto& nested_pv_bus: parser_.pv_buses()) {
        j = nested_pv_bus.bus_i;
        out << " + ";
        writeFP_ij_Equation(i, j);
    }
    //PQ Buses
    for (const auto& nested_pq_bus: parser_.pq_buses()) {
        j = nested_pq_bus.bus_i;
        out << " + ";
        writeFP_ij_Equation(i, j);
    }
}

void Writer::writeFQ_i_Equation(int i) {
    //SLACK
    int j = parser_.slack().bus_i;
    out << "\t";
    writeFQ_ij_Equation(i,j);

    //PV Buses
    for (const auto& nested_pv_bus: parser_.pv_buses()) {
        j = nested_pv_bus.bus_i;
        out << " + ";
        writeFQ_ij_Equation(i, j);
    }
    //PQ Buses
    for (const auto& nested_pq_bus: parser_.pq_buses()) {
        j = nested_pq_bus.bus_i;
        out << " + ";
        writeFQ_ij_Equation(i, j);
    }
}



void Writer::writeFP_ij_Equation(int i, int j) {
    out << "v_" << i << "*y_" << i << j << "*v_" << j
            << "*cos(phi_" << i << "-theta_" << i << j << "-phi_" << j <<")";
}

void Writer::writeFQ_ij_Equation(int i, int j) {
    out << "v_" << i << "*y_" << i << j << "*v_" << j
            << "*sin(phi_" << i << "-theta_" << i << j << "-phi_" << j <<")";
}



std::string Writer::extractBaseName(const std::string& filepath) {
    size_t lastSlash = filepath.find_last_of("/\\");
    size_t lastDot = filepath.find_last_of('.');

    if (lastSlash == std::string::npos) lastSlash = -1; // So substr starts at 0
    if (lastDot == std::string::npos || lastDot <= lastSlash) lastDot = filepath.length();

    return filepath.substr(lastSlash + 1, lastDot - lastSlash - 1);
}


