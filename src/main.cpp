#include <iomanip>
#include <iostream>
#include "service/MParser.h"
#include "service/DMODLWriter.h"
#include "service/ConfigXmlParser.h"

void input(std::string&, int&, double&);


int main()
{
    int maxIter = 50;
    double eps = 1e-6;
    std::string caseName;

    input(caseName, maxIter, eps);

    MParser parser(caseName);

    DMODLWriter writer(parser, maxIter, eps);
    writer.write();

    std::cout << "\n.m file successfully converted to .dmodl file.\n";
    std::cout << "Press any key to exit...";
    std::cin.get();
    return 0;
}

void inputCaseName(std::string& caseName);
void inputMaxIterations(int&);
void inputEps(double&);

void input(std::string& caseName, int& maxIter, double& eps) {
    inputCaseName(caseName);
    inputMaxIterations(maxIter);
    inputEps(eps);
}

void inputCaseName(std::string& caseName) {
    std::cout << "Enter MATPOWER .m filename (without .m): ";
    std::getline(std::cin, caseName);
}

void inputMaxIterations(int & maxIter) {
    std::string line;

    std::cout << "Enter \"maxIter\" (default " << maxIter << "): ";
    std::getline(std::cin, line);
    if (!line.empty()) {
        std::istringstream iss(line);
        if (!(iss >> maxIter)) {
            std::cerr << "Invalid integer input, keeping default " << maxIter << ".\n";
        }
    }
}

void inputEps(double &eps) {
    std::string line;

    std::cout << "Enter \"eps\" (default " << eps << "): ";
    std::getline(std::cin, line);
    if (!line.empty()) {
        std::istringstream iss(line);
        if (!(iss >> eps)) {
            std::cerr << "Invalid double input, keeping default " << eps << ".\n";
        }
    }
}

