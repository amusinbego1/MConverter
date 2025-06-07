//
// Created by Amer on 7. 6. 2025..
//

#ifndef PVBUS_H
#define PVBUS_H

struct PVBus {
    int bus_i;
    double Pd, Qd;
    double Vm, Va;
    double Pg, Qg, Pmax, Pmin, Qmax, Qmin;
};

#endif //PVBUS_H
