#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
	G::global()["showTerrain"] = true;
	G::global()["showRoads"] = true;
	G::global()["showBlocks"] = true;
	G::global()["showParcels"] = true;
	G::global()["showBuildings"] = true;
	G::global()["showParcels"] = true;
	G::global()["showTrees"] = true;

	G::global()["parksRatio"] = 0.05f;

	G::global()["roadAngleTolerance"] = 0.52f;
	G::global()["slopeTolerance"] = 0.3f;
	G::global()["sea_level"] = 55.55f;// 60.5f;
	G::global()["road_min_level"] = 61.0f;
	G::global()["acrossRiverTolerance"] = 200.0f;

	G::global()["MAX_Z"] = 1000.0f;
	G::global()["shader2D"] = true;

	G::global()["2DroadsStroke"] = 1.5f;

	G::global()["sidewalk_width"] = 2.0f;
	G::global()["parcel_area_mean"] = 1000.0f;
	G::global()["parcel_area_min"] = 200.0f;
	G::global()["parcel_area_deviation"] = 1.0f;
	G::global()["parcel_split_deviation"] = 0.2f;
	G::global()["parcel_setback_front"] = 5.0f;
	G::global()["parcel_setback_rear"] = 5.0f;
	G::global()["parcel_setback_sides"] = 5.0f;
	G::global()["building_max_depth"] = 0.0f;
	G::global()["building_stories_deviation"] = 0.0f;// 2.0f;
	G::global()["building_stories_mean"] = 5;
	G::global()["tree_setback"] = 1.3f;

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
