# GenericApp_Clean (CC2530DB)

## 目标
这是从 Z-Stack 3.0.2 的 `GenericApp` 提取出来的精简可编译工程。
保留应用层源码与 IAR 工程，底层 `Components/Tools/Libraries/ZMain` 通过外部路径引用。

## 工程位置
- Workspace: `D:\nanyun\ZigBee_通用工程\Project\GenericApp_Clean\CC2530DB\GenericApp.eww`
- Project: `D:\nanyun\ZigBee_通用工程\Project\GenericApp_Clean\CC2530DB\GenericApp.ewp`

## 已做处理
1. 清理构建产物，仅保留空的输出目录：
   - `CoordinatorEB\Exe|Obj|List`
   - `RouterEB\Exe|Obj|List`
2. 保留本地应用源码目录：
   - `CC2530DB\...`（局部驱动文件）
   - `Source\...`（GenericApp 应用层）
3. 将外部依赖路径重定向到当前安装路径：
   - `D:\Develop\Texas Instruments\Z-Stack 3.0.2\Components`
   - `D:\Develop\Texas Instruments\Z-Stack 3.0.2\Projects\zstack\Tools`
   - `D:\Develop\Texas Instruments\Z-Stack 3.0.2\Projects\zstack\Libraries`
   - `D:\Develop\Texas Instruments\Z-Stack 3.0.2\Projects\zstack\ZMain`

## 使用说明
1. 用 IAR 打开 `GenericApp.eww`。
2. 选择配置：`CoordinatorEB` 或 `RouterEB`。
3. 直接 Build。

## 常见可改参数
- 默认信道列表：`DEFAULT_CHANLIST`
- PAN ID：`ZDAPP_CONFIG_PAN_ID`

优先在对应配置的编译选项宏里改（IAR preprocessor defines），或在相关头文件统一管理。

## 注意
该精简包依赖本机 Z-Stack 安装目录 `D:\Develop\Texas Instruments\Z-Stack 3.0.2`。
如果将来迁移到其他电脑，请同步修改 `GenericApp.ewp` 中外部依赖绝对路径。
