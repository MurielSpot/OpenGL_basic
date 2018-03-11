/*
此处图片还是倒的,下一个练习就会正过来.
*/
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <learnopengl/shader_s.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//这里时顶点着色器,片段着色器,纹理图片的文件地址,根据需要修改.
char shader_vs_path[100] = "D:/Files/VisualStudioFiles/opengl/graphics/3.3.shader.vs";
char shader_fs_path[100] = "D:/Files/VisualStudioFiles/opengl/graphics/3.3.shader.fs";
char texture_picture_path[100] = "D:/multimedia/pictures/xiaomai.jpeg";

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

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

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader program
	// ------------------------------------
	Shader ourShader(shader_vs_path, shader_fs_path); // you can name your shader files however you like


	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	/*
	后面的这部分我们会使用glDrawElements绘制「你好，三角形」教程最后一部分的矩形。
	我们需要告知OpenGL如何采样纹理，所以我们必须使用纹理坐标更新顶点数据.

	由于我们添加了一个额外的顶点属性，我们必须告诉OpenGL我们新的顶点格式.

	这个数组不需要了,因为纹理vertices里面有纹理坐标了.
	float texCoords[] = {
		0.0f, 0.0f, // 左下角
		1.0f, 0.0f, // 右下角
		0.5f, 1.0f // 上中
	};
	*/
	float vertices[] = {
		//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
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
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	/*
	注意，我们同样需要调整前面两个顶点属性的步长参数为8 * sizeof(float)。

	接着我们需要调整顶点着色器使其能够接受顶点坐标为一个顶点属性，并把坐标传给片段着色器.
	#version 330 core
	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aColor;
	layout (location = 2) in vec2 aTexCoord;

	out vec3 ourColor;
	out vec2 TexCoord;

	void main()
	{
	gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoord = aTexCoord;
	}
	片段着色器应该接下来会把输出变量TexCoord作为输入变量。
	片段着色器也应该能访问纹理对象，但是我们怎样能把纹理对象传给片段着色器呢？
	GLSL有一个供纹理对象使用的内建数据类型，叫做采样器(Sampler)，它以纹理类型作为后缀，比如sampler1D、sampler3D，或在我们的例子中的sampler2D。
	我们可以简单声明一个uniform sampler2D把一个纹理添加到片段着色器中，稍后我们会把纹理赋值给这个uniform。

	#version 330 core
	out vec4 FragColor;

	in vec3 ourColor;
	in vec2 TexCoord;

	uniform sampler2D ourTexture;

	void main()
	{
	FragColor = texture(ourTexture, TexCoord);
	}
	我们使用GLSL内建的texture函数来采样纹理的颜色，它第一个参数是纹理采样器，第二个参数是对应的纹理坐标。
	texture函数会使用之前设置的纹理参数对相应的颜色值进行采样。
	这个片段着色器的输出就是纹理的（插值）纹理坐标上的(过滤后的)颜色。

	现在只剩下在调用glDrawElements之前绑定纹理了，它会自动把纹理赋值给片段着色器的采样器.见渲染循环.
	*/
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	// glBindVertexArray(0);

	// load and create a texture 
	// -------------------------
	/*
	stb_image.h是Sean Barrett的一个非常流行的单头文件图像加载库，它能够加载大部分流行的文件格式，并且能够很简单得整合到你的工程之中。stb_image.h可以在这里下载。下载这一个头文件，将它以stb_image.h的名字加入你的工程，并另创建一个新的C++文件，输入以下代码：
	#define STB_IMAGE_IMPLEMENTATION
	#include "stb_image.h"
	通过定义STB_IMAGE_IMPLEMENTATION，预处理器会修改头文件，让其只包含相关的函数定义源码，等于是将这个头文件变为一个 .cpp 文件了。
	现在只需要在你的程序中包含stb_image.h并编译就可以了。

	下面的教程中，我们会使用一张木箱的图片。要使用stb_image.h加载图片，我们需要使用它的stbi_load函数：
	int width, height, nrChannels;
	unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	这个函数首先接受一个图像文件的位置作为输入。
	接下来它需要三个int作为它的第二、第三和第四个参数，stb_image.h将会用图像的宽度、高度和颜色通道的个数填充这三个变量。
	我们之后生成纹理的时候会用到的图像的宽度和高度的。
	*/
	int width, height, nrChannels;
	unsigned char *data = stbi_load(texture_picture_path, &width, &height, &nrChannels, 0);

	/*
	和之前生成的OpenGL对象一样，纹理也是使用ID引用的。让我们来创建一个.

	glGenTextures函数首先需要输入生成纹理的数量，
	然后把它们储存在第二个参数的unsigned int数组中（我们的例子中只是单独的一个unsigned int），
	就像其他对象一样，我们需要绑定它，让之后任何的纹理指令都可以配置当前绑定的纹理.
	*/
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	/*
	纹理环绕方式

	纹理坐标的范围通常是从(0, 0)到(1, 1)，那如果我们把纹理坐标设置在范围之外会发生什么？
	OpenGL默认的行为是重复这个纹理图像（我们基本上忽略浮点纹理坐标的整数部分），但OpenGL提供了更多的选择：
	环绕方式 描述
	GL_REPEAT 对纹理的默认行为。重复纹理图像。
	GL_MIRRORED_REPEAT 和GL_REPEAT一样，但每次重复图片是镜像放置的。
	GL_CLAMP_TO_EDGE 纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
	GL_CLAMP_TO_BORDER 超出的坐标为用户指定的边缘颜色。

	前面提到的每个选项都可以使用glTexParameter*函数对单独的一个坐标轴设置
	（s、t（如果是使用3D纹理那么还有一个r）它们和x、y、z是等价的）:
	*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	/*
	第一个参数指定了纹理目标；我们使用的是2D纹理，因此纹理目标是GL_TEXTURE_2D。
	第二个参数需要我们指定设置的选项与应用的纹理轴。我们打算配置的是WRAP选项，并且指定S和T轴。
	最后一个参数需要我们传递一个环绕方式(Wrapping)，在这个例子中OpenGL会给当前激活的纹理设定纹理环绕方式为GL_MIRRORED_REPEAT。

	如果我们选择GL_CLAMP_TO_BORDER选项，我们还需要指定一个边缘的颜色。
	这需要使用glTexParameter函数的fv后缀形式，用GL_TEXTURE_BORDER_COLOR作为它的选项，并且传递一个float数组作为边缘的颜色值：
	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	*/

	/*
	纹理过滤
	纹理坐标不依赖于分辨率(Resolution)，它可以是任意浮点值，所以OpenGL需要知道怎样将纹理像素(Texture Pixel，也叫Texel，译注1)映射到纹理坐标。
	当你有一个很大的物体但是纹理的分辨率很低的时候这就变得很重要了。
	你可能已经猜到了，OpenGL也有对于纹理过滤(Texture Filtering)的选项。
	纹理过滤有很多个选项，但是现在我们只讨论最重要的两种：GL_NEAREST和GL_LINEAR。

	GL_NEAREST（也叫邻近过滤，Nearest Neighbor Filtering）是OpenGL默认的纹理过滤方式。
	当设置为GL_NEAREST的时候，OpenGL会选择中心点最接近纹理坐标的那个像素。

	GL_LINEAR（也叫线性过滤，(Bi)linear Filtering）它会基于纹理坐标附近的纹理像素，计算出一个插值，近似出这些纹理像素之间的颜色。
	一个纹理像素的中心距离纹理坐标越近，那么这个纹理像素的颜色对最终的样本颜色的贡献越大。

	那么这两种纹理过滤方式有怎样的视觉效果呢？
	让我们看看在一个很大的物体上应用一张低分辨率的纹理会发生什么吧（纹理被放大了，每个纹理像素都能看到）：
	GL_NEAREST产生了颗粒状的图案，我们能够清晰看到组成纹理的像素，而GL_LINEAR能够产生更平滑的图案，很难看出单个的纹理像素。
	GL_LINEAR可以产生更真实的输出，但有些开发者更喜欢8-bit风格，所以他们会用GL_NEAREST选项。

	当进行放大(Magnify)和缩小(Minify)操作的时候可以设置纹理过滤的选项，
	比如你可以在纹理被缩小的时候使用邻近过滤，被放大时使用线性过滤。
	我们需要使用glTexParameter*函数为放大和缩小指定过滤方式。这段代码看起来会和纹理环绕方式的设置很相似：
	*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*
	多级渐远纹理
	想象一下，假设我们有一个包含着上千物体的大房间，每个物体上都有纹理。
	有些物体会很远，但其纹理会拥有与近处物体同样高的分辨率。
	由于远处的物体可能只产生很少的片段，OpenGL从高分辨率纹理中为这些片段获取正确的颜色值就很困难，因为它需要对一个跨过纹理很大部分的片段只拾取一个纹理颜色。
	在小物体上这会产生不真实的感觉，更不用说对它们使用高分辨率纹理浪费内存的问题了。

	OpenGL使用一种叫做多级渐远纹理(Mipmap)的概念来解决这个问题，它简单来说就是一系列的纹理图像，后一个纹理图像是前一个的二分之一。
	多级渐远纹理背后的理念很简单：距观察者的距离超过一定的阈值，OpenGL会使用不同的多级渐远纹理，即最适合物体的距离的那个。
	由于距离远，解析度不高也不会被用户注意到。同时，多级渐远纹理另一加分之处是它的性能非常好。

	手工为每个纹理图像创建一系列多级渐远纹理很麻烦，幸好OpenGL有一个glGenerateMipmaps函数，在创建完一个纹理后调用它OpenGL就会承担接下来的所有工作了。
	后面的教程中你会看到该如何使用它。

	在渲染中切换多级渐远纹理级别(Level)时，OpenGL在两个不同级别的多级渐远纹理层之间会产生不真实的生硬边界。
	就像普通的纹理过滤一样，切换多级渐远纹理级别时你也可以在两个不同多级渐远纹理级别之间使用NEAREST和LINEAR过滤。
	为了指定不同多级渐远纹理级别之间的过滤方式，你可以使用下面四个选项中的一个代替原有的过滤方式：
	过滤方式 描述
	GL_NEAREST_MIPMAP_NEAREST 使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样
	GL_LINEAR_MIPMAP_NEAREST 使用最邻近的多级渐远纹理级别，并使用线性插值进行采样
	GL_NEAREST_MIPMAP_LINEAR 在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样
	GL_LINEAR_MIPMAP_LINEAR 在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样

	就像纹理过滤一样，我们可以使用glTexParameteri将过滤方式设置为前面四种提到的方法之一：
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	一个常见的错误是，将放大过滤的选项设置为多级渐远纹理过滤选项之一。
	这样没有任何效果，因为多级渐远纹理主要是使用在纹理被缩小的情况下的：
	纹理放大不会使用多级渐远纹理，为放大过滤设置多级渐远纹理的选项会产生一个GL_INVALID_ENUM错误代码。
	*/

	/*
	现在纹理已经绑定了，我们可以使用前面载入的图片数据生成一个纹理了。纹理可以通过glTexImage2D来生成.

	函数很长，参数也不少，所以我们一个一个地讲解：
	第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
	第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。
	第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。
	第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
	下个参数应该总是被设为0（历史遗留的问题）。
	第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。
	最后一个参数是真正的图像数据。

	当调用glTexImage2D时，当前绑定的纹理对象就会被附加上纹理图像。
	然而，目前只有基本级别(Base-level)的纹理图像被加载了，如果要使用多级渐远纹理，我们必须手动设置所有不同的图像（不断递增第二个参数）。
	或者，直接在生成纹理之后调用glGenerateMipmap。这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。
	*/
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	/*
	生成了纹理和相应的多级渐远纹理后，释放图像的内存是一个很好的习惯.
	*/
	stbi_image_free(data);

	/*
	纹理单元

	你可能会奇怪为什么sampler2D变量是个uniform，我们却不用glUniform给它赋值。
	使用glUniform1i，我们可以给纹理采样器分配一个位置值，这样的话我们能够在一个片段着色器中设置多个纹理。
	一个纹理的位置值通常称为一个纹理单元(Texture Unit)。
	一个纹理的默认纹理单元是0，它是默认的激活纹理单元，所以教程前面部分我们没有分配一个位置值。

	纹理单元的主要目的是让我们在着色器中可以使用多于一个的纹理。
	通过把纹理单元赋值给采样器，我们可以一次绑定多个纹理，只要我们首先激活对应的纹理单元。
	就像glBindTexture一样，我们可以使用glActiveTexture激活纹理单元，传入我们需要使用的纹理单元：

	激活纹理单元之后，接下来的glBindTexture函数调用会绑定这个纹理到当前激活的纹理单元，纹理单元GL_TEXTURE0默认总是被激活，
	所以我们在前面的例子里当我们使用glBindTexture的时候，无需激活任何纹理单元。
	OpenGL至少保证有16个纹理单元供你使用，也就是说你可以激活从GL_TEXTURE0到GL_TEXTRUE15。
	它们都是按顺序定义的，所以我们也可以通过GL_TEXTURE0 + 8的方式获得GL_TEXTURE8，这在当我们需要循环一些纹理单元的时候会很有用。

	glActiveTexture(GL_TEXTURE0); // 在绑定纹理之前先激活纹理单元
	glBindTexture(GL_TEXTURE_2D, texture);

	我们仍然需要编辑片段着色器来接收另一个采样器。这应该相对来说非常直接了：
	#version 330 core
	...

	uniform sampler2D texture1;
	uniform sampler2D texture2;

	void main()
	{
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
	}

	最终输出颜色现在是两个纹理的结合。
	GLSL内建的mix函数需要接受两个值作为参数，并对它们根据第三个参数进行线性插值。
	如果第三个值是0.0，它会返回第一个输入；如果是1.0，会返回第二个输入值。
	0.2会返回80%的第一个输入颜色和20%的第二个输入颜色，即返回两个纹理的混合色。

	我们现在需要载入并创建另一个纹理；你应该对这些步骤很熟悉了。
	记得创建另一个纹理对象，载入图片，使用glTexImage2D生成最终纹理。

	为了使用第二个纹理（以及第一个），我们必须改变一点渲染流程，先绑定两个纹理到对应的纹理单元，
	然后定义哪个uniform采样器对应哪个纹理单元：
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	我们还要通过使用glUniform1i设置每个采样器的方式告诉OpenGL每个着色器采样器属于哪个纹理单元。
	我们只需要设置一次即可，所以这个会放在渲染循环的前面：
	ourShader.use(); // 别忘记在激活着色器前先设置uniform！
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // 手动设置
	ourShader.setInt("texture2", 1); // 或者使用着色器类设置

	while(...)
	{
	[...]
	}

	你可能注意到纹理上下颠倒了！这是因为OpenGL要求y轴0.0坐标是在图片的底部的，但是图片的y轴0.0坐标通常在顶部。
	很幸运，stb_image.h能够在图像加载时帮助我们翻转y轴，只需要在加载任何图像前加入以下语句即可：
	stbi_set_flip_vertically_on_load(true);
	*/

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		/*
		现在只剩下在调用glDrawElements之前绑定纹理了，它会自动把纹理赋值给片段着色器的采样器.

		如果你的纹理代码不能正常工作或者显示是全黑，请继续阅读，并一直跟进我们的代码到最后的例子，它是应该能够工作的。
		在一些驱动中，必须要对每个采样器uniform都附加上纹理单元才可以，这个会在下面介绍。

		我们还可以把得到的纹理颜色与顶点颜色混合，来获得更有趣的效果。我们只需把纹理颜色与顶点颜色在片段着色器中相乘来混合二者的颜色：
		FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
		最终的效果应该是顶点颜色和纹理颜色的混合色.
		*/
		glBindTexture(GL_TEXTURE_2D, texture);

		ourShader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
