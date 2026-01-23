# Chapter32 采样器对象

采样器对象是在 OpenGL 3.3 中引入的，它提供了一种便捷的方式来为 GLSL 采样器变量指定采样参数。
指定纹理参数的传统方法是使用 `glTexParameter` 来指定，通常是在定义纹理时进行。
参数定义了关联纹理的采样状态(采样模式、包裹和夹取规则等)。
这本质上是将纹理及其采样状态合并为一个单独的对象。
当需要以多种方式从单个纹理中采样(例如，启用或禁用线性过滤)时，有两种选择，
要么修改纹理的采样状态，要么使用同一纹理的两个副本。
此外，当需要对多个纹理使用相同的一组纹理采样参数时，只能对每个纹理设置同样的参数。
使用采样器对象，可以只指定一次参数，并将其在多个纹理对象之间共享。

采样器对象将采样状态与纹理对象分开。
可以创建定义特定采样状态的采样器对象，并将其应用于多个纹理，或者将不同的采样器对象绑定到同一个纹理上。
单个采样器对象可以绑定到多个纹理，这使我们能够只定义一次特定的采样状态，并在多个纹理对象之间共享它。

采样器对象是在 OpenGL 端定义的(不是在 GLSL 中定义的)。

[返回](../../README.md)

## 31.1 创建纹理但不设置参数

``` C++
void InitTextures()
{
    GLint width = 128;
    GLint height = 128;
    GLint check_size = 4;
    std::vector<GLubyte> data(width * height * 4);
    for (int r = 0; r < height; ++r)
    {
        for (int c = 0; c < width; ++c)
        {
            GLubyte color = 0;
            if (((c / check_size) + (r / check_size)) % 2 == 0)
            {
                color = 0;
            }
            else
            {
                color = 255;
            }
            data[(r * width + c) * 4 + 0] = color;
            data[(r * width + c) * 4 + 1] = color;
            data[(r * width + c) * 4 + 2] = color;
            data[(r * width + c) * 4 + 3] = 255;
        }
    }

    glGenTextures(1, &check_board_texture);
    glBindTexture(GL_TEXTURE_2D, check_board_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, check_board_texture);
}
```

这段函数生成一个棋盘纹理，并且创建纹理后**不**设置采样参数

## 32.2 创建采样器

``` C++
void InitSamplers()
{
    glGenSamplers(2, samplers);
    GLuint linear_sampler = samplers[0];
    GLuint nearest_sampler = samplers[1];

    glSamplerParameteri(linear_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(linear_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glSamplerParameteri(nearest_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(nearest_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
```

这段函数生成了两个采样器，根据采样参数设置可以看出，一个是线性采样器，一个是最近点采样器。

## 32.3 绑定采样器

``` C++
glBindSampler(0, samplers[1]);
```

通过 `glBindSampler` 函数绑定采样器。第一个参数是绑定位序，第二个参数是被绑定的采样器。

## 32.4 使用采样器对象渲染展示

![渲染到纹理](./images/渲染到纹理.gif)

[返回](../../README.md)