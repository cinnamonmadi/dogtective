#include "inventory.hpp"

#include <iostream>

std::vector<Evidence> evidence;

void inventory_create_evidence(std::string name) {
    evidence.push_back((Evidence) {
        .name = name,
        .registered = false
    });
}

void inventory_register_evidence(std::string name) {
    for(int i = 0; i < evidence.size(); i++) {
        if(evidence[i].name == name) {
            evidence[i].registered = true;
        }
    }

    std::cout << "Error! Unable to register evidence " << name << " because it hasn't been created yet!" << std::endl;
}

bool inventory_is_evidence_registered(std::string name) {
    for(int i = 0; i < evidence.size(); i++) {
        if(evidence[i].name == name) {
            return evidence[i].registered;
        }
    }

    std::cout << "Error! Unable to check if evidence " << name << " is registered because it hasn't been created yet!" << std::endl;
    return false;
}
