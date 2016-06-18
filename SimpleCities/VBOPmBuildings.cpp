/************************************************************************************************
*		Procedural City Generation: Buildings
*		@author igarciad
************************************************************************************************/

#include "VBOPmBuildings.h"
#include "global.h"
#include "Utils.h"

bool VBOPmBuildings::generateBuildings(VBORenderManager& rendManager, std::vector< Block > &blocks) {
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
* Compute Building Footprint Polygon
**/
bool computeBuildingFootprintPolygon(float maxFrontage, float maxDepth,	std::vector<int> &frontEdges, std::vector<int> &rearEdges, std::vector<int> &sideEdges, Loop3D &buildableAreaCont, Loop3D &buildingFootprint) {
	if( (maxFrontage < 1.0f) || (maxDepth < 1.0f) ){
		buildingFootprint = buildableAreaCont;
		return true;
	}

	buildingFootprint.clear();

	int frontageIdx = -1;
	int frontageIdxNext;

	int baSz = buildableAreaCont.size();
	if(baSz < 3){
		return false;
	}

	float curLength;
	float maxBALength = 0;

	int thisIdx;
	int nextIdx;

	bool orientedCW = Polygon3D::reorientFace(buildableAreaCont, true);

	for(int i=0; i<frontEdges.size(); ++i){

		//std::cout << "i: " << i << "    FESz: " << frontEdges.size() <<  "\n"; std::fflush(stdout);
		thisIdx = frontEdges.at(i);
		if(orientedCW){			
			nextIdx = (thisIdx-1+baSz)%baSz;
		} else {
			nextIdx = (thisIdx+1)%baSz;		
		}
		curLength = (buildableAreaCont.at(thisIdx)-buildableAreaCont.at(nextIdx)).lengthSquared();		
		if(curLength > maxBALength){
			maxBALength = curLength;
			frontageIdx = thisIdx;
			frontageIdxNext = nextIdx;
		}
	}

	if(frontageIdx == -1){
		// 一番長いエッジを、正面としちゃう
		for (int i = 0; i < buildableAreaCont.size(); ++i) {
			nextIdx = (i + 1) % baSz;

			curLength = (buildableAreaCont[i] - buildableAreaCont[nextIdx]).lengthSquared();		
			if(curLength > maxBALength){
				maxBALength = curLength;
				frontageIdx = i;
				frontageIdxNext = nextIdx;
			}
		}
	}

	maxBALength = sqrt(maxBALength);

	//std::cout << "f: " << frontageIdx << "   n: " << frontageIdxNext << "    s: " << buildableAreaCont.size() << "\n";
	//std::fflush(stdout);

	QVector3D frontPtA, frontPtB;
	QVector3D rearPtA,  rearPtB;

	QVector3D frontageCenter = 0.5f*(buildableAreaCont.at(frontageIdx) + buildableAreaCont.at(frontageIdxNext));
	QVector3D frontageVector = (buildableAreaCont.at(frontageIdx)-
		buildableAreaCont.at(frontageIdxNext)).normalized();
	QVector3D depthVector(frontageVector.y(), -frontageVector.x(), frontageVector.z());

	float actualFrontage = std::min<float>(maxFrontage, maxBALength);
	float actualDepth = maxDepth + maxDepth*((qrand()*0.1f)/RAND_MAX-0.05f);//misctools::genRand(-0.05, 0.05)

	frontPtA = frontageCenter - 0.5*actualFrontage*frontageVector;
	frontPtB = frontageCenter + 0.5*actualFrontage*frontageVector;
	rearPtA  = frontPtA + depthVector*actualDepth;
	rearPtB  = frontPtB + depthVector*actualDepth;

	buildingFootprint.push_back(rearPtA);
	buildingFootprint.push_back(rearPtB);
	buildingFootprint.push_back(frontPtB);
	buildingFootprint.push_back(frontPtA);
	printf("buildingFootprint %d\n",buildingFootprint.size());

	return true;
}

/**
 * 指定されたParcelの中に、ビルを建てる。
 */
bool VBOPmBuildings::generateParcelBuildings(VBORenderManager& rendManager, Block &inBlock, Parcel &inParcel) {
	Loop3D pContourCpy;

	if (inParcel.parcelContour.isClockwise()) {
		int xxx = 0;
	}
	
	if (inParcel.isPark) return false;

	// Compute parcel frontage
	std::vector<int> frontEdges;
	std::vector<int> rearEdges;
	std::vector<int> sideEdges;

	inBlock.findParcelFrontAndBackEdges(inBlock, inParcel, frontEdges, rearEdges, sideEdges);

	// Compute buildable area polygon
	inParcel.computeBuildableArea(G::getFloat("parcel_setback_front"), G::getFloat("parcel_setback_rear"), G::getFloat("parcel_setback_sides"), frontEdges, rearEdges, sideEdges, inParcel.parcelBuildableAreaContour.contour);
	if (inParcel.parcelBuildableAreaContour.contour.size() == 0) return false;
	if (inParcel.parcelBuildableAreaContour.isSelfIntersecting()) {
		inParcel.parcelBuildableAreaContour.contour.clear();
		return false;
	}

	inParcel.myBuilding.buildingFootprint.contour = inParcel.parcelBuildableAreaContour.contour;

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
