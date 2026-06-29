#ifndef FRAMEBUFFER_CLASS_H
#define FRAMEBUFFER_CLASS_H
#include <vector>

#include"string.h"
#include "glm/glm.hpp"

class Framebuffer
{
public:
	
	struct FBOparameters
	{
		int internalFormat;
		int format;
		int type;
		int minFilter;
		int MinSamplingFilter;
		int MagFilter;
		int MagSamplingFilter;
		int wrap1;
		int clamp1;
		int wrap2;
		int clamp2;
		int colourAttachment;
	};
	
	struct RBOparameters
	{
		bool doRBO;
		int internalFormat;
		int attachment;
	};
	
	std::vector<unsigned int> textures;
	unsigned int FBO, RBO, texture;
	int width, height;
	
	void setup(FBOparameters iFBOparameters, RBOparameters iRBOparameters);
	
	void setupMulti(std::vector<FBOparameters> iFBOparametersMulti, RBOparameters iRBOparameters);
	
	void resize(int width, int height);
	
	void resizeMulti(int width, int height);
	
	void Delete();
	
private:
	FBOparameters nFBOparameters;
	RBOparameters nRBOparameters;
	std::vector<FBOparameters> FBOparametersMulti;
};

#endif
