# simple_container
一个可以运行systemd的简易容器
还未完成，请勿使用！！

## How to use
```
wget https://mirrors.tuna.tsinghua.edu.cn/lxc-images/images/debian/bullseye/amd64/default/20230718_05%3A25/rootfs.tar.xz
#地址会变动，请自行寻找
xz -d rootfs.tar.xz
tar -xvf rootfs.tar -C ./root
make -j$(nproc)
sudo ./run
```

## 进度
- [x] clone创建新名字空间
- [x] 挂载容器内的相关目录
- [ ] cgroups限制容器资源
- [ ] seccomp限制容器功能
- [ ] (optinal)使用fuse创建一个fake proc

## Note
程序必须在root权限下运行，以后可能考虑加入user namespace,从而可以非root运行