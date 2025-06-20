//
// Created by Amer on 7. 6. 2025..
//

#ifndef MPARSER_H
#define MPARSER_H
#include <complex>
#include <map>
#include <unordered_map>
#include <vector>

#include "domain/Branch.h"
#include "domain/GenParams.h"
#include "domain/PQBus.h"
#include "domain/PVBus.h"
#include "domain/SlackBus.h"

class MParser{

    int number_of_nodes = 0;
    double baseMVA_;
    SlackBus slack_;
    std::vector<PVBus> pv_buses_;
    std::vector<PQBus> pq_buses_;
    std::vector<Branch> branches_;
    std::map<std::pair<int, int>, std::complex<double>> Y_;
    std::string filename_;

    static const double PI;

    void parseBaseMVA();
    void parseSlackBus();
    std::unordered_map<int, GenParams>  parseGeneratorSetpoints();
    void parsePVBuses();
    void parsePQBuses();
    void parseBranchData();


public:
    explicit MParser(const std::string& filename)
        : filename_("resources/data/" + filename + ".m"){
        parse();
    }

    void parse();

    [[nodiscard]] int number_of_nodes1() const {
        return number_of_nodes;
    }

    [[nodiscard]] double base_mva() const {
        return baseMVA_;
    }

    [[nodiscard]] SlackBus slack() const {
        return slack_;
    }

    [[nodiscard]] std::vector<PVBus> pv_buses() const {
        return pv_buses_;
    }

    [[nodiscard]] std::vector<PQBus> pq_buses() const {
        return pq_buses_;
    }

    [[nodiscard]] const std::vector<Branch>& branches() const {
        return branches_;
    }

    [[nodiscard]] const std::map<std::pair<int, int>, std::complex<double>>& y() const {
        return Y_;
    }

    [[nodiscard]] std::string filename() const {
        return filename_;
    }
};

#endif //MPARSER_H
