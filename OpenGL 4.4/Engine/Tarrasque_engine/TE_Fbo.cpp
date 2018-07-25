#include "TE_Fbo.h"



///////////////////////
// BODY OF CLASS TE_Fbo //
///////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/**
* Constructor.
*/
TE_Fbo::TE_Fbo()
{
	// Init reserved data:
	memset(glRenderBufferId, 0, sizeof(GLuint) * MAX_ATTACHMENTS);
	for (unsigned int c = 0; c<TE_Fbo::MAX_ATTACHMENTS; c++)
	{
		texture[c] = 0;
		drawBuffer[c] = -1;	// -1 means empty
	}
	nrOfMrts = 0;
	mrt = nullptr;

	// Allocate OGL data:
	glGenFramebuffers(1, &glId);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/**
* Destructor.
*/
TE_Fbo::~TE_Fbo()
{																															   
	// Release reserved data:
	if (mrt)
		delete[] mrt;
	for (unsigned int c = 0; c<TE_Fbo::MAX_ATTACHMENTS; c++)
		if (glRenderBufferId[c])
			glDeleteRenderbuffers(1, &glRenderBufferId[c]);
	glDeleteFramebuffers(1, &glId);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/**
* Get texture connected to buffer #.
* @param textureNumber texture position
* @return texture glId or 0 if wrong/not available
*/
unsigned int TE_Fbo::getTexture(unsigned int textureNumber)
{
	if (textureNumber < TE_Fbo::MAX_ATTACHMENTS)
		return texture[textureNumber];
	else
		return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/**
* Check TE_Fbo consistency.
* @return true on success, false on fail and print error in console
*/
bool TE_Fbo::isOk()
{
	// Make TE_Fbo current:
	render();

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[ERROR] TE_Fbo not complete (error: " << status << ")" << std::endl;
		return false;
	}

	// Done:
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/**
* Binds a texture to the framebuffer.
* @param textureNumber a value between 0 and OvTE_Fbo::MAX_ATTACHMENTS to identify texture position
* @param operation one of the enumerated operations of type OvTE_Fbo::BIND_*
* @param texture pointer to a texture class
* @param param1 free param 1, according to the operation
* @param param2 free param 2, according to the operation
* @return true on success, false on fail
*/
bool TE_Fbo::bindTexture(unsigned int textureNumber, unsigned int operation, unsigned int texture, int param1, int param2)
{
	// Safety net:
	if (textureNumber >= TE_Fbo::MAX_ATTACHMENTS)
	{
		std::cout << "[ERROR] Invalid params" << std::endl;
		return false;
	}

	// Bind buffer:
	render();

	// Perform operation:   
	switch (operation)
	{
		//////////////////////////
	case BIND_COLORTEXTURE: //		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + param1, GL_TEXTURE_2D, texture, 0);
		drawBuffer[textureNumber] = param1;
		break;

		//////////////////////////
	case BIND_DEPTHTEXTURE: //
		//glReadBuffer(GL_NONE);
		//glDrawBuffer(GL_NONE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
		break;

		///////////
	default: //
		std::cout << "[ERROR] Invalid operation" << std::endl;
		return false;
	}

	// Done:
	this->texture[textureNumber] = texture;

	// Get some texture information:
	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &sizeX);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &sizeY);
	return updateMrtCache();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/**
* Initializes and binds a render buffer.
* @param renderBuffer a value between 0 and TE_Fbo::MAX_ATTACHMENTS to identify a render buffer
* @param operation one of the enumerated operations of type OvTE_Fbo::BIND_*
* @param sizeX TE_Fbo sizes, make sure to have the same for all attachments
* @param sizeY TE_Fbo sizes, make sure to have the same for all attachments
* @return true on success, false on fail
*/
bool TE_Fbo::bindRenderBuffer(unsigned int renderBuffer, unsigned int operation, int sizeX, int sizeY)
{
	// Safety net:
	if (renderBuffer >= TE_Fbo::MAX_ATTACHMENTS)
	{
		std::cout << "[ERROR] Invalid params" << std::endl;
		return false;
	}

	// Bind buffer:
	render();

	// If used, delete it first, then create and bind it:
	if (glRenderBufferId[renderBuffer])
		glDeleteRenderbuffers(1, &glRenderBufferId[renderBuffer]);
	glGenRenderbuffers(1, &glRenderBufferId[renderBuffer]);
	glBindRenderbuffer(GL_RENDERBUFFER, glRenderBufferId[renderBuffer]);

	// Perform operation:
	switch (operation)
	{
		/////////////////////////
	case BIND_DEPTHBUFFER: //
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, sizeX, sizeY);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, glRenderBufferId[renderBuffer]);
		break;

	default:
		std::cout << "[ERROR] Invalid operation" << std::endl;
		return false;
	}

	// Done:   
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	return updateMrtCache();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/**
* Update the MRT cache.
* @return true on success, false on fail
*/
bool TE_Fbo::updateMrtCache()
{
	// Delete previous buffer:
	if (mrt)
		delete[] mrt;

	// Count targets:   
	nrOfMrts = 0;
	for (unsigned int c = 0; c<TE_Fbo::MAX_ATTACHMENTS; c++)
		if (drawBuffer[c] != -1)
			nrOfMrts++;

	// Refresh buffer:
	if (nrOfMrts)
	{
		mrt = new GLenum[nrOfMrts];
		int bufferPosition = 0;
		for (int c = 0; c<TE_Fbo::MAX_ATTACHMENTS; c++)
			if (drawBuffer[c] != -1)
			{
				mrt[bufferPosition] = GL_COLOR_ATTACHMENT0 + drawBuffer[c];
				bufferPosition++;
			}
	}

	// Done: 
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/**
* TE_Fbo rendering procedure.
* @param data generic pointer to istance data
* @return true on success, false on fail
*/
void TE_Fbo::disable()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/**
* TE_Fbo rendering procedure.
* @param data generic pointer to istance data
* @return true on success, false on fail
*/
bool TE_Fbo::render(void *data)
{
	// Bind buffers:
	glBindFramebuffer(GL_FRAMEBUFFER, glId);
	if (nrOfMrts)
	{
		glDrawBuffers(nrOfMrts, mrt);
		glViewport(0, 0, sizeX, sizeY);
	}

	// Done:   
	return true;
}

