#include "Engine.h"

void Engine::addObject(obj* o)
{
	if(o != NULL)
		m_lObjects.push_back(o);
}

//TODO: Where is this even used
void Engine::updateSceneryLayer(physSegment* seg)
{
	for(multiset<physSegment*>::iterator layer = m_lScenery.begin(); layer != m_lScenery.end(); layer++)
	{
		if((*layer) == seg)
		{
			m_lScenery.erase(layer);
			break;
		}
	}
	m_lScenery.insert(seg);
}

//TODO: Why do we have these separate if we have drawAll() already? Doesn't that make these incorrect because of z-ordering?
void Engine::drawBg()
{
	for(multiset<physSegment*>::iterator layer = m_lScenery.begin(); layer != m_lScenery.end(); layer++)	//Draw bg layers
	{
		if((*layer)->depth > 0)
			break;
		(*layer)->draw();
	}
}

void Engine::drawObjects()
{
	for(list<obj*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)	//Draw objects
		(*i)->draw();
}

void Engine::drawFg()
{
	for(multiset<physSegment*>::iterator layer = m_lScenery.begin(); layer != m_lScenery.end(); layer++)	//Draw fg layers
	{
		if((*layer)->depth > 0)
			(*layer)->draw();
	}
}

void Engine::drawAll()
{
	multimap<float, Drawable*> drawList;	//Depth matters, and some Drawables will be at different depths, so draw all in one pass

	for(multiset<physSegment*>::iterator i = m_lScenery.begin(); i != m_lScenery.end(); i++)	//Add bg layers
		drawList.insert(make_pair((*i)->depth, (Drawable*)(*i)));
		
	for(list<obj*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)	//Add objects
		drawList.insert(make_pair((*i)->depth, (Drawable*)(*i)));
		
	for(list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)	//Add particles
		drawList.insert(make_pair((*i)->depth, (Drawable*)(*i)));
		
	//Draw everything in one pass
	for(multimap<float, Drawable*>::iterator i = drawList.begin(); i != drawList.end(); i++)
		i->second->draw();
}

void Engine::cleanupObjects()
{
	for(list<obj*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
		delete (*i);
	for(multiset<physSegment*>::iterator i = m_lScenery.begin(); i != m_lScenery.end(); i++)
		delete (*i);
	m_lScenery.clear();
	m_lObjects.clear();
}

void Engine::updateObjects(float32 dt)
{
	list<b2BodyDef*> lAddObj;
	for(list<obj*>::iterator i = m_lObjects.begin(); i != m_lObjects.end(); i++)
	{
		b2BodyDef* def = (*i)->update(dt);
		if(def != NULL)
			lAddObj.push_back(def);
	}
	for(list<b2BodyDef*>::iterator i = lAddObj.begin(); i != lAddObj.end(); i++)
	{
		objFromXML(*((string*)(*i)->userData), (*i)->position, (*i)->linearVelocity);
		delete (*i);	//Free up memory
	}
}