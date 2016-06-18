/************************************************************************************************
 *		Procedural City Generation: Buildings geometry
 *		@author igarciad
 ************************************************************************************************/

#pragma once

#include "Building.h"

class VBORenderManager;

class VBOGeoBuilding {
public:
	static void generateBuilding(VBORenderManager& rendManager, Building& building);
	static void generateHouse(VBORenderManager& rendManager, Building& building);
	static bool bldgInitialized;
	static void initBuildingsTex();
};
