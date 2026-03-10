# Chapter49 基于深度进行细分

[返回](../../README.md)

细分着色器(tessellation shaders)最出色的特性之一，就是实现细节层次(LOD, level-of-detail)算法的便捷性。
LOD 是计算机图形学中的通用术语，指的是根据物体与观察者的距离(或其他因素)，
增加或降低物体几何结构复杂度的过程。
当物体远离相机时，由于其整体尺寸会变小，因此不需要太多几何细节就能呈现其形状；
当物体靠近相机时，它在屏幕上占据的区域会越来越大，此时需要更多几何细节才能维持理想的视觉效果(如平滑度，或避免出现其他几何失真)。

下图展示了几个茶壶的渲染效果，其曲面细分级别(tessellation levels)取决于与相机的距离。
在 OpenGL 端，每个茶壶均采用完全相同的代码渲染；
细分控制着色器(TCS, Tessellation Control Shader)会基于深度自动调整曲面细分级别。

![茶壶细分示意图](./images/茶壶细分示意图.png)

使用细分着色器(tessellation shaders)时，曲面细分级别(tessellation level)是决定物体几何结构复杂度(geometric complexity)的关键因素。
由于曲面细分级别可在细分控制着色器(tessellation control shader)中设置，因此可以轻松地根据物体与相机的距离(distance from the camera)调整曲面细分级别。

在本示例中，将根据距离，在最小级别和最大级别之间线性调整曲面细分级别(tessellation levels)。
将相机坐标(camera coordinates)中 z 坐标的绝对值作为与相机的距离(当然，这并非真实距离，但对于本示例的用途而言完全适用)。
曲面细分级别随后将基于该值计算得出。
再定义另外两个值(作为统一变量(uniform variables)): 
- u_min_depth(最小深度)
- u_max_depth(最大深度)

距离相机比 u_min_depth 更近的物体将采用最大曲面细分级别，而距离相机比 u_max_depth 更远的物体将采用最小曲面细分级别；
介于两者之间的物体，其曲面细分级别将通过线性插值(linearly interpolated)计算得出。

## 48.2 细分 3D 曲面展示

![细分曲线](./images/细分曲线.png)

[返回](../../README.md)