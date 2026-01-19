# Chapter20 模拟雾

[返回](../../README.md)

可以通过将每个片元的颜色与一个恒定的雾色混合来实现简单的雾效。
雾色的影响程度取决于片段与摄像机的距离。
可以使用距离与雾色量之间的线性关系，也可以使用非线性关系，例如指数关系。

定义线性方程式: $f = \frac{d_{\max} - |z|}{d_{\max} - d_{\min}}$

定义指数方程式: **f = e^d**

在方程中:
- dmin 是从眼睛到雾最小处的距离(无雾影响)
- dmax 是雾的颜色遮蔽场景中所有其他颜色的距离
- 变量 z 表示从眼睛到物体的距离
- f 的值是雾因子，雾因子为零表示 100% 的雾，而因子为一表示没有雾。雾通常在较远的距离看起来最浓厚，当 |z| 等于 dmax 时，雾因子最小，而当 |z| 等于 dmin 时，雾因子最大。

**注:** 由于雾效是由片段着色器应用的，因此该效果**仅**会在渲染的物体上可见。它不会出现在场景中的任何空白区域(背景)。为了使雾效更一致，应该使用与最大雾色匹配的背景颜色。

## 20.1 创建 雾效果参数

``` GLSL
uniform struct FogInfo
{
    float max_distance;
    float min_distance;
    vec3 color;
} u_fog;
```

**uniform** 变量 `u_fog` 包含定义雾范围和颜色的参数:
- `min_distance` 字段是从眼睛到雾起始点的距离
- `max_distance` 是到雾达到最大程度的点的距离
- `color` 字段是雾的颜色

## 20.2 根据距离混合雾的颜色和物体的颜色

``` GLSL
float distance_in_view = abs(position_in_view.z);
float fog_factor = (u_fog.max_distance - distance_in_view) / (u_fog.max_distance - u_fog.min_distance);
fog_factor = clamp(fog_factor, 0.0, 1.0);

vec3 shade_color = CalculateBlinnPhong(position_in_view, normalize(normal_in_view));

vec3 color = mix(u_fog.color, shade_color, fog_factor);
fragment_color = vec4(color, 1.0);
```

`distance_in_view` 变量用于存储从表面点到眼睛位置的距离。
位置的 z 坐标被用作实际距离的估计值。
`fog_factor` 变量是使用前面的公式计算的。
由于 `distance_in_view` 可能不在 `u_fog.min_distance` 和 `u_fog.max_distance` 之间，将 `fog_factor` 的值限制在 0 到 1 之间。

然后调用 `CalculateBlinnPhong` 函数来计算模型反照颜色，将结果存储在 `shade_color` 变量中。

最后，根据雾因子 `fog_factor` 的值，将 `shade_color` 和 `u_fog.color` 混合在一起，结果用作片元颜色。

## 模拟雾渲染展示

![模拟雾渲染展示](./images/模拟雾渲染展示.gif)

[返回](../../README.md)