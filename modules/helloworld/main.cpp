typedef struct {
	void (*fb_printf)(char *str, ...);
} env_t;

const char message[] = "Привет из модуля!";

extern "C" long long _start(env_t *env) {
	env->fb_printf("[hello]message=[%s]\n", message);
	return (long long)&message;
}