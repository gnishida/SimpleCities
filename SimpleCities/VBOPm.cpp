/************************************************************************************************
 *		Procedural City Generation
 *		@author igarciad
 ************************************************************************************************/

#include "VBOPm.h"
#include "Polygon3D.h"

#include <qdir.h>
#include <QStringList>
#include <QTime>

#include "VBOPmBlocks.h"
#include "VBOPmParcels.h"
#include "VBOPmBuildings.h"
#include "BlockSet.h"
#include "VBOGeoBuilding.h"
#include "VBOVegetation.h"
#include "Polygon3D.h"
#include "Util.h"

bool VBOPm::generateBuildings(VBORenderManager& rendManager, BlockSet& blocks) {
	rendManager.removeStaticGeometry("3d_building");
		
	for (int bN = 0; bN < blocks.size(); bN++) {
		if (blocks[bN].isPark) continue;

		for (int pN = 0; pN < blocks[bN].myParcels.size(); ++pN) {
			if (blocks[bN].myParcels[pN].isPark) continue;
			if (blocks[bN].myParcels[pN].myBuilding.buildingFootprint.size() < 3) continue;

			VBOGeoBuilding::generateBuilding(rendManager, blocks[bN].myParcels[pN].myBuilding);
		}
	}
	printf("Building generation is done.\n");

	return true;
}

bool VBOPm::generateBuildings(VBORenderManager& rendManager, std::vector<Building>& buildings) {
	rendManager.removeStaticGeometry("3d_building");

	for (int i = 0; i < buildings.size(); i++) {
		VBOGeoBuilding::generateBuilding(rendManager, buildings[i]);
	}
	printf("Building generation is done.\n");

	return true;
}

bool VBOPm::generateVegetation(VBORenderManager& rendManager, BlockSet& blocks) {
	VBOVegetation::generateVegetation(rendManager, blocks.blocks);

	return true;
}
