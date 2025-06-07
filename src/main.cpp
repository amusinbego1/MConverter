#include <iomanip>
#include <iostream>
#include "service/MParser.h"

int main()
{
    std::string caseName;
    std::cout << "Enter MATPOWER .m filename (without .m): ";
    std::cin >> caseName;

    MParser parser(caseName);

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "\nSlack Bus:\n";
    std::cout << "Bus " << parser.slack().bus_i << " | Vm = " << parser.slack().Vm << " p.u. | Va = " << parser.slack().Va << "\u00b0\n";

    std::cout << "\nPV Buses:\n";
    for (const auto& bus : parser.pv_buses()) {
        std::cout << "Bus " << bus.bus_i
             << " | Pd = " << bus.Pd << " p.u."
             << " | Qd = " << bus.Qd << " p.u."
             << " | Pg = " << bus.Pg << " p.u."
             << " | Qg = " << bus.Qg << " p.u."
             << " | Pmax = " << bus.Pmax << " p.u."
             << " | Pmin = " << bus.Pmin << " p.u."
             << " | Qmax = " << bus.Qmax << " p.u."
             << " | Qmin = " << bus.Qmin << " p.u."
             << " | Vm = " << bus.Vm
             << " | Va = " << bus.Va << "\u00b0\n";
    }

    std::cout << "\nPQ Buses:\n";
    for (const auto& bus : parser.pq_buses()) {
        std::cout << "Bus " << bus.bus_i
             << " | Pd = " << bus.Pd << " p.u."
             << " | Qd = " << bus.Qd << " p.u."
             << " | Vm = " << bus.Vm
             << " | Va = " << bus.Va << "\u00b0\n";
    }

    std::cout << "\nYbus (Nodal Admittance Matrix):\n";
    for (int i = 0; i < parser.number_of_nodes1(); ++i) {
        for (int j = 0; j < parser.number_of_nodes1(); ++j) {
            std::cout << "(" << std::abs(parser.y()[i][j]) << " <" << std::arg(parser.y()[i][j]) << " rad) ";
        }
        std::cout << "\n";
    }


    return 0;
}
