#include "FBOHelper.h"
#include <stdlib.h>

FBOData* fData;
GLuint FBOTex;

void CheckFramebufferStatus(const char* msg)
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		fprintf(stderr, msg);
}

void InitFBO(void)
{
	fData = (FBOData*)malloc(sizeof(FBOData));

	//Increase resolution if needed POT!
	fData->width = 512;
	fData->height = 512;
	glGenTextures(1, &FBOTex);
	glBindTexture(GL_TEXTURE_2D, FBOTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, FBOTex);

	//Create framebuffer
	glGenFramebuffers(1, &(fData->fb));
	glBindFramebuffer(GL_FRAMEBUFFER, fData->fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOTex, 0);
	CheckFramebufferStatus("Framebuffer error!");

	//Create depthbuffer
	glGenRenderbuffers(1, &(fData->rb));
	glBindRenderbuffer(GL_RENDERBUFFER, fData->rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fData->width, fData->height);

	//Attach render buffer to frame buffer depth buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fData->rb);
	CheckFramebufferStatus("Error attaching renderbuffer!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FBOData GetFBOData()
{
	return *fData;
}