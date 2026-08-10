from pathlib import Path
from PIL import Image, ImageDraw, ImageFont


OUT = Path(r"D:\nanyun\物联网\_嵌入式综合实验室生成过程\2026评审整改图")
OUT.mkdir(parents=True, exist_ok=True)

W, H = 1800, 1050
BG = "#F7FAFC"
NAVY = "#123B5D"
BLUE = "#1976A3"
CYAN = "#DDF3F7"
GREEN = "#2E7D60"
LGREEN = "#E1F3E9"
ORANGE = "#D9792B"
LORANGE = "#FBE9D7"
RED = "#B84444"
GRAY = "#5E6B75"
LGRAY = "#E8EEF2"
WHITE = "#FFFFFF"

FONT_PATHS = [
    Path(r"C:\Windows\Fonts\msyh.ttc"),
    Path(r"C:\Windows\Fonts\simhei.ttf"),
    Path(r"C:\Windows\Fonts\simsun.ttc"),
]
FONT_PATH = next(p for p in FONT_PATHS if p.exists())


def font(size, bold=False):
    if bold and Path(r"C:\Windows\Fonts\msyhbd.ttc").exists():
        return ImageFont.truetype(r"C:\Windows\Fonts\msyhbd.ttc", size)
    return ImageFont.truetype(str(FONT_PATH), size)


def canvas(title, subtitle=""):
    im = Image.new("RGB", (W, H), BG)
    d = ImageDraw.Draw(im)
    d.rounded_rectangle((45, 35, W - 45, 145), 18, fill=NAVY)
    d.text((80, 58), title, font=font(44, True), fill=WHITE)
    if subtitle:
        d.text((82, 112), subtitle, font=font(21), fill="#D9E9F3")
    return im, d


def fit_lines(draw, text, fnt, max_width):
    lines, current = [], ""
    for ch in text:
        test = current + ch
        if draw.textbbox((0, 0), test, font=fnt)[2] <= max_width:
            current = test
        else:
            if current:
                lines.append(current)
            current = ch
    if current:
        lines.append(current)
    return lines


def box(draw, xy, title, body="", fill=WHITE, outline=BLUE, title_color=NAVY,
        body_color=GRAY, title_size=30, body_size=23, radius=18, width=3):
    x1, y1, x2, y2 = xy
    draw.rounded_rectangle(xy, radius, fill=fill, outline=outline, width=width)
    tf = font(title_size, True)
    bf = font(body_size)
    title_lines = fit_lines(draw, title, tf, x2 - x1 - 38)
    y = y1 + 20
    for line in title_lines:
        draw.text((x1 + 20, y), line, font=tf, fill=title_color)
        y += title_size + 8
    if body:
        y += 6
        for line in fit_lines(draw, body, bf, x2 - x1 - 38):
            draw.text((x1 + 20, y), line, font=bf, fill=body_color)
            y += body_size + 9


def arrow(draw, p1, p2, color=BLUE, width=5):
    draw.line((p1, p2), fill=color, width=width)
    x2, y2 = p2
    x1, y1 = p1
    import math
    a = math.atan2(y2 - y1, x2 - x1)
    l = 18
    pts = [
        (x2, y2),
        (x2 - l * math.cos(a - 0.55), y2 - l * math.sin(a - 0.55)),
        (x2 - l * math.cos(a + 0.55), y2 - l * math.sin(a + 0.55)),
    ]
    draw.polygon(pts, fill=color)


def footer(draw, text="初步设计示意｜用于项目方案评审，非施工图"):
    draw.text((W - 600, H - 34), text, font=font(18), fill="#7E8B94")


def save(im, name):
    im.save(OUT / name, quality=96)


def fig_policy():
    im, d = canvas("政策、产业与专业建设逻辑", "教职成〔2026〕1号：专业、课程、教材、教师、实习实训五要素联动改革")
    box(d, (90, 200, 1710, 315), "政策牵引", "坚持需求牵引，以产定教、以产引教、以产改教、以产促教；建设产教融合实习实训基地和虚拟仿真实训基地。", fill=CYAN)
    labels = [("专业", "对接畜禽智能化养殖岗位"), ("课程", "按生产流程组织项目"), ("教材", "指导书、视频与案例"), ("教师", "设备、平台与项目能力"), ("实习实训", "真实场景与虚实结合")]
    for i, (a, b) in enumerate(labels):
        x = 90 + i * 326
        box(d, (x, 370, x + 286, 525), a, b, fill=LGREEN, outline=GREEN, title_color=GREEN, title_size=32, body_size=21)
    arrow(d, (900, 315), (900, 365), GREEN)
    box(d, (90, 600, 560, 840), "产业任务", "猪舍与鸡舍环境监测、精准环控、饲喂饮水状态、设备巡检、异常告警与故障诊断。", fill=LORANGE, outline=ORANGE, title_color=ORANGE)
    box(d, (665, 600, 1135, 840), "技术响应", "ESP32无线感知 + STM32实时控制 + RT-Thread多任务 + RS485/Modbus工业通信。", fill=CYAN)
    box(d, (1240, 600, 1710, 840), "教学成果", "13套设备、每套4人协同、单班52人；硬件、软件、资源和过程评价一体化交付。", fill=LGREEN, outline=GREEN, title_color=GREEN)
    arrow(d, (560, 720), (655, 720), ORANGE)
    arrow(d, (1135, 720), (1230, 720), GREEN)
    footer(d)
    save(im, "01_policy_five_elements.png")


def fig_overall():
    im, d = canvas("嵌入式综合实验室整体建设形态", "从畜禽养殖生产任务出发，形成硬件、软件、场景与资源协同的实训环境")
    box(d, (90, 220, 470, 850), "畜禽养殖场景", "生猪舍环境监测\n蛋鸡舍温感联动\n通风、卷帘、补光\n饲喂饮水状态\n巡检与故障诊断", fill=LORANGE, outline=ORANGE, title_color=ORANGE)
    box(d, (550, 220, 930, 490), "13套学生实训设备", "每套支持4人协同：接线调试、程序开发、通信配置、数据分析。", fill=CYAN)
    box(d, (550, 580, 930, 850), "综合实训沙盘", "猪舍、鸡舍、RS485总线、嵌入式节点、执行机构、边缘网关与本地触控。", fill=LGREEN, outline=GREEN, title_color=GREEN)
    box(d, (1010, 220, 1390, 490), "虚拟仿真与云平台", "本地保存、选择性同步、任务闭环、AI辅助评分、教师复核与数据归档。", fill=CYAN)
    box(d, (1010, 580, 1390, 850), "教学资源交付", "指导书、教学视频、典型案例、完整源代码、点位表、协议与验收测试表。", fill=LGREEN, outline=GREEN, title_color=GREEN)
    box(d, (1470, 350, 1710, 720), "教学产出", "单班52人\n项目化教学\n实训留痕\n可持续使用", fill=NAVY, outline=NAVY, title_color=WHITE, body_color=WHITE)
    for y in (350, 700): arrow(d, (470, y), (540, y), ORANGE)
    for y in (350, 700): arrow(d, (930, y), (1000, y), BLUE)
    arrow(d, (1390, 525), (1460, 525), GREEN)
    footer(d)
    save(im, "02_overall_effect.png")


def fig_task_loop():
    im, d = canvas("教学做评一体化闭环", "真实设备与虚拟仿真共用任务、数据和评价标准")
    stages = [
        ("教师创建任务", "设备、步骤、指标、评分规则"),
        ("学生实验", "仿真接线或真实设备操作"),
        ("本地保存", "代码、接线、数据、运行日志"),
        ("选择性同步", "同步至学校本地云平台"),
        ("AI辅助评分", "按规则分析过程与结果"),
        ("教师复核归档", "确认成绩、批阅报告、沉淀资源"),
    ]
    coords = [(90,260,520,430),(685,260,1115,430),(1280,260,1710,430),(1280,650,1710,820),(685,650,1115,820),(90,650,520,820)]
    for i,(a,b) in enumerate(stages):
        fill = LORANGE if i==0 else (LGREEN if i in (4,5) else CYAN)
        outline = ORANGE if i==0 else (GREEN if i in (4,5) else BLUE)
        box(d, coords[i], a, b, fill=fill, outline=outline, title_color=outline)
    points=[((520,345),(675,345)),((1115,345),(1270,345)),((1495,430),(1495,640)),((1280,735),(1125,735)),((685,735),(530,735)),((305,650),(305,440))]
    for a,b in points: arrow(d,a,b,GREEN)
    box(d,(640,455,1160,610),"全过程数据留痕","任务记录、接线校验、代码版本、设备数据、评分结果和教师意见统一归档。",fill=WHITE,outline=NAVY,title_color=NAVY)
    footer(d)
    save(im, "03_teaching_loop.png")


def fig_progression():
    im, d = canvas("学生能力递进路线", "基础驱动—无线感知—实时系统—工业通信—行业项目")
    stages=[("1 基础驱动","STM32裸机、GPIO、ADC、PWM、定时器与中断"),("2 无线感知","ESP32、Wi-Fi/BLE、I2C传感器与低成本节点"),("3 实时系统","RT-Thread线程、IPC、设备驱动与异常处理"),("4 工业通信","RS485/Modbus、多节点轮询、网关协议转换"),("5 场景交付","猪舍/鸡舍环控、沙盘联调、平台闭环与故障诊断")]
    for i,(a,b) in enumerate(stages):
        x=70+i*345
        y=230+i*100
        box(d,(x,y,x+300,y+250),a,b,fill=CYAN if i<3 else LGREEN,outline=BLUE if i<3 else GREEN,title_color=BLUE if i<3 else GREEN,title_size=27,body_size=21)
        if i<4: arrow(d,(x+300,y+125),(x+335,y+200),ORANGE)
    footer(d)
    save(im, "04_capability_progression.png")


def fig_52_students():
    im, d = canvas("13套设备、52人协同实训组织", "每套设备支持4名学生按工程角色协同操作，沙盘作为综合联调与验收载体")
    for i in range(13):
        col=i%5; row=i//5
        x=80+col*280+(0 if row<2 else 280)
        y=220+row*235
        box(d,(x,y,x+235,y+175),f"设备 {i+1:02d}","接线｜程序\n通信｜数据",fill=CYAN,outline=BLUE,title_size=25,body_size=20)
        d.rounded_rectangle((x+78,y+128,x+158,y+160),10,fill=NAVY)
        d.text((x+91,y+132),"4人",font=font(20,True),fill=WHITE)
    box(d,(1460,255,1720,800),"综合沙盘", "教师演示\n小组轮换\nRS485联调\n鸡舍温感控制\n故障诊断\n项目验收",fill=LORANGE,outline=ORANGE,title_color=ORANGE,title_size=28,body_size=23)
    d.text((610,930),"13套 × 4人/套 = 单班52人",font=font(38,True),fill=NAVY)
    footer(d)
    save(im, "05_13sets_52students.png")


def fig_room_plan():
    im, d = canvas("约120㎡实训室空间布局示意", "13套学生设备、教师演示、沙盘联调、本地服务器与收纳维护分区")
    x1,y1,x2,y2=90,190,1710,900
    d.rectangle((x1,y1,x2,y2),outline=NAVY,width=6,fill=WHITE)
    d.rectangle((110,210,460,330),fill=CYAN,outline=BLUE,width=3); d.text((170,245),"教师演示区",font=font(30,True),fill=NAVY)
    d.rectangle((1320,210,1690,450),fill=LORANGE,outline=ORANGE,width=3); d.text((1380,245),"畜禽养殖沙盘",font=font(28,True),fill=ORANGE); d.text((1385,305),"CAD布局与联调",font=font(22),fill=GRAY)
    d.rectangle((1320,500,1690,650),fill=LGREEN,outline=GREEN,width=3); d.text((1380,535),"本地服务器区",font=font(28,True),fill=GREEN)
    d.rectangle((1320,700,1690,875),fill=LGRAY,outline=GRAY,width=3); d.text((1400,750),"收纳维护区",font=font(28,True),fill=GRAY)
    for i in range(13):
        col=i%4; row=i//4
        x=130+col*280; y=390+row*125
        if i==12: x=550
        d.rounded_rectangle((x,y,x+220,y+78),10,fill=CYAN,outline=BLUE,width=2)
        d.text((x+22,y+20),f"设备{i+1:02d}｜4人",font=font(22,True),fill=NAVY)
    d.text((510,345),"学生实训设备区（52人）",font=font(28,True),fill=NAVY)
    d.line((1240,360,1240,860),fill="#B8C5CE",width=4)
    d.text((1165,600),"主通道",font=font(22),fill=GRAY)
    footer(d)
    save(im, "06_room_plan_52.png")


def fig_network():
    im,d=canvas("实训室网络与数据安全拓扑", "实验数据本地优先保存，按教学任务选择性同步至校内云平台")
    box(d,(70,245,430,430),"13套学生设备","ESP32、STM32/RT-Thread、传感与执行模块",fill=CYAN)
    box(d,(70,635,430,820),"综合实训沙盘","RS485/Modbus、边缘节点、执行设备",fill=LORANGE,outline=ORANGE,title_color=ORANGE)
    box(d,(600,410,980,650),"边缘网关与交换网络","多协议接入、数据缓存、断网续传、访问控制",fill=LGREEN,outline=GREEN,title_color=GREEN)
    box(d,(1150,245,1510,430),"学校本地服务器","虚拟仿真、教学云平台、数据库、备份",fill=CYAN)
    box(d,(1150,635,1510,820),"教师与管理员","任务、评价、权限、日志、资源与运维",fill=LGREEN,outline=GREEN,title_color=GREEN)
    box(d,(1570,410,1740,650),"可选外联","经授权同步\n不作为运行前提",fill=WHITE,outline=GRAY,title_color=GRAY,body_size=20)
    for a,b in [((430,335),(590,500)),((430,725),(590,570)),((980,500),(1140,335)),((980,570),(1140,725)),((1510,525),(1560,525))]: arrow(d,a,b,GREEN)
    footer(d)
    save(im,"07_network_topology.png")


def fig_architecture():
    im,d=canvas("畜禽养殖嵌入式技术架构", "ESP32无线感知、STM32/RT-Thread实时控制、RS485工业通信与本地云平台协同")
    layers=[
        ("行业任务层","猪舍精准环控｜鸡舍温感联动｜饲喂饮水｜巡检告警｜故障诊断",LORANGE,ORANGE),
        ("教学平台层","虚拟仿真｜班级任务｜本地保存｜选择性同步｜AI辅助评分｜教师复核",LGREEN,GREEN),
        ("边缘网关层","协议转换｜数据缓存｜本地规则｜断网续传｜安全接入",CYAN,BLUE),
        ("控制与通信层","STM32 + RT-Thread｜RS485/Modbus｜继电器/PWM｜执行反馈",LGREEN,GREEN),
        ("无线感知层","ESP32｜Wi-Fi/BLE｜温湿度/光照/空气质量/重量/计数",CYAN,BLUE),
    ]
    y=190
    for title,body,fillc,outc in layers:
        box(d,(150,y,1650,y+130),title,body,fill=fillc,outline=outc,title_color=outc,title_size=29,body_size=22)
        if y<790: arrow(d,(900,y+130),(900,y+165),outc)
        y+=165
    footer(d)
    save(im,"08_technical_architecture.png")


def fig_box_product():
    im,d=canvas("嵌入式综合实验箱模块构成", "双硬件平台、实时操作系统、工业总线与养殖场景外设统一组合")
    d.rounded_rectangle((120,200,1680,870),28,fill=WHITE,outline=NAVY,width=7)
    modules=[("STM32控制","裸机+RT-Thread",GREEN), ("ESP32感知","Wi-Fi/BLE",BLUE), ("无线通信","ZigBee/LoRa/4G",ORANGE), ("边缘网关","协议转换",GREEN), ("环境采集","温湿度/光照",BLUE), ("工业通信","RS485/Modbus",ORANGE), ("执行控制","继电器/PWM",GREEN), ("识别巡检","RFID/二维码",BLUE), ("本地显示","TFT/OLED",ORANGE)]
    for i,(a,b,c) in enumerate(modules):
        col=i%3; row=i//3; x=180+col*500; y=270+row*185
        box(d,(x,y,x+390,y+135),a,b,fill=CYAN if c==BLUE else (LGREEN if c==GREEN else LORANGE),outline=c,title_color=c,title_size=27,body_size=21)
    d.text((520,790),"统一快接接口｜模块任意组合｜支持4人协同操作",font=font(32,True),fill=NAVY)
    footer(d)
    save(im,"09_training_box.png")


def fig_modular():
    im,d=canvas("模块化实验箱结构与教学组合", "9个同构接口位，按畜禽养殖任务组合主控、感知、通信与执行模块")
    d.rounded_rectangle((100,210,1130,850),24,fill=WHITE,outline=NAVY,width=6)
    for i in range(9):
        col=i%3; row=i//3; x=160+col*320; y=280+row*175
        d.rounded_rectangle((x,y,x+250,y+115),14,fill=CYAN,outline=BLUE,width=3)
        d.text((x+72,y+35),f"接口位 {i+1}",font=font(26,True),fill=NAVY)
    box(d,(1210,230,1710,830),"模块资源池","ESP32无线感知\nSTM32/RT-Thread控制\nRS485/Modbus通信\n温湿度与空气质量\nRFID/二维码巡检\n风机、卷帘与补光\n边缘网关与本地显示",fill=LGREEN,outline=GREEN,title_color=GREEN,title_size=30,body_size=24)
    for y in (330,500,670): arrow(d,(1130,y),(1200,y),GREEN)
    footer(d)
    save(im,"10_modular_combinations.png")


def fig_platform():
    im,d=canvas("虚拟仿真与云平台班级任务闭环", "学生、教师、管理员三类角色协同，实验数据本地保存并支持选择性同步")
    box(d,(70,210,470,820),"学生端","任务接收\n引脚悬停提示\n错误接线告警\n积木编程\nTFT/OLED显示\n本地保存\n选择性提交",fill=CYAN,title_size=30,body_size=24)
    box(d,(700,210,1100,820),"教师端","创建班级与任务\n绑定设备/仿真\n配置评分规则\n查看过程数据\n复核AI评分\n批阅实验报告\n归档教学资源",fill=LGREEN,outline=GREEN,title_color=GREEN,title_size=30,body_size=24)
    box(d,(1330,210,1730,820),"管理员端","用户与权限\n设备与协议\n资源与版本\n服务器部署\n日志与备份\n数据安全\n运行维护",fill=LORANGE,outline=ORANGE,title_color=ORANGE,title_size=30,body_size=24)
    arrow(d,(470,515),(690,515),GREEN); arrow(d,(1100,515),(1320,515),ORANGE)
    d.text((515,455),"提交/反馈",font=font(22,True),fill=GREEN); d.text((1135,455),"配置/运维",font=font(22,True),fill=ORANGE)
    footer(d)
    save(im,"11_platform_roles_loop.png")


def fig_resources():
    im,d=canvas("教学资源与项目化课程体系", "硬件、软件和资源同步交付、同步培训、同步验收")
    box(d,(80,230,420,800),"教学资源","实训指导书\n教学操作视频\n项目任务书\n完整源代码\n接线图与点位表\n协议说明\n评分量规\n验收测试表",fill=CYAN,title_size=30,body_size=23)
    projects=[("ESP32环境采集","低功耗节点、无线联网"),("STM32基础控制","ADC/PWM、传感器与执行器"),("RT-Thread环控","多任务、IPC、设备驱动"),("RS485鸡舍联动","温感、风机、卷帘与反馈"),("猪舍精准环控","空气质量、喷雾与告警"),("综合沙盘联调","平台闭环与故障诊断")]
    for i,(a,b) in enumerate(projects):
        col=i%2; row=i//2; x=540+col*450; y=230+row*190
        box(d,(x,y,x+390,y+145),a,b,fill=LGREEN if row>0 else LORANGE,outline=GREEN if row>0 else ORANGE,title_color=GREEN if row>0 else ORANGE,title_size=26,body_size=20)
    box(d,(1480,300,1730,720),"验收结果","教师能教\n学生能学\n设备能用\n数据可留\n资源可复用",fill=NAVY,outline=NAVY,title_color=WHITE,body_color=WHITE,title_size=29,body_size=23)
    arrow(d,(420,515),(530,515),BLUE); arrow(d,(1380,515),(1470,515),GREEN)
    footer(d)
    save(im,"12_teaching_resources.png")


def fig_cad_sandbox():
    im,d=canvas("畜禽智能养殖嵌入式感知与环控综合实训沙盘CAD初步效果图", "传感点位、控制节点、RS485总线及养殖专属设备联动逻辑可视化")
    # CAD dark panel
    d.rectangle((55,175,1745,930),fill="#0B1E29",outline="#5EC8D6",width=4)
    # zones
    d.rectangle((105,230,760,800),outline="#4FD1C5",width=4)
    d.text((125,245),"A区 生猪养殖栏位",font=font(28,True),fill="#70E1D4")
    d.rectangle((830,230,1485,800),outline="#F6C85F",width=4)
    d.text((850,245),"B区 蛋鸡舍与笼位",font=font(28,True),fill="#F6C85F")
    d.rectangle((1520,230,1695,800),outline="#9AE66E",width=4)
    d.text((1540,255),"控制柜",font=font(26,True),fill="#9AE66E")
    # pig pens and chicken cages
    for r in range(2):
        for c in range(3):
            x=140+c*195; y=330+r*205
            d.rectangle((x,y,x+150,y+130),outline="#4FD1C5",width=2)
            d.text((x+42,y+48),f"栏{r*3+c+1}",font=font(24),fill="#CDEFF2")
    for r in range(3):
        for c in range(4):
            x=865+c*140; y=325+r*145
            d.rectangle((x,y,x+105,y+85),outline="#F6C85F",width=2)
            d.text((x+32,y+27),f"笼{r*4+c+1}",font=font(20),fill="#FFF0C7")
    # bus
    d.line((150,740,1630,740),fill="#FF6B6B",width=6)
    d.text((640,755),"RS485 / Modbus RTU 工业总线",font=font(24,True),fill="#FF8D8D")
    # nodes and sensors
    for x,y,label,color in [(220,300,"ESP32-1","#55C2FF"),(520,300,"ESP32-2","#55C2FF"),(970,300,"ESP32-3","#55C2FF"),(1280,300,"ESP32-4","#55C2FF"),(1540,360,"STM32\nRT-Thread","#9AE66E"),(1540,540,"边缘网关","#9AE66E")]:
        d.ellipse((x-20,y-20,x+20,y+20),fill=color)
        d.text((x+28,y-22),label,font=font(19,True),fill=color)
        d.line((x,y+20,x,740),fill="#6C7F89",width=2)
    # actuators
    for x,y,label in [(190,610,"喷雾"),(415,610,"风机"),(640,610,"水泵"),(900,620,"补光"),(1115,620,"卷帘"),(1340,620,"风机")]:
        d.rectangle((x,y,x+80,y+40),outline="#FFB35C",width=2)
        d.text((x+10,y+6),label,font=font(19),fill="#FFD29B")
    # linkage callout
    d.rounded_rectangle((860,825,1685,895),12,outline="#F6C85F",width=3)
    d.text((885,842),"鸡舍温度越限 → RS485上报 → RT-Thread判断 → 风机/卷帘联动 → 状态反馈",font=font(22,True),fill="#F6C85F")
    # dimensions
    d.line((105,205,1485,205),fill="#A5B7C3",width=2); d.text((715,180),"约1600 mm",font=font(18),fill="#A5B7C3")
    d.line((80,230,80,800),fill="#A5B7C3",width=2); d.text((58,500),"1200",font=font(16),fill="#A5B7C3")
    footer(d,"CAD初步布局效果｜非施工图，尺寸与点位以深化设计为准")
    save(im,"13_cad_livestock_sandbox.png")


def fig_extension():
    im,d=canvas("产教融合与持续建设路线", "以已建实训条件为基础，逐步形成课程、师资、项目与社会服务成果")
    stages=[("近期：教学落地","完成设备、平台、资源和教师培训"),("中期：课程共建","引入企业生产任务与行业标准"),("深化：项目创新","优化养殖控制策略与故障诊断"),("拓展：服务产业","支持培训、技术服务与成果推广")]
    for i,(a,b) in enumerate(stages):
        x=90+i*420; y=300+(i%2)*230
        box(d,(x,y,x+350,y+180),a,b,fill=CYAN if i<2 else LGREEN,outline=BLUE if i<2 else GREEN,title_color=BLUE if i<2 else GREEN,title_size=26,body_size=21)
        if i<3: arrow(d,(x+350,y+90),(x+410,390+(i%2)*120),ORANGE)
    footer(d)
    save(im,"14_extension_route.png")


def fig_competition():
    im,d=canvas("竞赛训练与能力映射", "围绕嵌入式开发、工业通信、系统联调和工程表达形成可迁移能力")
    center=(900,520)
    d.ellipse((700,390,1100,650),fill=NAVY,outline=NAVY)
    d.text((780,465),"畜禽养殖\n嵌入式综合项目",font=font(34,True),fill=WHITE,align="center")
    nodes=[(120,220,"ESP32无线感知","联网、传感与低功耗"),(120,700,"STM32/RT-Thread","驱动、任务与实时控制"),(1280,220,"RS485/Modbus","工业设备接入与诊断"),(1280,700,"平台与工程表达","数据闭环、报告与答辩")]
    for x,y,a,b in nodes:
        box(d,(x,y,x+400,y+170),a,b,fill=CYAN if x<900 else LGREEN,outline=BLUE if x<900 else GREEN,title_color=BLUE if x<900 else GREEN,title_size=27,body_size=21)
        arrow(d,(x+200 if x>900 else x+400,y+85),center,GREEN if x>900 else BLUE)
    footer(d)
    save(im,"15_competition_map.png")


def fig_implementation():
    im,d=canvas("项目实施、培训与验收流程", "设备、软件、教学资源和应用能力同步建设、同步试运行、同步验收")
    stages=[("1 深化设计","空间、网络、CAD沙盘与点位"),("2 到货部署","13套设备、平台、场景系统"),("3 安装联调","ESP32、STM32/RT-Thread、RS485"),("4 资源与培训","指导书、视频、案例、源码"),("5 试运行验收","52人教学、任务闭环、资料归档")]
    for i,(a,b) in enumerate(stages):
        x=55+i*350
        box(d,(x,330,x+300,620),a,b,fill=CYAN if i<3 else LGREEN,outline=BLUE if i<3 else GREEN,title_color=BLUE if i<3 else GREEN,title_size=27,body_size=22)
        if i<4: arrow(d,(x+300,475),(x+340,475),ORANGE)
    box(d,(500,730,1300,850),"演示保障","提前调试｜全屏固定脚本｜本地离线数据｜备用录屏｜问题清单闭环",fill=LORANGE,outline=ORANGE,title_color=ORANGE,title_size=28,body_size=22)
    footer(d)
    save(im,"16_implementation_flow.png")


if __name__ == "__main__":
    fig_policy(); fig_overall(); fig_task_loop(); fig_progression(); fig_52_students(); fig_room_plan()
    fig_network(); fig_architecture(); fig_box_product(); fig_modular(); fig_platform(); fig_resources()
    fig_cad_sandbox(); fig_extension(); fig_competition(); fig_implementation()
    print(OUT)
