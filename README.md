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
# Cmake build commands
cmake -S ./ -B ./build -G "Ninja"
cmake --build ./build --config Release