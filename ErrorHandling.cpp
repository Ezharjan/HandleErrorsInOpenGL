#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include"GLErrorHandle.h"
using namespace std;
/*
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
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Lesson of reading a shader programe from a file and parse it. 
struct ShaderProgramSource
{
	string VertexSource;
	string FragmentSource;
};

static ShaderProgramSource ParseShader(const string filePath){ // Return our own struct type ---- ShaderProgramSource
	
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	ifstream stream(filePath);
	ShaderType type = ShaderType::NONE;
	string line;
	stringstream ss[2]; // One for vertex shader while the other is for the fragment shader
	while (getline(stream, line))
	{
		if (line.find("#shader") != string :: npos )
		{
			if (line.find("vertex") != string::npos)
			{
				//set vertex mode
				type = ShaderType::VERTEX;
			}
			else{
				//set fragment mode
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n'; // Add line to the stream string
		}
	}
	return {ss[0].str(), ss[1].str()}; // Return two variables of string is possible only in the way of using our own struct type
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Lesson of shader begining from anywhere, such as string,etc.
//CompilerShader and CreateShader is attached in one ---- CreateShader() calls CompilerShader in its content.
static int CompilerShader(unsigned int type, const string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str(); // "source.c_str()" is just a pointer to the begining of the data
	// "source.c_str()" equals to "&source[0]"
	// Make sure the "source" is stille alive in case of its out-of-scope range
	// The difference between "source.c_str()" and "GetStirng().c_str()" is that the former is represents
	//explict memory while the latter is dynamic and may be sometimes rubbished 
	GLCall(glShaderSource(id, 1, &src, nullptr));
	glCompileShader(id);

	// Error handling of the shader programe which can show error info in the console window
	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) // Press F12 to see its value is actually 0
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length *sizeof(char));
		// char message[length] is illegal because the length is variable whilt C++ needs a const here
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex " : "fragment ") << "shader" <<   endl;
		cout << message << endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static unsigned int CreateShader(const string& vertexShader, const string& fragmentShader) { 
	unsigned int programe = glCreateProgram();  // "unsigned int" equals to "GLuint"
	unsigned int vShder = CompilerShader(GL_VERTEX_SHADER, vertexShader);//Compiler called
	unsigned int fShder = CompilerShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(programe, vShder));
	GLCall(glAttachShader(programe, fShder));
	GLCall(glLinkProgram(programe));
	GLCall(glValidateProgram(programe));

	glDeleteShader(vShder);  // Also you can use glDetachShader()
	glDeleteShader(fShder);

	return programe;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	//Initialize with glew.h is vain here bacause it should be innitialized only after the valid window context exists!
	//glewInit();

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error(Don't worry, for this is just a test!)" << std::endl;  // Got an Error printing 
		// Reason: Because the glewInit() should be after creating a valid rendering context
		//As documentation said: creat a valid OpenGL rendering context and use glewInit()
		// So put it below the valid rendering context to solve this problem
	}

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(480, 480, "Shaders in OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error Again!!!" << std::endl;  // Will not an Error printing 
	}
	std::cout << "Your current GL version is : " << glGetString(GL_VERSION) << std::endl;

	float position[] = {
		-0.5f,-0.5f,//0
		0.5f, -0.5f,//1
		0.5f,  0.5f,//2
	   -0.5f,  0.5f//3
	};

	unsigned int indecies[] = {
		0,1,2,
		0,2,3	
	}; 

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), position, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8, 0));

	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indecies, GL_STATIC_DRAW));

	//Assign the shader source which caontain two strings to a varible to store it, for there is only ShaderProgramSource 
	//can the type of two strings in one varible, we use our own struct type ----- ShaderSourcePrograme type.
	ShaderProgramSource source = ParseShader("Basics.shader"); // Set the shader file path

	cout << "Vertex Shader Code Below" << endl;
	cout << source.VertexSource << endl;
	cout << "Fragment Shader Code Below" << endl;
	cout << source.FragmentSource << endl;

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);// It atually contained compile method in it
	GLCall(glUseProgram(shader)); // Make the GPU use the program of shader and run on itself

	glBindBuffer(GL_ARRAY_BUFFER, 0);// Set the bind free


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/*Draw here*/
		//glDrawArrays(GL_TRIANGLES, 0, 6);// Draw triangles using 6 vertecies in the array buffer

		// Draw triangles using 6 indecies using 4 vertecies in the element array buffer
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));// The 4th placce should be 'nullptr' as the vertecies has been bound
		


		GLClearError();// Must clear the errors above firstly as OpenGL always makes the error list in a mess
		//GLClearError() ---- Making sure that all of the errors are coming only from one-line code above the GLCheckError() method.
		//glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr);// When the type is not unsigned, the screen would be black.
		//GLCheckError();// Check whether the error occured on the code above.    ///Before optimization!

		/*	The console window will be looped with error "1280"; 
			Convert the decimal number "1280" to the hex number and find it in the glew header file;
			The number is defined with the error name.
			
			You can see the decimal display of "1280" by debugging at the error printing line 
			and click the right mouse buttton when it occurs the pin to see its hexaddecimal display.
			Try to remain first three mumbers and then see the remianed integers when finding.

			Above Is A Flaw!(What if I don't know the error line and set many GLCheckError() method before?)
			Solution: Double click the next line in the Debug Stack mode. Then the line above that line is the error line.
			
			We can also set an Assert() function of C++ which can automatically set breakpoint and pause when it breaks.
			See more at the "Optimization" lines above.
		*/


		//// 1st stage Optimization ---- Use assert and then include LogCall
		//glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr);
		//ASSERT(GLLogCall()); // Still waiting to be optimized further!

		//// 2nd stage Optimization ---- Wrap the all the suspected line of codes in the GLCall 
		//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr)); //Without the debugger, this way is hard for debugging too!

		//// 3rd stage Optimization ---- The funtion of GLLogCall can now output many useful information in the console window
		//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr)); // Every method should be wrapped in the GL

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Below are the Notes
/*
1. Shader is just a programme that runs on your GPU.
2. Vertex shader will be called three times in drawing a tirangle while the fragment shader will be called thousands of times which depends on the size of your drawing object.
3. The fragment shader is the programme that determines which color this pixcel should be.
4. Shader also runs on a state machine.
5. In C++, the string does not need to  connect with "+", directly using "a" "a" " " is OK to make them in one string
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////