

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
