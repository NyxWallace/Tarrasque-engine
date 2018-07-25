#pragma once
#include "Leap.h"

using namespace Leap;

class LeapListener : public Listener
{
public:
	LeapListener(float*);
	virtual void onConnect(const Controller&);
	virtual void onFrame(const Controller&);
	void setEngineStarted(bool);

private:
	bool m_first_time = true;
	bool m_engine_has_started = false;
	float* m_xPostion;
};

