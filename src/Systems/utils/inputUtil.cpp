#include "inputUtil.h"

float inputUtil::s_sensitivityX = 100.0f;
float inputUtil::s_sensitivityY = 100.0f;	

inputUtil::inputUtil()
{

}

void inputUtil::updateMouse(bool invertMouse[], float sensitivityvar) {

	switch (invertMouse[1]) { // x
	case true: s_sensitivityX = -sensitivityvar; break;
	case false: s_sensitivityX = sensitivityvar; break;
	default: s_sensitivityX = sensitivityvar; break; }

	switch (invertMouse[0]) { // Y
	case true: s_sensitivityY = -sensitivityvar; break;
	case false: s_sensitivityY = sensitivityvar; break;
	default: s_sensitivityY = sensitivityvar; break; }
}

void inputUtil::updatePitch(bool invertMouseX, float sensitivityvar){

}
void inputUtil::updateYaw(bool invertMouseY, float sensitivityvar) {

}

void inputUtil::MouseInput() {
	
}