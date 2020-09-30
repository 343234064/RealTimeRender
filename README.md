
Real Time Renderer
C++ Based
Build with Visual Studio 2019 

TODO:
cover all log\check\ to all file
complete all exception handler
-> Crash reporter
  -> Launch Crash report process
    -> platform process


Fatal log按照OutputDeviceError

log system:
add traceback to fatal log (generate by throw exception)

assertion failed 增加log输出stack


map
Thread Manager
inline allocator/heap allocator


memmory check -> 验证array clear element with pointer type
memory init, new 接口改为在实际需要调用时才调用
  -> 把所有new替换third party malloc?

replace all raw pointer and std pointer (替换所有直接指针)
-> 自己的smart pointer类




整理、简化窗口类

qt window

renderer 

ui

