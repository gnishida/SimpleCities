/************************************************************************************************
 *		Procedural City Generation: Buildings
 *		@author igarciad
 ************************************************************************************************/

#pragma once

#include "VBOBlock.h"

class VBOPmBuildings{
public:

	static bool generateBuildings(VBORenderManager& rendManager, std::vector< Block > &blocks);
	static bool generateParcelBuildings(VBORenderManager& rendManager, Block &inBlock, Parcel &inParcel);
};

