#if !defined(_VIDEOGL_H_) && defined(USE_GL)
#define _VIDEOGL_H_

#include <SDL.h>
#include <SDL_opengl.h>

#if defined(CYGWIN) && defined(__WIN32__)
#undef __WIN32__
#endif

#define VIDEO_GL_WIDTH 640
#define VIDEO_GL_HEIGHT 480

#ifndef DREAMCAST
#define PSX4ALL_FILTER_NONE     GL_NEAREST
#define PSX4ALL_FILTER_BILINEAR GL_LINEAR
#else
#define PSX4ALL_FILTER_NONE     GL_FILTER_NONE
#define PSX4ALL_FILTER_BILINEAR GL_FILTER_BILINEAR
#endif

extern SDL_Surface *gp2x_sdlwrapper_gl_screen;
extern void *gp2x_sdlwrapper_texture_buffer;
extern GLint gp2x_sdlwrapper_screen_texture;


static __inline__ void loadTextureParams(void)
{
//	glPixelStorei(GL_UNPACK_ROW_LENGTH, 16);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, PSX4ALL_FILTER_NONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, PSX4ALL_FILTER_NONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi(GL_TEXTURE_2D,GL_TEXTURE_ENV_MODE, GL_REPLACE);
}


#endif
