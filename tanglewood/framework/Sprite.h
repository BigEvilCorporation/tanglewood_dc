#include <ion/core/memory/Memory.h>
#include <ion/renderer/Renderer.h>
#include <ion/renderer/Primitive.h>
#include <ion/renderer/Texture.h>
#include <ion/renderer/Material.h>
#include <ion/beehive/Actor.h>
#include <ion/beehive/SpriteSheet.h>
#include <ion/beehive/SpriteAnimation.h>

class Sprite : public ion::render::Entity
{
public:
	Sprite();

	void LoadActor(Actor& actor);
	void SetAnimation(const std::string& sheetName, const std::string& animName);

	void Update(float deltaTime);
	void Render(ion::render::Renderer& renderer, const ion::Matrix4& cameraInv);

private:
	struct Sheet
	{
		struct Frame
		{
			ion::render::Texture* texture;
			ion::render::Material* material;
		};

		ion::render::Chessboard* m_primitive;
		std::vector<Frame> m_frames;
		std::map<std::string, SpriteAnimation*> m_animations;
	};

	void LoadSheet(SpriteSheet& spriteSheet);
	
	std::map<std::string, Sheet> m_sheets;
	Sheet* m_currentSheet;
	SpriteAnimation* m_currentAnim;
};