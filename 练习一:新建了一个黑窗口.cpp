/*
参考源码:
https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/1.1.hello_window/hello_window.cpp
*/
/*
请确认是在包含GLFW的头文件之前包含了GLAD的头文件。
GLAD的头文件包含了正确的OpenGL头文件（例如GL/gl.h），所以需要在其它依赖于OpenGL的头文件之前包含GLAD。

请确认您的系统支持OpenGL3.3或更高版本，否则此应用有可能会崩溃或者出现不可预知的错误。
如果想要查看OpenGL版本的话，在Linux上运行glxinfo，或者在Windows上使用其它的工具（例如OpenGL Extension Viewer）。
如果你的OpenGL版本低于3.3，检查一下显卡是否支持OpenGL 3.3+（不支持的话你的显卡真的太老了），并更新你的驱动程序，有必要的话请更新显卡。
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

/*
然而，当用户改变窗口的大小的时候，视口也应该被调整。
我们可以对窗口注册一个回调函数(Callback Function)，它会在每次窗口大小被调整的时候被调用。这个回调函数的原型如下.

这个帧缓冲大小函数需要一个GLFWwindow作为它的第一个参数，以及两个整数表示窗口的新维度。
每当窗口改变大小，GLFW会调用这个函数并填充相应的参数供你处理。

我们还需要注册这个函数，告诉GLFW我们希望每当窗口调整大小的时候调用这个函数.见main.

当窗口被第一次显示的时候framebuffer_size_callback也会被调用。
对于视网膜(Retina)显示屏，width和height都会明显比原输入值更高一点。

我们还可以将我们的函数注册到其它很多的回调函数中。比如说，我们可以创建一个回调函数来处理手柄输入变化，处理错误消息等。
我们会在创建窗口之后，渲染循环初始化之前注册这些回调函数。
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
	/*
	如果你现在编译你的cpp文件会得到大量的 undefined reference (未定义的引用)错误，也就是说你并未顺利地链接GLFW库。
	*/

	//glfwInit函数来初始化GLFW.
	glfwInit();
	 //glfwWindowHint函数来配置GLFW.第一个参数代表选项的名称，我们可以从很多以GLFW_开头的枚举值中选择；第二个参数接受一个整形，用来设置这个选项的值。该函数的所有的选项以及对应的值都可以在 GLFW’s window handling 这篇文档中找到。
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/*
	http://www.glfw.org/docs/latest/window.html#window_hints
	
	Window creation hints
	There are a number of hints that can be set before the creation of a window and context. Some affect the window itself, others affect the framebuffer or context. These hints are set to their default values each time the library is initialized with glfwInit, can be set individually with glfwWindowHint and reset all at once to their defaults with glfwDefaultWindowHints.
	Note that hints need to be set before the creation of the window and context you wish to have the specified attributes.
	*/
	
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//如果使用的是Mac OS X系统，你还需要加下面这行代码glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);到你的初始化代码中这些配置才能起作用（将上面的代码解除注释）
	
	//接下来我们创建一个窗口对象，这个窗口对象存放了所有和窗口相关的数据，而且会被GLFW的其他函数频繁地用到。

	/*
	glfwCreateWindow函数需要窗口的宽和高作为它的前两个参数。
	第三个参数表示这个窗口的名称（标题），这里我们使用"LearnOpenGL"，当然你也可以使用你喜欢的名称。最后两个参数我们暂时忽略。
	这个函数将会返回一个GLFWwindow对象，我们会在其它的GLFW操作中使用到。
	*/
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ZhangGuangJie's Window d=====(￣▽￣*)b", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	/*
	创建完窗口我们就可以通知GLFW将我们窗口的上下文设置为当前线程的主上下文了。
	*/
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/*
	在之前的教程中已经提到过，GLAD是用来管理OpenGL的函数指针的，所以在调用任何OpenGL的函数之前我们需要初始化GLAD。

	我们给GLAD传入了用来加载系统相关的OpenGL函数指针地址的函数。GLFW给我们的是glfwGetProcAddress，它根据我们编译的系统定义了正确的函数。
	*/
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	/*
	我们可不希望只绘制一个图像之后我们的应用程序就立即退出并关闭窗口。
	我们希望程序在我们主动关闭它之前不断绘制图像并能够接受用户输入。
	因此，我们需要在程序中添加一个while循环，我们可以把它称之为渲染循环(Render Loop)，它能在我们让GLFW退出前一直保持运行。
	下面几行的代码就实现了一个简单的渲染循环.

	glfwWindowShouldClose函数在我们每次循环的开始前检查一次GLFW是否被要求退出，如果是的话该函数返回true然后渲染循环便结束了，之后为我们就可以关闭应用程序了。
	glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。
	glfwSwapBuffers函数会交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上。
	
	双缓冲(Double Buffer)

	应用程序使用单缓冲绘图时可能会存在图像闪烁的问题。
	这是因为生成的图像不是一下子被绘制出来的，而是按照从左到右，由上而下逐像素地绘制而成的。
	最终图像不是在瞬间显示给用户，而是通过一步一步生成的，这会导致渲染的结果很不真实。
	为了规避这些问题，我们应用双缓冲渲染窗口应用程序。
	前缓冲保存着最终输出的图像，它会在屏幕上显示；而所有的的渲染指令都会在后缓冲上绘制。
	当所有的渲染指令执行完毕后，我们交换(Swap)前缓冲和后缓冲，这样图像就立即呈显出来，之前提到的不真实感就消除了。
	*/
	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/*
	当渲染循环结束后我们需要正确释放/删除之前的分配的所有资源。
	我们可以在main函数的最后调用glfwTerminate函数来完成。
	*/
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

/*
在我们开始渲染之前还有一件重要的事情要做，我们必须告诉OpenGL渲染窗口的尺寸大小，即视口(Viewport)，这样OpenGL才只能知道怎样根据窗口大小显示数据和坐标。
我们可以通过调用glViewport函数来设置窗口的维度(Dimension)：

glViewport函数前两个参数控制窗口左下角的位置。第三个和第四个参数控制渲染窗口的宽度和高度（像素）。

我们实际上也可以将视口的维度设置为比GLFW的维度小，这样子之后所有的OpenGL渲染将会在一个更小的窗口中显示，这样子的话我们也可以将一些其它元素显示在OpenGL视口之外。

OpenGL幕后使用glViewport中定义的位置和宽高进行2D坐标的转换，将OpenGL中的位置坐标转换为你的屏幕坐标。
例如，OpenGL中的坐标(-0.5, 0.5)有可能（最终）被映射为屏幕中的坐标(200,450)。
注意，处理过的OpenGL坐标范围只为-1到1，因此我们事实上将(-1到1)范围内的坐标映射到(0, 800)和(0, 600)。
*/
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
