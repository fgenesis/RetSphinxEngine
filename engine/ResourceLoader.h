#pragma once
#include <map>
#include <string>
#include <inttypes.h>
#include "tinyxml2.h"
#include "Rect.h"

class Image;
class Object;
class ParticleSystem;
class MouseCursor;
class ObjSegment;
class b2World;
class b2Body;
class ResourceCache;
class Mesh3D;
class PakLoader;

class ResourceLoader
{
	b2World* m_world;
	ResourceCache* m_cache;
	PakLoader* m_pakLoader;
	std::string m_sPakDir;

	uint64_t hash(std::string sHashStr);

	void readFixture(tinyxml2::XMLElement* fixture, b2Body* bod);
	ResourceLoader() {};
public:
	ResourceLoader(b2World* physicsWorld, std::string sPakDir);
	~ResourceLoader();

	void clearCache();

	//Images
	Image* getImage(std::string sID);

	//Meshes
	Mesh3D* getMesh(std::string sID);

	//Particles
	ParticleSystem* getParticleSystem(std::string sID);

	//Mouse cursors
	MouseCursor* getCursor(std::string sID);

	//TODO: Private
	ObjSegment* getObjSegment(tinyxml2::XMLElement* layer);

	Object* objFromXML(std::string sType, Vec2 ptOffset, Vec2 ptVel);
};