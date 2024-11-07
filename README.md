# 学呗，还能咋办？
从这个视频开始：[How Real Time Computer Graphics and Rasterization work - YouTube](https://www.youtube.com/watch?v=brDJVEPOeY8)，理解什么是**渲染管线**。
如果这个视频完全看懂了，那么可以开始学习**图形API**。如果挺疑惑的，看看[Games101](https://www.bilibili.com/video/BV1X7411F744?p=1)前十节的内容，作业可以不做。

什么是图形API？图形API就是让你可以用编程语言（C/C++）建立渲染管线的魔法工具。
图形API主要考虑OpenGL和Vulkan。


OpenGL，简单（相比较而言），有历史包袱，代码量少（相比较而言），老而且停止维护了。现代OpenGL也不够现代。
Vulkan，很复杂，很难。真的很复杂，真的挺难的。确实难。真的。很现代。

如果你可以忍受孤独，有比较强的心理素质，可以忍受先学二三十个概念，然后默默抄一千多行代码，就只画一个三角形。就直接学Vulkan吧。相信自己，我们欢迎学习vulkan，并一起讨论。写代码和一起讨论都是非常有效的学习途径。

从OpenGL开始也挺好的。从这里学习：[主页 - LearnOpenGL CN (learnopengl-cn.github.io)](https://learnopengl-cn.github.io/)。
学习下图红框框起来的内容。其中最后一块帧缓冲是非常重要的。学完这些内容，就可以学习Vulkan了😁，还是得学Vulkan😇。


![image.png](assets/images/image000.png#averageHue=%23abaaaa&clientId=uad4dbfc0-4d7f-4&from=paste&height=313&id=ue0049d62&originHeight=681&originWidth=987&originalType=binary&ratio=1.75&rotation=0&showTitle=false&size=74778&status=done&style=none&taskId=u4d1f72cd-0f96-44dd-b68d-82857756375&title=&width=454.0000305175781)


---

## Vulkan


#### 为什么学习 Vulkan？

1. **就业**：要想找图形引擎、游戏、元宇宙相关的工作，要么非常熟悉 Vulkan，要么非常熟悉商业引擎。这两者的关系是发动机和汽车的关系，看你想研究发动机，还是学飙车，都不好学。

2. **组里用**：学习 Vulkan 会让你对现代图形引擎有一个更深入的理解，不过以目前的情况来看，我们组里做的事情好像不太摩登，很复古。但组里干活总是需要熟悉一个图形 API 的，现在学 OpenGL 多多少少有点49年入国军的意思。

3. **编程的事其实是一通百通**：深入学习 Vulkan，你几乎可以涉及编程领域的所有高级话题：内存管理、资源管理、多线程与同步、缓存优化、异步编程、跨平台、并行计算和分布式。而且你可以和太阳系最炙手可热的硬件 GPU 深入打交道。

还是要提醒一下，Vulkan 的学习曲线陡峭、开发和调试的难度高，容易消耗大量时间，也容易让人感到沮丧。

---

#### 我的学习路径（去除了一些可能的弯路）

1. **官方资源**：首先看看官方的学习页面 [Learn | Vulkan | Cross platform 3D Graphics](https://www.vulkan.org/learn)，里面有不少很好的资源。比如 [How to Learn Vulkan – Jeremy's Blog](https://www.jeremyong.com/c++/vulkan/graphics/rendering/2018/03/26/how-to-learn-vulkan/) ，可以先看看这个。

2. **绘制三角形**：使用 Vulkan 绘制一个简单的三角形需要大量的代码。不要把画三角形当成很简单的事。GPU 和显示器都是很复杂的硬件，你使用一千多行代码，就可以把数据层层传递变成一个颜色渐变的三角形正确地显示在屏幕上，这是魔法！

3. **学习核心概念**：继续跟随 [Khronos Vulkan® Tutorial :: Vulkan Documentation Project](https://docs.vulkan.org/tutorial/latest/00_Introduction.html) 了解 Vulkan 的核心概念。概念不理解没关系，一是可以多问 GPT，二是写代码的过程中会反复用到这些概念，慢慢就熟悉了。

4. **代码示例**：在 [SaschaWillems/Vulkan](https://github.com/SaschaWillems/Vulkan) 中寻找自己感兴趣的示例或概念，自己实现一遍（抄一遍），然后更改代码看看会发生什么。我看的最多的是两个代码库，一个是上面提到的 SaschaWillems 的库，另一个是 [KhronosGroup/Vulkan-Samples](https://github.com/KhronosGroup/Vulkan-Samples)。

   - SaschaWillems 的示例更独立，有一个较小的封装被所有实例共用。
   - KhronosGroup 的示例分成三类：api、performance和extensions。api 示例和前者类似，展示 Vulkan 的用法；performance 示例注重性能，有较重的封装，阅读难度大；extensions 示例顾名思义展示一些有用的 Vulkan 扩展用法，如光线追踪等。

---

#### 整体思路

通过代码——理解概念，思考完成一件事（比如使用纹理）需要哪些步骤，需要改动/添加哪些部分，其中涉及了哪些概念。不必过于纠结细节，能跑、没有验证层错误就行，慢慢积少成多。

---

先写到这里

