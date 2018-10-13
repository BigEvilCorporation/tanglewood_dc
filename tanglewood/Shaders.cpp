#pragma once

#include "Shaders.h"

namespace Assets
{
	namespace Shaders
	{
		namespace FlatColoured
		{
			ion::io::ResourceHandle<ion::render::Shader> vertexShader;
			ion::io::ResourceHandle<ion::render::Shader> pixelShader;
		}

		namespace FlatTextured
		{
			ion::io::ResourceHandle<ion::render::Shader> vertexShader;
			ion::io::ResourceHandle<ion::render::Shader> pixelShader;
		}

		namespace IndexTexture
		{
			ion::io::ResourceHandle<ion::render::Shader> vertexShader;
			ion::io::ResourceHandle<ion::render::Shader> pixelShader;

			namespace Params
			{
				ion::render::Shader::ParamHndl<ion::render::Texture> indexedTexture;
				ion::render::Shader::ParamHndl<ion::render::Texture> paletteTexture;
			}
		}
	}
}
