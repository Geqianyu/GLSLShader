# Chapter18 模拟聚光灯

[返回](../../README.md)

固定功能管线能够将光源定义为聚光灯。此时，光源只在一个锥形范围内发光，其顶点位于光源位置。
此外，光线会衰减，其在锥轴上达到最大值，并向外缘逐渐减弱。
这使我们能够创建具有类似真实聚光灯视觉效果的光源。

![聚光灯示意图](./images/聚光灯示意图.PNG)

聚光灯的锥形由聚光灯方向(**d**)、截止角(**c**)和位置(**P**)定义。
聚光灯的强度被认为在锥形轴线上最强，并且随着向边缘移动而减弱。

## 18.1 修改光源结构体

spot_light.fs.glsl
``` GLSL
uniform struct LightInfo
{
    vec3 position_in_view;
    vec3 La;
    vec3 L;
    vec3 direction;
    float exponent;
    float cut_off;
} u_light;
```

这个结构定义了聚光灯的所有配置选项。
- **L** 字段表示聚光灯的强度(漫反射和高光)。
- **La** 表示环境光强度。
- **direction** 字段包含聚光灯指向的方向，定义了聚光灯光锥的中心轴。
- **exponent** 字段定义了在计算聚光灯角度衰减时使用的指数。
- **cut_off** 字段定义了聚光灯光锥中心轴与外缘之间的角度。

## 18.2 计算聚光灯光照

``` GLSL
vec3 CalculateBlinnPhongModel(vec3 position, vec3 normal)
{
    vec3 ambient_color = u_light.La * u_material.Ka;

    vec3 diffuse_color = vec3(0.0);
    vec3 specular_color = vec3(0.0);
    vec3 s = normalize(u_light.position_in_view - position);
    float cos_angle = dot(-s, normalize(u_light.direction));
    float angle = acos(cos_angle);
    float spot_scale = 0.0;
    if(angle >= 0.0 && angle < u_light.cut_off)
    {
        spot_scale = pow(cos_angle, u_light.exponent);
        float s_dot_n = max(dot(s, normal), 0.0);
        diffuse_color = u_material.Kd * s_dot_n;
        if(s_dot_n > 0.0)
        {
            vec3 v = normalize(-position.xyz);
            vec3 h = normalize(v + s);
            specular_color = u_material.Ks * pow(max(dot(h, normal), 0.0), u_material.shininess);
        }
    }

    return ambient_color + spot_scale * u_light.L * (diffuse_color + specular_color);
}
```

`CalculateBlinnPhongModel` 函数计算 Blinn-Phong 反射模型，使用聚光灯作为光源。
先计算环境光分量并将其存储在 `ambient_color` 变量中。
然后计算从表面位置到聚光灯位置(**s**)的向量。
接下来，计算从聚光灯到表面点的方向(**-s**)与聚光灯方向的点积，并将结果存储在 `cos_angle` 中。
然后计算它们之间的角度并存储在变量 `angle` 中。
变量 `spot_scale` 将用于缩放聚光灯漫反射/高光强度的值，初始设置为零。

然后将角度变量的值与 `u_light.cut_off` 变量的值进行比较。
如果角度大于零且小于 `u_light.cut_off`，那么表面点就在聚光灯的光锥内。
否则，表面点只接收环境光。

如果角度小于 `u_light.cut_off`，我们将 **-s** 和 `u_light.direction` 的点积的 `u_light.exponent` 幂次存储在变量 `spot_scale` 中。
`spot_scale` 的值用于缩放光的强度，使光在圆锥中心最强，并在向边缘移动时逐渐减弱。
最后，Blinn-Phong 反射模型按常规计算。

[返回](../../README.md)