//
// Created by Amer on 7. 6. 2025..
//

#ifndef WRITER_H
#define WRITER_H

#include "MParser.h"
#include <fstream>


class Writer{
    MParser parser_;
    std::string baseFilename_;
    std::ofstream out;

    static std::string extractBaseName(const std::string& filepath);
    void throwIfOutIsNotOpen();

    void writeModalHeader();
    void writeOutVariables();
    void writeParams();
    void writeSlackParams();
    void writeAdmittanceMatrix();
    void writePVBusParams();
    void writePQBusParams();



public:

    explicit Writer(const MParser& parser):  parser_(parser), baseFilename_(extractBaseName(parser.filename())),
        out(std::string(OUTPUT_PATH) + extractBaseName(parser.filename()) + ".txt"){
    }

    ~Writer() {
        out.close();
    }


    void write();
};

#endif //WRITER_H
