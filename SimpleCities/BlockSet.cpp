#include "BlockSet.h"
#include <QFile>
#include <QTextStream>
#include "Polygon2D.h"

int BlockSet::selectBlock(const QVector2D& pos) {
	for (int i = 0; i < blocks.size(); ++i) {
		Polygon2D polygon;
		for (int j = 0; j < blocks[i].blockContour.size(); ++j) {
			polygon.push_back(QVector2D(blocks[i].blockContour[j]));
		}
		polygon.correct();

		if (polygon.contains(pos)) {
			selectedBlockIndex = i;
			return i;
		}
	}

	selectedBlockIndex = -1;

	return -1;
}

std::pair<int, int> BlockSet::selectParcel(const QVector2D& pos) {
	for (int i = 0; i < blocks.size(); ++i) {
		for (int j = 0; j < blocks[i].parcels.size(); ++j) {
			Polygon2D polygon;
			for (int k = 0; k < blocks[i].parcels[j].parcelContour.size(); ++k) {
				polygon.push_back(QVector2D(blocks[i].parcels[j].parcelContour[k]));
			}
			polygon.correct();

			if (polygon.contains(pos)) {
				selectedBlockIndex = i;
				selectedParcelIndex = j;

				return std::make_pair(selectedBlockIndex, selectedParcelIndex);
			}
		}
	}

	selectedBlockIndex = -1;
	selectedParcelIndex = -1;

	return std::make_pair(-1, -1);
}

void BlockSet::removeSelectedBlock() {
	if (selectedBlockIndex < 0 || selectedBlockIndex >= blocks.size()) return;

	blocks.erase(blocks.begin() + selectedBlockIndex);

	selectedBlockIndex = -1;
	selectedParcelIndex = -1;
	//modified = true;
}

void BlockSet::clear() {
	blocks.clear();

	selectedBlockIndex = -1;
	selectedParcelIndex = -1;
	//modified = true;
}