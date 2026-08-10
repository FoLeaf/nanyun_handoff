# CC Switch 用户级 Skills 统一接管执行计划

> **执行约束：** 本计划分阶段执行。每个检查点通过后才能继续；任何文件删除都推迟到 14 天观察期结束。执行期间保留当前 Codex 任务作为控制台，不使用子代理。

**目标：** 将本机所有用户级 Skill 的实体统一到 `C:\Users\19y\.agents\skills`，由 CC Switch 3.17.0 负责登记、更新、启停和符号链接分发。

**设计依据：** [2026-07-14-cc-switch-user-skills-takeover-design.md](../specs/2026-07-14-cc-switch-user-skills-takeover-design.md)

**已知基线：** CC Switch 3.17.0、数据库 schema v13、5 条已托管记录；约 31 个唯一用户级 Skill；Claude/Codex/Gemini/OpenCode 已安装，Hermes 未安装。

---

## 全局安全规则

- 仅操作用户级 Skill；不得修改 `.codex\skills\.system` 和任何插件缓存。
- 所有递归移动或删除前，先解析并核对绝对路径位于预期用户目录。
- 备份目录固定在 `C:\Users\19y\.cc-switch-migration-backups\<run-id>`，不得位于 OneDrive、Git 仓库或网络盘。
- CC Switch 数据库含敏感配置；备份目录只允许当前 Windows 用户访问，输出中不得打印数据库内容或供应商配置。
- 同步方式必须是 `symlink`；如果创建失败，停止并修复权限，不回退为复制。
- 应用启用采用白名单；不确定兼容性时先保持禁用。
- 每个阶段生成检查点证据：文件清单、数据库计数、链接清单和错误日志摘要。

## 预期批次

| 批次 | 内容 | 预期数量 |
|---|---|---:|
| 已托管 | 当前 CC Switch 管理的 Skill | 5 |
| 共享待导入 | `.agents\skills` 中其余 Skill | 约 21 |
| Codex 独立项 | Codex 用户级独立 Skill | 5 |
| 最终唯一项 | CC Switch 管理总数 | 约 31 |

最终数量必须由 P0 扫描重新计算。若不等于 31，先解释新增、删除或同名合并原因，再更新预期值。

---

### Task 1：建立 P0 迁移前清单

**只读对象：**

- `C:\Users\19y\.cc-switch\skills`
- `C:\Users\19y\.agents\skills`
- `C:\Users\19y\.claude\skills`
- `C:\Users\19y\.codex\skills`
- `C:\Users\19y\.gemini\skills`
- `C:\Users\19y\.config\opencode\skills`
- `C:\Users\19y\.hermes\skills`
- `C:\Users\19y\.cc-switch\cc-switch.db`

- [ ] 确认 CC Switch 进程版本为 `3.17.0`。
- [ ] 以只读方式确认数据库 `user_version = 13`、`profiles` 表存在、`skills` 表当前为 5 行。
- [ ] 枚举上述目录的一级子目录，记录 `Name`、`FullName`、`LinkType`、`Target`。
- [ ] 只将包含 `SKILL.md` 的目录纳入用户级候选清单。
- [ ] 明确排除目录名以点开头的系统项，尤其是 `.codex\skills\.system`。
- [ ] 对候选目录的 `SKILL.md` 计算 SHA-256；按目录名聚合，识别同名同内容和同名异版。
- [ ] 读取 `.agents\.skill-lock.json`，只记录 Skill 名称和公开仓库来源，不输出任何凭据。
- [ ] 输出最终唯一 Skill 清单和来源分类：CC Switch、agents、Codex、本地未知。
- [ ] 记录已知特殊项：`.claude\skills\word-document` 是指向 `.agents` 的既有 Junction。
- [ ] 若唯一数量不是预期的约 31，暂停并解释差异。

**P0 清单通过标准：** 每个待迁移 Skill 都有唯一目录名、源路径、内容哈希和来源决策。

### Task 2：创建并验证本机备份

**创建：** `C:\Users\19y\.cc-switch-migration-backups\<run-id>`

- [ ] 使用时间戳建立唯一 `<run-id>`，确认解析后的备份路径位于当前用户目录且不在 OneDrive/Git/网络盘。
- [ ] 限制备份根目录 ACL，仅当前 Windows 用户和系统账户可访问。
- [ ] 完整复制 `.cc-switch`，保留数据库、设置、现有 5 个 Skill 和 CC Switch 自动备份。
- [ ] 完整复制 `.agents`，保留 Skill 内容和 `.skill-lock.json`。
- [ ] 复制 Codex 的 5 个独立用户级 Skill；不得复制或改动 `.system`。
- [ ] 保存 Claude、Codex、Gemini、OpenCode、Hermes 应用目录的链接清单；链接本身无需复制成实体内容。
- [ ] 在 CC Switch 界面执行一次配置导出，保存到本机备份目录。
- [ ] 随机读取至少 3 个备份 Skill 的 `SKILL.md`。
- [ ] 以只读方式打开备份数据库并确认可查询 `skills` 表。
- [ ] 记录备份根路径和完成时间，但不展示数据库内容。

**检查点：** `P0-迁移前完整备份`。未验证备份可读，不得继续。

### Task 3：处理 `find-skills` 冲突并建立 P1

**已知冲突：**

- 旧版：`C:\Users\19y\.agents\skills\find-skills`
- 新版：`C:\Users\19y\.cc-switch\skills\find-skills`

- [ ] 再次确认两者的上游均为 `vercel-labs/skills`。
- [ ] 记录两者 SHA-256 和最后修改时间。
- [ ] 确认新版内容存在于 P0 备份中。
- [ ] 将旧版从 `.agents\skills` 移到本次备份的隔离子目录；不得覆盖或删除。
- [ ] 确认 `.agents\skills\find-skills` 已空出，CC Switch 新版仍完好。
- [ ] 重新扫描全部候选项；若发现其他同名异版且上游不同，暂停该项并要求人工决策。

**检查点：** `P1-冲突已消解`。

**回滚：** 将隔离的旧版原样移回 `.agents\skills\find-skills`。

### Task 4：在 CC Switch 中迁移现有 5 项并建立 P2

**CC Switch 界面操作：**

- [ ] 打开“设置 → Skills 存储位置”，选择 `~/.agents/skills`。
- [ ] 确认迁移对话框显示的目标路径正确后执行。
- [ ] 打开“Skills 同步方式”，明确选择“软链接（Symlink）”，不保留 `auto`。
- [ ] 返回 Skills 主面板，确认原有 5 项仍显示为已安装。

**文件与数据库验证：**

- [ ] 确认设置中 `skillStorageLocation` 等效值为 `unified`，同步方式等效值为 `symlink`。
- [ ] 确认以下 5 项实体位于 `.agents\skills`：`claude-hud-statusline`、`find-skills`、`frontend-design`、`playwright-cli`、`skill-creator`。
- [ ] 确认 `.cc-switch\skills` 不再是这些项目的权威实体位置。
- [ ] 确认数据库仍为 5 条 Skill 记录，启用状态未意外改变。
- [ ] 检查 Claude、Codex、OpenCode 中已启用项的链接目标全部改为 `.agents\skills`。
- [ ] 检查断链数为 0。
- [ ] 检查没有因迁移产生实体复制目录。

**检查点：** `P2-现有托管项已迁移`。

**回滚：** 在 CC Switch 中将存储位置切回“CC Switch 内置存储”，恢复 P0 设置与 5 项链接；若数据库异常，退出 CC Switch 后恢复 P0 数据库。

### Task 5：建立兼容性白名单

**输入：** P0 最终唯一 Skill 清单。

- [ ] 对每个待导入 Skill 读取完整 `SKILL.md`，识别依赖的命令、MCP、插件、浏览器和应用专属路径。
- [ ] 按以下规则形成矩阵：
  - 仅使用通用提示和基础文件/终端能力：候选 Claude、Codex、Gemini、OpenCode。
  - 明确引用 Codex、Codex 插件或 Codex 专属工具：仅 Codex。
  - 明确引用 Claude Code 插件或 Claude 专属命令：仅 Claude。
  - 依赖某应用未安装的 MCP/程序：该应用保持禁用。
  - 无法确定：全部保持禁用，待单项验证。
- [ ] 将 Codex 独立 5 项初始矩阵设为仅 Codex。
- [ ] 将 Hermes 列全部设为禁用。
- [ ] 核对同一个目录名只对应一行矩阵。

**通过标准：** 每个 Skill 都有明确的初始启用集合，且没有“默认全开”。

### Task 6：通过“导入已有”接管共享 Skill并建立 P3

**CC Switch 界面操作：**

- [ ] 在 Skills 页面点击“导入已有”。
- [ ] 确认扫描结果主要为 `.agents\skills` 中约 21 个未管理项。
- [ ] 对照 P0 清单，排除系统项、插件项和任何无 `SKILL.md` 目录。
- [ ] 分小组导入，每组不超过 5 个；按兼容性矩阵选择应用。
- [ ] 每组导入后立即验证数据库增量、SSOT 实体和目标链接，再继续下一组。
- [ ] 公开来源 Skill 确认 CC Switch 记录了仓库信息。
- [ ] 无公开来源 Skill 确认为本地记录，并登记其本地 Git/ZIP 发布责任。
- [ ] 处理 `word-document` 前，记录既有 Junction 的目标；由 CC Switch 启用时确认其被安全替换为指向同一 SSOT 的符号链接，源目录内容不得变化。

**P3 通过标准：** 共享 Skill 全部被 CC Switch 登记；数据库预计从 5 增至约 26；无断链和实体副本。

**回滚：** 只撤销失败小组的 CC Switch 记录和新链接；从 P0 备份恢复该组原目录，不回滚已通过小组。

### Task 7：接管 Codex 独立 5 项并建立 P4

**候选：**

- `centbrowser-chatgpt-imagegen`
- `chrome-chatgpt-imagegen`
- `docx`
- `lab-solution-doc`
- `xlsx`

- [ ] 在“导入已有”中确认以上项目来自 `.codex\skills` 且包含 `SKILL.md`。
- [ ] 每次只导入一个，初始仅启用 Codex。
- [ ] 确认实体复制到 `.agents\skills` 后，Codex 原位置被 CC Switch 符号链接取代。
- [ ] 每项完成后对比迁移前后内容哈希。
- [ ] 确认 `.codex\skills\.system` 未发生任何状态、时间戳或内容变化。
- [ ] 完成后确认数据库最终数量与 P0 唯一清单一致，预期约为 31。

**检查点：** `P4-Codex 用户 Skill 已接管`。

**回滚：** 撤销失败项记录与链接，将 P0 中该 Skill 实体恢复到 `.codex\skills`。

### Task 8：执行应用分发并建立 P5

- [ ] 对照兼容性矩阵，在 CC Switch 中逐项调整 Claude、Codex、Gemini、OpenCode 开关。
- [ ] 每调整一组应用开关，检查目标目录只出现符号链接。
- [ ] 检查所有链接均指向 `.agents\skills\<skill-name>`。
- [ ] 检查没有链接指回 `.cc-switch\skills` 或其他应用目录。
- [ ] 检查没有目标应用目录中的实体 Skill 副本。
- [ ] 保持 Hermes 所有开关关闭；不创建 `.hermes\skills` 分发内容。
- [ ] 对照数据库启用状态与文件系统链接，差异数必须为 0。

**检查点：** `P5-应用分发完成`。

### Task 9：结构验收和新会话冒烟

**全量结构验收：**

- [ ] 每个数据库 Skill 都能映射到 `.agents\skills` 中唯一实体目录。
- [ ] 每个实体目录都有可读取的 `SKILL.md`。
- [ ] 所有启用状态都有对应链接，所有禁用状态都没有对应链接。
- [ ] 断链数为 0；实体副本数为 0。
- [ ] 系统和插件 Skill 未改动。
- [ ] `find-skills` 是选定的新版本，旧版备份可读。

**应用冒烟：**

- [ ] 保留当前 Codex 任务作为控制台，新建临时 Codex 任务确认能发现一个通用 Skill和一个 Codex 专属 Skill。
- [ ] 新建 Claude 会话，确认发现并触发一个通用 Skill。
- [ ] 新建 Gemini 会话，确认发现并触发一个兼容 Skill。
- [ ] 新建 OpenCode 会话，确认发现并触发一个兼容 Skill。
- [ ] 对每个工具专属类别额外抽查一个 Skill；不逐一执行全部 Skill。
- [ ] 冒烟测试只执行无副作用任务，例如说明触发条件或读取自身 `SKILL.md`，不发送外部消息、不修改业务文件。

**检查点：** `P6-CC Switch 完全接管`。

### Task 10：进入 14 天观察期

- [ ] 将所有不再使用的旧实体目录移入本次备份的隔离区；不得立即删除。
- [ ] 记录观察期开始时间、结束时间和备份路径。
- [ ] 观察期间所有新增和更新只通过 CC Switch；私有 Skill 使用本地 Git + ZIP 流程。
- [ ] 如发现问题，按最近检查点回滚单项或单批，不混用新数据库与旧文件布局。
- [ ] 14 天结束后再次执行数据库、链接、断链和代表性冒烟检查。
- [ ] 最终验收通过后，删除本次敏感迁移备份和隔离旧目录。
- [ ] 保留不含凭据的最终清单、兼容性矩阵和验收摘要。

---

## 完成定义

只有同时满足以下条件，才将任务标记为完成：

1. CC Switch 数据库记录与最终唯一用户级 Skill 清单一致。
2. 所有托管实体只存在于 `.agents\skills`。
3. 所有应用分发均为正确符号链接，且断链为 0。
4. 系统和插件 Skill 未被迁移或修改。
5. Claude、Codex、Gemini、OpenCode 的新会话冒烟通过。
6. Hermes 保持禁用。
7. P0–P6 检查点证据与回滚材料齐全。
8. 14 天观察期结束并完成最终复验。

