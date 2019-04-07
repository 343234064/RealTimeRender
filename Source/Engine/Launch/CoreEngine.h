/************************************
Core Engine 


*************************************/
#pragma once

#include "Global/GlobalType.h"



class CoreEngine
{
public:
	CoreEngine():
		TotalTickTime(0),
		CurrentTimeSeconds(0),
		LastTimeSeconds(0),
		LastFrameCycle(0),
		FPS(0),
		DeltaTime(0)
	{}
	virtual ~CoreEngine() {}

	int32 PreInit();
	int32 Init();
	void Tick();
	void Exit();

protected:
	void InitTime();


protected:
	//Total tick count
	double TotalTickTime;
	
	//Current time in seconds
	double CurrentTimeSeconds;
	
	//The previous value of CurrentTimeSeconds
	double LastTimeSeconds;

	//delta time = 1 / FPS 
	double DeltaTime;

	//The cycle count until last frame
	uint32 LastFrameCycle;

	//Frame per second
	uint32 FPS;
};


extern CoreEngine gCoreEngine;
