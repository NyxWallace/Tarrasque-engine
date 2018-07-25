#include "LeapListener.h"

LeapListener::LeapListener(float* xPosition){
	m_xPostion = xPosition;
}

void LeapListener::onConnect(const Controller& controller) {
	std::cout << "////////////////////////////////////////////////////////////////" << std::endl;
	std::cout << "//                                                            //" << std::endl;
	std::cout << "//                   Leap motion actions:                     //" << std::endl;
	std::cout << "//                                                            //" << std::endl;
	std::cout << "//                        Connected                           //" << std::endl;
	std::cout << "//                                                            //" << std::endl;
	std::cout << "////////////////////////////////////////////////////////////////" << std::endl;

	//controller.enableGesture(Gesture::TYPE_SWIPE);
}

void LeapListener::onFrame(const Controller& controller) {
	if (m_first_time){
		std::cout << "////////////////////////////////////////////////////////////////" << std::endl;
		std::cout << "//                                                            //" << std::endl;
		std::cout << "//                   Leap motion actions:                     //" << std::endl;
		std::cout << "//                                                            //" << std::endl;
		std::cout << "//                     Frame available                        //" << std::endl;
		std::cout << "//                                                            //" << std::endl;
		std::cout << "////////////////////////////////////////////////////////////////" << std::endl;
		m_first_time = false;
	}
	if (m_engine_has_started){
		const Frame frame = controller.frame();
		Leap::HandList hands = frame.hands();
		Leap::Hand firstHand = hands[0];
		/*std::cout << "Frame id: " << frame.id()
				  << ", timestamp: " << frame.timestamp()
				  << ", hands: " << frame.hands().count()
				  << ", fingers: " << frame.fingers().count()
				  << ", tools: " << frame.tools().count()
				  << ", gestures: " << frame.gestures().count() << std::endl;*/
		Leap::Vector position = firstHand.palmPosition();
		if (position.x < -75.0f)
			*m_xPostion = -75.0f;
		else if (position.x > 75.0f)
			*m_xPostion = 75.0f;
		else
			*m_xPostion = position.x;
	}
}

void LeapListener::setEngineStarted(bool hasStarted){
	m_engine_has_started = hasStarted;
}