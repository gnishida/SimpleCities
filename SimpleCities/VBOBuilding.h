/************************************************************************************************
*		VBO Building Class
*		@author igarciad
************************************************************************************************/
#pragma once

#include "Polygon3D.h"

//const int BLDG_NO_BLDG=0;// 0 NO BLDG
//const int BLDG_WITH_BLDG=1;	// 1 BLDG


class Building{

public:

	Building(void){
		bldType=-1;
		numStories=0;
	}

	/**
	* Destructor
	**/
	~Building(void){
	}

	/**
	* Data members
	**/
	Polygon3D buildingFootprint;
	int bldType;
	int subType;
	int numStories;
	QColor color;
};
