#include <ev.h>
#include <stdio.h>
#include <stdlib.h>

/* Watchers */
ev_io stdin_watcher;
ev_timer timeout_watcher;

/* callback for watcher */
static void
stdin_cb(EV_P_ ev_io *w, int revents)
{
	puts("stdin ready");

	/* One shot event, stop the watcher */
	ev_io_stop(EV_A_ w);

	/* stop all nested ev_runs */
	ev_break(EV_A_ EVBREAK_ALL);
}

/* callback for timeout */
static void
timeout_cb(EV_P_ ev_timer *w, int revents)
{
	puts("timeout");

	/* stop innermost ev_run */
	ev_break(EV_A_ EVBREAK_ONE);
}

int
main(void)
{
	/* use default loop */
	struct ev_loop *loop = EV_DEFAULT;

	/* Initialize io watch */
	//ev_io_init (&stdin_watcher, stdin_cb, 0, EV_READ);
	//ev_io_start (loop, &stdin_watcher);

	/* Initialize timer watch */
	ev_timer_init(&timeout_watcher, timeout_cb, 1, 0.);
	ev_timer_start(loop, &timeout_watcher);

	/* run hdparm command */
	if (system("hdparm -t /dev/sda") == -1)
		puts("Error calling system");

	/* wait for events */
	ev_run(loop, 0);


	return 0;
}
