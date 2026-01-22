# Chapter31 将场景渲染到纹理

[返回](../../README.md)

## 31.1 创建帧缓存

``` C++
void InitFrameBuffer()
{
    glGenFramebuffers(1, &frame_buffer_obj);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_obj);

    glGenTextures(1, &render_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, render_texture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_texture, 0);

    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 512, 512);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

    GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, draw_buffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer is complete" << std::endl;
    }
    else {
        std::cout << "Framebuffer error: " << result << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
```

1. 通过 `glGenFramebuffers` 创建帧缓存对象，然后用 `glBindFramebuffer` 将帧缓存绑定到当前渲染输出。
2. 创建纹理，作为帧的颜色输出对象。
3. 通过 `glFramebufferTexture2D` 将创建的纹理绑定到帧输出中，作为颜色输出。
4. 创建深度缓存，`glGenRenderbuffers` 创建渲染缓冲区。
5. 通过 `glBindRenderbuffer` 绑定当前渲染缓冲区。
6. 通过 `glRenderbufferStorage` 设定渲染缓冲区的类型和大小。
7. 通过 `glFramebufferRenderbuffer` 将渲染缓冲区绑定到当前帧缓存，作为深度缓存。

## 31.2 将内容渲染到帧缓存

``` C++
glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_obj);
// 渲染
glFlush();
```

通过 `glBindFramebuffer` 将当前帧缓存绑定为 `frame_buffer_obj`。

## 31.3 渲染到纹理展示

![立方体贴图折射](./images/立方体贴图折射.gif)

[返回](../../README.md)