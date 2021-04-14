#include <time.h>

#define ONE_E9 1000000000

static long long last;
static long long frame;

static long long time_get(void) {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return ts.tv_nsec + (long long) ONE_E9 * ts.tv_sec;
}

static void time_wait(long long t) {
	struct timespec ts = {t / ONE_E9, t % ONE_E9};
	nanosleep(&ts, NULL);
}

void fps_limit(int fps) {
	frame = ONE_E9 / fps;
	last = time_get();
}

void fps_frame(void) {
	long long now;

	now = time_get();
	if (now >= last && now < last + frame) {
		time_wait(last + frame - now);
		last += frame;
	}
	else {
		last = now;
	}
}
