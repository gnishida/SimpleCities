#pragma once

#include "Polygon2D.h"
#include "RoadGraph.h"
#include "MainWindow.h"
//#include "Terrain.h"
#include "VBORenderManager.h"

class PMRoadGenerator {
private:
	MainWindow *mainWin;
	RoadGraph &roads;
	VBORenderManager *vboRenderManager;
	Polygon2D targetArea;

public:
	PMRoadGenerator(MainWindow *mainWin, RoadGraph &roads, VBORenderManager *vboRenderManager, const Polygon2D& targetArea) : mainWin(mainWin), roads(roads), vboRenderManager(vboRenderManager), targetArea(targetArea) {}
	~PMRoadGenerator() {}

	void generateRoadNetwork();

private:
	void generateAvenueSeeds(std::list<RoadVertexDesc>& seeds);
	void addAvenueSeed(const QVector2D &pt, std::list<RoadVertexDesc>& seeds);
	void generateStreetSeeds(std::list<RoadVertexDesc> &seeds);
	void generateStreetSeeds2(std::list<RoadVertexDesc> &seeds);

	void attemptExpansion(int roadType, RoadVertexDesc srcDesc, std::list<RoadVertexDesc> &seeds);
	bool growRoadSegment(int roadType, RoadVertexDesc srcDesc, float step, int num_steps, float angle, float curvature, int lanes, float angleTolerance, std::list<RoadVertexDesc> &seeds);
	float getFirstEdgeAngle(RoadGraph& roads, RoadVertexDesc srcDesc);
	bool isRedundantEdge(RoadGraph& roads, RoadVertexDesc v_desc, const Polyline2D &polyline, float angleTolerance);
	bool isRedundantEdge(RoadGraph& roads, RoadVertexDesc v_desc, float angle, float angleTolerance);
	bool getVertexForSnapping(VBORenderManager& vboRenderManager, RoadGraph& roads, RoadVertexDesc srcDesc, float distance_threshold, float z_threshold, float angle, float angle_threshold, RoadVertexDesc& nearest_desc);
	bool submerged(int roadType, RoadGraph &roads, VBORenderManager *vboRenderManager);
	bool submerged(int roadType, const Polyline2D &polyline, VBORenderManager *vboRenderManager);
	bool submerged(VBORenderManager* vboRenderManager, Polyline2D& polyline, float seaLevel);
	bool extendRoadAcrossRiver(RoadGraph& roads, VBORenderManager* vboRenderManager, Polygon2D& targetArea, int roadType, RoadVertexDesc v_desc, std::list<RoadVertexDesc> &seeds, float angle_threshold, float max_length, RoadVertexDesc& tgtDesc);
	void removeEdge(RoadGraph& roads, RoadVertexDesc srcDesc, RoadEdgeDesc start_e_desc);
	void removeEdge(RoadGraph& roads, RoadVertexDesc srcDesc);
	void saveRoadImage(RoadGraph& roads, std::list<RoadVertexDesc>& seeds, const char* filename);
};

