//
// Created by Amer on 7. 6. 2025..
//

#include "service/MParser.h"

#include <iostream>
#include <fstream>
#include <regex>
#include <unordered_map>

#include "domain/GenParams.h"

void MParser::parseBaseMVA() {
    std::ifstream file(filename_);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename_ << std::endl;
        std::exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("mpc.baseMVA") != std::string::npos) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
               baseMVA_ = std::stod(line.substr(pos + 1));
                return;
            }
        }
    }

    std::cerr << "baseMVA not found in file.\n";
    std::exit(1);
}

void MParser::parseSlackBus() {
    auto gen_map = parseGeneratorSetpoints();
    std::ifstream file(filename_);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename_ << std::endl;
        std::exit(1);
    }

    std::string line;
    bool inBus = false;
    std::regex number_line(R"([\s]*[0-9\.\-eE\+]+)");

    while (std::getline(file, line)) {
        if (line.find("mpc.bus") != std::string::npos) {
            inBus = true;
            continue;
        }

        if (inBus) {
            if (line.find("];") != std::string::npos) break;
            if (!std::regex_search(line, number_line)) continue;

            std::stringstream ss(line);
            std::vector<double> values;
            double val;
            while (ss >> val) values.push_back(val);

            if (values.size() >= 9) {
                int bus_i = static_cast<int>(values[0]);
                int type = static_cast<int>(values[1]);
                double Va = values[8];

                if (type == 3) {
                    file.close();
                    slack_ = SlackBus{bus_i, gen_map[bus_i].Vg, Va};
                    return;
                }
            }
        }
    }

    std::cerr << "Slack bus not found.\n";
    std::exit(1);
}

std::unordered_map<int, GenParams> MParser::parseGeneratorSetpoints() {
    std::ifstream file(filename_);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename_ << std::endl;
        std::exit(1);
    }

    std::unordered_map<int, GenParams> gen_map;
    std::string line;
    bool inGen = false;
    std::regex number_line(R"([\s]*[0-9\.\-eE\+]+)");

    while (std::getline(file, line)) {
        if (line.find("mpc.gen") != std::string::npos) {
            inGen = true;
            continue;
        }

        if (inGen) {
            if (line.find("];" ) != std::string::npos) break;
            if (!std::regex_search(line, number_line)) continue;

            std::stringstream ss(line);
            std::vector<double> values;
            double val;
            while (ss >> val) values.push_back(val);

            if (values.size() >= 9) {
                int bus_i = static_cast<int>(values[0]);
                gen_map[bus_i] = GenParams{
                    values[1] / baseMVA_,
                    values[2] / baseMVA_,
                    values[3] / baseMVA_,
                    values[4] / baseMVA_,
                    values[5],
                    values[8] / baseMVA_,
                    values[9] / baseMVA_
                };
            }
        }
    }

    return gen_map;
}

void MParser::parsePVBuses() {
    const auto& gen_map = parseGeneratorSetpoints();
    std::ifstream file(filename_);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename_ << std::endl;
        std::exit(1);
    }

    std::vector<PVBus> pv_buses;
    std::string line;
    bool inBus = false;
    std::regex number_line(R"([\s]*[0-9\.\-eE\+]+)");

    while (std::getline(file, line)) {
        if (line.find("mpc.bus") != std::string::npos) {
            inBus = true;
            continue;
        }
        if (inBus) {
            if (line.find("];" ) != std::string::npos) break;
            if (!std::regex_search(line, number_line)) continue;

            std::stringstream ss(line);
            std::vector<double> values;
            double val;
            while (ss >> val) values.push_back(val);

            if (values.size() >= 9) {
                int type = static_cast<int>(values[1]);
                if (type == 2) {
                    int bus_i = static_cast<int>(values[0]);
                    auto it = gen_map.find(bus_i);
                    PVBus bus;
                    bus.bus_i = bus_i;
                    bus.Pd = values[2] / baseMVA_;
                    bus.Qd = values[3] / baseMVA_;
                    bus.Vm = values[7];
                    bus.Va = values[8];

                    if (it != gen_map.end()) {
                        const GenParams& g = it->second;
                        bus.Pg = g.Pg;
                        bus.Qg = g.Qg;
                        bus.Pmax = g.Pmax;
                        bus.Pmin = g.Pmin;
                        bus.Qmax = g.Qmax;
                        bus.Qmin = g.Qmin;
                        bus.Vm = g.Vg;
                    } else {
                        bus.Pg = bus.Qg = bus.Pmax = bus.Pmin = bus.Qmax = bus.Qmin = 0.0;
                    }
                    pv_buses.push_back(bus);
                }
            }
        }
    }

    pv_buses_ = pv_buses;
}

void MParser::parsePQBuses() {
    std::ifstream file(filename_);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename_ << std::endl;
        std::exit(1);
    }

    std::string line;
    bool inBus = false;
    std::regex number_line(R"([\s]*[0-9\.\-eE\+]+)");
    std::vector<PQBus> pq_buses;

    while (std::getline(file, line)) {
        if (line.find("mpc.bus") != std::string::npos) {
            inBus = true;
            continue;
        }

        if (inBus) {
            if (line.find("];" ) != std::string::npos) break;
            if (!std::regex_search(line, number_line)) continue;

            std::stringstream ss(line);
            std::vector<double> values;
            double val;
            while (ss >> val) values.push_back(val);

            if (values.size() >= 9) {
                int type = static_cast<int>(values[1]);
                if (type == 1) {
                    pq_buses.push_back({
                        static_cast<int>(values[0]),
                        values[2] / baseMVA_,
                        values[3] / baseMVA_,
                        values[7],
                        values[8]
                    });
                }
            }
        }
    }

    pq_buses_ = pq_buses;
}

void MParser::parseBranchData(){
    std::vector<Branch> branches;
    std::ifstream file(filename_);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename_ << std::endl;
        std::exit(1);
    }

    std::string line;
    bool inBranch = false;
    std::regex number_line(R"([\s]*[0-9\.\-eE\+]+)");

    while (std::getline(file, line)) {
        if (line.find("mpc.branch") != std::string::npos) {
            inBranch = true;
            continue;
        }

        if (inBranch) {
            if (line.find("];") != std::string::npos) break;
            if (!std::regex_search(line, number_line)) continue;

            std::stringstream ss(line);
            std::vector<double> vals;
            double val;
            while (ss >> val)
                vals.push_back(val);

            if (vals.size() >= 5) {
                int fbus = static_cast<int>(vals[0]);
                int tbus = static_cast<int>(vals[1]);
                double r = vals[2];
                double x = vals[3];
                double b = vals[4];
                number_of_nodes = std::max({number_of_nodes, fbus, tbus});
                branches.push_back({fbus, tbus, r, x, b});

                int i = fbus;
                int j = tbus;
                std::complex<double> z(r, x);
                std::complex<double> y = 1.0 / z;

                Y_[{i, j}] -= y;
                Y_[{j, i}] -= y;

                // Dijagonalni elementi
                Y_[{i, i}] += y + std::complex<double>(0.0, b / 2.0);
                Y_[{j, j}] += y + std::complex<double>(0.0, b / 2.0);
            }
        }
    }

    file.close();
    branches_ = branches;
}

void MParser::parse() {
    parseBaseMVA();
    parseSlackBus();
    parsePVBuses();
    parsePQBuses();
    parseBranchData();
}
