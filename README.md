

## 6.828

6.828 based on https://pdos.csail.mit.edu/6.828/2018/schedule.html



## env setup

### **basic dependencies**

- Virtualbox Version Version 6.1.44 r156814 (Qt5.6.3)
- ubuntu-16.04.6-desktop-i386.iso [Ubuntu mirror "Tsinghua University"](https://launchpad.net/ubuntu/+mirror/mirrors.tuna.tsinghua.edu.cn-release)
- mac host

```
uname -r
cat /etc/lsb-release
```



### **Guest Additions (shared folder issue)**

https://askubuntu.com/questions/655262/ubuntu-14-04-vbox-guest-additions-problem-installing

https://askubuntu.com/questions/22743/how-do-i-install-guest-additions-in-a-virtualbox-vm



### **ssh**

<img src="./raw/ssh_port.jpeg?raw=true" alt="ssh_port" style="zoom:35%;float: left" />

[How to enable ssh root access ubuntu 16.04 [duplicate\]](https://askubuntu.com/questions/951581/how-to-enable-ssh-root-access-ubuntu-16-04)

```
********on guest********
// change root passwd
passwd root

// install openssh-server
apt-get install openssh-server
service ssh enable

// enable root access
sudo sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
servie ssh restart

********on host********
ssh-copy-id -p 2222 -i ~/.ssh/id_rsa.pub root@localhost
ssh -p '2222' 'root@localhost'
```



### **user & key**

<u>jos: 123456</u>



### 6.828 compile & dev env

[Tools Used in 6.828](https://pdos.csail.mit.edu/6.828/2018/tools.html)

#### qemu compile

```
// make sure qemu source is not in vbox shared folder, would complain ln error

apt-get upgrade
apt-get update

apt-get install pkg-config libsdl1.2-dev libtool-bin libglib2.0-dev libz-dev libpixman-1-dev
./configure --disable-kvm --enable-debug --target-list="i386-softmmu x86_64-softmmu"
make && make install

// disable warning as error
config-host.mak # rm QEMU_CFLAGS: -Werror
// undefined reference to major
add "#include <sys/sysmacros.h>" to "qga/commands-posix.c"
```



## Labs

**[hw.md](zmd/hw.md)**

<img src="./raw/hw-cpu-alram.png?raw=true" alt="cpu alarm" style="zoom:35%;float: left" />

**[lab1.md](zmd/lab1.md)**

<img src="./raw/lab1-booting_process.png?raw=true" alt="booting" style="zoom:35%;float: left" />

**[lab2.md](zmd/lab2.md)**

<img src="./raw/lab2-14.png?raw=true" alt="jos paging" style="zoom:100%;float: left" />

**[lab3.md](zmd/lab3.md)**

<img src="./raw/lab3-kernel-space.png?raw=true" alt="u/k space" style="zoom:40%;float: left" />

**[lab4.md](zmd/lab4.md)**

<img src="./raw/lab4-stack-overview.png?raw=true" alt="jos stack view" style="zoom:100%; float:left" />

**[lab5.md](zmd/lab5.md)**

<img src="./raw/lab5-jos-file-ipc.png?raw=true" alt="jos file system" style="zoom:70%; float:left" />

**[lab6.md](zmd/lab6.md)**

<img src="./raw/lab6-nic-host-mapping.png?raw=true" alt="mapping" style="zoom:35%; float:left" />

