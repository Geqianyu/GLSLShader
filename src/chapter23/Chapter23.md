# Chapter23 应用多张纹理

[返回](../../README.md)

将多种纹理应用于一个表面可以用来创造各种效果。
底层纹理可能表示干净的表面，而第二层则可以提供额外的细节，例如阴影、瑕疵、粗糙或损坏。
在许多游戏中，光照贴图会作为额外的纹理层提供光照信息，有效地生成阴影和明暗效果，而无需显式计算反射模型。
这类纹理有时被称为预烘焙光照。
本章节将演示这种多重纹理技术，通过应用两层纹理来实现。

## 23.1 添加多张纹理

``` C++
brick_texture = glsl_shader::Texture::LoadTexture("../../assets/textures/brick1.jpg");
moss_texture = glsl_shader::Texture::LoadTexture("../../assets/textures/moss.png");

glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, brick_texture);

glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, moss_texture);
```

这里创建了2张纹理，分别绑定到0和1。

## 23.2 在 Shader 中添加多张纹理的 uniform

``` GLSL
layout (binding = 0) uniform sampler2D u_brick_texture;
layout (binding = 1) uniform sampler2D u_moss_texture;
```

这里在片元着色器中添加了2个纹理采样器，分别绑定到0和1。

## 23.3 混合采样结果

``` GLSL
vec4 brick_color = texture(u_brick_texture, uv_in_view);
vec4 moss_color = texture(u_moss_texture, uv_in_view);
vec3 color = mix(brick_color.rgb, moss_color.rgb, moss_color.a);
```

这里将2张纹理的采样结果根据 moss 的 alpha 通道混合，实现两张纹理颜色叠加的效果。

## 23.4 多张纹理渲染展示

![PBR渲染展示](./images/PBR渲染展示.gif)

[返回](../../README.md)