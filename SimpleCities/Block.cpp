#include "Block.h"
#include <QVector2D>

void Block::clear() {
	this->blockContour.contour.clear();
	this->sidewalkContour.clear();
	this->sidewalkContourRoadsWidths.clear();
	this->parcels.clear();
}

void Block::findParcelFrontAndBackEdges(const Polygon3D& parcelContour, std::vector<int>& frontEdges, std::vector<int>& rearEdges, std::vector<int>& sideEdges) {
	frontEdges.clear();
	rearEdges.clear();
	sideEdges.clear();

	float kDistTol = 0.5f;

	for (int i = 0; i < parcelContour.size(); ++i) {
		int next = ((i + 1)) % parcelContour.size();

		QVector3D midPt = (parcelContour[i] + parcelContour[next]) * 0.5f;

		float distPtThis = blockContour.distanceXYToPoint(parcelContour[i]);
		float distPtNext = blockContour.distanceXYToPoint(parcelContour[next]);
		float distPtMid = blockContour.distanceXYToPoint(midPt);
		int numPtsThatAreClose = (int)(distPtThis < kDistTol) + (int)(distPtMid < kDistTol) + (int)(distPtNext < kDistTol);

		bool midPtIsClose = (distPtThis < kDistTol);

		switch (numPtsThatAreClose) {
		case 0:
			rearEdges.push_back(i);
			break;
		case 1:					
		case 2:
			sideEdges.push_back(i);
			break;
		case 3:
			frontEdges.push_back(i);
			break;
		}
	}
}
