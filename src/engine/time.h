#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

class timing {
public:
	timing(bool frames = true);
	~timing();

	void update_time();
	float* get_time_change() { return deltaTime; }
	float* get_time_change_static() { return deltaTime_static; }

	void toggle_frame_rates() { draw_frame_rate = !draw_frame_rate; }
	void set_time_multipler(int i) { time_state = i; }

	void set_day();
	void set_night();
	bool is_day() { return daytime; }

	void set_timings();

	//getter functions

	float get_harvest_plant_time() { return harvest_plant; }
	float get_till_soil_time() { return till_soil; }
	float get_relax_time() { return relax; }
	float get_sacrifice_time() { return sacrifice; }
	float get_sleep_time() { return sleep; }
	float get_tend_plant_time() { return tend_plant; }
private:
	int time_state;//how fast time goes
	bool daytime;
	int passed_days;

	//timing between frames
	double lastTime;
	float currentFrame;
	float lastFrame;
	float* deltaTime;//for time that can change with state
	float* deltaTime_static;//just the time between rendering frames
	int nbFrames;
	bool draw_frame_rate;

	//how long each action takes

	float harvest_plant;
	float till_soil;
	float tend_plant;
	float relax;
	float sacrifice;
	float sleep;

};