#include "Block.h"
#include <QVector2D>

void Block::clear() {
	this->blockContour.contour.clear();
	this->sidewalkContour.clear();
	this->sidewalkContourRoadsWidths.clear();
	this->myParcels.clear();
}

void Block::findParcelFrontAndBackEdges(const Parcel& inParcel, std::vector<int>& frontEdges, std::vector<int>& rearEdges, std::vector<int>& sideEdges) {
	frontEdges.clear();
	rearEdges.clear();
	sideEdges.clear();

	float kDistTol = 0.01f;

	for (int i = 0; i < inParcel.parcelContour.size(); ++i) {
		int next = ((i + 1)) % inParcel.parcelContour.size();

		QVector3D midPt = (inParcel.parcelContour[i] + inParcel.parcelContour[next]) * 0.5f;

		float distPtThis = blockContour.distanceXYToPoint(inParcel.parcelContour[i]);
		float distPtNext = blockContour.distanceXYToPoint(inParcel.parcelContour[next]);
		float distPtMid = blockContour.distanceXYToPoint(midPt);
		int numPtsThatAreClose = (int)(distPtThis < kDistTol) + (int)(distPtMid < kDistTol) + (int)(distPtNext < kDistTol);

		bool midPtIsClose = (distPtThis < kDistTol);

		switch (numPtsThatAreClose) {
			//if neither one is close to block boundary, then segment is a rear segment
		case 0:
			rearEdges.push_back(i);
			break;
			//if only one between this and next is close to block boundary, then segment is a side segment
		case 1:					
		case 2:
			sideEdges.push_back(i);
			break;
			//if this or next are very close to block boundary, then segment is a front segment
		case 3:
			frontEdges.push_back(i);
			break;
		}
	}
}
