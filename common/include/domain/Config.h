//
// Created by Amer on 12. 6. 2025..
//

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <td/String.h>

struct Config {
    td::String v_symbol, phi_symbol, y_symbol, theta_symbol;
    bool comments_included;
};

#endif //CONFIG_H
