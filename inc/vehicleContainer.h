#ifndef _VEHICLE_CONTAINER_H_
#define _VEHICLE_CONTAINER_H_

#include <btBulletDynamicsCommon.h>
#include "vehicle.h"

class VehicleContainer
{
public:
	VehicleContainer();
	void cleanVehicleList();
private:
	Vehicle * vehicleList;
};

#endif //_VEHICLE_CONTAINER_H_
