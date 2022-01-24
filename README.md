# MyLinuxLearning
learning of Linux-0.11
运行环境:Ubuntu 16.04,Bochs 2.6.8

下载解压后进入到文件夹下，右键“打开终端”
make 
# 运行测试
bochs -q -f Qinix.bxrc 
# 调试（需要打开两个终端）
#第一个终端
bochs -q -f QDebug.bxrc（等待gdb连接）
#第二个终端
cd init
gdb system
(gdb 信息)
target remote localhost:1234


