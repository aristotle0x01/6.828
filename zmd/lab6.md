

## preparation reading

[Turtles on the wire: understanding how the OS uses the modern NIC](https://www.tritondatacenter.com/blog/virtualizing-nics) :  <font color="red">how os and NIC interact</font>

[Illustrated Guide to Monitoring and Tuning the Linux Networking Stack: Receiving Data](https://blog.packagecloud.io/illustrated-guide-monitoring-tuning-linux-networking-stack-receiving-data/)

<img src="../raw/lab6-nic-receive.jpeg?raw=true" alt="nic receive" style="zoom:20%; float:left" />

[qemu-networking](https://wiki.qemu.org/Documentation/Networking)

<img src="../raw/lab6-qemu-networking.png?raw=true" alt="nic receive" style="zoom:50%; float:left" />

[Linux networking stack from the ground up, part 1](https://www.privateinternetaccess.com/blog/linux-networking-stack-from-the-ground-up-part-1/) : on PCI init

Linux Network Receive Stack Monitoring and Tuning Deep Dive (Patrick Ladd, pdf): 

`lspci -nn`

<img src="../raw/lab6-lspci.jpg?raw=true" alt="nic receive" style="zoom:50%; float:left" />



## lab key points

<img src="../raw/lab6-ns.png?raw=true" alt="ns stack" style="zoom:90%; float:left" />

------

<img src="../raw/lab6-e1000-arch.png?raw=true" alt="arch e1000" style="zoom:50%; float:left" />

- PCI config & enable

  <img src="../raw/lab6-pci-config.png?raw=true" alt="registers" style="zoom:45%; float:left" />

  ------

  <img src="../raw/lab6-pci-regs.png?raw=true" alt="pci registers" style="zoom:45%; float:left" />

  <img src="../raw/lab6-bars.png?raw=true" alt="bars" style="zoom:45%; float:left" />

  ------

  <img src="../raw/lab6-registers.png?raw=true" alt="registers" style="zoom:45%; float:left" />

- memory-mapped I/O

  <img src="../raw/lab6-nic-host-mapping.png?raw=true" alt="registers" style="zoom:35%; float:left" />

- direct memory access

- transmit descriptor ring buffer (legacy mode)

  ```
  63            48 47   40 39   32 31   24 23   16 15             0
    +---------------------------------------------------------------+
    |                         Buffer address                        |
    +---------------+-------+-------+-------+-------+---------------+
    |    Special    |  CSS  | Status|  Cmd  |  CSO  |    Length     |
    +---------------+-------+-------+-------+-------+---------------+
  ```

  <img src="../raw/lab6-tx-desc-cmd.png?raw=true" alt="tx cmd" style="zoom:45%; float:left" />

  ------

  <img src="../raw/lab6-tx-desc-status.png?raw=true" alt="tx status" style="zoom:45%; float:left" />

  ------

  <img src="../raw/lab6-tx-ring.png?raw=true" alt="tx ring" style="zoom:45%; float:left" />


- receive descriptor ring buffer


  - Receive Address Registers (RAL and RAH)
  - <font color="red">attention to <u>RDT</u></font>

  <img src="../raw/lab6-rx-ring.png?raw=true" alt="rx ring" style="zoom:45%; float:left" />

















------

  <img src="../raw/lab6-rx-desc.png?raw=true" alt="rx desc" style="zoom:45%; float:left" />





------

  <img src="../raw/lab6-rx-desc-status.png?raw=true" alt="rx status" style="zoom:45%; float:left" />







- <font color="red">interrupt & poll</font>

  <img src="../raw/lab6-rx-intr.png?raw=true" alt="rx status" style="zoom:40%; float:left" />

- lwip tcp/ip stack; web server



## knowledge

- ethtool
