# GameEngineFromZero
learn game engine, write according wenli in zhihu

## learn ComputeGraphic
just for fun
## step
runtime -> editor
1. 输入管理 -> 获取用户输入
2. 策略 -> 执行策略
3. 场景 -> 管理和更新场景
4. 渲染 -> 渲染和画面输出
5. 音频音效
6. 网络通信
7. 文件IO
8. 内存管理
9. 驱动模块 -> 根据时间/事件驱动其他模块
10. 辅助 -> debug/log
11. 应用程序 -> 抽象处理配置文件
## Notes
1. 全屏绘制，主机没有窗口管理系统，直接全屏绘制，主机是单任务系统，窗口管理有额外开销
2. 离屏渲染，云计算等， 不显示，渲染不在窗口，仅仅是buffer

### Engine flow
a1. 创建跨平台模块，在不同os/API，创建基本的上下文，（window/framebuffer/off Screen buffer)
a2. 对平台硬件能力查询和遍历，找到硬件（GPU）支持的格式，创建上下文的FrameBuffer
a3. CPU使用平台所支持的图形API创建绘图所需的各种Heap/buffer/View, 生成资源描述（RootSignature或者Descriptor）各种meta data填入，传递到GPU
a4. CPU 根据场景描述信息对vertex data/index/texture/shader等的加载，将其展开在GPU可以看到的Buffer
a5. 帧循环开始
a6. CPU 读取用户输入，更新用户可操作场景物体的位置和状态
a7. CPU 执行游戏逻辑（动画/ai），更新对应物体的位置和状态
a8. CPU 进行物体的裁剪，找出需要绘制的物体
a9. CPU 将可见物体的位置和状态翻译为constant，将constant上传到GPU可见的constant buffer
a10. CPU 生成记录GPU绘制的command buffer（command list），记录绘制指令
a11. CPU 创建Fence，以及相关的Event， 进行CPU和GPU之间的同步
a12. CPU 提交记录了绘图指令Buffer（CommandList），等待GPU绘制（Fence交互）
a13. CPU 提交绘制结果，要求显示（Flip/Present）
a14. 帧循环结束

a1-a2: 11 应用程序
a3-a4: 4 渲染

### memory manager
提高程序在CPU端的执行效率，减少系统调用，在程序初始化阶段一次申领所需的资源，进行内部分配管理
block chain style: http://allenchou.net/2013/05/memory-management-part-1-of-3-the-allocator/
 you are already using the built-in new and delete operators to manage dynamic game data. However, this is not good enough. Every allocation (new) and deallocation (delete) call has its overhead, since the default memory manager (the implementation is compiler-dependent) needs to scan the available memory to look for a fit block
 To speed up the allocation and deallocation process, we will use the new operator to pre-allocate big chunks of raw memory (big char arrays), and manage these memory chunks on our own.
 allocator can only create fix-sized pages and allocate fix-sized blocks to client code. Each allocator has a page list that keeps track of all pages created by the allocator; it also has a free list that keeps track of all blocks that can be allocated.
The lists are implemented as singly-linked lists, so each page only has a memory overhead of one extra pointer. Each block would also have a pointer, but that memory space will be shared (i.e. union-ed) with user data once the block is allocated (because the pointer is not needed when a block is allocated).

我们可以通过一个线程与Allocator之间的绑定关系，迅速地实现线程的本地堆（Thread Local Storage）。这个堆由于为某个线程所独占，所以并不需要互锁机制，从而可以大大地加速线程的执行速度。
游戏引擎设计为多线程多模块异步平行执行模式。每个模块的任务类型很不一样，执行频率也不同。比如，渲染模块需要逐帧运行，涉及到大量的大块内存使用，但是这些buffer往往生命周期很短；场景加载模块则相对来说以很长的周期运行，其数据结构可能会在内存当中保持数分钟甚至数十分钟；而AI等逻辑模块则是典型的计算模块，会涉及到大量小buffer的高频分配与释放。
游戏场景是由场景物体组成的，我们的很多模块都需要以场景物体为单位进行处理。同一个模块对于不同场景物体的处理是类似的，也就是说对于内存的访问模式是类似的。我们可以很自然地把他们组织成为一个内存管理上的兄弟关系。

### scenario manager
场景管理：
    1. 与输入输出对接，完成场景资源的加载
    2. 与内存管理模块对接，完成场景资源在内存上的展开和安排
    3. 与策略模块对接，向游戏逻辑提供可编程的场景对象和参数
    4. 与动画模块对接，向动画模块提供动画对象以及动画时间线数据
    5. 与渲染模块对接，为渲染器提供网格（mesh）材质（material）光照（lighting）
#### 场景处理
    1. 基于性能资源管理角度，场景物体数量较少，场景还需要足够复杂，真实
    2. 场景的剖析与管理，场景-> 场景物体 -> 组件， 组件标准化归纳统一
    3. 场景整体是一个地图，记录了场景当中所有不同的场景物体种类的信息，场景物体种类所对应的实例的信息
    4. 网格Mesh, 材质，碰撞盒，动画数据，脚本属性，音频，特效等称为组件，component
    5. 类型与结构：
        场景（scene） 场景物体 （object）组件（component）属性（attribute）
##### 场景图结构
    1. 场景的层叠式组织结构（node trees)
        节点或者场景物体的坐标变换（4x4 矩阵/平移/旋转/缩放）
    2. 场景几何体对象,光照对象，摄像机对象
        包含多个LOD级别的顶点数据和索引数据的网络
        蒙皮网络（骨骼，骨骼绑定，骨骼影响权重因子）
        网格的多个变形目的对象，以及动画化了的变形权重
    3. 关键帧动画以及线性/贝塞尔/TCB动画曲线
        材质与贴图（漫反射，高光，法线，切线，自发光，透明度，凹凸，位移或者PBR类型的金属度，粗糙度，AO等）

##### 场景数据结构
    1. 场景图 （由节点与节点关系组成的图（树））
        1. 节点关系
            1.依存关系 （节点之间有依赖关系需要同步加载或者卸载）
            2.包含关系 （节点之间的从属关系，源节点拥有目的节点的所有权，源节点管理目的节点的生命周期）
            3.聚集关系 （节点之间有从属关系，源节点不拥有目的节点的所有权，源节点不管理目的节点的生命周期）
            4.扩展关系 （目的节点对于源节点进行扩展，两者不是从属关系，服务和服务对象的关系）
            5.参考引用关系 （表示源节点是目的节点的一个占位符）
        2. 节点（表示场景当中的层级和位置）
            1.普通节点（起到概念上分组和链接其他节点的作用，对从属节点施加统一的影响，如坐标变换，参数动画，特效范围，一些和空间位置相关的处理，反射材质球，volume)
            2.几何体节点 （geometry）
            3.骨骼节点 （指带有骨骼的几何体对象）
            4.摄像机节点 （摄像机对象）
            5.光照节点 （光照对象）
    2. 场景对象 （与场景结构无关的对象）
        1. 场景几何体对象
            1.网格
                1.顶点
                2.索引
                3.蒙皮
                    1.骨骼
            2.变形
    3.材质对象
        1.贴图
        2.颜色
        3.参数
    4.摄像机对象
        1.参数（FOV,裁剪平面）
    5.光照对象 
        1.贴图
        2.颜色
        3.参数 （类型，亮度）
        4.传递函数
    https://en.m.wikipedia.org/wiki/Open_Game_Engine_Exchange
#### 文件IO
    1.文件跨度很大，几十K到几十G
    2.文件系统是操作系统的一部分，但是对于PS来说，根据游戏需求提供好几种文件系统，高速读取优化，存储游戏本体的分区，高速读写小文件优化，临时目录
    3.可以控制资源文件打包的格式，多个资源文件合并在一个文件当中存储，减少文件系统当中文件的数量，加快文件检索的速度，减少系统当中需要管理的文件句柄的数量，减少系统的调用次数。
    4.文件压缩，有损/无损，模型的顶点/动画的变换矩阵数据，采用无损压缩，贴图/音频视频，有损， LZMA无损，S3 tex, avc/aac 音频压缩
    5.减少对文件系统的随机访问
    6.对资源的加载实现异步流式加载和部分加载， 相对于渲染线程来说是异步的

#### 资源加载
    1. 使用多线程，创建工作线程池，对同步阻塞型的f*系列API放到工作线程当中去执行
    2. 使用异步文件IO API，线程有操作系统创建


# Cmake build commands
cmake -S ./ -B ./build -G "Ninja"
cmake --build ./build --config Release
MSVC: cmake -S ./ -B ./build -G "Visual Studio 17 2022"