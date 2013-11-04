#ifdef HAVE_GL_GLX
#include <stdio.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include "../OGLRender.h"

static bool glx_beginOpenGL(void) { return 1; }
static void glx_endOpenGL(void) { }
static bool glx_init(void) { return true; }

int init_glx_3Demu(void) 
{
    Display *dpy = XOpenDisplay(NULL);
    XVisualInfo *vis;
    GLXContext ctx;
    GLXFBConfig *cfg;
    GLXPbuffer pbuf;
    int maj, min;

    if (!dpy)
        return 0;

    // Check if GLX is present
    if (!glXQueryVersion(dpy, &maj, &min))
        return 0;

    // We need GLX 1.3 at least
    if (maj < 1 || (maj == 1 && min < 3))
        return 0;

    const int vis_attr[] =  {
      GLX_RGBA, 
      GLX_RED_SIZE,       8, 
      GLX_GREEN_SIZE,     8, 
      GLX_BLUE_SIZE,      8, 
      GLX_ALPHA_SIZE,     8,
      GLX_DEPTH_SIZE,     24,
      GLX_STENCIL_SIZE,   8,
      GLX_DOUBLEBUFFER, 
      None
    };
    vis = glXChooseVisual(dpy, DefaultScreen(dpy), (int *)&vis_attr);

    if (!vis)
        return 0;

    const int fb_attr[] = { 
      GLX_RENDER_TYPE,    GLX_RGBA_BIT,
      GLX_DRAWABLE_TYPE,  GLX_PBUFFER_BIT,
      GLX_X_VISUAL_TYPE,  GLX_TRUE_COLOR,
      GLX_DOUBLEBUFFER,   true,
      GLX_X_RENDERABLE,   true,
      GLX_RED_SIZE,       8,
      GLX_GREEN_SIZE,     8,
      GLX_BLUE_SIZE,      8,
      GLX_ALPHA_SIZE,     8,
      GLX_DEPTH_SIZE,     24,
      GLX_STENCIL_SIZE,   8,
      None 
    };
    int configs;
    cfg = glXChooseFBConfig(dpy, DefaultScreen(dpy), (int *)&fb_attr, &configs);

    if (!cfg)
      return 0;

    const int pbuf_attr[] = {
      GLX_PBUFFER_WIDTH,  256,
      //GLX_PBUFFER_HEIGHT, 192, // Use a square size to prevent possible incompatibilities
      GLX_PBUFFER_HEIGHT, 256,
      None
    };
    // The first should match exactly, otherwise is the least wrong one
    pbuf = glXCreatePbuffer(dpy, cfg[0], (int *)&pbuf_attr);

    XFree(cfg);

    ctx = glXCreateContext(dpy, vis, NULL, true);

    if (!ctx)
      return 0;

    if (!glXMakeContextCurrent(dpy, pbuf, pbuf, ctx))
      return 0;

    printf("OGL/GLX Renderer has finished the initialization.\n");

    oglrender_init = glx_init;
    oglrender_beginOpenGL = glx_beginOpenGL;
    oglrender_endOpenGL = glx_endOpenGL;

    return 1;
}

#endif // HAVE_GLX