#pragma once

#include "Shaders.h"

namespace Assets
{
	namespace Shaders
	{
		namespace Default
		{
			ion::io::ResourceHandle<ion::render::Shader> vertexShader;
			ion::io::ResourceHandle<ion::render::Shader> pixelShader;

			namespace Params
			{
				ion::render::Shader::ParamHndl<ion::Matrix4> worldViewProjMtx;
				ion::render::Shader::ParamHndl<ion::Colour> diffuseColour;
				ion::render::Shader::ParamHndl<ion::render::Texture> texture;
			}
		}
	}
}
