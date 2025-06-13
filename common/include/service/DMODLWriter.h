//
// Created by Amer on 7. 6. 2025..
//

#ifndef WRITER_H
#define WRITER_H

#include "MParser.h"
#include <fstream>

#include "ConfigXmlParser.h"
#include "domain/Config.h"


class DMODLWriter{
    MParser parser_;
    std::string baseFilename_;
    std::ofstream out;
    int maxIter_;
    double eps_;
    Config config_;

    enum TrigFunction {
        SIN, COS
    };

    enum EquationType {
        Q, P
    };

    friend std::ostream& operator<<(std::ostream&, TrigFunction);

    static std::string extractBaseName(const std::string& filepath);
    [[nodiscard]] bool shouldParamBeIncluded(int i, int j) const;
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
    void writeEquationWithSlack(int i, EquationType);
    void writeEquationWithPV(int i, EquationType);
    void writeEquationWithPQ(int i, EquationType);
    void writeLimits();
    void writePowerParams(const PVBus& pv_bus);
    void writeSmallPowerParams(const PVBus& pv_bus);
    void writeMediumPowerParams(const PVBus& pv_bus);
    void writeHighPowerParams(const PVBus& pv_bus);
    void writeUltraHighPowerParams(const PVBus& pv_bus);
    void writeSmallGroup();
    void writeMediumGroup();
    void writeHighGroup();
    void writeUltraHighGroup();
    void writeGroupHeader(const char *);
    void writeOneLimit(const PVBus& pv_bus);


    void writeF_ij_Equation(int i, int j, TrigFunction trig_function);
    void rewriteEqualOverPlusSign();
    void rewriteEqualOverSpace();
    void writeComment(const char * comment);
    std::optional<Branch> findBranch(int from, int to) const;


public:

    explicit DMODLWriter(const MParser& parser, int maxIter, double eps):  parser_(parser), baseFilename_(extractBaseName(parser.filename())),
        out(std::string(OUTPUT_PATH) + extractBaseName(parser.filename()) + ".dmodl"),
        maxIter_(maxIter),
        config_(ConfigXmlParser().parse()),
        eps_(eps){}

    ~DMODLWriter() {
        out.close();
    }


    void write();
};

#endif //WRITER_H
