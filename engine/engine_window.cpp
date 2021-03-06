#include "Engine.h"
#include <SDL_syswm.h>
#include "easylogging++.h"

void Engine::changeScreenResolution(float w, float h)
{
	LOG(INFO) << "Changing screen resolution to " << w << ", " << h;
	int vsync = SDL_GL_GetSwapInterval();
//In Windoze, we copy the graphics memory to our new context, so we don't have to reload all of our images and stuff every time the resolution changes
//TODO: Look into SDL_GL_SHARE_WITH_CURRENT_CONTEXT for newer versions of SDL instead
#ifdef _WIN32
	SDL_SysWMinfo info;
 
	//Get window handle from SDL
	SDL_VERSION(&info.version);
	if(SDL_GetWindowWMInfo(m_Window, &info) == -1) 
	{
		LOG(ERROR) << "SDL_GetWMInfo #1 failed";
		return;
	}

	//Get device context handle
	HDC tempDC = GetDC(info.info.win.window);

	//Create temporary context
	HGLRC tempRC = wglCreateContext(tempDC);
	if(tempRC == NULL) 
	{
		LOG(ERROR) << "wglCreateContext failed";
		return;
	}
	
	//Share resources to temporary context
	SetLastError(0);
	if(!wglShareLists(wglGetCurrentContext(), tempRC))
	{
		LOG(ERROR) << "wglShareLists #1 failed";
		return;
	}
#endif
	
	m_iWidth = w;
	m_iHeight = h;
	
	if(m_bFullscreen)
		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	
	SDL_SetWindowSize(m_Window, m_iWidth, m_iHeight);

	SDL_GL_CreateContext(m_Window);
	if(SDL_GL_SetSwapInterval(vsync) == -1) //old vsync won't work in this new mode; default to vsync on
		SDL_GL_SetSwapInterval(1);
	
	//Set OpenGL back up
	setup_opengl();
	
#ifdef _WIN32
	//Previously used structure may possibly be invalid, to be sure we get it again
	SDL_VERSION(&info.version);
	if(SDL_GetWindowWMInfo(m_Window, &info) == -1) 
	{
		LOG(ERROR) << "SDL_GetWMInfo #2 failed";
		return;
	}
 
	//Share resources to our new SDL-created context
	if(!wglShareLists(tempRC, wglGetCurrentContext()))
	{
		LOG(ERROR) << "wglShareLists #2 failed";
		return;
	}
 
	//We no longer need our temporary context
	if(!wglDeleteContext(tempRC))
	{
		LOG(ERROR) << "wglDeleteContext failed";
		return;
	}
#else
	//TODO See what can be done on Linux/Mac about this
	#error need to research OpenGL context sharing for Linux/Mac
#endif
}

void Engine::setFullscreen(bool bFullscreen)
{
	if(m_bFullscreen == bFullscreen) 
		return;
	m_bFullscreen = !m_bFullscreen;
	if(m_bFullscreen)
		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN_DESKTOP);	//TODO: Does this work?
	else
		SDL_SetWindowFullscreen(m_Window, 0);
}

bool Engine::isMaximized()
{
#ifdef _WIN32	//Apparently SDL_GetWindowFlags() is completely broken in SDL2 for determining maximization. See: https://bugzilla.libsdl.org/show_bug.cgi?id=2282
				//TODO: This bug has been marked as fixed in SDL 2.0.4, test and see if it is!
	SDL_SysWMinfo info;
 
	//Get window handle from SDL
	SDL_VERSION(&info.version);
	if(SDL_GetWindowWMInfo(m_Window, &info) == -1) 
	{
		LOG(ERROR) << "SDL_GetWMInfo failed";
		return false;
	}
	
	return IsZoomed(info.info.win.window);
#else
	return (SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MAXIMIZED);	//TODO: Test in Mac/Linux
#endif
}

Vec2 Engine::getWindowPos()
{
	Vec2 p;
	int x, y;
	SDL_GetWindowPosition(m_Window, &x, &y);
	p.x = x;
	p.y = y;
	return p;
}

//If we used to be fullscreen, and now we're not, we don't want to be in the upper-left corner.
void Engine::setWindowPos(Vec2 pos)
{
	if(pos.x > 0 && pos.y > 0)
		SDL_SetWindowPosition(m_Window, pos.x, pos.y);
	else
		SDL_SetWindowPosition(m_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);	//Center if we're upper-left corner or above or some such.
}

void Engine::maximizeWindow()
{
	SDL_MaximizeWindow(m_Window);
}

