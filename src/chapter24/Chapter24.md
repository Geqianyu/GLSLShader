# Chapter24 使用 Alpha 贴图舍弃像素

[返回](../../README.md)

为了创建一个有孔的物体的效果，可以使用带有适当 alpha 通道的纹理，其中包含物体透明部分的信息。
这需要我们将深度缓冲区设置为只读，并且从后向前渲染所有的多边形，以避免混合问题。
需要根据相机位置对多边形进行排序，然后以正确的顺序渲染。
使用 GLSL 着色器，可以通过使用 discard 关键字来完全丢弃片段，当纹理贴图的 alpha 值低于某个值时，就不必考虑这些片段。
通过完全丢弃片段，就不需要修改深度缓冲区，因为被丢弃的片段根本不会与深度缓冲区进行比较。
这样也不需要对多边形进行深度排序，因为没有混合问题。

## 24.1 在 GLSL 中判断片元是否丢弃

``` GLSL
vec4 alpha = texture(u_alpha_texture, uv_in_view);
if (alpha.a < 0.15)
{
    discard;
}
else
{
    if (gl_FrontFacing)
    {
        fragment_color = vec4(CalculateBlinnPhong(position_in_view, normalize(normal_in_view)), 1.0);
    }
    else
    {
        fragment_color = vec4(CalculateBlinnPhong(position_in_view, -normalize(normal_in_view)), 1.0);
    }
}
```

根据 u_alpha_texture 采样获得纹理值，根据 **.a** 通道是否小于 0.15 判断是否丢弃该片元。

## 24.2 应用 Alpha 通道渲染展示

![PBR渲染展示](./images/PBR渲染展示.gif)

[返回](../../README.md)