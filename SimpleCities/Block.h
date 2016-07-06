#pragma once

#include "VBORenderManager.h"
#include "Parcel.h"
#include <QVector3D>
#include "Polygon3D.h"
#include "Polygon2D.h"

class Block {
public:
	Polygon3D blockContour;
	Polygon3D sidewalkContour;
	std::vector<float> sidewalkContourRoadsWidths;
	bool isPark;
	std::vector<Parcel> parcels;

public:
	Block() : isPark(false) {}

	void clear();
	void findParcelFrontAndBackEdges(const Polygon3D& parcelContour, std::vector<int>& frontEdges, std::vector<int>& rearEdges, std::vector<int>& sideEdges);
};

