# Chapter22 应用 2D 纹理

[返回](../../README.md)

在 GLSL 中，将纹理应用于表面涉及访问纹理内存以检索与纹理坐标相关联的颜色，然后将该颜色应用于输出片段。
将颜色应用于输出片段可能包括将颜色与着色模型产生的颜色混合、直接应用颜色、在反射模型中使用颜色，或其他混合过程。
在 GLSL 中，通过采样器变量访问纹理。采样器变量是纹理单元的句柄。
它通常在着色器中声明为 uniform 变量，并在主 OpenGL 应用程序中初始化，以指向相应的纹理单元。

OpenGL 4.2 引入了不可变存储纹理。
不可变存储纹理并不是无法改变的纹理，是指一旦纹理被分配，其存储空间就不能更改。
也就是说，纹理的大小、格式和层数是固定的，但纹理的内容本身可以修改。

**注:** 不可变存储纹理是使用 `glTexStorage*` 函数分配的。`glTexImage*` 分配的纹理是可变的。

## 22.1 添加加载纹理函数

``` C++
GLuint Texture::LoadTexture(const std::string& filename)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 4);
    GLuint texture = 0;
    if (data != nullptr)
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }

    return texture;
}
```

对纹理的加载使用了 [stb](https://github.com/nothings/stb) 库。

## 22.2 绑定纹理

``` C++
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texture);
```

以上代码将纹理绑定到纹理单元 0。

## 22.3 在 Shader 中添加采样器

``` GLSL
layout (binding = 0) uniform sampler2D u_diffuse_texture;
```

采样器的关键字是 `sampler2D`，这是一个 2D 的采样器。

## 22.4 纹理渲染展示

![PBR渲染展示](./images/PBR渲染展示.gif)

[返回](../../README.md)