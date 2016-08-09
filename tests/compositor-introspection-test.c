/*
 * Copyright © 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>

#include "wayland-client.h"
#include "wayland-server.h"
#include "test-runner.h"

/* Ensure the connection doesn't fail due to lack of XDG_RUNTIME_DIR. */
static const char *
require_xdg_runtime_dir(void)
{
	char *val = getenv("XDG_RUNTIME_DIR");
	assert(val && "set $XDG_RUNTIME_DIR to run this test");

	return val;
}

struct compositor {
	struct wl_display *display;
	struct wl_listener listener;
	struct wl_client *client;
};

static void
client_created(struct wl_listener *listener, void *data)
{
	struct compositor *c = wl_container_of(listener, c, listener);
	c->client = data;
}

TEST(new_client_connect)
{
	const char *socket;
	struct compositor compositor = { 0 };
	struct {
		struct wl_display *display;
	} client;

	require_xdg_runtime_dir();

	compositor.display = wl_display_create();
	socket = wl_display_add_socket_auto(compositor.display);

	compositor.listener.notify = client_created;
	wl_display_add_client_created_listener(compositor.display, &compositor.listener);

	client.display = wl_display_connect(socket);

	wl_event_loop_dispatch(wl_display_get_event_loop(compositor.display), 100);

	assert(compositor.client != NULL);

	wl_display_disconnect(client.display);

	wl_client_destroy(compositor.client);
	wl_display_destroy(compositor.display);
}
