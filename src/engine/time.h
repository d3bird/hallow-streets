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

private:

	void check();

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


};