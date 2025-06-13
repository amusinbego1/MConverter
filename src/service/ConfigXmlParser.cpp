//
// Created by Amer on 12. 6. 2025..
//

#include "service/ConfigXmlParser.h"

ConfigXmlParser::ConfigXmlParser():defaultConfig_({
    "v",
    "phi",
    "y",
    "theta",
    true,
    false,
    false,
    false,
    false,
    50,
    150,
1000}
){}

Config ConfigXmlParser::parse(){
    Config config = defaultConfig_;

    if (!parser_.parseFile(td::String("resources/config.xml")))
        return config;

    auto root = parser_.getRootNode();
    if (root -> getName().cCompare("Configuration") != 0)
    {
        std::cout << "Pogresan xml fajl config.xml" << td::endl;
        return config;
    }

    auto v_symbol = root.getChildNode("v_symbol");
    while (v_symbol.isOk())
    {
        config.v_symbol = v_symbol->getValue();
        ++v_symbol;
    }

    auto phi_symbol = root.getChildNode("phi_symbol");
    while (phi_symbol.isOk())
    {
        config.phi_symbol = phi_symbol->getValue();
        ++phi_symbol;
    }

    auto y_symbol = root.getChildNode("y_symbol");
    while (y_symbol.isOk())
    {
        config.y_symbol = y_symbol->getValue();
        ++y_symbol;
    }

    auto theta_symbol = root.getChildNode("theta_symbol");
    while (theta_symbol.isOk())
    {
        config.theta_symbol = theta_symbol->getValue();
        ++theta_symbol;
    }

    auto comments_included = root.getChildNode("comments_included");
    while (comments_included.isOk())
    {
        td::String comments_included_str = comments_included->getValue();
        config.comments_included = !comments_included_str.cCompareNoCase("true");
        ++comments_included;
    }

    auto small_power_limits = root.getChildNode("small_power_limits");
    while (small_power_limits.isOk())
    {
        td::String small_power_limits_str = small_power_limits->getValue();
        config.small_power_limits = !small_power_limits_str.cCompareNoCase("true");
        ++small_power_limits;
    }

    auto medium_power_limits = root.getChildNode("medium_power_limits");
    while (medium_power_limits.isOk())
    {
        td::String medium_power_limits_str = medium_power_limits->getValue();
        config.medium_power_limits = !medium_power_limits_str.cCompareNoCase("true");
        ++medium_power_limits;
    }

    auto high_power_limits = root.getChildNode("high_power_limits");
    while (high_power_limits.isOk())
    {
        td::String high_power_limits_str = high_power_limits->getValue();
        config.high_power_limits = !high_power_limits_str.cCompareNoCase("true");
        ++high_power_limits;
    }

    auto ultra_high_power_limits = root.getChildNode("ultra_high_power_limits");
    while (ultra_high_power_limits.isOk())
    {
        td::String ultra_high_power_limits_str = ultra_high_power_limits->getValue();
        config.ultra_high_power_limits = !ultra_high_power_limits_str.cCompareNoCase("true");
        ++ultra_high_power_limits;
    }

    return config;

}