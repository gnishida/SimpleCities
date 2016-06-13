#pragma once

#include "glew.h"

#include "VBORenderManager.h"
#include "RoadGraph.h"
#include "BlockSet.h"
#include <glm/glm.hpp>
#include "VBOBuilding.h"

class MainWindow;

class UrbanGeometry {
public:
	int width;
	int depth;
	MainWindow* mainWin;
	RoadGraph roads;
	BlockSet blocks;
	std::vector<Building> buildings;
	glm::vec3 minBound;
	glm::vec3 maxBound;

public:
	UrbanGeometry(MainWindow* mainWin);

	/** getter for width */
	int getWidth() { return width; }

	/** getter for depth */
	int getDepth() { return depth; }

	void clear();
	void clearGeometry();

	void generateBlocks();
	void generateParcels();
	void generateBuildings();
	void generateVegetation();
	void generateAll();

	void render(VBORenderManager &vboRenderManager);
	void update(VBORenderManager& vboRenderManager);

	void loadParcels(const std::string& filename);
	void loadBuildings(const std::string& filename);

	void loadRoads(const QString &filename);
	void saveRoads(const QString &filename);
	void clearRoads();

};
