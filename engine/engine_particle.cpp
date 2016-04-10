#include "Engine.h"

void Engine::cleanupParticles()
{
	for(list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)
		delete *i;
	m_particles.clear();
}

void Engine::drawParticles()
{
	for(list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)
		(*i)->draw();
}

void Engine::updateParticles(float32 dt)
{
	for(list<ParticleSystem*>::iterator i = m_particles.begin(); i != m_particles.end(); i++)
	{
		(*i)->update(dt);
		if((*i)->done())
		{
			delete *i;
			i = m_particles.erase(i);
			continue;
		}
	}
}