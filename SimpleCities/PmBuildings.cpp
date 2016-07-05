﻿#include "PmBuildings.h"
#include "global.h"
#include "Util.h"

bool PmBuildings::generateBuildings(VBORenderManager& rendManager, std::vector<Block>& blocks) {
	for (int i = 0; i < blocks.size(); ++i) {
		for (int j = 0; j < blocks[i].parcels.size(); ++j) {
			if (!generateBuilding(rendManager, blocks[i], blocks[i].parcels[j])) {
				blocks[i].parcels[j].isPark = true;
			}
		}
	}

	return true;
}

/**
 * 指定されたParcelの中に、ビルを建てる。
 * ビルを建てられない場合は、falseを返却する。
 */
bool PmBuildings::generateBuilding(VBORenderManager& rendManager, Block& block, Parcel& parcel) {
	Loop3D pContourCpy;
	
	if (parcel.isPark) return false;

	// Compute parcel frontage
	std::vector<int> frontEdges;
	std::vector<int> rearEdges;
	std::vector<int> sideEdges;

	block.findParcelFrontAndBackEdges(parcel, frontEdges, rearEdges, sideEdges);

	// Compute buildable area polygon
	Loop3D footprint;
	parcel.computeBuildableArea(G::getFloat("parcel_setback_front"), G::getFloat("parcel_setback_rear"), G::getFloat("parcel_setback_sides"), frontEdges, rearEdges, sideEdges, footprint);
	if (footprint.size() == 0) return false;
	if (footprint.isSelfIntersecting()) {
		return false;
	}

	parcel.building.buildingFootprint.contour = footprint;

	// もしfootprintの一辺の長さが短すぎたら、または、短い辺と長い辺の比が大きすぎたら、ビルの建設を中止する
	QVector3D obbSize;
	QMatrix4x4 obbMat;
	parcel.building.buildingFootprint.getMyOBB(obbSize, obbMat);
	if (obbSize.x() < 5 || obbSize.y() < 5) return false;
	if (obbSize.x() > obbSize.y() * 10 || obbSize.y() > obbSize.x() * 10) return false;

	// set the elevation
	for (int i = 0; i < parcel.building.buildingFootprint.size(); ++i) {
		parcel.building.buildingFootprint[i].setZ(0.0f);
	}

	// Set building attributes
	parcel.building.numStories = std::max(1.0f, Util::genRandNormal(G::getInt("building_stories_mean"), G::getFloat("building_stories_deviation")));
	float c = rand() % 192;
	parcel.building.color = QColor(c, c, c);
	parcel.building.bldType = 1;

	return true;
}
