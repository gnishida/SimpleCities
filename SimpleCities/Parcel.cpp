#include "Parcel.h"

Parcel::Parcel() {
	isPark = false;
}

/**
* Compute Parcel Buildable Area
**/
float Parcel::computeBuildableArea(float frontSetback, float rearSetback, float sideSetback, const std::vector<int> &frontEdges, const std::vector<int> &rearEdges, const std::vector<int> &sideEdges, Loop3D &pgonInset) {
	pgonInset.clear();

	int contourSz = this->parcelContour.contour.size();

	if(contourSz < 3) return 0.0f;

	//--- first, initialize values to side setback (most edges are sides)
	std::vector<float> offsetValues(contourSz, sideSetback);

	//--- then, append front ant back values
	for(int i=0; i<frontEdges.size(); ++i){
		if(frontEdges[i]<offsetValues.size()){
			offsetValues[frontEdges[i]] = frontSetback;
		}
	}

	for(int i=0; i<rearEdges.size(); ++i){
		if(rearEdges[i]<offsetValues.size()){
			offsetValues[rearEdges[i]] = rearSetback;
		}
	}

	for(int i=0; i<sideEdges.size(); ++i){
		if(sideEdges[i]<offsetValues.size()){
			offsetValues[sideEdges[i]] = sideSetback;
		}
	}

	// compute irregular offset and the inset area
	std::vector<Loop3D> offsetPolygons;
	parcelContour.offsetInside(offsetValues, offsetPolygons);

	if (offsetPolygons.size() > 0) {
		pgonInset = offsetPolygons[0];
	}
	else {
		pgonInset.clear();
	}
		
	return pgonInset.area();
}
