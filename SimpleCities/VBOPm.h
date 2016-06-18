/************************************************************************************************
 *		Procedural City Generation
 *		@author igarciad
 ************************************************************************************************/

#pragma once

#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

#include "Building.h"
#include "BlockSet.h"
#include <vector>

class VBORenderManager;

class VBOPm {
public:
	static bool generateBuildings(VBORenderManager& rendManager, BlockSet& blocks);
	static bool generateBuildings(VBORenderManager& rendManager, std::vector<Building>& buildings);
	static bool generateVegetation(VBORenderManager& rendManager, BlockSet& blocks);
};


