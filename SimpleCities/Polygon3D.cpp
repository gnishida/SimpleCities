﻿#include "Polygon3D.h"
#include <QVector2D>
#include <QMatrix4x4>
#include "Util.h"
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/create_offset_polygons_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Partition_traits_2.h>
#include <CGAL/partition_2.h>

bool Loop3D::isPointWithinLoop(const QVector3D& pt) const {
	Loop3D closedLoop = *this;
	if (closedLoop.front().x() != closedLoop.back().x() || closedLoop.front().y() != closedLoop.back().y()) {
		closedLoop.push_back(closedLoop.front());
	}
	if (!closedLoop.isClockwise()) {
		std::reverse(closedLoop.begin(), closedLoop.end());
	}
	return boost::geometry::within(pt, closedLoop);
}

bool Loop3D::isClockwise() const {
	float tmpSum = 0.0f;

	for (int i = 0; i < size(); ++i) {
		int next = (i + 1) % size();
		tmpSum = tmpSum + (this->at(next).x() - this->at(i).x()) * (this->at(next).y() + this->at(i).y());
	}

	if (tmpSum > 0.0f) {
		return true;
	}
	else {
		return false;
	}
}

float Loop3D::area() const {
	boost::geometry::ring_type<Polygon3D>::type bg_pgon;
	boost::geometry::assign(bg_pgon, *this);
	boost::geometry::correct(bg_pgon);

	return boost::geometry::area(bg_pgon);
}

bool Loop3D::isSelfIntersecting() const {
	boost::geometry::ring_type<Polygon3D>::type bg_pgon;
	boost::geometry::assign(bg_pgon, *this);
	boost::geometry::correct(bg_pgon);
	return boost::geometry::intersects(bg_pgon);
}

float Loop3D::distanceXYToPoint(const QVector3D &pt) const {
	float minDist = std::numeric_limits<float>::max();

	for (size_t i = 0; i < size(); ++i) {
		int next = (i + 1) % size();

		QVector2D closestPt;
		float dist = Util::pointSegmentDistanceXY(QVector2D(at(i)), QVector2D(at(next)), QVector2D(pt), closestPt);
		if (dist < minDist) {
			minDist = dist;
		}
	}
	return minDist;
}

void Loop3D::simplify(float threshold = 1.0f) {
	float thresholdSq = threshold * threshold;

	if (size() < 3) {
		return;
	}

	boost::geometry::ring_type<Polygon3D>::type bg_pin;
	boost::geometry::ring_type<Polygon3D>::type bg_pout;
	boost::geometry::assign(bg_pin, *this);
	boost::geometry::correct(bg_pin);
	boost::geometry::simplify(bg_pin, bg_pout, threshold);

	// copy points back
	clear();
	for (size_t i = 0; i < bg_pout.size(); ++i) {
		push_back(QVector3D(bg_pout[i].x(), bg_pout[i].y(), 0));
	}

	// remove last point
	if ((front() - back()).lengthSquared() < thresholdSq) {
		pop_back();
	}

	// clean angles
	float angleThreshold = 0.01f;
	for (size_t i = 0; i < size(); ++i) {
		int next = (i + 1) % size();
		int prev = (i - 1 + size()) % size();
		QVector3D cur_prev = at(prev) - at(i);
		QVector3D cur_next = at(next) - at(i);

		float ang = Util::angleBetweenVectors(cur_prev, cur_next);
		if ((fabs(ang) < angleThreshold) || (fabs(ang - 3.14159265f) < angleThreshold) || (!(ang == ang))) {
			erase(begin() + i);
			--i;
		}
	}

	if (isClockwise()) {
		std::reverse(begin(), end());
	}
}

bool Polygon3D::isClockwise() const {
	return contour.isClockwise();
}

void Polygon3D::correct() {
	if (isClockwise()) {
		std::reverse(contour.begin(), contour.end());
	}
}

/*double angleBetweenVectors(QVector3D &vec1, QVector3D &vec2){	
	return acos( 0.999*(QVector3D::dotProduct(vec1, vec2)) / ( vec1.length() * vec2.length() ) );
}*/

void Polygon3D::computeInset(std::vector<float> &offsetDistances, Loop3D &pgonInset) const {
	pgonInset.clear();

	Loop3D cleanPgon; 
	double tol = 0.01f;

	cleanPgon = this->contour;

	int prev, next;
	int cSz = cleanPgon.size();

	if (cSz < 3) return;

	/*if (reorientFace(cleanPgon)){				
		std::reverse(offsetDistances.begin(), offsetDistances.end() - 1);
	}*/

	//if offsets are zero, add a small epsilon just to avoid division by zero
	for(size_t i=0; i<offsetDistances.size(); ++i){
		if(fabs(offsetDistances[i]) < tol){
			offsetDistances[i] = tol;
		}
	}

	QVector3D intPt;

	if (offsetDistances[0] > 0) {	// 内側へのoffsetの計算（Block生成、footprint生成などに使用）
		// ToDo: We should improve this logic to make it more stable.
		pgonInset.clear();
		for (int cur = 0; cur<cSz; ++cur){
			//Some geometry and trigonometry

			//point p1 is the point with index cur
			prev = (cur - 1 + cSz) % cSz; //point p0
			next = (cur + 1) % cSz;	  //point p2

			if (Util::diffAngle(cleanPgon[prev] - cleanPgon[cur], cleanPgon[next] - cleanPgon[cur]) < 0.1f) {
				// For deanend edge
				QVector3D vec = cleanPgon[cur] - cleanPgon[prev];
				QVector3D vec2(-vec.y(), vec.x(), 0);

				float angle = atan2f(vec2.y(), vec2.x());
				for (int i = 0; i <= 10; ++i) {
					float a = angle - (float)i * M_PI / 10.0f;
					intPt = QVector3D(cleanPgon[cur].x() + cosf(a) * offsetDistances[cur], cleanPgon[cur].y() + sinf(a) * offsetDistances[cur], cleanPgon[cur].z());
					pgonInset.push_back(intPt);
				}
			}
			else {
				Util::getIrregularBisector(cleanPgon[prev], cleanPgon[cur], cleanPgon[next], offsetDistances[prev], offsetDistances[cur], intPt);
				
				// For acute angle
				/*if (pgonInset.size() >= 2) {
					if (Util::diffAngle(pgonInset[pgonInset.size() - 2] - pgonInset[pgonInset.size() - 1], intPt - pgonInset[pgonInset.size() - 1]) < 0.1f) {
						pgonInset.erase(pgonInset.begin() + pgonInset.size() - 1);
					}
				}*/

				pgonInset.push_back(intPt);
			}
		}
	}
	else {	// 外側へのoffsetの計算（ビルの屋根などで使用）
		// ToDo: We should improve this logic to make it more stable.
		pgonInset.clear();
		for (int cur = 0; cur<cSz; ++cur){
			//Some geometry and trigonometry

			//point p1 is the point with index cur
			prev = (cur - 1 + cSz) % cSz; //point p0
			next = (cur + 1) % cSz;	  //point p2

			if (Util::diffAngle(cleanPgon[prev] - cleanPgon[cur], cleanPgon[next] - cleanPgon[cur]) < 0.1f) {
				// For deanend edge
				QVector3D vec = cleanPgon[cur] - cleanPgon[prev];
				QVector3D vec2(-vec.y(), vec.x(), 0);

				float angle = atan2f(vec2.y(), vec2.x());
				for (int i = 0; i <= 10; ++i) {
					float a = angle - (float)i * M_PI / 10.0f;
					intPt = QVector3D(cleanPgon[cur].x() + cosf(a) * offsetDistances[cur], cleanPgon[cur].y() + sinf(a) * offsetDistances[cur], cleanPgon[cur].z());
					pgonInset.push_back(intPt);
				}
			}
			else {
				Util::getIrregularBisector(cleanPgon[prev], cleanPgon[cur], cleanPgon[next], offsetDistances[prev], offsetDistances[cur], intPt);
				
				// For acute angle
				if (pgonInset.size() >= 2) {
					if (Util::diffAngle(pgonInset[pgonInset.size() - 2] - pgonInset[pgonInset.size() - 1], intPt - pgonInset[pgonInset.size() - 1]) < 0.1f) {
						pgonInset.erase(pgonInset.begin() + pgonInset.size() - 1);
					}
				}

				pgonInset.push_back(intPt);
			}
		}
	}
	
	if (pgonInset.size() > 0) {
		boost::geometry::ring_type<Polygon3D>::type bg_contour;
		boost::geometry::ring_type<Polygon3D>::type bg_contour_inset;

		boost::geometry::assign(bg_contour_inset, pgonInset);
		boost::geometry::correct(bg_contour_inset);

		if (boost::geometry::intersects(bg_contour_inset)) {
			computeInset2(offsetDistances[0], pgonInset);
			return;
		}
		else {
			// 内側へのinsetなのに、areaが大きくなった場合は、CGALを利用する。
			if (offsetDistances[0] > 0) {
				if (pgonInset.area() > contour.area()) {
					computeInset2(offsetDistances[0], pgonInset);
					return;
				}
			}

			// 元のポリゴンと交差する場合は、CGALを利用する
			bool intersected = false;
			for (int i = 0; i < pgonInset.size() && !intersected; ++i) {
				int next = (i + 1) % pgonInset.size();

				for (int j = 0; j < contour.size(); ++j) {
					int next2 = (j + 1) % contour.size();

					float tab, tcd;
					QVector3D intPoint;
					if (Util::segmentSegmentIntersectXY3D(pgonInset[i], pgonInset[next], contour[j], contour[next2], &tab, &tcd, true, intPoint)) {
						intersected = true;
						break;
					}
				}
			}

			if (intersected) {
				computeInset2(offsetDistances[0], pgonInset);
				return;
			}
		}
	}
}

void Polygon3D::transformLoop(const Loop3D& pin, Loop3D& pout, const QMatrix4x4& transformMat) {
	pout = pin;
	for (int i = 0; i < pin.size(); ++i) {
		pout.at(i) = transformMat*pin.at(i);
	}
}

//Only works for polygons with no holes in them
bool Polygon3D::splitMeWithPolyline(const std::vector<QVector3D> &pline, Loop3D &pgon1, Loop3D &pgon2) {
	bool polylineIntersectsPolygon = false;

	int plineSz = pline.size();
	int contourSz = this->contour.size();

	if (plineSz < 2 || contourSz < 3) {
		//std::cout << "ERROR: Cannot split if polygon has fewer than three vertices of if polyline has fewer than two points\n.";
		return false;
	}
		
	QVector2D firstIntPt;
	QVector2D secondIntPt;
	int firstIntPlineIdx    = -1;
	int secondIntPlineIdx   = -1;
	int firstIntContourIdx  = -1;
	int secondIntContourIdx = -1;
	int intCount = 0;
	
	//iterate along polyline
	for (int i = 0; i < plineSz - 1; ++i) {
		int iNext = i + 1;

		for (int j = 0; j < contourSz; ++j) {
			int jNext = (j + 1) % contourSz;

			QVector2D tmpIntPt;
			float tPline, tPgon;
			if (Util::segmentSegmentIntersectXY(QVector2D(pline[i]), QVector2D(pline[iNext]), QVector2D(contour[j]), QVector2D(contour[jNext]), &tPline, &tPgon, true, tmpIntPt)) {
				polylineIntersectsPolygon = true;

				//first intersection
				if(intCount == 0){
					firstIntPlineIdx = i;
					firstIntContourIdx = j;
					firstIntPt = tmpIntPt;
				} else if(intCount == 1) {
					secondIntPlineIdx = i;
					secondIntContourIdx = j;
					secondIntPt = tmpIntPt;
				} else {
					//std::cout << "Cannot split - Polyline intersects polygon at more than two points.\n";
					return false;
				}
				intCount++;
			}
		}
	}

	if(intCount != 2){
		//std::cout << "Cannot split - Polyline intersects polygon at " << intCount <<" points\n";
		return false;
	}

	//Once we have intersection points and indexes, we reconstruct the two polygons
	pgon1.clear();
	pgon2.clear();
	int pgonVtxIte;
	int plineVtxIte;

	//If first polygon segment intersected has an index greater
	//	than second segment, modify indexes for correct reconstruction
	if(firstIntContourIdx > secondIntContourIdx){
		secondIntContourIdx += contourSz;
	}

	//==== Reconstruct first polygon
	//-- append polygon contour
	pgon1.push_back(firstIntPt);
	pgonVtxIte = firstIntContourIdx;
	while( pgonVtxIte < secondIntContourIdx){
		pgon1.push_back(contour[(pgonVtxIte+1)%contourSz]);
		pgonVtxIte++;
	}
	pgon1.push_back(secondIntPt);
	//-- append polyline points
	plineVtxIte = secondIntPlineIdx;
	while(plineVtxIte > firstIntPlineIdx){
		pgon1.push_back(pline[(plineVtxIte)]);
		plineVtxIte--;
	}

	//==== Reconstruct second polygon
	//-- append polygon contour
	pgon2.push_back(secondIntPt);
	pgonVtxIte = secondIntContourIdx;
	while( pgonVtxIte < firstIntContourIdx + contourSz){
		pgon2.push_back(contour[(pgonVtxIte+1)%contourSz]);
		pgonVtxIte++;
	}
	pgon2.push_back(firstIntPt);
	//-- append polyline points
	plineVtxIte = firstIntPlineIdx;
	while(plineVtxIte < secondIntPlineIdx){
		pgon2.push_back(pline[(plineVtxIte + 1)]);
		plineVtxIte++;
	}


	//verify that two polygons are created after the split. If not, return false
	/////
	if(pgon1.size() < 3 || pgon2.size() < 3){
		//std::cout << "Invalid split - Resulting polygons have fewer than three vertices.\n";
		return false;
	}

	return polylineIntersectsPolygon;
}

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2 Point_2;
typedef CGAL::Polygon_2<Kernel> Polygon_2;
typedef CGAL::Polygon_with_holes_2<Kernel> Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2> Pwh_list_2;

/**
 * Split the polygon by a line.
 * uses CGAL library and supports concave polygons.
 */
bool Polygon3D::split(const std::vector<QVector3D> &pline, std::vector<Polygon3D>& pgons) {
	bool polylineIntersectsPolygon = false;

	int plineSz = pline.size();
	int contourSz = this->contour.size();

	if(plineSz < 2 || contourSz < 3){
		//std::cout << "ERROR: Cannot split if polygon has fewer than three vertices of if polyline has fewer than two points\n.";
		return false;
	}

	Polygon_2 P;
	for (int i = 0; i < this->contour.size(); ++i) {
		if (i == this->contour.size() - 1 && fabs(this->contour.back().x() - this->contour[0].x()) == 0.0f && fabs(this->contour.back().y() - this->contour[0].y()) == 0.0f) break;
		P.push_back(Point_2(this->contour[i].x(), this->contour[i].y()));
	}

	// check if the polygon is self-intersecting.
	if (!P.is_simple()) {
		return false;
	}

	//if (Polygon3D::reorientFace(this->contour, true)) {
	if (!P.is_counterclockwise_oriented()) {
		std::reverse(P.vertices_begin(), P.vertices_end());
	}

	Polygon_2 Q1;
	QVector3D dir1 = pline[0] - pline[1];
	QVector3D pdir1(dir1.y(), -dir1.x(), 0);
	QVector3D pt1 = pline[0] + pdir1.normalized() * 10000.0f;

	QVector3D dir2 = pline.back() - pline[pline.size() - 2];
	QVector3D pdir2(-dir2.y(), dir2.x(), 0);
	QVector3D pt2 = pline.back() + pdir2.normalized() * 10000.0f;

	Q1.push_back(Point_2(pt1.x(), pt1.y()));
	for (int i = 0; i < pline.size(); ++i) {
		Q1.push_back(Point_2(pline[i].x(), pline[i].y()));
	}
	Q1.push_back(Point_2(pt2.x(), pt2.y()));
	if (!Q1.is_counterclockwise_oriented()) {
		std::reverse(Q1.vertices_begin(), Q1.vertices_end());
	}

	Polygon_2 Q2;
	pt1 = pline[0] - pdir1.normalized() * 10000.0f;
	pt2 = pline.back() - pdir2.normalized() * 10000.0f;
	Q2.push_back(Point_2(pt2.x(), pt2.y()));
	for (int i = pline.size() - 1; i >= 0; --i) {
		Q2.push_back(Point_2(pline[i].x(), pline[i].y()));
	}
	Q2.push_back(Point_2(pt1.x(), pt1.y()));
	if (!Q2.is_counterclockwise_oriented()) {
		std::reverse(Q2.vertices_begin(), Q2.vertices_end());
	}

	Pwh_list_2 intR1;
	CGAL::intersection (P, Q1, std::back_inserter(intR1));

	Pwh_list_2 intR2;
	CGAL::intersection (P, Q2, std::back_inserter(intR2));

	// set the resulting polygons
	pgons.clear();
	for (auto it = intR1.begin(); it != intR1.end(); ++it) {
		Polygon3D loop;
		for (auto edge = it->outer_boundary().edges_begin(); edge != it->outer_boundary().edges_end(); ++edge) {
			auto source = edge->source();

			loop.push_back(QVector3D(CGAL::to_double(source.x()), CGAL::to_double(source.y()), 0));
		}
		loop.push_back(loop.contour[0]);

		pgons.push_back(loop);
	}
	for (auto it = intR2.begin(); it != intR2.end(); ++it) {
		Polygon3D loop;
		for (auto edge = it->outer_boundary().edges_begin(); edge != it->outer_boundary().edges_end(); ++edge) {
			auto source = edge->source();

			loop.push_back(QVector3D(CGAL::to_double(source.x()), CGAL::to_double(source.y()), 0));
		}
		loop.push_back(loop.contour[0]);

		pgons.push_back(loop);
	}

	return true;
}

/**
* @brief: Given a polygon, this function computes the polygon's inwards offset. The offset distance
* is not assumed to be constant and must be specified in the vector offsetDistances. The size of this
* vector must be equal to the number of vertices of the polygon.
* Note that the i-th polygon segment is defined by vertices i and i+1.
* The polygon vertices are assumed to be oriented clockwise
* @param[in] offsetDistances: Perpendicular distance from offset segment i to polygon segment i.
* @param[out] pgonInset: The vertices of the polygon inset
* @return insetArea: Returns the area of the polygon inset		
**/
void Polygon3D::computeInset2(float offsetDistance, Loop3D &pgonInset) const {
	typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
	typedef CGAL::Partition_traits_2<K> Traits;
	typedef Traits::Polygon_2 Polygon_2;
	typedef boost::shared_ptr<Polygon_2> PolygonPtr;

	pgonInset.clear();

	Polygon_2 poly;

	for (int i = 0; i < contour.size(); ++i) {
		poly.push_back(K::Point_2(contour[i].x(), contour[i].y()));
	}
	poly.push_back(K::Point_2(contour[0].x(), contour[0].y()));

	std::vector<PolygonPtr> offset_poly;
	if (offsetDistance < 0) {
		K::FT lOffset = -offsetDistance;
		offset_poly = CGAL::create_exterior_skeleton_and_offset_polygons_2(lOffset, poly);

		if (offset_poly.size() >= 2) {
			for (auto it = offset_poly[1]->vertices_begin(); it != offset_poly[1]->vertices_end(); ++it) {
				pgonInset.push_back(QVector3D(it->x(), it->y(), 0));
			}
			std::reverse(pgonInset.begin(), pgonInset.end());
		}
	}
	else {
		K::FT lOffset = offsetDistance;
		offset_poly = CGAL::create_interior_skeleton_and_offset_polygons_2(lOffset, poly);

		if (offset_poly.size() > 0) {
			for (auto it = offset_poly[0]->vertices_begin(); it != offset_poly[0]->vertices_end(); ++it) {
				pgonInset.push_back(QVector3D(it->x(), it->y(), 0));
			}
		}
	}
}


void Polygon3D::computeInset(float offsetDistance, Loop3D &pgonInset) const {
	pgonInset.clear();
	if (contour.size() < 3) return;				

	std::vector<float> offsetDistances(contour.size(), offsetDistance);
	computeInset(offsetDistances, pgonInset);
}

float Polygon3D::distanceXYToPoint(const QVector3D &pt) const {
	return contour.distanceXYToPoint(pt);
}

/**
* Get polygon oriented bounding box
**/
void Polygon3D::getLoopOBB(const Loop3D &pin, QVector3D &size, QMatrix4x4 &xformMat){
	float alpha = 0.0f;			
	float deltaAlpha = 0.05*3.14159265359f;
	float bestAlpha;

	Loop3D rotLoop;
	QMatrix4x4 rotMat;
	QVector3D minPt, maxPt;
	QVector3D origMidPt;
	QVector3D boxSz;
	QVector3D bestBoxSz;
	float curArea;
	float minArea = FLT_MAX;

	rotLoop = pin;
	Polygon3D::getLoopAABB(rotLoop, minPt, maxPt);
	origMidPt = 0.5f*(minPt + maxPt);

	//while(alpha < 0.5f*_PI){
	int cSz = pin.size();
	QVector3D difVec;
	for(int i=0; i<pin.size(); ++i){
		difVec = (pin.at((i+1)%cSz) - pin.at(i)).normalized();
		alpha = atan2(difVec.x(), difVec.y());
		rotMat.setToIdentity();				
		rotMat.rotate(57.2957795f*(alpha), 0.0f, 0.0f, 1.0f);//57.2957795 rad2degree				

		transformLoop(pin, rotLoop, rotMat);
		boxSz = Polygon3D::getLoopAABB(rotLoop, minPt, maxPt);
		curArea = boxSz.x() * boxSz.y();
		if(curArea < minArea){
			minArea = curArea;
			bestAlpha = alpha;
			bestBoxSz = boxSz;
		}
		//alpha += deltaAlpha;
	}

	xformMat.setToIdentity();											
	xformMat.rotate(57.2957795f*(bestAlpha), 0.0f, 0.0f, 1.0f);//57.2957795 rad2degree
	xformMat.setRow(3, QVector4D(origMidPt.x(), origMidPt.y(), origMidPt.z(), 1.0f));			
	size = bestBoxSz;
}//

/**
 * Get polygon oriented bounding box
 * xformMat is a matrix that transform the original polygon to the axis aligned bounding box centered at the origin.
 */
void Polygon3D::getLoopOBB2(const Loop3D &pin, QVector3D &size, QMatrix4x4 &xformMat){
	float alpha = 0.0f;			
	float deltaAlpha = 0.05*3.14159265359f;
	float bestAlpha;

	Loop3D rotLoop;
	QMatrix4x4 rotMat;
	QVector3D minPt, maxPt;
	QVector3D origMidPt;
	QVector3D boxSz;
	QVector3D bestBoxSz;
	float curArea;
	float minArea = FLT_MAX;

	rotLoop = pin;
	int cSz = pin.size();
	QVector3D difVec;
	for(int i=0; i<pin.size(); ++i){
		difVec = (pin.at((i+1)%cSz) - pin.at(i)).normalized();
		alpha = atan2(difVec.y(), difVec.x());
		rotMat.setToIdentity();				
		rotMat.rotate(57.2957795f*(-alpha), 0.0f, 0.0f, 1.0f);//57.2957795 rad2degree				

		transformLoop(pin, rotLoop, rotMat);
		boxSz = Polygon3D::getLoopAABB(rotLoop, minPt, maxPt);
		curArea = boxSz.x() * boxSz.y();
		if(curArea < minArea){
			minArea = curArea;
			bestAlpha = alpha;
			bestBoxSz = boxSz;
		}
	}

	xformMat.setToIdentity();											
	xformMat.rotate(57.2957795f*(-bestAlpha), 0.0f, 0.0f, 1.0f);//57.2957795 rad2degree
			
	size = bestBoxSz;

	transformLoop(pin, rotLoop, xformMat);
	Polygon3D::getLoopAABB(rotLoop, minPt, maxPt);
	QVector3D midPt = (minPt + maxPt) * 0.5f;
	xformMat.setColumn(3, QVector4D(-midPt.x(), -midPt.y(), -midPt.z(), 1.0f));	
}

/**
 * Get polygon oriented bounding box
 */
Loop3D Polygon3D::getLoopOBB3(const Loop3D &pin) {
	float alpha = 0.0f;			
	float deltaAlpha = 0.05*3.14159265359f;
	float bestAlpha;

	Loop3D rotLoop;
	QMatrix4x4 rotMat;
	QVector3D minPt, maxPt;
	QVector3D origMidPt;
	QVector3D boxSz;
	QVector3D bestBoxSz;
	float curArea;
	float minArea = FLT_MAX;

	rotLoop = pin;
	Polygon3D::getLoopAABB(rotLoop, minPt, maxPt);
	origMidPt = 0.5f*(minPt + maxPt);

	//while(alpha < 0.5f*_PI){
	int cSz = pin.size();
	QVector3D difVec;
	QVector3D leftBottom;
	for(int i=0; i<pin.size(); ++i){
		difVec = (pin.at((i+1)%cSz) - pin.at(i)).normalized();
		alpha = atan2(difVec.x(), difVec.y());
		rotMat.setToIdentity();				
		rotMat.rotate(57.2957795f*(alpha), 0.0f, 0.0f, 1.0f);//57.2957795 rad2degree				

		transformLoop(pin, rotLoop, rotMat);
		boxSz = Polygon3D::getLoopAABB(rotLoop, minPt, maxPt);
		curArea = boxSz.x() * boxSz.y();
		if(curArea < minArea){
			minArea = curArea;
			bestAlpha = alpha;
			bestBoxSz = boxSz;
			leftBottom = pin[i];
		}
		//alpha += deltaAlpha;
	}

	QMatrix4x4 xformMat;
	xformMat.setToIdentity();										
	xformMat.rotate(57.2957795f*(bestAlpha), 0.0f, 0.0f, 1.0f);//57.2957795 rad2degree

	transformLoop(pin, rotLoop, xformMat);
	QVector3D minCorner, maxCorner;
	Polygon3D::getLoopAABB(rotLoop, minCorner, maxCorner);

	Loop3D box;
	box.resize(4);
	box[0] = minCorner;
	box[1] = QVector3D(maxCorner.x(), minCorner.y(), minCorner.z());
	box[2] = maxCorner;
	box[3] = QVector3D(minCorner.x(), maxCorner.y(), minCorner.z());

	Loop3D origBox;
	xformMat.setToIdentity();
	xformMat.rotate(57.2957795f*(-bestAlpha), 0.0f, 0.0f, 1.0f);//57.2957795 rad2degree
	transformLoop(box, origBox, xformMat);

	return origBox;
}//

void Polygon3D::getMyOBB(QVector3D &size, QMatrix4x4 &xformMat){
	Polygon3D::getLoopOBB(this->contour, size, xformMat);
}//

/**
* @brief: Get polygon axis aligned bounding box
* @return: The dimensions of the AABB 
**/
QVector3D Polygon3D::getLoopAABB(Loop3D &pin, QVector3D &minCorner, QVector3D &maxCorner)
{
	maxCorner = QVector3D(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	minCorner = QVector3D( FLT_MAX,  FLT_MAX,  FLT_MAX);

	QVector3D curPt;

	for(int i=0; i<pin.size(); ++i){
		curPt = pin.at(i);
		if( curPt.x() > maxCorner.x() ){
			maxCorner.setX(curPt.x());
		}
		if( curPt.y() > maxCorner.y() ){
			maxCorner.setY(curPt.y());
		}
		if( curPt.z() > maxCorner.z() ){
			maxCorner.setZ(curPt.z());
		}
		//------------
		if( curPt.x() < minCorner.x() ){
			minCorner.setX(curPt.x());
		}
		if( curPt.y() < minCorner.y() ){
			minCorner.setY(curPt.y());
		}
		if( curPt.z() < minCorner.z() ){
			minCorner.setZ(curPt.z());
		}
	}
	return QVector3D(maxCorner - minCorner);
}//

void Polygon3D::getBBox3D(QVector3D &ptMin, QVector3D &ptMax) const {
	ptMin.setX(FLT_MAX);
	ptMin.setY(FLT_MAX);
	ptMin.setZ(FLT_MAX);
	ptMax.setX(-FLT_MAX);
	ptMax.setY(-FLT_MAX);
	ptMax.setZ(-FLT_MAX);

	for (size_t i = 0; i < contour.size(); ++i) {
		if (contour[i].x() < ptMin.x()) { ptMin.setX(contour[i].x()); }
		if (contour[i].y() < ptMin.y()) { ptMin.setY(contour[i].y()); }
		if (contour[i].z() < ptMin.z()) { ptMin.setZ(contour[i].z()); }

		if (contour[i].x() > ptMax.x()) { ptMax.setX(contour[i].x()); }
		if (contour[i].y() > ptMax.y()) { ptMax.setY(contour[i].y()); }
		if (contour[i].z() > ptMax.z()) { ptMax.setZ(contour[i].z()); }
	}
}

bool Polygon3D::isSelfIntersecting() const {
	return contour.isSelfIntersecting();
}

BBox Polygon3D::envelope() const {
	boost::geometry::ring_type<Polygon3D>::type bg_pgon;
	boost::geometry::assign(bg_pgon, this->contour);
	boost::geometry::correct(bg_pgon);

	BBox bbox;
	boost::geometry::envelope(bg_pgon, bbox);
	return bbox;
}

float Polygon3D::area() const {
	return contour.area();
}

/**
 * GEN
 * このポリゴンが細すぎるかどうかチェックする。
 * OBBを計算し、縦横比がratioより大きく、且つ、最小エッジがmin_side未満なら、細すぎると判定する。
 */
bool Polygon3D::isTooNarrow(float ratio, float min_side) const {
	QVector3D size;
	QMatrix4x4 xformMat;
	getLoopOBB(contour, size, xformMat);
	
	if ((std::max)(size.x() / size.y(), size.y() / size.x()) > ratio && (std::min)(size.x(), size.y()) < min_side)  return true;
	else return false;
}

