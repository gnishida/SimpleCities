/************************************************************************************************
*		Polygon 3D
*		@author igarciad
************************************************************************************************/
#pragma once

#include "glew.h"
#include "common.h"
#include "BBox.h"
#include <vector>
#include <QVector3D>

#ifndef Q_MOC_RUN
#include <boost/graph/adjacency_list.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/ring.hpp>
#include <boost/geometry/multi/multi.hpp>
#endif

#ifndef BOOST_TYPEOF_SILENT
#define BOOST_TYPEOF_SILENT
#endif//BOOST_TYPEOF_SILENT



/**
* Geometry.
* Classes and functions for geometric data
**/

class Loop3D : public std::vector<QVector3D> {
public:
	bool isPointWithinLoop(const QVector3D& pt) const;
	bool isClockwise() const;
	float area() const;
	bool isSelfIntersecting() const;
};

/**
* Stores a polygon in 3D represented by its
*     exterior contour.
**/
class Polygon3D {
public:
	Loop3D contour;

public:
	Polygon3D() {}

	void clear() { contour.clear(); }

	bool isClockwise() const ;
	void correct();

	QVector3D& operator[](const int idx) {	
		return contour.at(idx);
	}

	const QVector3D& operator[](const int idx) const {
		return contour.at(idx);
	}

	inline void push_back(const QVector3D &pt) {
		contour.push_back(pt);
	}

	inline void getBBox3D(QVector3D &ptMin, QVector3D &ptMax)
	{
		ptMin.setX(FLT_MAX);
		ptMin.setY(FLT_MAX);
		ptMin.setZ(FLT_MAX);
		ptMax.setX(-FLT_MAX);
		ptMax.setY(-FLT_MAX);
		ptMax.setZ(-FLT_MAX);

		for(size_t i=0; i<contour.size(); ++i){
			if(contour[i].x() < ptMin.x()){ ptMin.setX(contour[i].x()); }
			if(contour[i].y() < ptMin.y()){ ptMin.setY(contour[i].y()); }
			if(contour[i].z() < ptMin.z()){ ptMin.setZ(contour[i].z()); }

			if(contour[i].x() > ptMax.x()){ ptMax.setX(contour[i].x()); }
			if(contour[i].y() > ptMax.y()){ ptMax.setY(contour[i].y()); }
			if(contour[i].z() > ptMax.z()){ ptMax.setZ(contour[i].z()); }
		}
	}

	//Is self intersecting
	bool isSelfIntersecting() const;

	BBox envelope() const;
	float area() const;

	//Only works for polygons with no holes in them
	bool splitMeWithPolyline(std::vector<QVector3D> &pline, Loop3D &pgon1, Loop3D &pgon2);
	bool split(std::vector<QVector3D> &pline, std::vector<Polygon3D>& pgons);

	//Only works for polygons with no holes in them
	float computeInset(float offsetDistance, Loop3D &pgonInset, bool computeArea = true) const;
	float computeInset2(float offsetDistance, Loop3D& pgonInset, bool computeArea = true) const;
	float computeInset(std::vector<float> &offsetDistances, Loop3D &pgonInset, bool computeArea = true) const;

	float computeArea(bool parallelToXY = false);

	bool isPointWithinLoop(const QVector3D& pt) const {
		return contour.isPointWithinLoop(pt);
	}

	static bool reorientFace(Loop3D &pface, bool onlyCheck = false);

	static int cleanLoop(Loop3D &pin, Loop3D &pout, float threshold);

	static void transformLoop(const Loop3D& pin, Loop3D& pout, const QMatrix4x4& transformMat);

	static QVector3D getLoopAABB(Loop3D &pin, QVector3D &minCorner, QVector3D &maxCorner);

	static void getLoopOBB(const Loop3D &pin, QVector3D &size, QMatrix4x4 &xformMat);
	static void getLoopOBB2(const Loop3D &pin, QVector3D &size, QMatrix4x4 &xformMat);
	static Loop3D getLoopOBB3(const Loop3D &pin);

	void getMyOBB(QVector3D &size, QMatrix4x4 &xformMat);

	static bool segmentSegmentIntersectXY(QVector2D &a, QVector2D &b, QVector2D &c, QVector2D &d,
		float *tab, float *tcd, bool segmentOnly, QVector2D &intPoint);

	//Shortest distance from a point to a polygon
	static float distanceXYToPoint(Loop3D &pin, QVector3D &pt);

	static bool getIrregularBisector(QVector3D &p0, QVector3D &p1, QVector3D &p2, float d01, float d12,	QVector3D &intPt);

	bool isTooNarrow(float ratio, float min_side);
};	

class BBox3D {
public:
	QVector3D minPt;
	QVector3D maxPt;

public:
	BBox3D(){
		this->resetMe();	
	}

	inline void resetMe(void) {
		minPt.setX(FLT_MAX);
		minPt.setY(FLT_MAX);
		minPt.setZ(FLT_MAX);
		maxPt.setX(-FLT_MAX);
		maxPt.setY(-FLT_MAX);
		maxPt.setZ(-FLT_MAX);
	}

	inline bool overlapsWithBBox3D(BBox3D &other){
		return  
			( (this->minPt.x() <= other.maxPt.x()) && (this->maxPt.x() >= other.minPt.x()) ) &&
			( (this->minPt.y() <= other.maxPt.y()) && (this->maxPt.y() >= other.minPt.y()) ) &&
			( (this->minPt.z() <= other.maxPt.z()) && (this->maxPt.z() >= other.minPt.z()) );
	}

	void combineWithBBox3D(BBox3D &other){
		minPt.setX(qMin(minPt.x(), other.minPt.x()));
		minPt.setY(qMin(minPt.y(), other.minPt.y()));
		minPt.setZ(qMin(minPt.z(), other.minPt.z()));

		maxPt.setX(qMax(maxPt.x(), other.maxPt.x()));
		maxPt.setY(qMax(maxPt.y(), other.maxPt.y()));
		maxPt.setZ(qMax(maxPt.z(), other.maxPt.z()));

		return;
	}

	void addPoint(QVector3D &newPt){
		minPt.setX(qMin(minPt.x(), newPt.x()));
		minPt.setY(qMin(minPt.y(), newPt.y()));
		minPt.setZ(qMin(minPt.z(), newPt.z()));

		maxPt.setX(qMax(maxPt.x(), newPt.x()));
		maxPt.setY(qMax(maxPt.y(), newPt.y()));
		maxPt.setZ(qMax(maxPt.z(), newPt.z()));
	}

	inline bool overlapsWithBBox3DXY(BBox3D &other){
		return  
			( (this->minPt.x() <= other.maxPt.x()) && (this->maxPt.x() >= other.minPt.x()) ) &&
			( (this->minPt.y() <= other.maxPt.y()) && (this->maxPt.y() >= other.minPt.y()) );					
	}

	inline QVector3D midPt(void) {
		return (0.5*(minPt + maxPt));
	}
};


// We can conveniently use macro's to register point and ring
//BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(QVector3D, double, boost::geometry::cs::cartesian, x, y, setX, setY)
BOOST_GEOMETRY_REGISTER_RING(Loop3D)

	namespace boost {
		namespace geometry {

			namespace traits {

				template<> struct tag<Polygon3D> { typedef polygon_tag type; };

			} // namespace traits

			template<> struct ring_type<Polygon3D> { typedef Loop3D type; };

		} // namespace geometry
} // namespace boost

