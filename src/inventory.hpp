#pragma once

#include <vector>
#include <string>

typedef struct Evidence {
    std::string name;
    bool registered;
} Evidence;

extern std::vector<Evidence> evidence;

void inventory_create_evidence(std::string name);
void inventory_register_evidence(std::string name);
bool inventory_is_evidence_registered(std::string name);
