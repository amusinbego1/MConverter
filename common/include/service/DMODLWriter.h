//
// Created by Amer on 7. 6. 2025..
//

#ifndef WRITER_H
#define WRITER_H

#include "MParser.h"
#include <fstream>


class DMODLWriter{
    MParser parser_;
    std::string baseFilename_;
    std::ofstream out;
    int maxIter_;
    double eps_;

    static std::string extractBaseName(const std::string& filepath);
    void throwIfOutIsNotOpen();

    void writeModalHeader();
    void writeOutVariables();
    void writeParams();
    void writeSlackParams();
    void writeAdmittanceMatrix();
    void writePVBusParams();
    void writePQBusParams();
    void writeNLEs();
    void writePVBusNLEs();
    void writePQBusNLEs();
    void writeFP_i_Equation(int i);
    void writeFQ_i_Equation(int i);
    void writeFP_ij_Equation(int i, int j);
    void writeFQ_ij_Equation(int i, int j);



public:

    explicit DMODLWriter(const MParser& parser, int maxIter, double eps):  parser_(parser), baseFilename_(extractBaseName(parser.filename())),
        out(std::string(OUTPUT_PATH) + extractBaseName(parser.filename()) + ".dmodl"),
        maxIter_(maxIter),
        eps_(eps){}

    ~DMODLWriter() {
        out.close();
    }


    void write();
};

#endif //WRITER_H
