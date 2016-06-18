#pragma once

#include <QSettings>
#include "Building.h"
#include "Polygon3D.h"

class Parcel {
public:
	Polygon3D parcelContour;
	bool isPark;
	Building myBuilding;

public:
	Parcel();

	float computeBuildableArea(float frontSetback, float rearSetback, float sideSetback, const std::vector<int> &frontEdges, const std::vector<int> &rearEdges, const std::vector<int> &sideEdges, Loop3D &pgonInset);
};

