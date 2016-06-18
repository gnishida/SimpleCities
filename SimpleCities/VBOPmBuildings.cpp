/************************************************************************************************
*		Procedural City Generation: Buildings
*		@author igarciad
************************************************************************************************/

#include "VBOPmBuildings.h"
#include "global.h"
#include "Utils.h"

bool VBOPmBuildings::generateBuildings(VBORenderManager& rendManager, std::vector<Block>& blocks) {
	for (int i = 0; i < blocks.size(); ++i) {
		Block::parcelGraphVertexIter vi, viEnd;

		for (boost::tie(vi, viEnd) = boost::vertices(blocks[i].myParcels); vi != viEnd; ++vi) {
			if (!generateParcelBuildings(rendManager, blocks[i], blocks[i].myParcels[*vi])) {
				blocks[i].myParcels[*vi].isPark = true;
			}
		}
	}

	return true;
}

/**
 * 指定されたParcelの中に、ビルを建てる。
 * ビルを建てられない場合は、falseを返却する。
 */
bool VBOPmBuildings::generateParcelBuildings(VBORenderManager& rendManager, Block& inBlock, Parcel& inParcel) {
	Loop3D pContourCpy;
	
	if (inParcel.isPark) return false;

	// Compute parcel frontage
	std::vector<int> frontEdges;
	std::vector<int> rearEdges;
	std::vector<int> sideEdges;

	inBlock.findParcelFrontAndBackEdges(inParcel, frontEdges, rearEdges, sideEdges);

	// Compute buildable area polygon
	Loop3D footprint;
	inParcel.computeBuildableArea(G::getFloat("parcel_setback_front"), G::getFloat("parcel_setback_rear"), G::getFloat("parcel_setback_sides"), frontEdges, rearEdges, sideEdges, footprint);
	if (footprint.size() == 0) return false;
	if (footprint.isSelfIntersecting()) {
		return false;
	}

	inParcel.myBuilding.buildingFootprint.contour = footprint;

	// もしfootprintの一辺の長さが短すぎたら、または、短い辺と長い辺の比が大きすぎたら、ビルの建設を中止する
	QVector3D obbSize;
	QMatrix4x4 obbMat;
	inParcel.myBuilding.buildingFootprint.getMyOBB(obbSize, obbMat);
	if (obbSize.x() < 5 || obbSize.y() < 5) return false;
	if (obbSize.x() > obbSize.y() * 10 || obbSize.y() > obbSize.x() * 10) return false;

	// set the elevation
	for (int i = 0; i < inParcel.myBuilding.buildingFootprint.contour.size(); ++i) {
		inParcel.myBuilding.buildingFootprint[i].setZ(0.0f);
	}

	// Set building attributes
	inParcel.myBuilding.numStories = std::max(1.0, utils::normal_rand(G::getInt("building_stories_mean"), G::getFloat("building_stories_deviation")));
	float c = rand() % 192;
	inParcel.myBuilding.color = QColor(c, c, c);
	inParcel.myBuilding.bldType = 1;

	return true;
}
