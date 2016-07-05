#pragma once

#include "Block.h"

class PmParcels {
public:
	static bool generateParcels(VBORenderManager& rendManager, std::vector<Block>& blocks);

private:
	static void subdivideBlockIntoParcels(Block &block);
	static bool subdivideParcel(Parcel& parcel, float areaMean, float areaMin, float areaVar, float splitIrregularity, std::vector<Parcel>& outParcels);
};
