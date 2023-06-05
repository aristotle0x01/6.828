

# 6.828

6.828 based on https://pdos.csail.mit.edu/6.828/2018/schedule.html



## env setup

**basic dependencies:**

- Virtualbox Version Version 6.1.44 r156814 (Qt5.6.3)
- ubuntu-20.04.5-desktop-amd64.iso
- mac as host

```
uname -r
cat /etc/lsb-release
```



**ssh vm:**

<img src="./raw/ssh_port.jpeg?raw=true" alt="ssh_port" style="zoom:50%;float: left" />

[How to enable ssh root access ubuntu 16.04 [duplicate\]](https://askubuntu.com/questions/951581/how-to-enable-ssh-root-access-ubuntu-16-04)

```
// change root passwd
passwd root

// install openssh-server
apt-get install openssh-server
service ssh enable

// enable root access
sudo sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
servie ssh restart

ssh-copy-id -p 2222 -i ~/.ssh/id_rsa.pub root@localhost
ssh -p '2222' 'root@127.0.0.1'
```



**user & key**

<u>jos: 123456</u>



**qemu compile**

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

 [lab1.md](lab1.md)

 [lab2.md](lab2.md)
