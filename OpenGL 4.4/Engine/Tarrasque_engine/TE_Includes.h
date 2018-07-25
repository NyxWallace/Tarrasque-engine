#pragma once

//GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_inverse.hpp>

// Glew (include it before GL.h):
#include <GL/glew.h>

//Freeglut
#include <GL/freeglut.h>
#include <GL/glut.h>
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

//freeimage
#include "FreeImage.h"

// Assimp:
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/version.h>

//system libs
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cstdio>
#include <sys/stat.h>
#include <stdexcept>
#include <ctime>
#include <cstddef>

#ifdef WIN32
#define snprintf sprintf_s
#endif
