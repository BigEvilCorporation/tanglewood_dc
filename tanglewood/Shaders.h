#pragma once

#include <ion/io/ResourceHandle.h>
#include <ion/renderer/Shader.h>

namespace Assets
{
	namespace Shaders
	{
		namespace FlatColoured
		{
			extern ion::io::ResourceHandle<ion::render::Shader> vertexShader;
			extern ion::io::ResourceHandle<ion::render::Shader> pixelShader;
		}

		namespace FlatTextured
		{
			extern ion::io::ResourceHandle<ion::render::Shader> vertexShader;
			extern ion::io::ResourceHandle<ion::render::Shader> pixelShader;
		}

		namespace IndexTexture
		{
			extern ion::io::ResourceHandle<ion::render::Shader> vertexShader;
			extern ion::io::ResourceHandle<ion::render::Shader> pixelShader;

			namespace Params
			{
				extern ion::render::Shader::ParamHndl<ion::render::Texture> indexedTexture;
				extern ion::render::Shader::ParamHndl<ion::render::Texture> paletteTexture;
			}
		}
	}
}
