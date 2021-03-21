/////////////////////////////////////////////
//Shader.cpp
//Copyright(c) 2019 Luta Vlad
//https://github.com/meemknight/OpenGLEngine
/////////////////////////////////////////////
#include "Shader.h"
#include "tools.h"
#include <cstdio>
#include <iostream>

ShaderProgram::ShaderProgram()
{

}

ShaderProgram::ShaderProgram(const VertexShader & vs, const FragmentShader & fs) :
	vs(vs), fs(fs)
{
	compileProgram();
}

//todo delete parts of the program
void ShaderProgram::compileProgram()
{
	id = glCreateProgram();
	glAttachShader(id, vs.id);
	glAttachShader(id, fs.id);
	glLinkProgram(id);

	int info;
	glGetProgramiv(id, GL_LINK_STATUS, &info);
	if (info != GL_TRUE)
	{
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &info);
		char *p = new char[info];

		glGetProgramInfoLog(id, info, 0, p);
		elog("Error compiling Shader:", p);
		delete[] p;
	}

	glValidateProgram(id);



}

void ShaderProgram::bind()
{
	glUseProgram(id);
}

void ShaderProgram::unBind()
{
	glUseProgram(0);
}

void ShaderProgram::deleteProgram()
{
	glDeleteProgram(id);
	id = 0;
}
