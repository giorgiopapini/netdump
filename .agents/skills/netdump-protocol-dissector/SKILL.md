---
name: netdump-protocol-dissector
description: Generate a new network protocol dissector for the netdump C project from a protocol definition, place it in the correct protocols/<layer>/ directory, and wire it into the correct protocol dispatch table (dlt_protos, ethertypes, ip_protos, net_ports, nlpid_protos, ppp_protos) — or add a new table only when no existing number space can route to it. Use when asked to "add protocol X support", "write a dissector for X", "where does this dissector go", "parse a new header", "register a protocol in a table", or "add a new protocol table".
---

# Netdump protocol dissector

This skill produces a netdump dissector: a header file with byte-access macros, a
`.c` file exposing `dissect_<proto>`, and the registration entry in the protocol
dispatch table that routes to it. The Makefile recurses over every `*.c`
(`SRC != find . -name '*.c' ...`), so **new files are picked up automatically** —
no Makefile edit is required.

## Library contract (no more, no less)

A dissector depends only on the exposed **libnetdump** API and the same standard
headers the existing dissectors use — nothing else. Do not add any other library,
framework, or loading mechanism.

- **Required library**: `../../libs/libnetdump/protocol.h` (the `protocol_info`
  contract) and `../../libs/libnetdump/visualizer.h` (ASCII-art rendering).
  These are the only pieces of the exposed library a dissector calls.
- **Allowed headers**: `<stddef.h>`, `<stdint.h>`, `<stdio.h>`, `<string.h>`, plus
  the project helpers that existing dissectors already include: `../../utils/formats.h`
  (e.g. `print_mac`, `print_ipv4`) and `../../utils/string_utils.h`
  (e.g. `uint_to_bin_str`), and the relevant table header (e.g. `../net_ports.h`).
  Match exactly the include set of the closest existing dissector; add a helper
  header only if you actually call it.
- **Do NOT use** `utils/custom_dissectors.h`, `utils/shared_lib.h`, or export
  `get_custom_protocols_mapping`. Those belong to the external shared-library
  mechanism, not to native dissectors.
- Registration is the existing `ADD_PROTO_HANDLER_ENTRY` defined in
  `utils/hashmap.h`, exactly as the other tables do it.

## 1. Extract the definition before writing code

From the protocol definition / spec, determine and write down:

- **Layer**: datalink | network | transport | application.
- **Trigger**: which enclosing protocol exposes the value that identifies this
  protocol, and in which field (EtherType, IP protocol number, TCP/UDP port, PPP
  protocol, NLPID, ...). This selects the table.
- **Header layout**: each field name, offset/bit-width, endianness, and whether
  the header length is fixed or computed from a field (e.g. IHL).
- **Encapsulated child**: which field names the next protocol and which table to
  look it up in, or `NO_ENCAP_PROTO` / `NO_ENCAP_PROTO_TABLE` if the payload is
  opaque (typical for application protocols).

### Where the files go

The dissector's own files live in the directory matching its **layer** (not the
directory of the enclosing protocol and not the table's root file):

| Layer | Directory |
|---|---|
| datalink | `protocols/datalink/` |
| network | `protocols/network/` |
| transport | `protocols/transport/` |
| application | `protocols/application/` |

So a network-layer header goes in `protocols/network/<proto>.h` + `.c`, even when
it is registered from `ethertypes.c` or `ip_protos.c`. The six table files
(`dlt_protos.c`, `ethertypes.c`, `ip_protos.c`, `net_ports.c`, `ppp_protos.c`,
`nlpid_protos.c`) stay at the `protocols/` root and only receive the registration
line and `#include` — never the dissector implementation.

If the trigger or header layout is ambiguous, ask the user rather than guessing.

## 2. Choose the registration table (do NOT create one unless forced)

| Enclosing protocol / access path | Table | Enum constant |
|---|---|---|
| libpcap datalink type (entry point) | DLT_PROTOS | `DLT_PROTOS` |
| Ethernet / 802.1Q VLAN / SNAP EtherType | ETHERTYPES | `ETHERTYPES` |
| IPv4 / IPv6 `protocol` / `next_header` | IP_PROTOS | `IP_PROTOS` |
| TCP / UDP port (well-known, `< 1023`) | NET_PORTS | `NET_PORTS` |
| PPP protocol field | PPP_PROTOS | `PPP_PROTOS` |
| Frame Relay / IS-IS NLPID | NLPID_PROTOS | `NLPID_PROTOS` |

The table is selected by the **parent** dissector's return value, so match the
table the enclosing dissector already points at:

- `ether.c` → `encap_proto_table_num = ETHERTYPES`
- `ip.c`/`ipv6.c` → `IP_PROTOS`
- `tcp.c`/`udp.c` → `NET_PORTS` (TCP only recurses when ACK/PSH is set)
- `ppp.c` → `PPP_PROTOS`, `frame_relay.c`/`snap.c` → `NLPID_PROTOS`/`ETHERTYPES`

Note: this codebase only recurses into `NET_PORTS` for ports `< 1023`
(`IS_WELL_DEFINED_PORT`). A port-keyed protocol on a higher port will not be
reached by the built-in chain; flag that to the user.

**Add a new table only if** the identifying value has no home in an existing
table (e.g. an MPLS label, a GRE protocol type, a nested application sub-protocol
id). See section 6.

## 3. Deliverable 1 — header `protocols/<layer>/<proto>.h`

Copy the conventions from `protocols/datalink/ether.h` and `protocols/network/ip.h`.
Do not define structs for the wire format; use macros.

```c
#ifndef MYPROTO_H
#define MYPROTO_H

#include <stddef.h>
#include <stdint.h>

#include "../../libs/libnetdump/protocol.h"

/* Labels are consumed by the ASCII-art visualizer. One per displayed field. */
#define MYPROTO_FIELD_LABEL     "My Field"

/* Byte-access macros: cast every byte so -Wconversion/-Wsign-conversion pass. */
#define MYPROTO_FIELD(pkt)      (((uint16_t)(pkt)[0] << 8) | (uint16_t)(pkt)[1])
#define MYPROTO_CHILD(pkt)      ((pkt)[2])

#define MYPROTO_HDR_LEN         4

protocol_info dissect_myproto(const uint8_t *pkt, size_t pkt_len);

#endif
```

- Include path is relative from `protocols/<layer>/`: `../../libs/libnetdump/protocol.h`.
- Every non-`static` function needs a prototype here (`-Wmissing-prototypes`,
  `-Wmissing-declarations`).
- The `<proto>.h` must be included by the table loader that registers it.

## 4. Deliverable 2 — source `protocols/<layer>/<proto>.c`

Copy the shape of `protocols/network/ip.c` / `protocols/transport/udp.c`:
two static renderers + one public `dissect_<proto>`.

```c
#include "myproto.h"

#include <stdio.h>

#include "../../libs/libnetdump/visualizer.h"        /* exposed libnetdump API */
#include "../../libs/libnetdump/protocol.h"
/* add ../../utils/formats.h or ../../utils/string_utils.h only if you call them */

static void _print_myproto_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);
static void _visualize_myproto_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len);

static void _print_myproto_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    if (!pkt || pkt_len < hdr_len) return;
    printf("my_field: %u, child: %u", MYPROTO_FIELD(pkt), MYPROTO_CHILD(pkt));
}

static void _visualize_myproto_hdr(const uint8_t *pkt, size_t pkt_len, size_t hdr_len) {
    char my_field[6];  /* 16-bit value + '\0' */

    if (!pkt || pkt_len < hdr_len) return;

    snprintf(my_field, sizeof(my_field), "%u", MYPROTO_FIELD(pkt));

    start_printing();
    print_field(MYPROTO_FIELD_LABEL, my_field, 0);
    end_printing();
}

protocol_info dissect_myproto(const uint8_t *pkt, size_t pkt_len) {
    if (!pkt || pkt_len < MYPROTO_HDR_LEN) return NO_PROTO_INFO;
    return (protocol_info){
        .print_protocol_func     = _print_myproto_hdr,
        .visualize_protocol_func = _visualize_myproto_hdr,
        .hdr_len                 = MYPROTO_HDR_LEN,
        .encap_protocol          = MYPROTO_CHILD(pkt),   /* or NO_ENCAP_PROTO */
        .encap_proto_table_num   = IP_PROTOS,            /* or NO_ENCAP_PROTO_TABLE */
    };
}
```

Rules that match the rest of the tree:

- Names: `dissect_<proto>`, `_print_<proto>_hdr`, `_visualize_<proto>_hdr`.
- Always bounds-check: `if (!pkt || pkt_len < <HDR_LEN>) return NO_PROTO_INFO;`
  For variable-length headers compute the length first, then check.
- Return `NO_PROTO_INFO` on short/malformed input — never read past `pkt_len`.
- Application/payload dissectors set `.hdr_len = 0` and both encap fields to
  `NO_ENCAP_PROTO` / `NO_ENCAP_PROTO_TABLE` (see `protocols/application/http.c`).
  `hdr_len == 0` tells the core the dissector owns the remaining bytes.
- Initialize every field you rely on; the compound literal must set all five.
- `visualizer.h` API: `start_printing()` / `print_field(label, value, newline)` /
  `print_additional_info(str)` / `end_printing()`. `print_field` takes strings,
  so `snprintf` numeric fields into a local buffer sized for the max value.

## 5. Register in the chosen table

Add one line to the `load_*()` function in the matching root file:

- `protocols/dlt_protos.c` (`load_dlt_protos`), key from `<pcap/dlt.h>`
- `protocols/ethertypes.c` (`load_ethertypes`), key `ETHERTYPE_*`
- `protocols/ip_protos.c` (`load_ip_protos`), key `IPPROTO_*`
- `protocols/net_ports.c` (`load_net_ports`), key `PORT_*`
- `protocols/ppp_protos.c` (`load_ppp_protos`), key `PPP_*`
- `protocols/nlpid_protos.c` (`load_nlpid_protos`), key `NLPID_*`

```c
ADD_PROTO_HANDLER_ENTRY(ip_protos, IPPROTO_MYPROTO, PROTOCOL_LAYER_NETWORK, dissect_myproto, "MyProto");
```

- Include the new `#include "<layer>/myproto.h"` at the top of that root file.
- If the identifier constant does not exist yet, add it to the table's `.h`
  (e.g. add `#define IPPROTO_MYPROTO 42` in `protocols/ip_protos.h`, guarded by
  `#ifndef`), or add `#define PORT_MYPROTO <n>` in `protocols/net_ports.h`.
- `PROTOCOL_LAYER_*` must match the layer you chose. The `protocol_names` string
  is what the CLI prints in `(Name)` and in hierarchy output.

## 6. Adding a new table (only when truly required)

Required only when no existing table can key the trigger value. Then make all
of these edits, exactly mirroring an existing table such as `ip_protos`:

1. `libs/libnetdump/protocol.h`
   - Add `#define MY_LABEL "my_table"` next to the other `*_LABEL` defines.
   - Add `MY_TABLE,` to the `proto_table_id` enum **before** `PROTO_TABLE_COUNT`.
2. Create `protocols/my_table.h` (buckets `#define`, `extern hashmap *my_table;`,
   `void load_my_table(void);`) and `protocols/my_table.c` following
   `protocols/ip_protos.c`.
3. `protocols/proto_tables_handler.c`
   - `#include "my_table.h"`
   - register it in `_load_proto_hashmaps_table()` → `proto_tables[MY_TABLE] = my_table;`
   - register its label in `_load_proto_hashmaps_labels()`
   - call `load_my_table();` in `load_proto_hashmaps()`
   - `destroy_hashmap(my_table, free);` in `destroy_proto_hashmaps()`
4. Make the parent dissector point at it: `.encap_proto_table_num = MY_TABLE`.

Bucket count: use a power of two large enough for the id space (existing tables
use 64–1024). `get_proto_table_from_name()` lowercases the label, so the label
string is the user-facing `-from` value.

## 7. Build and verify

```bash
make                 # must exit 0; -Werror is on
```

Then confirm the protocol is registered and actually dissects a real packet.
The CLI is a raw-mode terminal app: piping commands into it crashes
(`term_cols` is uninitialized off a TTY). Drive it through a PTY instead:

```bash
python3 - <<'PY'
import os, pty, fcntl, termios, struct, select, time, sys
cmds = [
    'protocols -from "ip_protos"',      # confirm registration: shows MyProto {num, layer}
    'analyze -r "tests_pcap/tcp.pcap" -e -t -a',
    'exit',
]
pid, fd = pty.fork()
if pid == 0:
    os.execv('./netdump', ['./netdump'])
fcntl.ioctl(fd, termios.TIOCSWINSZ, struct.pack('HHHH', 24, 100, 0, 0))
out = b''
time.sleep(0.3)
for c in cmds:
    os.write(fd, (c + '\n').encode()); time.sleep(0.5)
    while select.select([fd], [], [], 0.2)[0]:
        try: chunk = os.read(fd, 4096)
        except OSError: break
        if not chunk: break
        out += chunk
sys.stdout.write(out.decode('utf-8', 'replace'))
PY
```

Acceptance checks:

1. `make` exits 0 with no warnings.
2. `protocols -from "<table>"` lists the new protocol with the right number and layer.
3. `analyze -r "<pcap that contains it>" -e -t -a` prints the expected `(Name)`
   header. Use a pcap under `tests_pcap/`, or craft one with `text2pcap`/Scapy.
4. The ASCII-art path works too: add `-output art` to the `print` command.
5. Boundary: a truncated/malformed header collapses to `(Unknown)` rather than
   reading out of bounds.

## 8. Checklist

- [ ] Layer and trigger identified; files placed in the matching `protocols/<layer>/` directory.
- [ ] Correct registration table chosen (new table justified).
- [ ] `protocols/<layer>/<proto>.h` with labels, byte macros, `HDR_LEN`, prototype.
- [ ] `protocols/<layer>/<proto>.c` with `_print_*`, `_visualize_*`, `dissect_*`.
- [ ] Bounds-checked, returns `NO_PROTO_INFO` on short input.
- [ ] Encap fields point at the correct table/id (or NO_ENCAP*).
- [ ] `ADD_PROTO_HANDLER_ENTRY` added to the matching `load_*()`.
- [ ] Constants added to the table `.h` if missing.
- [ ] If new table: enum + label + table handler registration all updated.
- [ ] `make` clean; PTY smoke test shows the protocol.
