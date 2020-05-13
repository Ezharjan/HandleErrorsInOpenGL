#pragma once

#include<GLFW/glfw3.h>
#include<iostream>
using namespace std;

//by Alexander

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Lesson of error handling ability in OpenGL.

//Optimization
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))
	
/////////////////////////////////////////////////////
static void GLClearError(){
	while (glGetError() != GL_NO_ERROR){	}; // Just clear the error history while there is no error.

}

//// Before seting an Assert() function and witout optimization.
//static void GLCheckError(){
//	while (GLenum error = glGetError()) // Log error info  while there is the errors.
//	{
//		cout << "[OpenGL Error] (" << error << ")" << endl;
//	}
//}

//// 1st and 2nd stage of Optimization through the C++ Assert() method
//static bool GLLogCall(){
//	while (GLenum error = glGetError()) // Log error info  while there is the errors.
//	{
//		cout << "[OpenGL Error] (" << error << ")" << endl;
//		return false;
//	}
//	return true;
//}

//// 3rd stage of Optimization
static bool GLLogCall(const char* function, const char* file, int line){
	while (GLenum error = glGetError()) // Log error info  while there is the errors.
	{
		cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << endl;
		return false;
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
