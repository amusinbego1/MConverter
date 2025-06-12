//
// Created by Amer on 12. 6. 2025..
//

#include "service/ConfigXmlParser.h"

ConfigXmlParser::ConfigXmlParser():defaultConfig_({"v", "phi", "y", "theta", true}){}

Config ConfigXmlParser::parse(){
    Config config = defaultConfig_;

    if (!parser_.parseFile(td::String("config.xml")))
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


    return config;

}