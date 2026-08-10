from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(r"D:\nanyun\物联网")
OUT = ROOT / "_嵌入式综合实验室生成过程" / "full-review-redraw"
OUT.mkdir(parents=True, exist_ok=True)
FONT = r"C:\Windows\Fonts\msyh.ttc"
BOLD = r"C:\Windows\Fonts\msyhbd.ttc"

BLUE = "#1769AA"
ORANGE = "#D98200"
GREEN = "#3B7F52"
INK = "#152033"
MUTED = "#536171"
LINE = "#7C8793"
PALE = "#F7F8FA"


def f(size, bold=False):
    return ImageFont.truetype(BOLD if bold else FONT, size)


def header(draw, title, subtitle, width=1600):
    draw.text((70, 45), title, font=f(36, True), fill="#111111")
    draw.text((72, 102), subtitle, font=f(20), fill=MUTED)
    draw.line((70, 142, width - 70, 142), fill="#222222", width=3)
    draw.rectangle((70, 158, 125, 163), fill=BLUE)
    draw.rectangle((125, 158, 175, 163), fill="#70828A")
    draw.rectangle((175, 158, 225, 163), fill=ORANGE)


def center(draw, box, text, font, fill=INK, spacing=6):
    x0, y0, x1, y1 = box
    bb = draw.multiline_textbbox((0, 0), text, font=font, spacing=spacing, align="center")
    tw, th = bb[2] - bb[0], bb[3] - bb[1]
    draw.multiline_text(
        ((x0 + x1 - tw) / 2, (y0 + y1 - th) / 2),
        text,
        font=font,
        fill=fill,
        spacing=spacing,
        align="center",
    )


def card(draw, box, title, lines, accent=BLUE, title_size=25, body_size=19):
    draw.rectangle(box, fill="white", outline=LINE, width=2)
    draw.rectangle((box[0], box[1], box[2], box[1] + 7), fill=accent)
    center(draw, (box[0] + 10, box[1] + 24, box[2] - 10, box[1] + 88), title, f(title_size, True))
    center(draw, (box[0] + 15, box[1] + 88, box[2] - 15, box[3] - 18), "\n".join(lines), f(body_size), spacing=7)


def arrow(draw, start, end, color="#363B40", width=4):
    draw.line((*start, *end), fill=color, width=width)
    x, y = end
    if abs(end[0] - start[0]) >= abs(end[1] - start[1]):
        sign = 1 if end[0] > start[0] else -1
        draw.polygon([(x, y), (x - sign * 22, y - 13), (x - sign * 22, y + 13)], fill=color)
    else:
        sign = 1 if end[1] > start[1] else -1
        draw.polygon([(x, y), (x - 13, y - sign * 22), (x + 13, y - sign * 22)], fill=color)


def footer(draw, text, width=1600):
    draw.rectangle((100, 845, width - 100, 915), fill=PALE)
    draw.rectangle((100, 845, 107, 915), fill="#222222")
    draw.text((130, 867), text, font=f(18), fill="#273444")


def save_native(image, name, size):
    if image.size != size:
        image = image.resize(size, Image.Resampling.LANCZOS)
    path = OUT / name
    image.save(path, optimize=True)
    return path


def policy_logic():
    im = Image.new("RGB", (1600, 940), "white")
    d = ImageDraw.Draw(im)
    header(d, "政策—专业群—技术—建设响应逻辑图", "职业教育要求与畜禽智能化养殖专业群共同牵引嵌入式实训条件升级")
    xs = [80, 450, 820, 1190]
    data = [
        ("政策与专业群牵引", ["职业教育数字化", "现代职业教育体系", "畜禽智能化养殖专业群"], BLUE),
        ("岗位与教学需求", ["养殖环境感知", "嵌入式设备控制", "设备巡检与数据运维"], ORANGE),
        ("嵌入式技术底座", ["STM32 / OpenHarmony", "边缘网关与协议转换", "Modbus / MQTT"], BLUE),
        ("项目建设响应", ["13套设备 / 26人", "双养殖系统 + 实训沙盘", "预算69.7230万元"], ORANGE),
    ]
    for x, (title, lines, color) in zip(xs, data):
        card(d, (x, 300, x + 300, 625), title, lines, color, 24, 19)
    for x in [380, 750, 1120]:
        arrow(d, (x, 462), (x + 65, 462))
    footer(d, "建设结果：形成以嵌入式开发为主线、以畜禽养殖环境智能监测为项目出口的综合实训环境。")
    return save_native(im, "image1.png", (1600, 940))


def lab_effect():
    src = ROOT / "_嵌入式综合实验室生成过程" / "full-review-images" / "image2.png"
    im = Image.open(src).convert("RGB").resize((1448, 1086), Image.Resampling.LANCZOS)
    overlay = Image.new("RGBA", im.size, (0, 0, 0, 0))
    d = ImageDraw.Draw(overlay)
    d.rounded_rectangle((55, 765, 1393, 1040), radius=10, fill=(255, 255, 255, 238), outline=(90, 100, 110, 255), width=2)
    d.rectangle((55, 765, 724, 774), fill=BLUE)
    d.rectangle((724, 765, 1393, 774), fill=GREEN)
    d.text((95, 805), "学生实训区", font=f(34, True), fill=INK)
    d.text((95, 860), "13套嵌入式综合实验箱", font=f(27), fill=INK)
    d.text((95, 905), "26人双人分组 / 模块化开发调试", font=f(25), fill=MUTED)
    d.text((770, 805), "畜禽智能养殖沙盘联调区", font=f(32, True), fill="#214E30")
    d.text((770, 860), "生猪区 + 蛋鸡区 + 嵌入式节点", font=f(25), fill="#294D34")
    d.text((770, 905), "边缘网关—执行联动—云平台", font=f(25), fill="#52665A")
    d.rounded_rectangle((55, 40, 355, 105), radius=5, fill=(255, 255, 255, 225))
    d.text((82, 57), "嵌入式综合实验室整体效果", font=f(24, True), fill=INK)
    im = Image.alpha_composite(im.convert("RGBA"), overlay).convert("RGB")
    return save_native(im, "image2.png", (1448, 1086))


def ability_route():
    im = Image.new("RGB", (1600, 940), "white")
    d = ImageDraw.Draw(im)
    header(d, "学生能力递进路线图", "由底层接口驱动逐步进入边缘控制、平台联调与畜禽养殖行业项目")
    data = [
        ("① 基础验证", ["GPIO / UART / I2C / SPI", "ADC / PWM / 中断", "程序烧录与串口调试"], BLUE),
        ("② 模块组合", ["传感 / 通信 / RFID", "风机 / 水泵 / 喷雾 / 补光", "无线组网与数据采集"], ORANGE),
        ("③ 系统联调", ["Modbus / MQTT", "边缘规则与离线控制", "平台建模、告警与看板"], GREEN),
        ("④ 行业项目", ["猪舍精准环控", "禽舍状态联动", "沙盘故障注入与验收"], ORANGE),
    ]
    xs = [95, 475, 855, 1235]
    for x, (title, lines, color) in zip(xs, data):
        card(d, (x, 295, x + 270, 680), title, lines, color, 25, 18)
    for x in [365, 745, 1125]:
        arrow(d, (x, 485), (x + 105, 485))
    footer(d, "能力输出：接口驱动、传感采集、协议通信、边缘控制、平台联调、故障诊断和项目交付。")
    return save_native(im, "image4.png", (3200, 1880))


def network_topology():
    im = Image.new("RGB", (1600, 940), "white")
    d = ImageDraw.Draw(im)
    header(d, "实训室网络拓扑图", "13组学生工位、教师终端、边缘网关、实训云平台与畜禽养殖沙盘统一接入")
    card(d, (650, 205, 950, 305), "校园网 / 互联网", ["安全访问与资源服务"], BLUE, 23, 16)
    card(d, (650, 345, 950, 455), "防火墙 / 路由器", ["访问控制与网络出口"], ORANGE, 23, 16)
    card(d, (650, 500, 950, 620), "核心交换机", ["千兆有线 / 无线汇聚"], BLUE, 24, 17)
    arrow(d, (800, 305), (800, 340))
    arrow(d, (800, 455), (800, 495))
    lower = [
        (80, "教师管理终端", ["投屏 / 大屏 / 任务发布"], ORANGE),
        (390, "13组学生工位", ["实验箱 / PC / 双人分组"], BLUE),
        (700, "边缘网关与触控", ["协议转换 / 本地规则"], GREEN),
        (1010, "畜禽养殖实训沙盘", ["猪舍 / 禽舍 / 执行联动"], GREEN),
        (1320, "实训云平台", ["设备管理 / 数据备份"], ORANGE),
    ]
    for x, title, lines, color in lower:
        card(d, (x, 700, x + 230, 825), title, lines, color, 20, 15)
        arrow(d, (800, 620), (x + 115, 695), width=3)
    footer(d, "网络原则：学生工位与沙盘通过边缘网关接入平台，支持真实设备、虚拟设备和离线控制协同。")
    return save_native(im, "image6.png", (3200, 1880))


def architecture():
    im = Image.new("RGB", (1600, 940), "white")
    d = ImageDraw.Draw(im)
    header(d, "端—边—云总体架构图", "从嵌入式端侧采集与执行，到边缘决策、云端教学管理和养殖场景应用")
    layers = [
        ("场景应用层", "生猪精准环控  /  蛋鸡禽舍联动  /  畜禽智能养殖实训沙盘", GREEN),
        ("嵌入式智能综合实训云平台", "设备建模、数据可视化、规则联动、任务发布、过程评价", ORANGE),
        ("边缘网关层", "Modbus / MQTT协议转换、本地规则、数据缓存、断网续传、触控看板", BLUE),
        ("嵌入式控制层", "STM32、OpenHarmony、ZigBee节点、WiFi / LoRa / 4G通信模块", ORANGE),
        ("感知与执行层", "温湿度、气体、光照、重量、RFID、风机、水泵、喷雾、补光", BLUE),
    ]
    y = 220
    for title, body, color in layers:
        box = (160, y, 1440, y + 105)
        d.rectangle(box, fill="white", outline=LINE, width=2)
        d.rectangle((160, y, 1440, y + 7), fill=color)
        d.text((230, y + 22), title, font=f(23, True), fill=INK)
        d.text((520, y + 28), body, font=f(17), fill="#374455")
        if y < 720:
            arrow(d, (800, y + 105), (800, y + 132), width=3)
        y += 135
    footer(d, "架构重点：端侧可编程、边缘可自治、云端可管理、场景可验证，形成完整嵌入式工程闭环。")
    return save_native(im, "image7.png", (1600, 940))


def modular_box():
    im = Image.new("RGB", (1600, 940), "white")
    d = ImageDraw.Draw(im)
    header(d, "模块化实验箱结构与教学组合图", "9个同构接口位支撑主控、通信、感知、执行、网关与畜禽养殖项目快速重组")
    central = (625, 355, 975, 690)
    d.rectangle(central, fill="#FBFCFD", outline=LINE, width=2)
    d.rectangle((625, 355, 975, 363), fill=BLUE)
    center(d, (625, 370, 975, 430), "嵌入式综合实验箱", f(24, True))
    for i in range(9):
        rr, cc = divmod(i, 3)
        bx = (670 + cc * 95, 460 + rr * 70, 745 + cc * 95, 510 + rr * 70)
        d.rectangle(bx, fill="white", outline="#87919A", width=1)
        center(d, bx, f"接口{i+1}", f(15, True))
    d.text((685, 660), "同构快接 / 免跳线 / 任务化组合", font=f(15), fill=MUTED)
    surrounding = [
        ((90, 260, 430, 405), "主控组合", ["STM32 / OpenHarmony / ZigBee", "程序下载、外设驱动、串口调试"], ORANGE),
        ((1170, 260, 1510, 405), "通信组合", ["WiFi / 4G / BLE / LoRa", "MQTT / HTTP / TCP / UDP"], BLUE),
        ((90, 620, 430, 775), "感知识别组合", ["温湿度 / 气体 / 重量 / RFID", "环境采集、设备巡检、阈值判断"], BLUE),
        ((1170, 620, 1510, 775), "执行控制组合", ["风机 / 水泵 / 喷雾 / 补光", "PWM调节、规则联动、反馈记录"], ORANGE),
        ((630, 205, 970, 315), "网关与平台组合", ["协议转换 / 本地看板 / 实训云平台"], GREEN),
        ((630, 735, 970, 830), "畜禽养殖项目组合", ["生猪 / 蛋鸡系统 + 综合实训沙盘"], GREEN),
    ]
    for box, title, lines, color in surrounding:
        card(d, box, title, lines, color, 20, 15)
    for pt in [(430, 340), (1170, 340), (430, 700), (1170, 700), (800, 315), (800, 735)]:
        arrow(d, pt, (800, 520), width=3)
    footer(d, "模块化价值：围绕嵌入式接口、协议、数据与系统联调快速重组，减少固定硬件绑定。")
    return save_native(im, "image9.png", (1600, 940))


def course_system():
    im = Image.new("RGB", (1600, 940), "white")
    d = ImageDraw.Draw(im)
    header(d, "嵌入式与畜禽养殖项目化课程体系设计图", "基础开发、核心模块、系统联调和养殖行业项目四层递进")
    data = [
        ("① 基础开发课程", ["电工电子技术", "C语言 / 嵌入式C", "嵌入式系统基础", "STM32 / OpenHarmony"], BLUE),
        ("② 核心模块课程", ["传感器与接口", "无线嵌入式网络", "RFID与设备巡检", "执行器控制"], ORANGE),
        ("③ 系统联调课程", ["边缘网关与协议转换", "嵌入式云平台应用", "数据采集与可视化", "规则联动与故障诊断"], GREEN),
        ("④ 行业项目课程", ["猪舍环境监测与环控", "禽舍状态联动控制", "沙盘综合联调", "系统集成 / 毕业设计"], ORANGE),
    ]
    xs = [95, 475, 855, 1235]
    for x, (title, lines, color) in zip(xs, data):
        card(d, (x, 295, x + 270, 710), title, lines, color, 22, 18)
    for x in [365, 745, 1125]:
        arrow(d, (x, 500), (x + 105, 500))
    footer(d, "课程资源配套：实验指导书、源码、接线图、点位表、平台配置、报告模板、评价量规与教师培训材料。")
    return save_native(im, "image11.png", (3200, 1880))


def sandbox_effect():
    im = Image.new("RGB", (1448, 1086), "white")
    d = ImageDraw.Draw(im)
    d.text((65, 48), "畜禽智能养殖嵌入式实训沙盘联调示意图", font=f(39, True), fill="#111111")
    d.text((68, 108), "生猪与蛋鸡场景共用嵌入式节点、边缘网关、执行机构和教学平台", font=f(23), fill=MUTED)
    d.line((65, 155, 1383, 155), fill="#222222", width=3)
    d.rectangle((65, 174, 120, 179), fill=BLUE)
    d.rectangle((120, 174, 170, 179), fill="#70828A")
    d.rectangle((170, 174, 220, 179), fill=ORANGE)
    # Sand table base
    base = [(150, 850), (1298, 850), (1160, 315), (290, 315)]
    d.polygon(base, fill="#F3F5F6", outline="#59636E")
    d.line((150, 850, 1298, 850), fill="#333333", width=4)
    # Pig and chicken houses
    pig = (260, 420, 610, 690)
    chicken = (820, 420, 1170, 690)
    for box, title, accent, items in [
        (pig, "生猪养殖区", GREEN, ["温湿度 / 气体 / 饮水料位", "风机 / 喷雾 / 声光告警"]),
        (chicken, "蛋鸡养殖区", ORANGE, ["光照 / 环境 / 产蛋计数", "补光 / 通风 / 设备巡检"]),
    ]:
        d.rounded_rectangle(box, radius=8, fill="white", outline=LINE, width=2)
        d.rectangle((box[0], box[1], box[2], box[1] + 9), fill=accent)
        center(d, (box[0], box[1] + 25, box[2], box[1] + 95), title, f(28, True))
        center(d, (box[0] + 20, box[1] + 100, box[2] - 20, box[3] - 20), "\n".join(items), f(20), spacing=12)
    # Gateway and control console
    gateway = (570, 250, 880, 385)
    d.rounded_rectangle(gateway, radius=7, fill="white", outline=LINE, width=2)
    d.rectangle((570, 250, 880, 259), fill=BLUE)
    center(d, gateway, "边缘网关与触控终端\n协议转换 / 本地规则 / 离线控制", f(21, True), spacing=8)
    console = (470, 765, 980, 860)
    d.rounded_rectangle(console, radius=7, fill="white", outline=LINE, width=2)
    d.rectangle((470, 735, 980, 744), fill=BLUE)
    center(d, console, "嵌入式智能综合实训云平台\n设备建模 / 数据曲线 / 告警记录 / 教学评价", f(21, True), spacing=7)
    # Connections
    for start, end in [((610, 560), (690, 385)), ((820, 560), (760, 385)), ((725, 385), (725, 760))]:
        arrow(d, start, end, color="#40505A", width=4)
    # Small nodes
    for x, y, label in [(195, 695, "STM32节点"), (1095, 695, "OpenHarmony节点"), (365, 695, "RFID巡检"), (925, 695, "执行器反馈")]:
        d.rounded_rectangle((x, y, x + 150, y + 55), radius=5, fill="#FFFFFF", outline="#8A949E", width=2)
        center(d, (x, y, x + 150, y + 55), label, f(16, True))
    d.rectangle((90, 930, 1358, 1035), fill=PALE)
    d.rectangle((90, 930, 98, 1035), fill="#222222")
    d.text((125, 952), "联调流程：传感采集 → 嵌入式节点 → 边缘决策 → 执行联动 → 云端记录与评价", font=f(23), fill="#273444")
    return save_native(im, "image12.png", (1448, 1086))


def competition_map():
    im = Image.new("RGB", (1600, 940), "white")
    d = ImageDraw.Draw(im)
    header(d, "嵌入式与畜禽智能养殖竞赛训练能力映射图", "面向嵌入式开发、边缘控制、养殖设备联调和OpenHarmony方向预留训练条件")
    center_box = (650, 390, 950, 560)
    card(d, center_box, "综合训练条件", ["实验箱 + 开发包", "双养殖系统 + 实训沙盘"], BLUE, 24, 17)
    surrounding = [
        ((100, 250, 440, 390), "嵌入式系统开发", ["MCU外设 / 驱动 / 调试"], BLUE),
        ((1160, 250, 1500, 390), "边缘网关与协议集成", ["Modbus / MQTT / 本地规则"], ORANGE),
        ((100, 650, 440, 800), "养殖智能装备控制", ["传感采集 / 环控联动 / 巡检"], GREEN),
        ((1160, 650, 1500, 800), "OpenHarmony嵌入式创新", ["轻量设备 / WiFi配网 / 云端交互"], ORANGE),
    ]
    for box, title, lines, color in surrounding:
        card(d, box, title, lines, color, 21, 16)
    for start, end in [((440, 320), (650, 430)), ((1160, 320), (950, 430)), ((440, 725), (650, 520)), ((1160, 725), (950, 520))]:
        arrow(d, start, end, width=3)
    footer(d, "训练说明：按能力方向组织模块组合、限时联调、故障排查、项目文档和现场答辩，不写既有竞赛成绩。")
    return save_native(im, "image14.png", (1600, 940))


if __name__ == "__main__":
    outputs = [
        policy_logic(),
        lab_effect(),
        ability_route(),
        network_topology(),
        architecture(),
        modular_box(),
        course_system(),
        sandbox_effect(),
        competition_map(),
    ]
    for output in outputs:
        print(output)
