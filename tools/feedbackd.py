#!/usr/bin/env python3
"""The native port's feedback and crash-report service (docs/port.md, "Feedback and crash reports").

The port (build/port/fall, Daggerfall.app) sends a report when the player presses F8 (with a
description) or when it crashes: a .tar.gz of screenshots, the session's log and input (a
PORT_SCRIPT that replays it), the game's memory, and a save of the game when it is in the
world. This service takes them over HTTP on this Mac's network, checks the bearer token the
port's build carries, and unpacks each into build/feedback/<time>_<kind>_<id>/ (gitignored: the
reports hold game data).

  feedbackd.py init        the token and URL (~/.config/daggerfall-feedback/: tools/port_build.py
                           bakes them into the build)
  feedbackd.py install     init, and a LaunchAgent that runs the service at login and keeps it
                           running
  feedbackd.py uninstall   stops and removes the LaunchAgent (the token stays)
  feedbackd.py serve       runs the service in the foreground (what the LaunchAgent runs)
  feedbackd.py status      whether it answers, and where reports go
  feedbackd.py list [N]    the last N reports

The protocol: POST /v1/report, Authorization: Bearer TOKEN, X-Report-Id, X-Report-Kind
(feedback, crash), the body a gzip'd tar. GET /v1/ping (with the token) answers ok.
"""
import datetime
import hmac
import http.server
import io
import json
import os
import plistlib
import re
import secrets
import shutil
import socket
import subprocess
import sys
import tarfile
import threading

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CONFIG = os.path.expanduser("~/.config/daggerfall-feedback")
STORE = os.path.join(ROOT, "build", "feedback")
PORT = 8742
MAX_BODY = 1 << 30                       # 1 GB
LABEL = "io.github.brotchie.daggerfall-feedbackd"
AGENT = os.path.expanduser("~/Library/LaunchAgents/%s.plist" % LABEL)
ID_RE = re.compile(r"^[A-Za-z0-9_.-]{1,80}$")
KINDS = {"feedback", "crash", "test"}


def read_config(name):
    try:
        with open(os.path.join(CONFIG, name)) as f:
            return f.read().strip()
    except OSError:
        return None


def notify(title, text):
    """a macOS notification (best effort)"""
    script = 'display notification %s with title %s' % (json.dumps(text), json.dumps(title))
    subprocess.run(["osascript", "-e", script], capture_output=True, timeout=10)


class Handler(http.server.BaseHTTPRequestHandler):
    server_version = "daggerfall-feedbackd/1"
    token = None
    lock = threading.Lock()

    def log_message(self, fmt, *args):
        sys.stderr.write("%s %s %s\n" % (datetime.datetime.now().isoformat(timespec="seconds"),
                                          self.client_address[0], fmt % args))

    def reply(self, code, obj):
        body = json.dumps(obj).encode()
        self.send_response(code)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def authorised(self):
        given = self.headers.get("Authorization", "")
        want = "Bearer " + self.token
        return hmac.compare_digest(given.encode(), want.encode())

    def do_GET(self):
        if self.path != "/v1/ping":
            return self.reply(404, {"error": "not found"})
        if not self.authorised():
            return self.reply(401, {"error": "unauthorised"})
        return self.reply(200, {"ok": True})

    def do_POST(self):
        if self.path != "/v1/report":
            return self.reply(404, {"error": "not found"})
        if not self.authorised():
            return self.reply(401, {"error": "unauthorised"})
        rid = self.headers.get("X-Report-Id", "")
        kind = self.headers.get("X-Report-Kind", "")
        try:
            length = int(self.headers.get("Content-Length", "-1"))
        except ValueError:
            length = -1
        if not ID_RE.match(rid) or kind not in KINDS:
            return self.reply(400, {"error": "bad id or kind"})
        if length <= 0 or length > MAX_BODY:
            return self.reply(413, {"error": "bad length"})
        stamp = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
        name = "%s_%s_%s" % (stamp, kind, rid)
        incoming = os.path.join(STORE, "incoming")
        os.makedirs(incoming, exist_ok=True)
        archive = os.path.join(incoming, name + ".tar.gz")
        left = length
        with open(archive, "wb") as f:
            while left > 0:
                chunk = self.rfile.read(min(left, 1 << 20))
                if not chunk:
                    break
                f.write(chunk)
                left -= len(chunk)
        if left:
            os.unlink(archive)
            return self.reply(400, {"error": "short body"})
        dest = os.path.join(STORE, name)
        try:
            with tarfile.open(archive, "r:gz") as t:
                t.extractall(dest, filter="data")       # no absolute paths, .., links out
        except (tarfile.TarError, OSError, EOFError) as e:
            return self.reply(400, {"error": "bad archive: %s" % e})
        raw_screen_to_bmp(dest)
        os.makedirs(os.path.join(STORE, "archives"), exist_ok=True)
        shutil.move(archive, os.path.join(STORE, "archives", name + ".tar.gz"))
        meta = {}
        for m in (os.path.join(dest, "meta.json"), os.path.join(dest, rid, "meta.json")):
            if os.path.exists(m):
                try:
                    meta = json.load(open(m))
                except ValueError:
                    pass
                break
        entry = {"time": stamp, "kind": kind, "id": rid, "dir": os.path.relpath(dest, ROOT),
                 "bytes": length, "from": self.client_address[0],
                 "summary": (meta.get("description") or meta.get("crash") or "")[:200]}
        with self.lock:
            with open(os.path.join(STORE, "index.jsonl"), "a") as f:
                f.write(json.dumps(entry) + "\n")
        self.log_message("stored %s (%d bytes)", entry["dir"], length)
        notify("Daggerfall %s" % kind, entry["summary"] or name)
        return self.reply(200, {"ok": True, "stored": entry["dir"]})


def raw_screen_to_bmp(dest):
    """a crash's screen320.raw (320x200 palette indices) and palette.raw (768 six-bit
    components), written by the crash handler, as screen320.bmp"""
    for root, _dirs, files in os.walk(dest):
        if "screen320.raw" in files and "palette.raw" in files:
            px = open(os.path.join(root, "screen320.raw"), "rb").read()
            pal = open(os.path.join(root, "palette.raw"), "rb").read()
            if len(px) != 64000 or len(pal) != 768:
                continue
            rgb = [((pal[i * 3] << 2) | (pal[i * 3] >> 4), (pal[i * 3 + 1] << 2) | (pal[i * 3 + 1] >> 4),
                    (pal[i * 3 + 2] << 2) | (pal[i * 3 + 2] >> 4)) for i in range(256)]
            rows = bytearray()
            for y in range(199, -1, -1):
                for x in range(320):
                    r, g, b = rgb[px[y * 320 + x]]
                    rows += bytes((b, g, r))
            head = b"BM" + (54 + len(rows)).to_bytes(4, "little") + bytes(4) + (54).to_bytes(4, "little")
            info = (40).to_bytes(4, "little") + (320).to_bytes(4, "little") + (200).to_bytes(4, "little") \
                + (1).to_bytes(2, "little") + (24).to_bytes(2, "little") + bytes(4) \
                + len(rows).to_bytes(4, "little") + bytes(16)
            with open(os.path.join(root, "screen320.bmp"), "wb") as f:
                f.write(head + info + rows)


def serve():
    token = read_config("token")
    if not token:
        sys.exit("no token: run feedbackd.py install")
    os.makedirs(STORE, exist_ok=True)
    Handler.token = token
    httpd = http.server.ThreadingHTTPServer(("0.0.0.0", PORT), Handler)
    sys.stderr.write("feedbackd: listening on port %d, storing in %s\n" % (PORT, STORE))
    httpd.serve_forever()


def local_hostname():
    r = subprocess.run(["scutil", "--get", "LocalHostName"], capture_output=True, text=True)
    name = r.stdout.strip() or socket.gethostname().split(".")[0]
    return name + ".local"


def init():
    os.makedirs(CONFIG, mode=0o700, exist_ok=True)
    tok = os.path.join(CONFIG, "token")
    if not os.path.exists(tok):
        fd = os.open(tok, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600)
        os.write(fd, (secrets.token_urlsafe(32) + "\n").encode())
        os.close(fd)
    url = "http://%s:%d/v1/report" % (local_hostname(), PORT)
    with open(os.path.join(CONFIG, "url"), "w") as f:
        f.write(url + "\n")
    os.makedirs(STORE, exist_ok=True)
    print("feedbackd: url %s\n  token in %s (tools/port_build.py bakes both into the build)"
          % (url, tok))
    return url, tok


def install():
    url, tok = init()
    python = os.path.join(ROOT, ".venv", "bin", "python")
    if not os.path.exists(python):
        python = sys.executable
    plist = {
        "Label": LABEL,
        "ProgramArguments": [python, os.path.abspath(__file__), "serve"],
        "RunAtLoad": True,
        "KeepAlive": True,
        "WorkingDirectory": ROOT,
        "StandardOutPath": os.path.join(STORE, "feedbackd.log"),
        "StandardErrorPath": os.path.join(STORE, "feedbackd.log"),
    }
    os.makedirs(os.path.dirname(AGENT), exist_ok=True)
    with open(AGENT, "wb") as f:
        plistlib.dump(plist, f)
    uid = os.getuid()
    subprocess.run(["launchctl", "bootout", "gui/%d/%s" % (uid, LABEL)], capture_output=True)
    r = subprocess.run(["launchctl", "bootstrap", "gui/%d" % uid, AGENT], capture_output=True,
                       text=True)
    if r.returncode:
        sys.exit("launchctl bootstrap: " + r.stderr)
    print("feedbackd: installed %s\n  url %s\n  token in %s (baked into the next port build)\n"
          "  reports in %s" % (AGENT, url, tok, STORE))


def uninstall():
    subprocess.run(["launchctl", "bootout", "gui/%d/%s" % (os.getuid(), LABEL)],
                   capture_output=True)
    if os.path.exists(AGENT):
        os.unlink(AGENT)
    print("feedbackd: removed the LaunchAgent (token and reports kept)")


def status():
    url, token = read_config("url"), read_config("token")
    if not url or not token:
        sys.exit("not installed: run feedbackd.py install")
    ping = url.replace("/v1/report", "/v1/ping")
    # curl, as the port sends: Python's resolver does not always find .local names
    r = subprocess.run(["curl", "-sS", "--max-time", "5", "-H", "Authorization: Bearer " + token,
                        ping], capture_output=True, text=True)
    if r.returncode == 0:
        print("feedbackd: %s answers %s" % (ping, r.stdout.strip()))
    else:
        print("feedbackd: %s does not answer: %s" % (ping, r.stderr.strip()))
    print("reports in", STORE)


def list_reports(n):
    path = os.path.join(STORE, "index.jsonl")
    if not os.path.exists(path):
        print("no reports yet")
        return
    lines = open(path).read().splitlines()[-n:]
    for line in lines:
        e = json.loads(line)
        print("%s  %-8s %s  %s" % (e["time"], e["kind"], e["dir"], e.get("summary", "")))


def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else "status"
    if cmd == "serve":
        serve()
    elif cmd == "init":
        init()
    elif cmd == "install":
        install()
    elif cmd == "uninstall":
        uninstall()
    elif cmd == "status":
        status()
    elif cmd == "list":
        list_reports(int(sys.argv[2]) if len(sys.argv) > 2 else 20)
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
