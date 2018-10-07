#pragma once

#include <ion/io/ResourceHandle.h>
#include <ion/renderer/Shader.h>

namespace Assets
{
	namespace Shaders
	{
		namespace Default
		{
			extern ion::io::ResourceHandle<ion::render::Shader> vertexShader;
			extern ion::io::ResourceHandle<ion::render::Shader> pixelShader;

			namespace Params
			{
				extern ion::render::Shader::ParamHndl<ion::Matrix4> worldViewProjMtx;
				extern ion::render::Shader::ParamHndl<ion::Colour> diffuseColour;
				extern ion::render::Shader::ParamHndl<ion::render::Texture> texture;
			}
		}
	}
}
