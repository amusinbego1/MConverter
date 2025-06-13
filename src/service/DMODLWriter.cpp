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
    writeLimits();
    out << "\nend";
}

void DMODLWriter::throwIfOutIsNotOpen() {
    if (!out.is_open()) {
        std::string message = "Failed to open file for writing: " + baseFilename_ ;
        std::cerr << message << td::endl;
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

    out <<"\n"; writeComment("\tPV Buses\n");

    SlackBus slack = parser_.slack();
    for(const auto& pv_bus: parser_.pv_buses()) {
        out << "\t" << config_.v_symbol << "_" << pv_bus.bus_i << "=" << slack.Vm;
        out << "; " << config_.phi_symbol <<"_" << pv_bus.bus_i << "=" << slack.Va << "\n";
    }

    out <<"\n"; writeComment("\tPQ Buses\n");
    for(const auto& pq_bus: parser_.pq_buses()) {
        out << "\t" << config_.v_symbol << "_" <<  pq_bus.bus_i << "=" << pq_bus.Vm;
        out << "; " << config_.phi_symbol <<"_" << pq_bus.bus_i << "=" << pq_bus.Va << "\n";
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
    out <<"\n"; writeComment("\tSlack Bus\n");
    out << "\t" << config_.v_symbol << "_" << parser_.slack().bus_i << "=" << parser_.slack().Vm << " [out=true]";
    out << "; " << config_.phi_symbol <<"_" << parser_.slack().bus_i << "=" << parser_.slack().Va << " [out=true]\n";
}

void DMODLWriter::writeAdmittanceMatrix() {
    out << "\n"; writeComment("\tAdmittance Matrix\n");
    for (const auto& entry : parser_.y()) {
        int i = entry.first.first;
        int j = entry.first.second;
        const std::complex<double>& y_ij = entry.second;
        out << "\t" << config_.y_symbol << "_" << i<< "_" << j << "=" << std::abs(y_ij);
        out << "; " << config_.theta_symbol <<"_" << i << "_" << j << "=" << std::arg(y_ij) << "     ";

        if (i > j)
            std::swap(i, j);
        if (i != j) {
            Branch currentBranch = findBranch(i, j).value();
            std::string branchType = currentBranch.ratio != 0 ? "transformer" : "line";
            writeComment((branchType + " " + std::to_string(i) + "-" + std::to_string(j)).c_str());
        }

        out << "\n";
    }
}

bool DMODLWriter::shouldParamBeIncluded(int i, int j) const {
    auto it = parser_.y().find({i, j});
    return it != parser_.y().end();
}

void DMODLWriter::writePVBusParams() {
    out << "\n"; writeComment("\tPV Buses\n");
    for(const auto& pv_bus: parser_.pv_buses()) {
        out << "\tPg_" << pv_bus.bus_i << "=" << pv_bus.Pg;
        out << "; Pd_" << pv_bus.bus_i << "=" << pv_bus.Pd;
        out << "; vsp_" << pv_bus.bus_i << "=" << pv_bus.Vm << "\n";
        writePowerParams(pv_bus);
        out << "\n";
    }
}

void DMODLWriter::writePowerParams(const PVBus& pv_bus) {
    writeSmallPowerParams(pv_bus);
    writeMediumPowerParams(pv_bus);
    writeHighPowerParams(pv_bus);
    writeUltraHighPowerParams(pv_bus);
}

void DMODLWriter::writeSmallPowerParams(const PVBus& pv_bus) {
    if ((pv_bus.Qg - pv_bus.Qd) * parser_.base_mva() <= config_.small_upper_limit)
        out << "\tQinj_" << pv_bus.bus_i << "=" << pv_bus.Qg
            << "; Qinj_min_" << pv_bus.bus_i << "=" << pv_bus.Qmin
            << "; Qinj_max_" << pv_bus.bus_i << "=" << pv_bus.Qmax << "\n";
}

void DMODLWriter::writeMediumPowerParams(const PVBus& pv_bus) {
    double Qinj = (pv_bus.Qg - pv_bus.Qd) * parser_.base_mva();
    if (Qinj > config_.small_upper_limit && Qinj <= config_.medium_upper_limit)
        out << "; Qinj_" << pv_bus.bus_i << "=" << pv_bus.Qg - pv_bus.Qd
            << "; Qinj_min_" << pv_bus.bus_i << "=" << pv_bus.Qmin
            << "; Qinj_max_" << pv_bus.bus_i << "=" << pv_bus.Qmax << "\n";
}

void DMODLWriter::writeHighPowerParams(const PVBus& pv_bus) {
    double Qinj = (pv_bus.Qg - pv_bus.Qd) * parser_.base_mva();
    if (Qinj > config_.medium_upper_limit && Qinj <= config_.high_upper_limit)
        out << "; Qinj_" << pv_bus.bus_i << "=" << pv_bus.Qg - pv_bus.Qd
            << "; Qinj_min_" << pv_bus.bus_i << "=" << pv_bus.Qmin
            << "; Qinj_max_" << pv_bus.bus_i << "=" << pv_bus.Qmax << "\n";
}

void DMODLWriter::writeUltraHighPowerParams(const PVBus& pv_bus) {
    double Qinj = (pv_bus.Qg - pv_bus.Qd) * parser_.base_mva();
    if (Qinj > config_.high_upper_limit)
        out << "; Qinj_" << pv_bus.bus_i << "=" << pv_bus.Qg - pv_bus.Qd
            << "; Qinj_min_" << pv_bus.bus_i << "=" << pv_bus.Qmin
            << "; Qinj_max_" << pv_bus.bus_i << "=" << pv_bus.Qmax << "\n";
}


void DMODLWriter::writePQBusParams() {
    out <<"\n"; writeComment("\tPQ Buses\n");
    for(const auto& pq_bus: parser_.pq_buses()) {
        out << "\tPd_" << pq_bus.bus_i << "=" << pq_bus.Pd;
        out << "; Qd_" << pq_bus.bus_i << "=" << pq_bus.Qd << "\n";
    }
}

void DMODLWriter::writeNLEs() {
    out << "\nNLEs:\n";
    writePVBusNLEs();
    writePQBusNLEs();
}

void DMODLWriter::writePVBusNLEs() {
    out << "\n"; writeComment("\tPV Buses\n");
    for(const auto& pv_bus: parser_.pv_buses()) {
        int i = pv_bus.bus_i;
        writeFP_i_Equation(i);
        rewriteEqualOverPlusSign();
        out << "Pg_" << i << "-Pd_" << i << "\n";
        out << "\t" << config_.v_symbol << "_" << i << " = vsp_" << i << "\n\n";
    }
}

void DMODLWriter::writePQBusNLEs() {
    out <<"\n"; writeComment("\tPQ Buses\n");
    for(const auto& pq_bus: parser_.pq_buses()) {
        int i = pq_bus.bus_i;
        writeFP_i_Equation(i);
        rewriteEqualOverPlusSign();
        out << "-Pd_" << i << "\n";
        writeFQ_i_Equation(i);
        rewriteEqualOverPlusSign();
        out << "-Qd_" << i << "\n\n";
    }
}

void DMODLWriter::writeLimits() {
    out << "\nLimits:\n";
    writeSmallGroup();
    writeMediumGroup();
    writeHighGroup();
    writeUltraHighGroup();
}

void DMODLWriter::writeSmallGroup() {
    if (config_.small_power_limits) {
        writeGroupHeader("smallPower");
        for(const auto& pv_bus: parser_.pv_buses()) {
            if ((pv_bus.Qg - pv_bus.Qd) * parser_.base_mva() <= config_.small_upper_limit)
                writeOneLimit(pv_bus);
        }
        out << "\tend\n";
    }
}

void DMODLWriter::writeMediumGroup() {
    if (config_.medium_power_limits) {
        writeGroupHeader("mediumPower");
        for(const auto& pv_bus: parser_.pv_buses()) {
            double Qinj = (pv_bus.Qg - pv_bus.Qd) * parser_.base_mva();
            if (Qinj > config_.small_upper_limit && Qinj <= config_.medium_upper_limit)
                writeOneLimit(pv_bus);
        }
        out << "\tend\n";
    }
}

void DMODLWriter::writeHighGroup() {
    if (config_.high_power_limits) {
        writeGroupHeader("highPower");
        for(const auto& pv_bus: parser_.pv_buses()) {
            double Qinj = (pv_bus.Qg - pv_bus.Qd) * parser_.base_mva();
            if (Qinj > config_.medium_upper_limit && Qinj <= config_.high_upper_limit)
                writeOneLimit(pv_bus);
        }
        out << "\tend\n";
    }
}

void DMODLWriter::writeUltraHighGroup() {
    if (config_.ultra_high_power_limits) {
        writeGroupHeader("ultraHighPower");
        for(const auto& pv_bus: parser_.pv_buses()) {
            double Qinj = (pv_bus.Qg - pv_bus.Qd) * parser_.base_mva();
            if (Qinj > config_.high_upper_limit)
                writeOneLimit(pv_bus);
        }
        out << "\tend\n";
    }
}

void DMODLWriter::writeGroupHeader(const char * groupName) {
    out << "\t" << "group [name=\"" << groupName << "\" enabled=true]" << "\n";
}

void DMODLWriter::writeOneLimit(const PVBus& pv_bus) {
    out << "\t";
    writeFQ_i_Equation(pv_bus.bus_i);
    rewriteEqualOverPlusSign();
    out << "Qinj_" << pv_bus.bus_i << "\n";
    out << "\t\tif Qinj_" << pv_bus.bus_i << " <= " << "Qinj_min_" << pv_bus.bus_i << " [signal=TooLow]:\n";
    out << "\t\t\tQinj_" << pv_bus.bus_i << "=Qinj_min_" << pv_bus.bus_i << "\n";
    out << "\t\telse:\n";
    out << "\t\t\tif Qinj_" << pv_bus.bus_i << " >= " << "Qinj_max_" << pv_bus.bus_i << " [signal=TooHigh]:\n";
    out << "\t\t\t\tQinj_" << pv_bus.bus_i << "=Qinj_max_" << pv_bus.bus_i << "\n";
    out << "\t\t\tend\n";
    out << "\t\tend\n\n";
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
    if(shouldParamBeIncluded(i, j))
        writeF_ij_Equation(i,j, TrigFunction(eqType));
}

void DMODLWriter::writeEquationWithPV(int i, EquationType eqType) {
    for (const auto& nested_pv_bus: parser_.pv_buses()) {
        int j = nested_pv_bus.bus_i;
        if(shouldParamBeIncluded(i, j))
            writeF_ij_Equation(i, j, TrigFunction(eqType));
    }
}

void DMODLWriter::writeEquationWithPQ(int i, EquationType eqType) {
    for (const auto& nested_pq_bus: parser_.pq_buses()) {
        int j = nested_pq_bus.bus_i;
        if(shouldParamBeIncluded(i, j))
            writeF_ij_Equation(i, j, TrigFunction(eqType));
    }
}

void DMODLWriter::writeF_ij_Equation(int i, int j, TrigFunction trig_function) {
    out << config_.v_symbol << "_" << i << "*" << config_.y_symbol << "_" << i << "_" << j << "*" << config_.v_symbol << "_" << j
            << "*" << trig_function << "(" << config_.phi_symbol << "_" << i << "-" << config_.theta_symbol << "_" << i << "_" << j << "-" << config_.phi_symbol << "_" << j <<") + ";
}

void DMODLWriter::writeComment(const char *comment) {
    if (config_.comments_included)
        out << "//" << comment ;
}

std::optional<Branch> DMODLWriter::findBranch(int from, int to) const {
    auto foundBranch = std::find_if(parser_.branches().begin(), parser_.branches().end(), [from, to](const Branch& branch){return branch.fbus == from && branch.tbus == to;});
    return {*foundBranch};
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


