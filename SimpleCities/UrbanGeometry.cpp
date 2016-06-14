/*********************************************************************
This file is part of QtUrban.

    QtUrban is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    QtUrban is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QtUrban.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#include "UrbanGeometry.h"
#include <limits>
#include <iostream>
#include <QFile>
#include "common.h"
#include "global.h"
#include "GraphUtil.h"
#include "MainWindow.h"
#include "Util.h"
#include "RoadMeshGenerator.h"
#include "BlockMeshGenerator.h"
#include "VBOPm.h"
#include "VBOPmBlocks.h"
#include "VBOPmParcels.h"
#include "VBOPmBuildings.h"
#include "VBOVegetation.h"
#include "GShapefile.h"

UrbanGeometry::UrbanGeometry(MainWindow* mainWin) {
	this->mainWin = mainWin;
}

void UrbanGeometry::clear() {
	clearGeometry();
}

void UrbanGeometry::clearGeometry() {
	//if (&mainWin->glWidget->vboRenderManager != NULL) delete &mainWin->glWidget->vboRenderManager;

	roads.clear();
	update(mainWin->glWidget->vboRenderManager);
}

#if 0
void UrbanGeometry::generateRoadsPM(std::vector<ExFeature> &features) {
	if (areas.selectedIndex == -1) return;
	if (areas.selectedArea()->hintLine.size() == 0) return;

	if (G::getBool("useLayer")) {
		PMRoadGenerator generator(mainWin, areas.selectedArea()->roads, areas.selectedArea()->area, areas.selectedArea()->hintLine, &mainWin->glWidget->vboRenderManager, features);
		generator.generateRoadNetwork();
	} else {
		PMRoadGenerator generator(mainWin, roads, areas.selectedArea()->area, areas.selectedArea()->hintLine, &mainWin->glWidget->vboRenderManager, features);
		generator.generateRoadNetwork();
	}
	update(mainWin->glWidget->vboRenderManager);
}
#endif

void UrbanGeometry::generateBlocks() {
	VBOPmBlocks::generateBlocks(&mainWin->glWidget->vboRenderManager, roads, blocks);
	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::generateParcels() {
	VBOPmParcels::generateParcels(mainWin->glWidget->vboRenderManager, blocks.blocks);
	VBOPmBuildings::generateBuildings(mainWin->glWidget->vboRenderManager, blocks.blocks);
	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::generateBuildings() {
	VBOPmBuildings::generateBuildings(mainWin->glWidget->vboRenderManager, blocks.blocks);
	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::generateVegetation() {
	VBOVegetation::generateVegetation(mainWin->glWidget->vboRenderManager, blocks.blocks);
	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::generateAll() {
	VBOPmBlocks::generateBlocks(&mainWin->glWidget->vboRenderManager, roads, blocks);
	VBOPmParcels::generateParcels(mainWin->glWidget->vboRenderManager, blocks.blocks);
	VBOPmBuildings::generateBuildings(mainWin->glWidget->vboRenderManager, blocks.blocks);
	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::render(VBORenderManager& vboRenderManager) {
	glLineWidth(5.0f);
	glPointSize(10.0f);

#if 0
	// draw the areas
	for (int i = 0; i < areas.size(); ++i) {
		areas[i]->adaptToTerrain(&vboRenderManager);
		QColor colorArea(0, 0, 255);
		QColor colorHintLine(255, 0, 0);
		if (i != areas.selectedIndex) {
			colorArea = QColor(196, 196, 255);
			colorHintLine = QColor(255, 196, 196);
		}

		// draw the area and the hint line
		RendererHelper::renderPolyline(vboRenderManager, areas[i]->area3D, colorArea);
		RendererHelper::renderPolyline(vboRenderManager, areas[i]->hintLine3D, colorHintLine);
	}

	vboRenderManager.removeStaticGeometry("lines");
#endif
}

/**
 * 道路、歩道、区画、ビル、木のジオミトリを作成しなおす。
 * この関数を頻繁に呼ぶべきではない。道路が更新/生成された時、2D/3D表示の変更、PMメニューから新規にジオミトリを生成した時だけ。
 */
void UrbanGeometry::update(VBORenderManager& vboRenderManager) {
	printf("updating geometry.\n");

	// 地面が変わっている可能性などがあるので、ビルなどのジオミトリも一旦削除してしまう。
	// 道路以外のジオミトリは、別途、PMメニューから作成すること
	vboRenderManager.removeStaticGeometry("3d_blocks");
	vboRenderManager.removeStaticGeometry("3d_parks");
	vboRenderManager.removeStaticGeometry("3d_parcels");
	vboRenderManager.removeStaticGeometry("3d_roads");
	vboRenderManager.removeStaticGeometry("3d_roads_inter");
	vboRenderManager.removeStaticGeometry("3d_roads_interCom");
	vboRenderManager.removeStaticGeometry("3d_building");
	vboRenderManager.removeStaticGeometry("3d_building_fac");
	vboRenderManager.removeStaticGeometry("tree");
	vboRenderManager.removeStaticGeometry("streetLamp");

	if (G::getBool("shader2D")) {
		RoadMeshGenerator::generate2DRoadMesh(vboRenderManager, roads);
		BlockMeshGenerator::generate2DParcelMesh(vboRenderManager, blocks);
	} else {
		RoadMeshGenerator::generateRoadMesh(vboRenderManager, roads);
		BlockMeshGenerator::generateBlockMesh(vboRenderManager, blocks);
		BlockMeshGenerator::generateParcelMesh(vboRenderManager, blocks);
		//VBOPm::generateBuildings(mainWin->glWidget->vboRenderManager, blocks);
		VBOPm::generateBuildings(mainWin->glWidget->vboRenderManager, buildings);
		VBOVegetation::generateVegetation(mainWin->glWidget->vboRenderManager, blocks.blocks);
	}
}

void UrbanGeometry::loadTerrain(const std::string& filename) {
	mainWin->glWidget->vboRenderManager.vboTerrain.loadTerrain(filename.c_str());

	/*
	gs::DEM dem;
	dem.load(filename);

	mainWin->glWidget->vboRenderManager->changeTerrainDimensions(glm::vec2(dem.width, dem.height));
	mainWin->glWidget->vboRenderManager.vboTerrain.layerData = cv::Mat(dem.height, dem.width, CV_32FC1, dem.data.data()).clone();
	cv::flip(mainWin->glWidget->vboRenderManager.vboTerrain.layerData, mainWin->glWidget->vboRenderManager.vboTerrain.layerData, 0);
	*/
}

void UrbanGeometry::loadParcels(const std::string& filename) {
	blocks.clear();

	gs::Shape shape;
	shape.load(filename);
	if (minBound.x == 0 && minBound.y == 0) {
		minBound = shape.minBound;
		maxBound = shape.maxBound;
	}

	glm::vec3 offset = (maxBound + minBound) * 0.5f;

	for (int i = 0; i < shape.shapeObjects.size(); ++i) {
		for (int j = 0; j < shape.shapeObjects[i].parts.size(); ++j) {
			Block block;
			for (int k = shape.shapeObjects[i].parts[j].points.size() - 1; k >= 0; --k) {
				QVector3D pt;
				pt.setX(shape.shapeObjects[i].parts[j].points[k].x - offset.x);
				pt.setY(shape.shapeObjects[i].parts[j].points[k].y - offset.y);
				pt.setZ(shape.shapeObjects[i].parts[j].points[k].z);
				block.blockContour.push_back(pt);
			}
			
			Parcel parcel;
			parcel.setContour(block.blockContour);
			Block::parcelGraphVertexDesc pvd = boost::add_vertex(block.myParcels);
			block.myParcels[pvd] = parcel;

			blocks.blocks.push_back(block);

			// we use only the first part
			break;
		}
	}

	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::loadBuildings(const std::string& filename) {
	buildings.clear();

	gs::Shape shape;
	shape.load(filename);
	if (minBound.x == 0 && minBound.y == 0) {
		minBound = shape.minBound;
		maxBound = shape.maxBound;
	}

	glm::vec3 offset = (maxBound + minBound) * 0.5f;

#if 0
	Building building;
	building.numStories = 8;
	building.color = QColor(rand() % 256, rand() % 256, 255);
	building.bldType = 0;
	building.buildingFootprint.push_back(QVector3D(-10, -10, 0));
	building.buildingFootprint.push_back(QVector3D(10, -10, 0));
	building.buildingFootprint.push_back(QVector3D(10, 10, 0));
	building.buildingFootprint.push_back(QVector3D(-10, 10, 0));
	buildings.push_back(building);
#endif

#if 1
	for (int i = 0; i < shape.shapeObjects.size(); ++i) {
		for (int j = 0; j < shape.shapeObjects[i].parts.size(); ++j) {
			Building building;

			if (shape.shapeObjects[i].attributes.find("NbreEtages") != shape.shapeObjects[i].attributes.end()) {
				building.numStories = shape.shapeObjects[i].attributes["NbreEtages"].intValue();
			}
			if (building.numStories <= 0) {
				building.numStories = 2;
			}

			building.color = QColor(rand() % 256, rand() % 256, 255);

			// For now, only one type of building is implemented.
			if (building.numStories == 1) {	// Low building with flat roof
				building.bldType = 1;
			}
			else if (building.numStories > 6) {	// High tower with flat roof
				building.bldType = 1;
			}
			else {	// residential house with gable roof
				building.bldType = 1;
			}

			for (int k = shape.shapeObjects[i].parts[j].points.size() - 1; k >= 0; --k) {
				QVector3D pt;
				pt.setX(shape.shapeObjects[i].parts[j].points[k].x - offset.x);
				pt.setY(shape.shapeObjects[i].parts[j].points[k].y - offset.y);
				pt.setZ(shape.shapeObjects[i].parts[j].points[k].z);

				building.buildingFootprint.push_back(pt);
			}

			buildings.push_back(building);
			
			// we use only the first part
			break;
		}
	}
#endif

	// remove the building that is within the other buildings
	for (int i = 0; i < buildings.size(); ) {
		bool inside = false;
		for (int k = 0; k < buildings[i].buildingFootprint.contour.size() && !inside; ++k) {
			for (int j = 0; j < buildings.size(); ++j) {
				if (i == j) continue;

				if (boost::geometry::within(buildings[i].buildingFootprint.contour[k], buildings[j].buildingFootprint.contour)) {
					inside = true;
					break;
				}
			}
		}

		if (inside) {
			buildings.erase(buildings.begin() + i);
		}
		else {
			i++;
		}
	}

	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::loadRoads(const QString &filename) {
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		std::cerr << "The file is not accessible: " << filename.toUtf8().constData() << endl;
		throw "The file is not accessible: " + filename;
	}

	roads.clear();
	GraphUtil::loadRoads(roads, filename);

	update(mainWin->glWidget->vboRenderManager);
}

void UrbanGeometry::saveRoads(const QString &filename) {
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		std::cerr << "The file is not accessible: " << filename.toUtf8().constData() << endl;
		throw "The file is not accessible: " + filename;
	}

	GraphUtil::saveRoads(roads, filename);
}

void UrbanGeometry::clearRoads() {
	roads.clear();
	update(mainWin->glWidget->vboRenderManager);
}


