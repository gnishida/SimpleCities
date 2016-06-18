/************************************************************************************************
 *		Procedural City Generation: Parcel
 *		@author igarciad
 ************************************************************************************************/

#include "VBOPmParcels.h"
#include "qmatrix4x4.h"
#include "global.h"
#include "Util.h"

bool VBOPmParcels::generateParcels(VBORenderManager& rendManager, std::vector< Block > &blocks) {
	srand(0);
	for (int i = 0; i < blocks.size(); ++i) {
		if (blocks[i].valid) {
			subdivideBlockIntoParcels(blocks[i]);
		}
	}

	return true;
}

void VBOPmParcels::subdivideBlockIntoParcels(Block &block) {
	//srand(block.randSeed);
	std::vector<Parcel> tmpParcels;

	block.myParcels.clear();

	// set the initial parcel be the block itself
	Parcel tmpParcel;
	tmpParcel.parcelContour = block.blockContour;

	if (block.isPark) {
		tmpParcel.isPark = true;
		tmpParcels.push_back(tmpParcel);
	} else {
		subdivideParcel(tmpParcel, G::getFloat("parcel_area_mean"), G::getFloat("parcel_area_min"), G::getFloat("parcel_area_deviation"), G::getFloat("parcel_split_deviation"), tmpParcels);
	}

	for (int i = 0; i < tmpParcels.size(); ++i) {
		//add parcel to block
		block.myParcels.push_back(tmpParcels[i]);

		if (block.myParcels.back().parcelContour.isClockwise()) {
			std::reverse(block.myParcels.back().parcelContour.contour.begin(), block.myParcels.back().parcelContour.contour.end());
		}

		if (Util::genRand() < 0.05) {
			block.myParcels.back().isPark = true;
		}
	}
}

/**
* Parcel subdivision
* @desc: Recursive subdivision of a parcel using OBB technique
* @return: true if parcel was successfully subdivided, false otherwise
* @areaMean: mean target area of parcels after subdivision
* @areaVar: variance of parcels area after subdivision (normalized in 0-1)
* @splitIrregularity: A normalized value 0-1 indicating how far
*					from the middle point the split line should be
**/
bool VBOPmParcels::subdivideParcel(Parcel& parcel, float areaMean, float areaMin, float areaStd,	float splitIrregularity, std::vector<Parcel> &outParcels) {
	float thresholdArea = areaMean + areaStd * areaMean * Util::genRand(-1, 1);
	
	if (parcel.parcelContour.area() <= std::max(thresholdArea, areaMin)) {
		outParcels.push_back(parcel);
		return true;
	}

	// compute OBB
	QVector3D obbSize;
	QMatrix4x4 obbMat;
	parcel.parcelContour.getMyOBB(obbSize, obbMat);

	// compute split line passing through center of OBB TODO (+/- irregularity)
	//		and with direction parallel/perpendicular to OBB main axis
	QVector3D slEndPoint;
	QVector3D dirVectorInit, dirVector, dirVectorOrthogonal;
	QVector3D midPt(0.0f, 0.0f, 0.0f);
	QVector3D auxPt(1.0f, 0.0f, 0.0f);
	QVector3D midPtNoise(0.0f, 0.0f, 0.0f);
	std::vector<QVector3D> splitLine;	

	midPt = midPt*obbMat;

	dirVectorInit = (auxPt*obbMat - midPt);
	dirVectorInit.normalize();
	if (obbSize.x() > obbSize.y()) {
		dirVector.setX(-dirVectorInit.y());
		dirVector.setY(dirVectorInit.x());
	} else {
		dirVector = dirVectorInit;
	}

	midPtNoise.setX(splitIrregularity * Util::genRand(-10, 10));
	midPtNoise.setY(splitIrregularity * Util::genRand(-10, 10));
	midPt = midPt + midPtNoise;

	slEndPoint = midPt + 10000.0f*dirVector;
	splitLine.push_back(slEndPoint);
	slEndPoint = midPt - 10000.0f*dirVector;
	splitLine.push_back(slEndPoint);

	// split parcel with line and obtain two new parcels
	Polygon3D pgon1, pgon2;

	float kDistTol = 0.01f;

	std::vector<Polygon3D> pgons;

	// Use the simple splitting because this is fast
	if (parcel.parcelContour.splitMeWithPolyline(splitLine, pgon1.contour, pgon2.contour)) {
		Parcel parcel1;
		Parcel parcel2;

		parcel1.parcelContour = pgon1;
		parcel2.parcelContour = pgon2;

		// call recursive function for both parcels
		subdivideParcel(parcel1, areaMean, areaMin, areaStd, splitIrregularity, outParcels);
		subdivideParcel(parcel2, areaMean, areaMin, areaStd, splitIrregularity, outParcels);
	} else {
		// If the simple splitting fails, try CGAL version which is slow
		if (parcel.parcelContour.split(splitLine, pgons)) {
			for (int i = 0; i < pgons.size(); ++i) {
				Parcel parcel;
				parcel.parcelContour = pgons[i];

				subdivideParcel(parcel, areaMean, areaMin, areaStd, splitIrregularity, outParcels);
			}
		} else {
			parcel.isPark = true;
			outParcels.push_back(parcel);
		}
	}

	return true;
}
