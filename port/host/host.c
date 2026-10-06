/* host.c: what the native build's host side shares (port_host.h): stopping SDL and the virtual
   PC, stopping on a stub, a fatal error or a fault with the call chain, and the check for a
   pointer that lost its top half. */
#include <execinfo.h>
#include <sys/ucontext.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <SDL3/SDL.h>

#include "port_host.h"
#include "port_vpc.h"

static int sdl_started;
static SDL_ThreadID main_thread;

void host_shutdown(void)
{
    if (sdl_started) {
        vpc_shutdown();
        SDL_Quit();
        sdl_started = 0;
    }
}

/* before stopping on a fatal error: SDL_Quit only from the main thread (on SDL's timer
   thread, where the SOS timer events run the game's callbacks, it would wait for itself) */
static void shutdown_for_abort(void)
{
    if (SDL_GetCurrentThreadID() == main_thread)
        host_shutdown();
}

/* the call chain on stderr, for a stub that stops the run or a fault */
static void backtrace_stderr(void)
{
    void *frames[48];
    int n = backtrace(frames, 48);

    backtrace_symbols_fd(frames, n, 2);
}

void port_unimplemented(const char *name)
{
    fprintf(stderr, "port: %s is not in the native build yet\n", name);
    backtrace_stderr();
    shutdown_for_abort();
    abort();
}

void port_fatal(const char *fmt, ...)
{
    va_list ap;

    fprintf(stderr, "port: ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    backtrace_stderr();
    shutdown_for_abort();
    abort();
}

void port_check_ptr(const void *p, const char *what)
{
    if (p != NULL && ((uintptr_t)p >> 32) == 0)
        port_fatal("%s: %p is a pointer cut to 32 bits", what, p);
}

/* a fault (often a pointer cut to 32 bits, docs/port.md): name it, show where, and end */
static void on_fault(int sig, siginfo_t *info, void *ucontext)
{
    static const char msg[] = "port: fatal signal, backtrace:\n";

    /* a read or write through a null pointer: DOS's low memory (zeropage.c) */
    if ((sig == SIGSEGV || sig == SIGBUS) && port_zero_page_fault(info, ucontext))
        return;
    if (info != NULL && ucontext != NULL) {
        char buf[160];
        int n = snprintf(buf, sizeof buf, "port: signal %d at address 0x%llX, pc 0x%llX\n", sig,
                         (unsigned long long)(uintptr_t)info->si_addr,
                         (unsigned long long)((ucontext_t *)ucontext)->uc_mcontext->__ss.__pc);
        write(2, buf, (size_t)n);
    }

    write(2, msg, sizeof msg - 1);
    backtrace_stderr();
    signal(sig, SIG_DFL);
    raise(sig);
}

void host_started(void)
{
    main_thread = SDL_GetCurrentThreadID();
    sdl_started = 1;
}

void host_install_fault_handlers(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = on_fault;
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    sigaction(SIGILL, &sa, NULL);
    sigaction(SIGFPE, &sa, NULL);
    sigaction(SIGTRAP, &sa, NULL);      /* libmalloc's checks (heap corruption) trap */
}
