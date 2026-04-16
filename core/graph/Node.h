#ifndef SEMINARALLOCATIONENGINE_NODE_H
#define SEMINARALLOCATIONENGINE_NODE_H

#include <string>
#include "NodeType.h"

struct Node {
    int id;
    std::string label;
    NodeType type;
};

#endif
