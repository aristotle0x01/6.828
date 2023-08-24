

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



**Ubuntu 14.04 VBox Guest Additions problem (shared folder)**

https://askubuntu.com/questions/655262/ubuntu-14-04-vbox-guest-additions-problem-installing

https://askubuntu.com/questions/22743/how-do-i-install-guest-additions-in-a-virtualbox-vm



**ssh vm:**

<img src="./raw/ssh_port.jpeg?raw=true" alt="ssh_port" style="zoom:50%;float: left" />

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



**user & key**

<u>jos: 123456</u>



**qemu compile**

```
// make sure qemu source is not in vbox shared folder, would complain ln error

apt-get upgrade
apt-get update

// ubuntu20
apt-get install pkg-config libsdl1.2-dev libtool-bin libglib2.0-dev libz-dev libpixman-1-dev
// utuntu14
apt-get install pkg-config libsdl1.2-dev libtool libglib2.0-dev zlib1g-dev libpixman-1-dev
./configure --disable-kvm --enable-debug --target-list="i386-softmmu x86_64-softmmu"
make && make install

// disable warning as error
config-host.mak # rm QEMU_CFLAGS: -Werror
// undefined reference to major
add "#include <sys/sysmacros.h>" to "qga/commands-posix.c"
```



## Labs

 [hw.md](hw.md)

 [lab1.md](lab1.md)

 [lab2.md](lab2.md)

 [lab3.md](lab3.md)

[lab4.md](lab4.md)

[lab5.md](lab6.md)

