/************************************************************************************************
 *		Procedural City Generation: Buildings
 *		@author igarciad
 ************************************************************************************************/

#pragma once

#include "Block.h"

class VBOPmBuildings{
public:

	static bool generateBuildings(VBORenderManager& rendManager, std::vector<Block>& blocks);
	static bool generateBuilding(VBORenderManager& rendManager, Block& block, Parcel& parcel);
};

