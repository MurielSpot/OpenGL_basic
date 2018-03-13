/*
通过鼠标和滚轮控制视野.

问题是我还不怎么明白模型矩阵model的用处,一加上这个矩阵,图片就不翼而飞了,或者疯狂旋转,要么就是三角形切片没有图案.

所以我就在vs文件里把model给去掉了.
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    //gl_Position = projection * view * model * vec4(aPos, 1.0f);
	//gl_Position = view*model* vec4(aPos, 1.0f);
	//gl_Position =model*vec4(aPos, 1.0f);
	gl_Position=projection * view * vec4(aPos, 1.0f);
	//gl_Position=projection * view * model * vec4(aPos, 1.0f);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

下面是fs,和之前比起来没有变化.
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;
void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}

*/

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include <iostream>

#include <learnopengl/shader_m.h>

#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);


/*
我们介绍的摄像机系统是一个FPS风格的摄像机，它能够满足大多数情况需要，而且与欧拉角兼容，
但是在创建不同的摄像机系统，比如飞行模拟摄像机，时就要当心。
每个摄像机系统都有自己的优点和不足，所以确保对它们进行了详细研究。
比如，这个FPS摄像机不允许俯仰角大于90度，而且我们使用了一个固定的上向量(0, 1, 0)，这在需要考虑滚转角的时候就不能用了。
*/


/*
为了计算俯仰角和偏航角，我们需要让GLFW监听鼠标移动事件。
（和键盘输入相似）我们会用一个回调函数来完成，函数的原型如下.

这里的xpos和ypos代表当前鼠标的位置。当我们用GLFW注册了回调函数之后，鼠标一移动mouse_callback函数就会被调用.
*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

char shader_vs_path[100] = "D:/Files/VisualStudioFiles/Cosmos/vs.vs";
char shader_fs_path[100] = "D:/Files/VisualStudioFiles/Cosmos/fs.fs";
char texture_picture_path_1[100] = "D:/multimedia/pictures/xiaomai.jpeg";
char texture_picture_path_2[100] = "D:/multimedia/pictures/back.png";

//设置照相机.
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -0.1f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

/*
我们跟踪两个全局变量来计算出deltaTime值：
*/
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

/*
在处理FPS风格摄像机的鼠标输入的时候，我们必须在最终获取方向向量之前做下面这几步：
计算鼠标距上一帧的偏移量。
把偏移量添加到摄像机的俯仰角和偏航角中。
对偏航角和俯仰角进行最大和最小值的限制。
计算方向向量。

第一步是计算鼠标自上一帧的偏移量。
我们必须先在程序中储存上一帧的鼠标位置，我们把它的初始值设置为屏幕的中心（屏幕的尺寸是800x600）：
*/
float lastX = 800.0f/2, lastY = 600.0f/2;
//全局角度值,与鼠标有关.
float yaw=-90.0f, pitch=0.0f;
//判断鼠标是否是第一次进入窗口.
bool firstMouse = true;

//控制鼠标滚轮.
float fov=45.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	/*
	这里的xpos和ypos代表当前鼠标的位置。当我们用GLFW注册了回调函数之后，鼠标一移动mouse_callback函数就会被调用：
	*/
	glfwSetCursorPosCallback(window, mouse_callback);
	/*
	最后不要忘记注册鼠标滚轮的回调函数：
	*/
	glfwSetScrollCallback(window, scroll_callback);

	/*
	俯仰角是描述我们如何往上或往下看的角，可以在第一张图中看到。
	第二张图展示了偏航角，偏航角表示我们往左和往右看的程度。
	滚转角代表我们如何翻滚摄像机，通常在太空飞船的摄像机中使用。
	每个欧拉角都有一个值来表示，把三个角结合起来我们就能够计算3D空间中任何的旋转向量了。

	对于我们的摄像机系统来说，我们只关心俯仰角和偏航角，所以我们不会讨论滚转角。
	给定一个俯仰角和偏航角，我们可以把它们转换为一个代表新的方向向量的3D向量。

	偏航角和俯仰角是通过鼠标（或手柄）移动获得的，水平的移动影响偏航角，竖直的移动影响俯仰角。
	它的原理就是，储存上一帧鼠标的位置，在当前帧中我们当前计算鼠标位置与上一帧的位置相差多少。
	如果水平/竖直差别越大那么俯仰角或偏航角就改变越大，也就是摄像机需要移动更多的距离。

	首先我们要告诉GLFW，它应该隐藏光标，并捕捉(Capture)它。
	捕捉光标表示的是，如果焦点在你的程序上，光标应该停留在窗口中（除非程序失去焦点或者退出）。
	我们可以用一个简单地配置调用来完成.

	在调用这个函数之后，无论我们怎么去移动鼠标，光标都不会显示了，它也不会离开窗口。对于FPS摄像机系统来说非常完美。
	*/
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	// ------------------------------------
	Shader ourShader(shader_vs_path, shader_fs_path); // you can name your shader files however you like


													  // set up vertex data (and buffer(s)) and configure vertex attributes
													  // ------------------------------------------------------------------
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	unsigned int indices[] = {
		0, 1, 2, // first triangle
		3, 4, 5  // second triangle
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	// glBindVertexArray(0);

	// load and create a texture 
	// -------------------------
	int width, height, nrChannels;
	unsigned char *data;//用来存图像的指针.
	stbi_set_flip_vertically_on_load(true);

	unsigned int texture1, texture2;
	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps.
	data = stbi_load(texture_picture_path_1, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps.
	data = stbi_load(texture_picture_path_2, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	//此处,把激活和绑定纹理的代码放在循环外面也没什么影响.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	//这部分也可以放外面.
	glBindVertexArray(VAO);

	
	//glm::mat4 model = glm::translate(model, glm::vec3(0.0f,0.0f,-6.0f));
	////model=glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//ourShader.setMat4("model", model);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		/*
		在每一帧中我们计算出新的deltaTime以备后用。
		*/
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------背景色这部分不要放出去.
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//注意不要在画完图之后使用深度测试,不然就什么图像都没有了.

		//激活着色器.
		ourShader.use();

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);//设置好lookat矩阵之后增加一些键盘的处理,见输入处理函数.
		ourShader.setMat4("view", view);

		glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);
		
		

		//画三角形.
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed *5* cameraFront;//*5放在最后乘,会报错,可能与cameraFront是矩阵有关.
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed *5* cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	/*
	如果你现在运行代码，你会发现在窗口第一次获取焦点的时候摄像机会突然跳一下。
	这个问题产生的原因是，在你的鼠标移动进窗口的那一刻，鼠标回调函数就会被调用，
	这时候的xpos和ypos会等于鼠标刚刚进入屏幕的那个位置。
	这通常是一个距离屏幕中心很远的地方，因而产生一个很大的偏移量，所以就会跳了。
	我们可以简单的使用一个bool变量检验我们是否是第一次获取鼠标输入，
	如果是，那么我们先把鼠标的初始位置更新为xpos和ypos值，这样就能解决这个问题；
	接下来的鼠标移动就会使用刚进入的鼠标位置坐标来计算偏移量了：
	*/
	if (firstMouse) // 这个bool变量初始时是设定为true的
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
	lastX = xpos;
	lastY = ypos;

	/*
	注意我们把偏移量乘以了sensitivity（灵敏度）值。
	如果我们忽略这个值，鼠标移动就会太大了；你可以自己实验一下，找到适合自己的灵敏度值。
	*/
	float sensitivity = 0.005f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	/*
	接下来我们把偏移量加到全局变量pitch和yaw上：
	*/
	yaw += xoffset;
	pitch += yoffset;
	/*
	第三步，我们需要给摄像机添加一些限制，这样摄像机就不会发生奇怪的移动了（这样也会避免一些奇怪的问题）。
	对于俯仰角，要让用户不能看向高于89度的地方（在90度时视角会发生逆转，所以我们把89度作为极限），
	同样也不允许小于 - 89度。这样能够保证用户只能看到天空或脚下，但是不能超越这个限制。
	我们可以在值超过限制的时候将其改为极限值来实现.

	注意我们没有给偏航角设置限制，这是因为我们不希望限制用户的水平旋转。
	当然，给偏航角设置限制也很容易，如果你愿意可以自己实现。
	*/
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	/*
	第四也是最后一步，就是通过俯仰角和偏航角来计算以得到真正的方向向量：
	*/
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
	/*
	计算出来的方向向量就会包含根据鼠标移动计算出来的所有旋转了。
	由于cameraFront向量已经包含在GLM的lookAt函数中，我们这就没什么问题了。
	*/
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	/*
	当滚动鼠标滚轮的时候，yoffset值代表我们竖直滚动的大小。
	当scroll_callback函数被调用后，我们改变全局变量fov变量的内容。
	因为45.0f是默认的视野值，我们将会把缩放级别(Zoom Level)限制在1.0f到45.0f。

	我们现在在每一帧都必须把透视投影矩阵上传到GPU，但现在使用fov变量作为它的视野.
	*/
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}
