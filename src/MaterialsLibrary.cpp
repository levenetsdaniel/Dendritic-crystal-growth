#include "MaterialsLibrary.h"
#include <iostream>

Parameters MaterialsLibrary::getMaterial(MaterialType type) {
    switch (type) {
        case MaterialType::SCN:
            return scn();
        case MaterialType::SILVER:
            return silver();
        case MaterialType::ALUMINUM_OXIDE:
            return aluminumOxide();
        case MaterialType::ICE:
            return ice();
        case MaterialType::SILICON:
            return silicon();
        default:
            throw std::runtime_error("Unknown material type");
    }
}

Parameters MaterialsLibrary::getMaterialByName(const std::string &name) {
    if (name == "SCN" || name == "scn") {
        return getMaterial(MaterialType::SCN);
    } else if (name == "SILVER" || name == "silver" || name == "Ag") {
        return getMaterial(MaterialType::SILVER);
    } else if (name == "AL2O3" || name == "al2o3" || name == "sapphire" || name == "ALUMINUM_OXIDE") {
        return getMaterial(MaterialType::ALUMINUM_OXIDE);
    } else if (name == "ICE" || name == "ice" || name == "water") {
        return getMaterial(MaterialType::ICE);
    } else if (name == "SILICON" || name == "silicon" || name == "Si") {
        return getMaterial(MaterialType::SILICON);
    } else {
        throw std::runtime_error("Unknown material: " + name +
                                 ". Use: SCN, SILVER, ALUMINUM_OXIDE, ICE, or SILICON");
    }
}

void MaterialsLibrary::printAvailableMaterials() {
    std::cout << "\n=== Available Materials ===\n"
            << "1. SCN               - Succinonitrile (default, experimental standard)\n"
            << "2. SILVER            - Silver (high thermal conductivity)\n"
            << "3. ALUMINUM_OXIDE    - Al2O3 Sapphire (ceramic, high T_m)\n"
            << "4. ICE               - Water ice (snowflake patterns)\n"
            << "5. SILICON           - Silicon (semiconductor, high T_m)\n"
            << std::endl;
}
