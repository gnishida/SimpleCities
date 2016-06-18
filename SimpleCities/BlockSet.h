#pragma once

#include "VBORenderManager.h"
#include <vector>
#include <QString>
#include <QVector2D>
#include "Block.h"
#include "Parcel.h"

class BlockSet {
public:
	std::vector<Block> blocks;
	int selectedBlockIndex;
	int selectedParcelIndex;

public:
	BlockSet() : selectedBlockIndex(-1), selectedParcelIndex(-1) {}

	int selectBlock(const QVector2D& pos);
	std::pair<int, int> selectParcel(const QVector2D& pos);
	void removeSelectedBlock();

	Block& operator[](int index) { return blocks[index]; }
	Block& at(int index) { return blocks.at(index); }
	const Block& operator[](int index) const { return blocks[index]; }
	size_t size() const { return blocks.size(); }
	void clear();
};

