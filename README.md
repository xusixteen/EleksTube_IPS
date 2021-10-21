# EleksTube_IPS
## 时间线变动器

![EleksTube](https://github.com/xusixteen/EleksTube_IPS/blob/main/EleksTube_IPS.JPG)

### 代码上参考了https://github.com/xusixteen/EleksTubeIPSHack
### 网络同步时间参考了https://github.com/ClimbSnail/HoloCubic_AIO
### 硬件设计与原版EleksTube不兼容，主要表现在屏幕小板的13pin金手指改成了12pin的板间连接器



### 现存不足
#### 1、ESP-WROOM-32D自动下载电路存在问题，改为外部按键控制GPIO0与EN复位。
#### 2、EN引脚需外加10K上拉电阻和0.1uF对地电容。
#### 3、RX与TX接反，R8与R3需要飞线交换。
#### 4、main与midextend板间1.27mm排线需采用反向12p。
#### 5、按键封装需旋转90°。


### TODO
#### 1、整合IMU功能。
#### 2、增加SD卡或利用内部FLASH，增加图案。
#### 3、增加有线同步时间功能。上位机运行脚本获取网络时间，通过串口发送至ESP32。
