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
		VBOPm::generateBuildings(mainWin->glWidget->vboRenderManager, blocks);
		VBOVegetation::generateVegetation(mainWin->glWidget->vboRenderManager, blocks.blocks);
	}
}

void UrbanGeometry::loadParcels(const std::string& filename) {
	blocks.clear();

	gs::Shape shape;
	shape.load(filename);
	minBound = shape.minBound;
	maxBound = shape.maxBound;

	glm::vec3 offset = (shape.maxBound + shape.minBound) * 0.5f;

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
		}
	}

	//mainWin->glWidget->vboRenderManager.changeTerrainDimensions(std::max(maxBound.x - minBound.x, maxBound.y - minBound.y), 100);

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


