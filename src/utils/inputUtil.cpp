#include "inputUtil.h"

float inputUtil::sensitivityX = 100.0f;
float inputUtil::sensitivityY = 100.0f;	

inputUtil::inputUtil()
{

}

void inputUtil::updateMouse(bool invertMouse[], float sensitivityvar) {

	switch (invertMouse[1]) { // x
	case true: sensitivityX = -sensitivityvar; break;
	case false: sensitivityX = sensitivityvar; break;
	default: sensitivityX = sensitivityvar; break; }

	switch (invertMouse[0]) { // Y
	case true: sensitivityY = -sensitivityvar; break;
	case false: sensitivityY = sensitivityvar; break;
	default: sensitivityY = sensitivityvar; break; }
}

void inputUtil::updatePitch(bool invertMouseX, float sensitivityvar){

}
void inputUtil::updateYaw(bool invertMouseY, float sensitivityvar) {

}

void inputUtil::MouseInput() {
	
}