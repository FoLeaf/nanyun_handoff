# Zigbee V1.8 reverse notes

Target: `Zigbee V1.8.exe`

- SHA256: `D5587F777617D8862AE0B67E68DA4B2080CBE8D851F6E2BBE4EF329B0E8D2CB1`
- PE: 32-bit .NET WinForms, CLR v4.0.30319, internal name `Zigbee.exe`
- Title string: `Ebyte ZigBee 3.0Setting V1.8`
- Main serial receive handler: `serialPort1_DataReceived`
- Frame builder helpers: `DATAPRO.cmdAdd`, `DATAPRO.Get_CheckXor`, `DATAPRO.dateAdd`

## Frame format

The V1.8 tool uses the same HEX frame layout as the public EBYTE protocol:

```text
55 LL TT CC [DATA...] XOR
```

- `55`: frame header
- `LL`: payload length plus checksum slot, implemented as `len(cmdType + cmdCode + data) + 1`
- `TT`: command type
- `CC`: command code
- `XOR`: XOR of `TT ^ CC ^ DATA...`

## Command types

| Type | Name |
| --- | --- |
| `0x00` | local/config command |
| `0x01` | ZDO request |
| `0x02` | ZCL send |
| `0x80` | notification |
| `0x81` | ZDO response |
| `0x82` | ZCL indication |
| `0x8F` | send confirm |

## Local/config commands

| Code | Name |
| --- | --- |
| `0x00` | status |
| `0x01` | start/boot |
| `0x02` | open network |
| `0x03` | close network |
| `0x04` | reset/restore |
| `0x05` | node type |
| `0x06` | channel |
| `0x07` | get PAN ID |
| `0x08` | set PAN ID |
| `0x09` | view group |
| `0x0A` | add group |
| `0x0B` | remove group |
| `0x0D` | set power |
| `0x0F` | test |
| `0x10` | read local attribute |
| `0x11` | write local attribute |
| `0x20` | get UTC |
| `0x21` | set UTC |
| `0x22` | get address table |
| `0x28` | retransmit device information |

## ZDO request commands

| Code | Name |
| --- | --- |
| `0x00` | NWK address request |
| `0x01` | IEEE address request |
| `0x02` | node descriptor request |
| `0x03` | simple descriptor request |
| `0x04` | active endpoint request |
| `0x21` | bind request |
| `0x22` | unbind request |
| `0x33` | management bind request |
| `0x34` | management leave request |

## ZCL send commands

| Code | Name |
| --- | --- |
| `0x00` | read attributes |
| `0x01` | write attributes |
| `0x02` | read reporting config |
| `0x03` | write reporting config |
| `0x04` | discover attributes |
| `0x05` | discover attributes extended |
| `0x06` | discover received commands |
| `0x07` | discover generated commands |
| `0x0F` | ZCL command / transparent command |

## Notifications and responses

| Type | Code | Name |
| --- | --- | --- |
| `0x80` | `0x00` | boot |
| `0x80` | `0x01` | network status |
| `0x80` | `0x02` | network open |
| `0x80` | `0x03` | node join |
| `0x80` | `0x04` | node address |
| `0x80` | `0x05` | device join |
| `0x80` | `0x06` | leave |
| `0x80` | `0x10` | find/bind |
| `0x80` | `0x11` | identify |
| `0x81` | `0x00` | NWK address response |
| `0x81` | `0x01` | IEEE address response |
| `0x81` | `0x02` | node descriptor response |
| `0x81` | `0x04` | simple descriptor response |
| `0x81` | `0x05` | active endpoint response |
| `0x81` | `0x21` | bind response |
| `0x81` | `0x22` | unbind response |
| `0x81` | `0x33` | management bind response |
| `0x81` | `0x36` | management leave response |
| `0x82` | `0x0A` | ZCL report indication |
| `0x82` | `0x0B` | ZCL default response |
| `0x82` | `0x0F` | ZCL command indication |
| `0x8F` | `0x01` | ZDO send confirm |
| `0x8F` | `0x02` | ZCL send confirm |

## UI event to command mapping

| V1.8 UI handler | Built command |
| --- | --- |
| `Open_Net_Bt_Click` | `00 02` |
| `Close_Net_Bt_Click` | `00 03` |
| `Reset_Bt_Click` | `00 04` |
| `ReadUTC_Bt_Click` | `00 20` |
| `SetUTC_Bt_Click` | `00 21` |
| `Read_Device_Status_Bt_Click` | `00 22` |
| `RE_DEVICE_INF_Click` | `00 28` |
| `Read_Con_Bt_Click` | `01 33` |
| `Set_Con_Bt_Click` | `01 21` |
| `Auto_Con_Bt_Click` | `01 22` |
| `Node_Shortaddr_Bt_Click` | `01 00`, then `01 01` |
| `Del_node_Bt_Click` | `01 34` |
| `Senddate_Bt_Click` | `02 00`, `02 01`, or `02 0F` depending on UI mode |
