#pragma once

#include "glew.h"

#include "VBORenderManager.h"
#include "RoadGraph.h"
#include "BlockSet.h"
#include <glm/glm.hpp>
#include "Building.h"

class MainWindow;

class UrbanGeometry {
public:
	int width;
	int depth;
	MainWindow* mainWin;
	RoadGraph roads;
	BlockSet blocks;
	glm::vec2 minBound;
	glm::vec2 maxBound;
	Polygon2D zone;

public:
	UrbanGeometry(MainWindow* mainWin);

	/** getter for width */
	int getWidth() { return width; }

	/** getter for depth */
	int getDepth() { return depth; }

	void generateRoads();
	void generateBlocks();
	void generateParcels();
	void generateBuildings();
	void generateVegetation();
	void generateAll();

	void render(VBORenderManager &vboRenderManager);
	void update(VBORenderManager& vboRenderManager);

	void loadZone(const std::string& filename);
	void loadTerrain(const std::string& filename);
	void loadRoads(const std::string& filename);
	void saveRoads(const std::string &filename);
	void loadParcels(const std::string& filename);
	void saveParcels(const std::string& filename);
	void loadBuildings(const std::string& filename);
	void saveBuildings(const std::string& filename);

	void clear();

};
