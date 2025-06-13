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
    bool small_power_limits, medium_power_limits, high_power_limits, ultra_high_power_limits;
    double small_upper_limit, medium_upper_limit, high_upper_limit;
};

#endif //CONFIG_H
