///////////////////////////////////////////////////////////////
// (c) 2017 Matt Phillips, Big Evil Corporation
//
// File:		Entity.h
// Date:		12th January 2017
// Authors:		Matt Phillips
// Description:	Basic game entity
//				(loosely mirrors Mega Drive framework)
///////////////////////////////////////////////////////////////

#include <ion/renderer/Renderer.h>
#include <ion/beehive/GameObject.h>

class Entity
{
public:
	Entity(const GameObject& gameObject, const GameObjectType& gameObjType);
	virtual ~Entity();

	//Update/render
	virtual void Update(float deltaTime) {}
	virtual void Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv) {}

	std::string m_name;			//Name
	ion::Vector2 m_worldPos;	//World position
	ion::Vector2 m_size;		//Game object size
	bool m_active;				//Active flag
};