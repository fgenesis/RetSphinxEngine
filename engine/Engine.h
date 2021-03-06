/*
	GameEngine header - Engine.h
	Copyright (c) 2014 Mark Hutcheson
*/
#pragma once

#include "Object.h"
#include "Text.h"
#include "HUD.h"
#include "MouseCursor.h"
#include "EngineContactListener.h"
#include "Node.h"
#include "DebugDraw.h"

class b2World;
class Image;
class ResourceLoader;
class EntityManager;

#define VELOCITY_ITERATIONS 8
#define PHYSICS_ITERATIONS 3

//SDL codes that should be defined but aren't
#define SDL_BUTTON_FORWARD	SDL_BUTTON_X2
#define SDL_BUTTON_BACK		SDL_BUTTON_X1
#define SDL_SCANCODE_CTRL	(SDL_NUM_SCANCODES)
#define SDL_SCANCODE_SHIFT 	(SDL_NUM_SCANCODES+1)
#define SDL_SCANCODE_ALT	(SDL_NUM_SCANCODES+2)
#define SDL_SCANCODE_GUI	(SDL_NUM_SCANCODES+3)

const float soundFreqDefault = 44100.0;

typedef struct
{
	std::string sSwitch, sValue;
} commandlineArg;

class Engine
{
private:
	//Variables for use by the engine
	std::string m_sTitle;
	std::string m_sAppName;
	std::string m_sIcon;
	SDL_Window* m_Window;
	std::list<commandlineArg> lCommandLine;
	b2World* m_physicsWorld;
	EngineContactListener m_clContactListener;
	DebugDraw m_debugDraw;
	bool m_bDebugDraw;
	bool m_bObjDebugDraw;
	Vec2 m_ptCursorPos;
	bool  m_bShowCursor;
	float m_fFramerate;
	float m_fAccumulatedTime;
	float m_fTargetTime;
	
	bool m_bQuitting;   //Stop the game if this turns true
	float m_fTimeScale;	//So we can scale time if we want
	uint16_t m_iWidth, m_iHeight;
	const Uint8 *m_iKeystates;	//Keep track of keys that are pressed/released so we can poll as needed
	int m_iNumScreenModes;	  //Number of screen modes that are available
	bool m_bFullscreen;
	bool m_bResizable;
	float m_fGamma;		//Overall screen brightness
	bool m_bPaused;			//If the game is paused due to not being focused
	bool m_bPauseOnKeyboardFocus;	//If the game pauses when keyboard focus is lost
	bool m_bSoundDied;  //If tyrsound fails to load, don't try to use it
	int m_iMSAA;		//Antialiasing (0x, 2x, 4x, 8x, etc)
	MouseCursor* m_cursor;
	bool m_bCursorShow;
	bool m_bCursorOutOfWindow;	//If the cursor is outside of the window, don't draw it
#ifdef _DEBUG
	bool m_bStepFrame;
	bool m_bSteppingPhysics;
#endif
	ResourceLoader* m_resourceLoader;
	EntityManager* m_entityManager;
	
	
	//multimap<string, FMOD_CHANNEL*> m_channels;
	//map<string, FMOD_SOUND*> m_sounds;
	//FMOD_SYSTEM* m_audioSystem;

	//Engine-use function definitions
	bool _frame();
	void _render();
	
	void setup_sdl();
	void setup_opengl();
	void _loadicon();					//Load icon and set window to have said icon

	Engine(){}; //Default constructor isn't callable

protected:

	//Functions to override in your own class definition
	virtual void frame(float dt) = 0;   //Function that's called every frame
	virtual void draw() = 0;	//Actual function that draws stuff
	virtual void init(std::list<commandlineArg> sArgs) = 0;	//So we can load all our images and such
	virtual void handleEvent(SDL_Event event) = 0;  //Function that's called for each SDL input event
	virtual void pause() = 0;	//Called when the window is deactivated
	virtual void resume() = 0;	//Called when the window is activated again

public:
	//Constructor/destructor
	Engine(uint16_t iWidth, uint16_t iHeight, std::string sTitle, std::string sAppName, std::string sIcon, bool bResizable = false);
	~Engine();

	//Misc. methods
	void commandline(std::list<std::string> argv);	//Pass along commandline arguments for the engine to use
	void start();   //Runs engine and doesn't exit until the engine ends
	void quit() {m_bQuitting = true;};  //Stop the engine and quit nicely
	std::string getSaveLocation();		//Get good location to save config files/save files
	Rect getCameraView(Vec3 Camera);		//Return the rectangle, in world position z=0, that the camera can see
	Vec2 worldPosFromCursor(Vec2 cursorpos, Vec3 Camera);	//Get the worldspace position of the given mouse cursor position
	Vec2 worldMovement(Vec2 cursormove, Vec3 Camera);		//Get the worldspace transform of the given mouse transformation
	
	//Drawing functions
	Rect getScreenRect()	{Rect rc(0,0,getWidth(),getHeight()); return rc;};
	void drawDebug();
	void drawCursor();
	void fillScreen(Color col);
	
	//Window functions - engine_window.cpp
	void changeScreenResolution(float w, float h);  //Change resolution mid-game and reload OpenGL textures as needed
	//void toggleFullscreen();							//Switch between fullscreen/windowed modes
	void setFullscreen(bool bFullscreen);				//Set fullscreen to true or false as needed
	void setInitialFullscreen() {SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN_DESKTOP);};
	bool isFullscreen()	{return m_bFullscreen;};
	bool isMaximized();	
	Vec2 getWindowPos();	//Get the window position
	void setWindowPos(Vec2 pos);	//Set window position
	void maximizeWindow();								//Call window manager to maximize application window
	void pauseOnKeyboard(bool p)	{m_bPauseOnKeyboardFocus = p;};
	bool pausesOnFocusLost()		{return m_bPauseOnKeyboardFocus;};
	uint16_t getWidth() {return m_iWidth;};
	uint16_t getHeight() {return m_iHeight;};
	
	//Sound functions - engine_sound.cpp
	void createSound(std::string sPath, std::string sName);   //Creates a sound from this name and file path
	virtual void playSound(std::string sName, float volume = 1.0f, float pan = 0.0f, float pitch = 1.0f);	 //Play a sound
	//FMOD_CHANNEL* getChannel(string sSoundName);	//Return the channel of this sound
	void playMusic(std::string sName, float volume = 1.0f, float pan = 0.0f, float pitch = 1.0f);	 //Play looping music, or resume paused music
	void musicLoop(float startSec, float endSec);	//Set the starting and ending loop points for the currently-playing song
	void pauseMusic();									//Pause music that's currently playing
	void resumeMusic();									//Resume music that was paused
	void restartMusic();
	void stopMusic();
	void seekMusic(float fTime);
	float getMusicPos();								//Opposite of seekMusic() -- get where we currently are
	void volumeMusic(float fVol);						//Set the music to a particular volume
	void setMusicFrequency(float freq);
	float getMusicFrequency();
	bool hasMic();										//If we have some form of mic-like input
	void updateSound();
	
	//Keyboard functions
	bool keyDown(int32_t keyCode);  //Test and see if a key is currently pressed
	
	//Physics functions
	b2Body* createBody(b2BodyDef* bdef);
	void setGravity(Vec2 ptGravity);
	void setGravity(float x, float y);
	void stepPhysics(float dt);	//Update our physics world and handle collisions
	void setDebugDraw(bool b) {m_bDebugDraw = b;};
	bool getDebugDraw() {return m_bDebugDraw;};
	void setObjDebugDraw(bool b) {m_bObjDebugDraw = b;};
	bool getObjDebugDraw() {return m_bObjDebugDraw;};
	void toggleDebugDraw() {m_bDebugDraw = !m_bDebugDraw;};
	void toggleObjDebugDraw() {m_bObjDebugDraw = !m_bObjDebugDraw;};
	b2World* getWorld() {return m_physicsWorld;};
#ifdef _DEBUG
	void playPausePhysics()	{m_bSteppingPhysics = !m_bSteppingPhysics;};
	void pausePhysics()		{m_bSteppingPhysics = true;};
	void stepPhysics()		{m_bStepFrame = true;};
#endif
	
	//Mouse functions
	Vec2 getCursorPos()	{return m_ptCursorPos;};
	void setCursorPos(int32_t x, int32_t y);
	bool getCursorDown(int iButtonCode);
	void showCursor()	{m_bCursorShow = true;};
	void hideCursor()	{m_bCursorShow = false;};
	void setCursor(MouseCursor* cur)	{m_cursor = cur;};
	bool isMouseGrabbed();
	void grabMouse(bool bGrab = true);
	
	//Time functions
	float getTimeScale()	{return m_fTimeScale;};
	void setTimeScale(float fScale)	{m_fTimeScale = fScale;};
	unsigned getTicks();
	float getSeconds();
	void setFramerate(float fFramerate);
	float getFramerate()   {return m_fFramerate;};	

	//OpenGL methods
	void setDoubleBuffered(bool bDoubleBuffered);
	bool getDoubleBuffered();
	void setVsync(int iVsync);
	int getVsync();
	int getMSAA()				{return m_iMSAA;};
	void setMSAA(int iMSAA);
	void setGamma(float fGamma)	{m_fGamma = fGamma;};
	float getGamma()				{return m_fGamma;};
	
	//---------------------------------------------------------
	// Entity manager
	EntityManager* getEntityManager() { return m_entityManager; };
	
	//---------------------------------------------------------
	// Resource loader
	ResourceLoader* getResourceLoader() { return m_resourceLoader; };

};
