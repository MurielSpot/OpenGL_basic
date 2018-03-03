# 参考资料
## 创建窗口
[环境配置OpenGL+GLFW+GLAD](https://learnopengl-cn.github.io/01%20Getting%20started/02%20Creating%20a%20window/)

注意安装的时候如果用的时32位的文件,那就从头到尾都用32位的,我中途某个文件用了64位的,结果报了一大堆错,说某个量没找到或没定义之类的.

还有openlg32.lib打不开的问题,教程里配置了包含目录,库目录,链接器中的输入.<br/>
但是他只是设置自己创建的include目录和lib目录.上网查到说opengl的文件放在window sdk里,所以应该要加上$(WindowsSDK_IncludePath);和$(WindowsSDK_LibraryPath_x86);<br/>
我从其它正常的文件中招了路径复制进去就能打开openlg了.<br/>
包含目录:<br/>
$(VC_IncludePath);$(WindowsSDK_IncludePath);D:\Files\ThirdPartyLibrary\Librabies\includes;$(IncludePath)<br/>
库目录<br/>
$(VC_LibraryPath_x86);$(WindowsSDK_LibraryPath_x86);$(NETFXKitsDir)Lib\um\x86;D:\Files\ThirdPartyLibrary\Librabies\libs<br/>
链接器->输入:<br/>
opengl32.lib;glfw3.lib;%(AdditionalDependencies)<br/>

## 
