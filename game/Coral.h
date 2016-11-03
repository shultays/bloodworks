#pragma once

const float update_interval = 0.01f;
const float draw_interval = 1.0f / 60.0f;

class Coral {
	float lastDrawTime;
	float lastUpdateTime;

public:

	Coral();
	void init();
	void tick();
};
