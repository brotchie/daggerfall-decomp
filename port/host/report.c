/* report.c: feedback and crash reports (port_host.h, docs/port.md "Feedback and crash
   reports"). A report is a folder in the spool, <overlay>/reports/pending/<id>/:

     meta.json          what, when, which build, the player's notes, the game's facts
     screen.bmp         the window's picture (OpenGL's when it is on), screen320.bmp the VGA's
     log.txt            the session's stderr so far
     input.script       the session's keys, clicks and mouse motion as a PORT_SCRIPT timeline
                        (replay: PORT_SCRIPT="$(tr '\n' ';' < input.script)" PORT_SRAND=<seed>)
     memory/            the game's memory: FALL.EXE's data block in address order, the virtual
                        PC's low memory; fall.symbols maps the data block's addresses to names
     save/              the game saved into the report's own folder (save_game, slot 5
                        redirected) when the game is in the world
     crash.txt          a crash's signal, address and call chain

   A folder with a "ready" file is sent: tar and curl (no HTTP code here), to the URL and with
   the bearer token the build carries (tools/port_build.py from ~/.config/daggerfall-feedback,
   never in the repository). A build without them keeps the reports in the spool.

   Crashes are written with plain system calls from the fault handler (host.c); the process
   then execs itself as `fall --upload-reports SPOOL`, which sends the report and ends. */
#include <errno.h>
#include <fcntl.h>
#include <mach-o/dyld.h>
#include <spawn.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <dirent.h>
#include <execinfo.h>
#include <time.h>
#include <unistd.h>

#include <SDL3/SDL.h>

#include "port_host.h"
#include "port_vpc.h"

extern char **environ;

/* the build's feedback service (tools/port_build.py writes the definitions into gen/; weak
   here so a build without them, and the demos, link) */
__attribute__((weak)) const char port_feedback_url[] = "";
__attribute__((weak)) const char port_feedback_token[] = "";
__attribute__((weak)) const char port_build_commit[] = "unknown";

static char spool[1024];                /* <overlay>/reports */
static char pending_dir[1100], sent_dir[1100], session_dir[1100];
static char exe_path[1024];
static char symbols_path[1100];
static int log_fd = -1, input_fd = -1;
static SDL_Mutex *input_lock;
static int busy;                        /* a feedback report under way */

/* ---- small helpers (the crash path's are plain system calls) -------------------------------- */

static void put(int fd, const char *s)
{
    size_t n = strlen(s);

    while (n > 0) {
        ssize_t w = write(fd, s, n);
        if (w <= 0)
            return;
        s += w;
        n -= (size_t)w;
    }
}

/* decimal, for the signal handler (no stdio there) */
static void put_num(int fd, unsigned long long v, int base)
{
    char b[24];
    int i = 23;

    b[i] = '\0';
    do {
        b[--i] = "0123456789abcdef"[v % (unsigned)base];
        v /= (unsigned)base;
    } while (v && i > 0);
    put(fd, b + i);
}

static void copy_fd_to(int from, const char *to)
{
    char buf[65536];
    ssize_t n;
    int out = open(to, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (out < 0)
        return;
    lseek(from, 0, SEEK_SET);
    while ((n = read(from, buf, sizeof buf)) > 0)
        if (write(out, buf, (size_t)n) != n)
            break;
    close(out);
}

static void copy_file_to(const char *from, const char *to)
{
    int in = open(from, O_RDONLY);

    if (in < 0)
        return;
    copy_fd_to(in, to);
    close(in);
}

static void join(char *out, size_t size, const char *a, const char *b)
{
    snprintf(out, size, "%s/%s", a, b);
}

/* a new report folder's id: time, process, a count */
static void new_id(char *id, size_t size, const char *kind)
{
    static int count;
    time_t t = time(NULL);
    struct tm tm;

    localtime_r(&t, &tm);
    snprintf(id, size, "%04d%02d%02d-%02d%02d%02d-%d-%d-%s", tm.tm_year + 1900, tm.tm_mon + 1,
             tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (int)getpid(), ++count, kind);
}

/* a JSON string's contents */
static void json_escape(char *out, size_t size, const char *s)
{
    size_t k = 0;

    for (; *s && k + 7 < size; s++) {
        unsigned char c = (unsigned char)*s;

        if (c == '"' || c == '\\') {
            out[k++] = '\\';
            out[k++] = (char)c;
        } else if (c == '\n') {
            out[k++] = '\\';
            out[k++] = 'n';
        } else if (c < 0x20) {
            k += (size_t)snprintf(out + k, size - k, "\\u%04x", c);
        } else {
            out[k++] = (char)c;
        }
    }
    out[k] = '\0';
}

/* ---- the memory ---------------------------------------------------------------------------- */

#define MAX_REGIONS 8
static struct region {
    const char *name;
    const void *p;
    size_t size;
} regions[MAX_REGIONS];
static int nregions;

void report_add_region(const char *name, const void *p, size_t size)
{
    if (nregions < MAX_REGIONS) {
        regions[nregions].name = name;
        regions[nregions].p = p;
        regions[nregions].size = size;
        nregions++;
    }
}

/* memory/<region>.bin and memory/regions.txt (name, native address, size); plain calls */
static void write_memory(const char *dir)
{
    char path[1300], mem[1200];
    int i, fd;

    join(mem, sizeof mem, dir, "memory");
    mkdir(mem, 0755);
    join(path, sizeof path, mem, "regions.txt");
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    for (i = 0; i < nregions; i++) {
        int out;
        char file[1400];

        if (fd >= 0) {
            put(fd, regions[i].name);
            put(fd, " 0x");
            put_num(fd, (unsigned long long)(uintptr_t)regions[i].p, 16);
            put(fd, " ");
            put_num(fd, regions[i].size, 10);
            put(fd, "\n");
        }
        snprintf(file, sizeof file, "%s/%s.bin", mem, regions[i].name);
        out = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out >= 0) {
            const char *p = regions[i].p;
            size_t left = regions[i].size;

            while (left > 0) {
                ssize_t w = write(out, p, left > (1u << 20) ? (1u << 20) : left);
                if (w <= 0)
                    break;
                p += w;
                left -= (size_t)w;
            }
            close(out);
        }
    }
    if (fd >= 0)
        close(fd);
    if (symbols_path[0]) {
        join(path, sizeof path, mem, "fall.symbols");
        copy_file_to(symbols_path, path);
    }
}

/* ---- the session's log and input -------------------------------------------------------------- */

static int log_pipe[2] = {-1, -1}, real_stderr = -1;

/* stderr into a pipe; this thread copies it to the real stderr and the session's log */
static int tee_thread(void *arg)
{
    char buf[4096];
    ssize_t n;

    (void)arg;
    while ((n = read(log_pipe[0], buf, sizeof buf)) > 0) {
        if (write(real_stderr, buf, (size_t)n) < 0) {
            /* (nothing to do) */
        }
        if (log_fd >= 0 && write(log_fd, buf, (size_t)n) < 0) {
            /* (nothing to do) */
        }
    }
    return 0;
}

/* the script clock (vpc_script.c's): the times a replay's PORT_SCRIPT uses */
static double script_now(void)
{
    return (double)(SDL_GetTicksNS() - vpc_script_start_ns()) / 1e9;
}

void report_input(const char *step)
{
    char line[256];
    int n;

    if (input_fd < 0)
        return;
    n = snprintf(line, sizeof line, "%.3f %s\n", script_now(), step);
    if (input_lock)
        SDL_LockMutex(input_lock);
    if (write(input_fd, line, (size_t)n) < 0) {
        /* (nothing to do) */
    }
    if (input_lock)
        SDL_UnlockMutex(input_lock);
}

void report_note(const char *fmt, ...)
{
    char line[512];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(line, sizeof line, fmt, ap);
    va_end(ap);
    fprintf(stderr, "port: %s\n", line);
}

/* ---- sending ------------------------------------------------------------------------------- */

/* a program and its arguments, waited for; its exit status (or -1) */
static int run(char *const argv[], const char *out_path)
{
    pid_t pid;
    int status;
    posix_spawn_file_actions_t fa;

    posix_spawn_file_actions_init(&fa);
    if (out_path)
        posix_spawn_file_actions_addopen(&fa, 1, out_path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (posix_spawn(&pid, argv[0], &fa, NULL, argv, environ) != 0) {
        posix_spawn_file_actions_destroy(&fa);
        return -1;
    }
    posix_spawn_file_actions_destroy(&fa);
    while (waitpid(pid, &status, 0) < 0)
        if (errno != EINTR)
            return -1;
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

/* the kind from an id's last part */
static const char *kind_of(const char *id)
{
    const char *k = strrchr(id, '-');

    return k ? k + 1 : "feedback";
}

/* sends pending/<id> (tar.gz, curl); moves it to sent/ when the service took it */
static int send_one(const char *id)
{
    char dir[1300], tarball[1300], headers[1300], answer[1300], ready[1400];
    char auth[512], hid[200], hkind[64];
    int fd, ok;

    if (!port_feedback_url[0] || !port_feedback_token[0])
        return 0;
    join(dir, sizeof dir, pending_dir, id);
    snprintf(tarball, sizeof tarball, "%s.tar.gz", dir);
    snprintf(headers, sizeof headers, "%s.headers", dir);
    snprintf(answer, sizeof answer, "%s.answer", dir);
    {
        char *tar[] = {"/usr/bin/tar", "--no-mac-metadata", "--exclude", "ready", "-czf",
                       tarball, "-C", pending_dir, (char *)id, NULL};
        if (run(tar, NULL) != 0)
            return 0;
    }
    /* the token in a header file (0600), not on curl's command line where ps shows it */
    fd = open(headers, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0)
        return 0;
    snprintf(auth, sizeof auth, "Authorization: Bearer %s\n", port_feedback_token);
    snprintf(hid, sizeof hid, "X-Report-Id: %s\n", id);
    snprintf(hkind, sizeof hkind, "X-Report-Kind: %s\n", kind_of(id));
    put(fd, auth);
    put(fd, hid);
    put(fd, hkind);
    put(fd, "Content-Type: application/gzip\n");
    close(fd);
    {
        char hdr_arg[1400], data_arg[1400];
        char *curl[] = {"/usr/bin/curl", "-sS", "--fail", "--max-time", "300",
                        "-H", hdr_arg, "--data-binary", data_arg, (char *)port_feedback_url,
                        NULL};

        snprintf(hdr_arg, sizeof hdr_arg, "@%s", headers);
        snprintf(data_arg, sizeof data_arg, "@%s", tarball);
        ok = run(curl, answer) == 0;
    }
    unlink(headers);
    unlink(tarball);
    if (ok) {
        char to[1300];

        join(to, sizeof to, sent_dir, id);
        snprintf(ready, sizeof ready, "%s/ready", dir);
        unlink(ready);
        rename(dir, to);
        fprintf(stderr, "port: report %s sent\n", id);
    } else {
        fprintf(stderr, "port: report %s not sent (kept in %s)\n", id, pending_dir);
    }
    unlink(answer);
    return ok;
}

/* every pending report that is ready */
static int send_pending(void)
{
    DIR *d = opendir(pending_dir);
    struct dirent *e;
    int sent = 0;

    if (d == NULL)
        return 0;
    while ((e = readdir(d)) != NULL) {
        char ready[1400];
        struct stat st;

        if (e->d_name[0] == '.')
            continue;
        snprintf(ready, sizeof ready, "%s/%s/ready", pending_dir, e->d_name);
        if (stat(ready, &st) == 0)
            sent += send_one(e->d_name);
    }
    closedir(d);
    return sent;
}

static SDL_Mutex *send_lock;

static int sender_thread(void *arg)
{
    (void)arg;
    SDL_LockMutex(send_lock);
    send_pending();
    SDL_UnlockMutex(send_lock);
    return 0;
}

static void send_soon(void)
{
    SDL_Thread *t = SDL_CreateThread(sender_thread, "report-send", NULL);

    if (t)
        SDL_DetachThread(t);
}

int report_upload_main(const char *dir)
{
    snprintf(spool, sizeof spool, "%s", dir);
    join(pending_dir, sizeof pending_dir, spool, "pending");
    join(sent_dir, sizeof sent_dir, spool, "sent");
    mkdir(sent_dir, 0755);
    if (send_pending() > 0) {
        char *note[] = {"/usr/bin/osascript", "-e",
                        "display notification \"A crash report was sent.\" with title "
                        "\"Daggerfall\"", NULL};
        run(note, "/dev/null");
    }
    return 0;
}

/* after a crash the process ends as a crash still: 70 (EX_SOFTWARE) */
int report_upload_after_crash(const char *dir)
{
    report_upload_main(dir);
    return 70;
}

/* ---- start-up ------------------------------------------------------------------------------- */

void report_init(const char *overlay, const char *argv0)
{
    char path[1200];
    uint32_t size = sizeof exe_path;
    struct stat st;

    (void)argv0;
    join(spool, sizeof spool, overlay, "reports");
    join(pending_dir, sizeof pending_dir, spool, "pending");
    join(sent_dir, sizeof sent_dir, spool, "sent");
    join(session_dir, sizeof session_dir, spool, "session");
    mkdir(spool, 0755);
    mkdir(pending_dir, 0755);
    mkdir(sent_dir, 0755);
    mkdir(session_dir, 0755);
    input_lock = SDL_CreateMutex();
    send_lock = SDL_CreateMutex();

    if (_NSGetExecutablePath(exe_path, &size) != 0)
        exe_path[0] = '\0';
    /* the symbols: next to the executable (fall.symbols), or the app's Resources */
    snprintf(symbols_path, sizeof symbols_path, "%s.symbols", exe_path);
    if (stat(symbols_path, &st) != 0) {
        char *slash = strrchr(exe_path, '/');

        snprintf(symbols_path, sizeof symbols_path, "%.*s/../Resources/fall.symbols",
                 slash ? (int)(slash - exe_path) : 0, exe_path);
        if (stat(symbols_path, &st) != 0)
            symbols_path[0] = '\0';
    }

    /* the session: the last one's files go, this one's log and input */
    join(path, sizeof path, session_dir, "log.txt");
    log_fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    join(path, sizeof path, session_dir, "input.script");
    input_fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (pipe(log_pipe) == 0) {
        real_stderr = dup(2);
        dup2(log_pipe[1], 2);
        setvbuf(stderr, NULL, _IONBF, 0);
        SDL_DetachThread(SDL_CreateThread(tee_thread, "report-log", NULL));
    }
    fprintf(stderr, "port: reports in %s (%s)\n", spool,
            port_feedback_url[0] ? port_feedback_url : "no feedback service in this build");
    send_soon();                        /* what an earlier session left */
}

/* ---- the report's common parts ------------------------------------------------------------------ */

static void (*game_meta)(char *buf, size_t size);
static int (*game_can_save)(void);

void report_set_game_meta(void (*meta)(char *buf, size_t size))
{
    game_meta = meta;
}

void report_set_can_save(int (*can_save)(void))
{
    game_can_save = can_save;
}

static void write_meta(const char *dir, const char *id, const char *kind,
                       const char *description, const char *extra)
{
    char path[1300], desc[8192], game[2048] = "", host[256] = "";
    struct utsname u;
    FILE *f;
    time_t t = time(NULL);

    join(path, sizeof path, dir, "meta.json");
    f = fopen(path, "w");
    if (f == NULL)
        return;
    json_escape(desc, sizeof desc, description ? description : "");
    if (game_meta)
        game_meta(game, sizeof game);
    if (uname(&u) == 0)
        snprintf(host, sizeof host, "%s %s %s", u.sysname, u.release, u.machine);
    fprintf(f, "{\n  \"id\": \"%s\",\n  \"kind\": \"%s\",\n  \"time\": %lld,\n"
               "  \"commit\": \"%s\",\n  \"host\": \"%s\",\n  \"description\": \"%s\",\n"
               "  \"session_seconds\": %.1f,\n  \"game\": {%s}%s%s\n}\n",
            id, kind, (long long)t, port_build_commit, host, desc, script_now(), game,
            extra ? ",\n  " : "", extra ? extra : "");
    fclose(f);
}

static void mark_ready(const char *dir)
{
    char path[1300];
    int fd;

    join(path, sizeof path, dir, "ready");
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0)
        close(fd);
}

/* ---- feedback ------------------------------------------------------------------------------- */

static char save_request[1300];         /* the folder the game's thread saves into */
static SDL_AtomicInt save_state;        /* 0 none, 1 wanted, 2 done, 3 failed */
static char fb_dir[1300], fb_id[200];

const char *report_save_wanted(void)
{
    return SDL_GetAtomicInt(&save_state) == 1 ? save_request : NULL;
}

void report_save_done(int ok)
{
    SDL_SetAtomicInt(&save_state, ok ? 2 : 3);
}

/* the player's notes: a dialog of System Events (in front of everything), a few lines tall;
   NULL when cancelled */
static char *ask_notes(void)
{
    char answer[1300];
    char *argv[] = {
        "/usr/bin/osascript",
        "-e", "tell application \"System Events\"",
        "-e", "activate",
        "-e", "set r to display dialog \"What went wrong? (the screen, the game's memory, "
              "the session's input and, in the world, a save were captured when you pressed "
              "F8)\" default answer (linefeed & linefeed & linefeed & linefeed) "
              "buttons {\"Cancel\", \"Send\"} default button \"Send\" "
              "with title \"Daggerfall feedback\"",
        "-e", "return text returned of r",
        "-e", "end tell",
        NULL};
    FILE *f;
    char *text;
    long n;
    const char *given = getenv("PORT_FEEDBACK_NOTES");

    if (given)                          /* scripted runs: the notes without the dialog */
        return strdup(given);
    snprintf(answer, sizeof answer, "%s/notes.tmp", fb_dir);
    if (run(argv, answer) != 0) {
        unlink(answer);
        return NULL;
    }
    f = fopen(answer, "r");
    if (f == NULL)
        return NULL;
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    text = calloc(1, (size_t)n + 1);
    if (fread(text, 1, (size_t)n, f) != (size_t)n) {
        /* (what was read) */
    }
    fclose(f);
    unlink(answer);
    while (n > 0 && (text[n - 1] == '\n' || text[n - 1] == '\r' || text[n - 1] == ' '))
        text[--n] = '\0';
    {
        char *p = text;                 /* the leading blank lines of the default answer */
        while (*p == '\n' || *p == '\r')
            p++;
        memmove(text, p, strlen(p) + 1);
    }
    return text;
}

static void remove_tree(const char *dir)
{
    char *argv[] = {"/bin/rm", "-rf", (char *)dir, NULL};

    run(argv, NULL);
}

/* the rest of a feedback report, off the main thread: the game's save (or not), the notes,
   the upload */
static int feedback_thread(void *arg)
{
    char *notes;
    int k, state;
    char extra[256];

    (void)arg;
    /* the game's thread saves at its next frame of the world; not there, no save */
    fprintf(stderr, "port: feedback %s: captured%s\n", fb_id,
            SDL_GetAtomicInt(&save_state) == 1 ? ", waiting for the game's save" : "");
    for (k = 0; k < 300 && (state = SDL_GetAtomicInt(&save_state)) == 1; k++)
        SDL_Delay(10);
    if (state == 1) {
        SDL_SetAtomicInt(&save_state, 0);       /* the game did not come to save */
        rmdir(save_request);
    }
    snprintf(extra, sizeof extra, "\"save\": %s",
             state == 2 ? "\"save/ (slot 5's files: load with tools or copy to a SAVEn)\""
                        : "null");
    fprintf(stderr, "port: feedback %s: %s, asking for notes\n", fb_id,
            state == 2 ? "saved" : "no save");
    notes = ask_notes();
    if (notes == NULL) {
        fprintf(stderr, "port: feedback cancelled\n");
        remove_tree(fb_dir);
        busy = 0;
        return 0;
    }
    write_meta(fb_dir, fb_id, "feedback", notes, extra);
    free(notes);
    mark_ready(fb_dir);
    fprintf(stderr, "port: feedback %s %s\n", fb_id,
            port_feedback_url[0] ? "sending" : "kept (no service in this build)");
    SDL_LockMutex(send_lock);
    send_pending();
    SDL_UnlockMutex(send_lock);
    busy = 0;
    return 0;
}

void report_feedback(void)
{
    char path[1400];

    if (busy || spool[0] == '\0')
        return;
    busy = 1;
    new_id(fb_id, sizeof fb_id, "feedback");
    join(fb_dir, sizeof fb_dir, pending_dir, fb_id);
    mkdir(fb_dir, 0755);
    /* now: the screen, the memory, the log and input so far */
    join(path, sizeof path, fb_dir, "screen.bmp");
    vpc_screenshot(path);
    join(path, sizeof path, fb_dir, "screen320.bmp");
    vpc_screenshot_vga(path);
    write_memory(fb_dir);
    join(path, sizeof path, fb_dir, "log.txt");
    copy_fd_to(log_fd, path);
    join(path, sizeof path, fb_dir, "input.script");
    SDL_LockMutex(input_lock);
    copy_fd_to(input_fd, path);
    lseek(input_fd, 0, SEEK_END);
    SDL_UnlockMutex(input_lock);
    lseek(log_fd, 0, SEEK_END);
    /* the save, from the game's thread, when it is in its frame loop (not the title or
       character creation: nothing to save there) */
    if (game_can_save == NULL || game_can_save()) {
        join(save_request, sizeof save_request, fb_dir, "save");
        mkdir(save_request, 0755);
        SDL_SetAtomicInt(&save_state, 1);
    } else {
        SDL_SetAtomicInt(&save_state, 0);
    }
    /* the notes need the mouse: let the window go of it */
    vpc_input_release_mouse();
    SDL_DetachThread(SDL_CreateThread(feedback_thread, "report-feedback", NULL));
}

/* ---- crashes ---------------------------------------------------------------------------------- */

void report_crash(const char *what, int sig, const void *addr, const void *pc)
{
    static int once;
    char dir[1300], path[1400];
    char id[200];
    int fd;
    time_t t = time(NULL);

    if (once++ || spool[0] == '\0')
        return;
    /* (the id without localtime: a signal handler's) */
    snprintf(id, sizeof id, "%lld-%d-crash", (long long)t, (int)getpid());
    join(dir, sizeof dir, pending_dir, id);
    mkdir(dir, 0755);
    join(path, sizeof path, dir, "crash.txt");
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        void *frames[64];
        int n = backtrace(frames, 64);

        put(fd, what);
        put(fd, "\nsignal ");
        put_num(fd, (unsigned long long)sig, 10);
        put(fd, "\naddress 0x");
        put_num(fd, (unsigned long long)(uintptr_t)addr, 16);
        put(fd, "\npc 0x");
        put_num(fd, (unsigned long long)(uintptr_t)pc, 16);
        put(fd, "\ncommit ");
        put(fd, port_build_commit);
        put(fd, "\n\n");
        backtrace_symbols_fd(frames, n, fd);
        close(fd);
    }
    /* the meta by hand: no stdio here */
    join(path, sizeof path, dir, "meta.json");
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        put(fd, "{\n  \"id\": \"");
        put(fd, id);
        put(fd, "\",\n  \"kind\": \"crash\",\n  \"time\": ");
        put_num(fd, (unsigned long long)t, 10);
        put(fd, ",\n  \"commit\": \"");
        put(fd, port_build_commit);
        put(fd, "\",\n  \"crash\": \"signal ");
        put_num(fd, (unsigned long long)sig, 10);
        put(fd, ": see crash.txt\"\n}\n");
        close(fd);
    }
    /* the VGA's picture as raw bytes (screen320.raw, 320x200 indices; palette.raw: 768 6-bit
       components): the uploader's side turns them into a picture */
    join(path, sizeof path, dir, "screen320.raw");
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        if (write(fd, port_low_memory + 0xA0000, 64000) < 0) {
            /* (nothing to do) */
        }
        close(fd);
    }
    join(path, sizeof path, dir, "palette.raw");
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        if (write(fd, vpc_video_dac(), 768) < 0) {
            /* (nothing to do) */
        }
        close(fd);
    }
    write_memory(dir);
    if (log_fd >= 0) {
        join(path, sizeof path, dir, "log.txt");
        copy_fd_to(log_fd, path);
    }
    if (input_fd >= 0) {
        join(path, sizeof path, dir, "input.script");
        copy_fd_to(input_fd, path);
    }
    mark_ready(dir);
    if (real_stderr >= 0) {
        put(real_stderr, "port: crash report ");
        put(real_stderr, dir);
        put(real_stderr, "\n");
    }
    /* become the uploader: it sends the report and ends (exec is safe in a handler) */
    if (port_feedback_url[0] && exe_path[0]) {
        char *argv[] = {exe_path, "--upload-reports", spool, "--after-crash", NULL};

        execve(exe_path, argv, environ);
    }
}
