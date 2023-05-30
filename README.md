

# 6.828

6.828 based on https://pdos.csail.mit.edu/6.828/2018/schedule.html



## env setup

**basic dependencies:**

- Virtualbox Version 7.0.6 r155176 (Qt5.15.2)
- ubuntu-16.04.4-desktop-i386.iso
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



**disable sleep**

[How do I disable my system from going to sleep?](https://askubuntu.com/questions/47311/how-do-i-disable-my-system-from-going-to-sleep)

```
sudo systemctl mask sleep.target suspend.target hibernate.target hybrid-sleep.target

sudo vi /etc/default/acpi-support # and then set SUSPEND_METHODS="none"
sudo /etc/init.d/acpid restart
```

[Ubuntu 16.04 - GUI freezes on login start page](https://unix.stackexchange.com/questions/368748/ubuntu-16-04-gui-freezes-on-login-start-page)

```
apt-get update 
apt-get install xserver-xorg-input-all
apt-get install ubuntu-desktop
apt-get install ubuntu-minimal
apt-get install xorg xserver-xorg
apt-get install xserver-xorg-input-evdev    //I think this packet was the problem
apt-get install xserver-xorg-video-vmware
/etc/init.d/lightdm restart
reboot
```



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



## Lab1

### Software Setup

```
git clone https://pdos.csail.mit.edu/6.828/2018/jos.git lab
with: SSL certificate problem: certificate has expired error => 

git -c http.sslVerify=false clone https://pdos.csail.mit.edu/6.828/2018/jos.git lab
```



**Exercise 7**

the first instruction failed would be `jmp *%eax `  (entry.S line 68: jmp 0xf010002c)

<img src="./raw/lab1-e7-1.jpg?raw=true" alt="ssh_port" style="zoom:50%;float: left" />

<img src="./raw/lab1-e7-2.jpg?raw=true" alt="ssh_port" style="zoom:50%;float: left" />



**Exercise 9**

in entry.S line 77: `movl	$(bootstacktop),%esp`   =>  `mov    $0xf0110000,%esp`

Stack space is 0x10000 == 64k. Sit on the high end of memory.



**Exercise 10**

Three 32-bit words would each recursive nesting level of `test_backtrace` push on the stack: parameter, ret address, saved value of calling ebp.

<img src="./raw/lab1-e10-1.jpg?raw=true" alt="call convention" style="zoom:50%;float: left" />

from: [The 32 bit x86 C Calling Convention](https://aaronbloomfield.github.io/pdr/book/x86-32bit-ccc-chapter.pdf)



With the image above, it is easy to understand: `add    $0x10,%esp`

```
	// Test the stack backtrace function (lab 1 only)
	test_backtrace(5);
f01000c8:	c7 04 24 05 00 00 00 	movl   $0x5,(%esp)
f01000cf:	e8 6c ff ff ff       	call   f0100040 <test_backtrace>
f01000d4:	83 c4 10             	add    $0x10,%esp
```

