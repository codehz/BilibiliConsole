# BilibiliConsole
Bilibili的Linux客户端

由知乎用户[@躺糖](https://www.zhihu.com/people/jixian-li)的[回答](https://www.zhihu.com/question/49137033/answer/114490277)启发，准备写一个基于FrameBuffer的BiliBili视频播放器。

功能设想
----

1. 播放视频(直接通过参数指定av号和分P来播放)，优先级：非常高
2. 显示弹幕(可配置)，优先级：高
3. 账户登陆，优先级：中
4. 发送弹幕，优先级：低
5. 显示视频列表，优先级：非常低
6. 插件系统，优先级：非常低

暂时就想到这么多了，还有其他想法的赶紧提issue

基础设定
----
开发语言：**C++**

播放视频采用的库：**LibVLC**

*其他设定待定*
