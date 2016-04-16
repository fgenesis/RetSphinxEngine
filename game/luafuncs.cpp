#include "luafuncs.h"
#include "luainterface.h"
#include "GameEngine.h"

extern GameEngine* g_pGlobalEngine; //TODO Would love to get rid of this

//-----------------------------------------------------------------------------------------------------------
//Class for interfacing between GameEngine and Lua
//(defined here instead of inside header because of weird cross-inclusion stuff)
//-----------------------------------------------------------------------------------------------------------
class GameEngineLua
{
public:
	static void rumble(float32 fAmt, float32 len, int priority = 0)
	{
		g_pGlobalEngine->rumbleController(fAmt, len, priority);
	}
	
	static void camera_centerOnXY(Point pt)
	{
		g_pGlobalEngine->CameraPos.x = -pt.x;
		g_pGlobalEngine->CameraPos.y = -pt.y;
	}
	
	static obj* xmlParseObj(string sClassName, Point ptOffset = Point(0,0), Point ptVel = Point(0,0))
	{
		return g_pGlobalEngine->objFromXML(sClassName, ptOffset, ptVel);
	}
	
	static void addObject(obj* o)
	{
		g_pGlobalEngine->addAfterUpdate(o);
	}
	
	static obj* getPlayerObject()
	{
		return g_pGlobalEngine->player;
	}
	
	static void setPlayerObject(obj* o)
	{
		g_pGlobalEngine->player = o;
	}
	
	static void loadMap(string sMap, string sNode = "")
	{
		g_pGlobalEngine->m_sLoadScene = sMap;
		g_pGlobalEngine->m_sLoadNode = sNode;
	}
	
	static bool keyDown(int key)
	{
		return g_pGlobalEngine->keyDown(key);
	}
	
	static bool joyDown(int button)
	{
		if(g_pGlobalEngine->m_joy)
			return (SDL_JoystickGetButton(g_pGlobalEngine->m_joy, button) > 0);
		return false;
	}
	
	static int joyAxis(int axis)
	{
		if(g_pGlobalEngine->m_joy)
			return SDL_JoystickGetAxis(g_pGlobalEngine->m_joy, axis);
		return 0;
	}
	
	static ParticleSystem* createParticles(string sName)
	{
		ParticleSystem* pSys = new ParticleSystem();
		pSys->fromXML(sName);
		g_pGlobalEngine->addParticles(pSys);
		return pSys;
	}
	
	static LuaInterface* getLua()
	{
		return g_pGlobalEngine->Lua;
	}
	
	static float getFramerate()
	{
		return g_pGlobalEngine->getFramerate();
	}
};




//-----------------------------------------------------------------------------------------------------------
// Lua->C++ casting functions and such
//-----------------------------------------------------------------------------------------------------------
template<typename T> T *getObj(lua_State *L, unsigned pos = 1)
{
	LuaObjGlue *glue = (LuaObjGlue*)lua_touserdata(L, pos);
	void *p = T::TYPE == glue->type ? glue->obj : NULL; // TODO ALSO ERROR CHECKS
	return (T*)p;
}

//-----------------------------------------------------------------------------------------------------------
// Lua interface functions (can be called from lua)
//-----------------------------------------------------------------------------------------------------------

luaFunc(controller_rumble)	//void controller_rumble(float force, float sec) --force is range [0,1]
{
	float32 force = lua_tonumber(L, 1);
	float32 sec = lua_tonumber(L, 2);
	int priority = lua_tointeger(L, 3);
	GameEngineLua::rumble(force, sec, priority);
	luaReturnNil();
}

luaFunc(map_load)	//void map_load(string sFilename, string sNodeToPlacePlayerAt = "")
{
	if(lua_isstring(L, 2))
		GameEngineLua::loadMap(lua_tostring(L, 1), lua_tostring(L, 2));
	else
		GameEngineLua::loadMap(lua_tostring(L, 1));
	luaReturnNil();
}

luaFunc(getFramerate)	//float getFramerate()
{
	luaReturnNum(GameEngineLua::getFramerate());
}

//-----------------------------------------------------------------------------------------------------------
// Object functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(obj_setAngle)	//void obj_setAngle(obj* o, float angle)
{
	obj *o = getObj<obj>(L);
	float f = lua_tonumber(L,2);
	if(o)
	{
		b2Body* b = o->getBody();
		if(b)
		{
			Point p = b->GetPosition();
			b->SetTransform(p, f);
		}
	}
	luaReturnNil();
}

luaFunc(obj_getAngle)	//float obj_getAngle(obj* o)
{
	obj *o = getObj<obj>(L);
	float f = 0.0f;
	if(o)
	{
		b2Body* b = o->getBody();
		if(b)
			f = b->GetAngle();
	}
	luaReturnNum(f);
}

luaFunc(obj_setVelocity)	//void obj_setVelocity(obj* o, float xvel, float yvel)
{
	obj *o = getObj<obj>(L);
	Point p(lua_tonumber(L,2),lua_tonumber(L, 3));
	if(o)
	{
		b2Body* b = o->getBody();
		if(b)
			b->SetLinearVelocity(p);
	}
	luaReturnNil();
}

luaFunc(obj_getVelocity)	//float x, y obj_getVelocity(obj* o)
{
	Point p(0,0);
	obj *o = getObj<obj>(L);
	if(o)
	{
		b2Body* b = o->getBody();
		if(b)
			p = b->GetLinearVelocity();
	}
	luaReturnVec2(p.x, p.y);
}

luaFunc(obj_applyForce)	//void obj_applyForce(obj* o, float x, float y)
{
	obj *o = getObj<obj>(L);
	Point pForce(lua_tonumber(L,2),lua_tonumber(L, 3));
	if(o)
	{
		b2Body* b = o->getBody();
		if(b)
			b->ApplyForceToCenter(pForce, true);
	}
	luaReturnNil();
}

luaFunc(obj_getPos)		//float x, float y obj_getPos(obj* o)
{
	obj *o = getObj<obj>(L);
	Point pt(0,0);
	if(o)
	{
		b2Body* bod = o->getBody();
		if(bod != NULL)
			pt = bod->GetPosition();
	}
	luaReturnVec2(pt.x, pt.y);
}

luaFunc(obj_setPos)	//void obj_setPos(obj* o, float x, float y)
{
	obj *o = getObj<obj>(L);
	Point p(lua_tonumber(L,2), lua_tonumber(L,3));
	if(o)
		o->setPosition(p);
	luaReturnNil();
}

luaFunc(obj_create) //obj* obj_create(string className, float xpos, float ypos, float xvel, float yvel)
{
	if(!lua_isstring(L,1)) luaReturnNil();
	
	Point ptPos(0,0);
	Point ptVel(0,0);
	if(lua_isnumber(L, 2))
		ptPos.x = lua_tonumber(L, 2);
	if(lua_isnumber(L, 3))
		ptPos.y = lua_tonumber(L, 3);
	if(lua_isnumber(L, 4))
		ptVel.x = lua_tonumber(L, 4);
	if(lua_isnumber(L, 5))
		ptVel.y = lua_tonumber(L, 5);
	obj* o = GameEngineLua::xmlParseObj(lua_tostring(L, 1), ptPos, ptVel);
	if(o)
	{
		GameEngineLua::addObject(o);
		luaReturnObj(o);
	}
	luaReturnNil();
}

luaFunc(obj_getPlayer)	//obj* obj_getPlayer()
{
	obj* o = GameEngineLua::getPlayerObject();
	if(o)
		luaReturnObj(o);
	luaReturnNil();
}

luaFunc(obj_registerPlayer)	//void obj_registerPlayer(obj* o)
{
	obj *o = getObj<obj>(L);
	if(o)
		GameEngineLua::setPlayerObject(o);
	luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Camera functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(camera_centerOnXY)	//camera_centerOnXY(float x, float y)
{
	Point pt(lua_tonumber(L, 1), lua_tonumber(L, 2));
	GameEngineLua::camera_centerOnXY(pt);
	luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Node functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(node_getProperty)	//string node_getProperty(Node* n, string propName)
{
	string s;
	Node* n = getObj<Node>(L);
	if(n)
	{
		string sProp = lua_tostring(L, 2);
		if(n->propertyValues.count(sProp))
			s = n->propertyValues[sProp];
	}
	luaReturnString(s);
}

luaFunc(node_getVec2Property)	//float x, float y node_getVec2Property(Node* n, string propName)
{
	Point pt(0,0);
	Node* n = getObj<Node>(L);
	if(n)
	{
		string sProp = lua_tostring(L, 2);
		if(n->propertyValues.count(sProp))
			pt = pointFromString(n->propertyValues[sProp]);
	}
	luaReturnVec2(pt.x, pt.y);
}

luaFunc(node_getPos)	//float x, float y node_getPos(Node* n)
{
	Point pos(0,0);
	Node* n = getObj<Node>(L);
	if(n)
		pos = n->pos;
	luaReturnVec2(pos.x, pos.y);
}

//-----------------------------------------------------------------------------------------------------------
// Particles functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(particles_create)	//ParticleSystem* particles_create(string sXMLFile)
{
	if(!lua_isstring(L,1)) luaReturnNil();
	
	ParticleSystem* ps = GameEngineLua::createParticles(lua_tostring(L, 1));
	LuaInterface* LI = GameEngineLua::getLua();
	ps->glue = LI->createObject(ps, ps->TYPE, NULL);
	luaReturnObj(ps);
}

luaFunc(particles_setFiring)	//void particles_setFiring(ParticleSystem* p, bool fire)
{
	ParticleSystem* ps = getObj<ParticleSystem>(L);
	if(ps)
		ps->firing = lua_toboolean(L, 2);
	luaReturnNil();
}

luaFunc(particles_setFireRate)	//void particles_setFirerate(ParticleSystem* p, float rate [0..1+])
{
	ParticleSystem* ps = getObj<ParticleSystem>(L);
	if(ps)
		ps->curRate = lua_tonumber(L, 2);
	luaReturnNil();
}

luaFunc(particles_setEmitPos)	//void particles_setEmitPos(ParticleSystem* p, float x, float y)
{
	ParticleSystem* ps = getObj<ParticleSystem>(L);
	if(ps)
		ps->emitFrom.centerOn(Point(lua_tonumber(L, 2), lua_tonumber(L, 3)));
	luaReturnNil();
}

luaFunc(particles_setEmitVel)	//void particles_setEmitVel(ParticleSystem* p, float x, float y)
{
	ParticleSystem* ps = getObj<ParticleSystem>(L);
	if(ps)
	{
		ps->emissionVel.x = lua_tonumber(L, 2);
		ps->emissionVel.y = lua_tonumber(L, 3);
	}
	luaReturnNil();
}

//-----------------------------------------------------------------------------------------------------------
// Input functions
//-----------------------------------------------------------------------------------------------------------
luaFunc(key_isDown) //bool key_isDown(SDL_Scancode key)
{
	luaReturnBool(GameEngineLua::keyDown(lua_tointeger(L, 1)));
}

luaFunc(joy_isDown) //bool joy_isDown(int button)
{
	luaReturnBool(GameEngineLua::joyDown(lua_tointeger(L, 1)));
}

luaFunc(joy_getAxis) //int joy_getAxis(int axis)
{
	luaReturnInt(GameEngineLua::joyAxis(lua_tointeger(L, 1)));
}

//-----------------------------------------------------------------------------------------------------------
// Lua constants & functions registerer
//-----------------------------------------------------------------------------------------------------------
static LuaFunctions s_functab[] =
{
	luaRegister(controller_rumble),
	luaRegister(getFramerate),
	luaRegister(obj_setVelocity),
	luaRegister(obj_getVelocity),
	luaRegister(obj_getPos),
	luaRegister(obj_setPos),
	luaRegister(obj_create),
	luaRegister(obj_applyForce),
	luaRegister(obj_getPlayer),
	luaRegister(obj_registerPlayer),
	luaRegister(obj_setAngle),
	luaRegister(obj_getAngle),
	luaRegister(camera_centerOnXY),
	luaRegister(node_getProperty),
	luaRegister(node_getVec2Property),
	luaRegister(node_getPos),
	luaRegister(map_load),
	luaRegister(particles_create),
	luaRegister(particles_setFiring),
	luaRegister(particles_setFireRate),
	luaRegister(particles_setEmitPos),
	luaRegister(particles_setEmitVel),
	luaRegister(key_isDown),
	luaRegister(joy_isDown),
	luaRegister(joy_getAxis),
	{NULL, NULL}
};

static const struct {
	const char *name;
	int value;
} luaConstantTable[] = {
	
	//Joystick
	luaConstant(JOY_MIN),
	luaConstant(JOY_MAX),
	
	//Keyboard
	luaConstant(SDL_SCANCODE_SPACE),
};

void lua_register_all(lua_State *L)
{	
	//Register functions
	for(unsigned int i = 0; s_functab[i].name; ++i)
		lua_register(L, s_functab[i].name, s_functab[i].func);
	
	//Register constants
	for (unsigned int i = 0; i < sizeof(luaConstantTable)/sizeof(*luaConstantTable); i++)
	{
		lua_pushinteger(L, luaConstantTable[i].value);
		lua_setglobal(L, luaConstantTable[i].name);
	}
}
