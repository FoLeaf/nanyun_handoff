# CC Switch 用户级 Skills 统一接管设计

日期：2026-07-14  
状态：已确认，待执行

## 1. 目标

使用 CC Switch 3.17.0 统一管理本机 Claude Code、Codex、Gemini CLI、OpenCode 和 Hermes 的用户级 Skill。`C:\Users\19y\.agents\skills` 是唯一源目录（SSOT），CC Switch 负责登记、来源、更新、启停、符号链接分发、卸载备份和恢复。

本设计只覆盖用户级 Skill。Codex 的 `.codex\skills\.system`、CC Switch/Codex/Claude 插件缓存及其他系统或插件自带 Skill 均不迁移。

## 2. 已确认基线

- CC Switch 已升级至 3.17.0。
- 数据库已从 schema v11 自动迁移至 v13，`profiles` 表存在。
- CC Switch 数据库仍有 5 条已托管 Skill 记录。
- 当前盘点约有 31 个唯一用户级 Skill；执行前必须重新扫描并生成最终清单。数量或来源与基线不一致时暂停迁移，先解释差异。
- Claude、Codex、Gemini、OpenCode 已安装；Hermes 未安装。
- Windows 符号链接已能正常工作。
- 已发现 `find-skills` 在 `.agents` 与 `.cc-switch` 中同名异版，二者来自同一上游；保留 CC Switch 中较新的版本，旧版先备份。

## 3. 目标架构

```text
C:\Users\19y\.agents\skills                    # 唯一实体源目录
  └─ <skill-name>\SKILL.md
          │
          ├─ symlink -> C:\Users\19y\.claude\skills\<skill-name>
          ├─ symlink -> C:\Users\19y\.codex\skills\<skill-name>
          ├─ symlink -> C:\Users\19y\.gemini\skills\<skill-name>
          ├─ symlink -> C:\Users\19y\.config\opencode\skills\<skill-name>
          └─ symlink -> C:\Users\19y\.hermes\skills\<skill-name>
```

CC Switch 的同步方式固定为 `symlink`。如果创建链接失败，迁移停止并修复权限，不回退到复制。

## 4. 治理规则

1. `.agents\skills` 是唯一可写源目录；各应用 Skill 目录不得直接编辑。
2. 所有新增或更新均从 CC Switch 发起。外部安装的 Skill 必须随后执行“导入已有”。
3. Skill 按兼容性白名单分发，不默认对所有应用启用。
4. Codex 专属 Skill 初始只启用 Codex；确认跨工具兼容后才能扩大范围。
5. Hermes 本轮保持全部禁用，不预建未经验证的链接。
6. 同一上游的同名 Skill 保留较新版本；不同上游或来源不明时禁止自动覆盖，必须重命名或人工选定。
7. 公开 GitHub Skill 由 CC Switch 检查和执行更新。
8. 本地或私有 Skill 在独立本地 Git 仓库中维护，以发布 ZIP 交给 CC Switch 安装或导入。CC Switch 负责分发与启停，但不宣称能自动更新私有来源。
9. 迁移前备份只保存在本机非云同步、非 Git 目录，并限制为当前 Windows 用户访问；观察期结束后删除。

## 5. 分阶段迁移

### P0：迁移前完整备份

- 重新扫描所有候选目录，记录名称、路径、来源、内容哈希、链接类型和当前应用状态。
- 导出 CC Switch 配置。
- 备份 `.cc-switch`、`.agents\skills` 及各应用用户级 Skill。
- 验证备份可读，并记录恢复路径。
- 保留当前 Codex 任务作为迁移控制台；关闭其他活动 CLI 会话。

### P1：消解冲突

- 将 `.agents\skills\find-skills` 的旧版移至隔离备份区。
- 保留 `.cc-switch\skills\find-skills` 的较新版本。
- 扫描其他同名异版目录；发现不同上游冲突时仅暂停该项，不影响无冲突项。

### P2：迁移现有 CC Switch Skill

- 在 CC Switch 中将 Skill 存储位置切换为 `~/.agents/skills`。
- 将同步方式固定为 `symlink`。
- 迁移现有 5 个托管 Skill。
- 刷新已有应用链接，并确认目标全部位于 `.agents\skills`。

### P3：接管共享 Skill

- 使用 CC Switch“导入已有”登记 `.agents\skills` 中剩余 Skill。
- 保留 `.agents\.skill-lock.json` 的公开仓库来源映射。
- 无公开来源的 Skill 标记为本地 Skill，纳入本地 Git/ZIP 发布流程。
- 导入时按兼容性白名单选择应用。

### P4：接管 Codex 独立 Skill

接管 `centbrowser-chatgpt-imagegen`、`chrome-chatgpt-imagegen`、`docx`、`lab-solution-doc`、`xlsx` 等 Codex 用户级 Skill，初始仅启用 Codex。`.system` 明确排除。

### P5：应用分发

- 按白名单逐项启用 Claude、Codex、Gemini、OpenCode。
- 每次启用后确认目标是符号链接，不产生实体副本。
- Hermes 保持全部禁用。

### P6：验收与观察

- 在新建的临时 Codex 任务中验证 Skill 发现；当前任务继续作为控制台。
- 分别启动 Claude、Gemini、OpenCode 新会话进行代表性冒烟测试。
- 旧实体目录进入隔离备份区，保留 14 天。
- 14 天内无异常后再删除迁移前备份和旧实体目录。

每个阶段只有在验收通过后才能进入下一阶段。任一阶段失败，只回滚当前批次至上一检查点。

## 6. 验收标准

- 最终唯一 Skill 清单与 CC Switch 数据库记录一一对应。
- 所有托管目录均包含可读取的 `SKILL.md`。
- 所有托管实体均位于 `.agents\skills`。
- 各应用目录中的托管项全部是指向 SSOT 的符号链接。
- 断开的符号链接数量为 0。
- CC Switch 启用开关与文件系统实际链接完全一致。
- `.codex\skills\.system` 和插件 Skill 未被修改。
- `find-skills` 使用选定的新版本，旧版本可恢复。
- Claude、Codex、Gemini、OpenCode 的新会话均能发现并触发一个代表性通用 Skill。
- 每类工具专属 Skill 至少抽查一个；无需逐一执行全部 Skill。
- Hermes 没有启用记录或新建分发链接。
- 公开 Skill 能由 CC Switch 检查更新；本地/私有 Skill 能对应到本地 Git 版本和发布 ZIP。
- 完整备份、分阶段检查点和恢复路径均可读。

## 7. 回滚设计

- P1 失败：恢复冲突项隔离备份。
- P2 失败：将存储位置切回 `cc_switch`，恢复原有 5 项链接和设置。
- P3/P4 失败：撤销该批 CC Switch 记录，恢复该批目录快照，不影响已通过批次。
- P5 失败：关闭对应应用开关并移除该应用新增链接。
- 数据库异常：退出 CC Switch 后恢复迁移前数据库备份。
- 全局失败：恢复完整目录备份及迁移前设置，不混用新数据库与旧文件布局。
- 回滚完成后重新运行清单、哈希和断链检查，确认基线恢复。

## 8. 非目标

- 不改造 Cursor、Windsurf、GitHub Copilot 等工具的 Rules/Instructions 格式。
- 不把系统 Skill 或插件 Skill 转为用户级 Skill。
- 不在本轮安装 Hermes。
- 不逐一重写 Skill 以实现跨工具兼容。
- 不在迁移期间清理 14 天观察期内的备份。

