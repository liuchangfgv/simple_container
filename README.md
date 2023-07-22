# simple_container
一个可以运行systemd的简易容器
## How to use
```
wget https://mirrors.tuna.tsinghua.edu.cn/lxc-images/images/debian/bullseye/amd64/default/20230718_05%3A25/rootfs.tar.xz
#地址会变动，请自行寻找
xz -d rootfs.tar.xz
tar -xvf rootfs.tar -C ./root
gcc main.c -o run
sudo ./run
```

## TODO
- 网络支持
