//
// Created by Amer on 12. 6. 2025..
//

#ifndef CONFIGXMLPARSER_H
#define CONFIGXMLPARSER_H

#include <xml/DOMParser.h>
#include "domain/Config.h"

class ConfigXmlParser {
    xml::FileParser parser_;
    Config defaultConfig_;

public:
    ConfigXmlParser();
    Config parse();
};



#endif //CONFIGXMLPARSER_H
