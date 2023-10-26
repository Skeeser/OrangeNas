<div >

# OrangeNas

<br>

<div>
    <img alt="c++" src="https://img.shields.io/badge/c++-11-%2300599C">
</div>
<div>
    <img alt="platform" src="https://img.shields.io/badge/platform-Linux%20-blueviolet">
</div>
<div>
    <img alt="license" src="https://img.shields.io/github/license/Skeeser/OrangeNas">
    <img alt="commit" src="https://img.shields.io/github/commit-activity/m/Skeeser/OrangeNas?color=%23ff69b4">
    <img alt="stars" src="https://img.shields.io/github/stars/Skeeser/OrangeNas?style=social">
</div>
<br>

[简体中文](README_ZH.md)  | [English](README_EN.md) 

计网实验，用香橙派做一个nas  

客户端和服务端均在Linux环境下运行



<br>

</div>

<br>

## 功能特性

<!-- 描述该项目的核心功能点 -->
<br>

## 文件结构
- assets => 资源文件
- doc => 存放开发相关文档

<br>

## 软件架构

<!-- 可以描述下项目的架构 -->
<br>

## 系统架构

Linux

<br>

## 快速开始

### 依赖检查


<br>

### 构建

<!-- 描述如何构建该项目 -->
<br>

### 运行

#### 服务端使用：

1. 将doc目录下的server文件夹放到服务端主机上

2. 修改server.c中的端口号（默认为8888），最大客户端数量（默认为10），文件缓冲区大小（默认为1024）；不修改则跳过这步。

3. 在server目录下，用命令行运行：

   ```shell
   make
   ./server
   ```


   即可运行服务端程序



#### 客户端使用：

1. 将doc目录下的client文件夹放到客户端主机上

2. 修改client.c中的服务端的IP（默认与服务端在同一台主机），端口号（默认为8888，要与服务端一致），文件缓冲区大小（默认为1024）；不修改则跳过这步。

3. 在client目录下，用命令行运行：

   ```shell
   make
   ./client
   ```

   即可运行客户端程序

   

### 测试
server文件夹、client文件夹都放了测试文件，可以根据程序进行测试。

<br>

## 使用指南

<!-- 描述如何使用该项目 -->
<br>

## 如何贡献
如果你碰巧看见这个项目, 想要参与开发

可以查看这个文档 [如何参与开源项目](doc/github参与开源项目流程.md)

<br>

## 关于作者
keeser

<br>

## 许可证
MPL 2.0