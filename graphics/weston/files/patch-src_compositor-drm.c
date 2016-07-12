--- src/compositor-drm.c.orig	2015-09-14 20:23:31 +0200
+++ src/compositor-drm.c
@@ -33,7 +33,14 @@
 #include <fcntl.h>
 #include <unistd.h>
 #include <linux/input.h>
+#if defined(__FreeBSD__)
+#include <sys/consio.h>
+#include <sys/mouse.h>
+#include <sys/ioctl.h>
+#include <sys/socket.h>
+#else
 #include <linux/vt.h>
+#endif
 #include <assert.h>
 #include <sys/mman.h>
 #include <dlfcn.h>
@@ -44,19 +51,34 @@
 #include <drm_fourcc.h>
 
 #include <gbm.h>
+#if defined(__FreeBSD__)
+#include <devattr.h>
+#else
 #include <libudev.h>
+#endif
 
+#if defined(__FreeBSD__)
+#include "kbdev.h"
+#endif
 #include "shared/helpers.h"
 #include "shared/timespec-util.h"
+#if !defined(__FreeBSD__)
 #include "libbacklight.h"
+#endif
 #include "compositor.h"
 #include "gl-renderer.h"
 #include "pixman-renderer.h"
+#if defined(__FreeBSD__)
+#include "libinput-device.h"
+#else
 #include "libinput-seat.h"
+#endif
 #include "launcher-util.h"
 #include "vaapi-recorder.h"
 #include "presentation_timing-server-protocol.h"
+//#if !defined(__FreeBSD__)
 #include "linux-dmabuf.h"
+//#endif
 
 #ifndef DRM_CAP_TIMESTAMP_MONOTONIC
 #define DRM_CAP_TIMESTAMP_MONOTONIC 0x6
@@ -89,11 +111,23 @@
 	struct weston_backend base;
 	struct weston_compositor *compositor;
 
+#if defined(__FreeBSD__)
+	struct weston_seat syscons_seat;
+
+	int sysmouse_fd;
+	struct wl_event_source *sysmouse_source;
+
+	int kbd_fd;
+	struct kbdev_state *kbdst;
+	struct wl_event_source *kbd_source;
+#endif
 	struct udev *udev;
 	struct wl_event_source *drm_source;
 
+#if !defined(__FreeBSD__)
 	struct udev_monitor *udev_monitor;
 	struct wl_event_source *udev_drm_source;
+#endif
 
 	struct {
 		int id;
@@ -126,7 +160,9 @@
 
 	uint32_t prev_state;
 
+#if !defined(__FreeBSD__)
 	struct udev_input input;
+#endif
 
 	int32_t cursor_width;
 	int32_t cursor_height;
@@ -184,7 +220,9 @@
 	struct weston_view *cursor_view;
 	int current_cursor;
 	struct drm_fb *current, *next;
+#if !defined(__FreeBSD__)
 	struct backlight *backlight;
+#endif
 
 	struct drm_fb *dumb[2];
 	pixman_image_t *image[2];
@@ -237,6 +275,10 @@
 static void
 drm_output_update_msc(struct drm_output *output, unsigned int seq);
 
+static void
+page_flip_handler(int fd, unsigned int frame,
+		  unsigned int sec, unsigned int usec, void *data);
+
 static int
 drm_sprite_crtc_supported(struct drm_output *output, uint32_t supported)
 {
@@ -391,7 +433,7 @@
 				    format, handles, pitches, offsets,
 				    &fb->fb_id, 0);
 		if (ret) {
-			weston_log("addfb2 failed: %m\n");
+			weston_log("addfb2 failed: %s\n", strerror(errno));
 			backend->no_addfb2 = 1;
 			backend->sprites_are_broken = 1;
 		}
@@ -402,7 +444,7 @@
 				   fb->stride, fb->handle, &fb->fb_id);
 
 	if (ret) {
-		weston_log("failed to create kms fb: %m\n");
+		weston_log("failed to create kms fb: %s\n", strerror(errno));
 		goto err_free;
 	}
 
@@ -532,7 +574,8 @@
 
 	bo = gbm_surface_lock_front_buffer(output->surface);
 	if (!bo) {
-		weston_log("failed to lock front buffer: %m\n");
+		weston_log("failed to lock front buffer: %s\n",
+		    strerror(errno));
 		return;
 	}
 
@@ -604,7 +647,7 @@
 				 output->crtc_id,
 				 size, r, g, b);
 	if (rc)
-		weston_log("set gamma failed: %m\n");
+		weston_log("set gamma failed: %s\n", strerror(errno));
 }
 
 /* Determine the type of vblank synchronization to use for the output.
@@ -659,7 +702,7 @@
 				     &output->connector_id, 1,
 				     &mode->mode_info);
 		if (ret) {
-			weston_log("set mode failed: %m\n");
+			weston_log("set mode failed: %s\n", strerror(errno));
 			goto err_pageflip;
 		}
 		output_base->set_dpms(output_base, WESTON_DPMS_ON);
@@ -668,7 +711,7 @@
 	if (drmModePageFlip(backend->drm.fd, output->crtc_id,
 			    output->next->fb_id,
 			    DRM_MODE_PAGE_FLIP_EVENT, output) < 0) {
-		weston_log("queueing pageflip failed: %m\n");
+		weston_log("queueing pageflip failed: %s\n", strerror(errno));
 		goto err_pageflip;
 	}
 
@@ -781,16 +824,18 @@
 						PRESENTATION_FEEDBACK_INVALID);
 			return;
 		}
-	}
+	} else
+		weston_log("failed to get valid timestamp, ret=%d\n", ret);
 
 	/* Immediate query didn't provide valid timestamp.
 	 * Use pageflip fallback.
 	 */
 	fb_id = output->current->fb_id;
 
+	weston_log("Using page flip fallback\n");
 	if (drmModePageFlip(backend->drm.fd, output->crtc_id, fb_id,
 			    DRM_MODE_PAGE_FLIP_EVENT, output) < 0) {
-		weston_log("queueing pageflip failed: %m\n");
+		weston_log("queueing pageflip failed: %s\n", strerror(errno));
 		goto finish_frame;
 	}
 
@@ -864,9 +909,11 @@
 
 	output->page_flip_pending = 0;
 
-	if (output->destroy_pending)
+	if (output->destroy_pending) {
 		drm_output_destroy(&output->base);
-	else if (!output->vblank_pending) {
+		return;
+	}
+	if (!output->vblank_pending) {
 		ts.tv_sec = sec;
 		ts.tv_nsec = usec * 1000;
 		weston_output_finish_frame(&output->base, &ts, flags);
@@ -1154,7 +1201,7 @@
 	wl_shm_buffer_end_access(buffer->shm_buffer);
 
 	if (gbm_bo_write(bo, buf, sizeof buf) < 0)
-		weston_log("failed update cursor: %m\n");
+		weston_log("failed update cursor: %s\n", strerror(errno));
 }
 
 static void
@@ -1187,7 +1234,8 @@
 		handle = gbm_bo_get_handle(bo).s32;
 		if (drmModeSetCursor(b->drm.fd, output->crtc_id, handle,
 				b->cursor_width, b->cursor_height)) {
-			weston_log("failed to set cursor: %m\n");
+			weston_log("failed to set cursor: %s\n",
+			    strerror(errno));
 			b->cursors_are_broken = 1;
 		}
 	}
@@ -1196,7 +1244,8 @@
 	y = (ev->geometry.y - output->base.y) * output->base.current_scale;
 	if (output->cursor_plane.x != x || output->cursor_plane.y != y) {
 		if (drmModeMoveCursor(b->drm.fd, output->crtc_id, x, y)) {
-			weston_log("failed to move cursor: %m\n");
+			weston_log("failed to move cursor: %s\n",
+			    strerror(errno));
 			b->cursors_are_broken = 1;
 		}
 
@@ -1305,8 +1354,10 @@
 		return;
 	}
 
+#if !defined(__FreeBSD__)
 	if (output->backlight)
 		backlight_destroy(output->backlight);
+#endif
 
 	drmModeFreeProperty(output->dpms_prop);
 
@@ -1456,20 +1507,32 @@
 static int
 init_drm(struct drm_backend *b, struct udev_device *device)
 {
+	struct _drmSetVersion ver;
 	const char *filename, *sysnum;
 	uint64_t cap;
 	int fd, ret;
 	clockid_t clk_id;
 
+#if defined(__FreeBSD__)
+	b->drm.id = udev_device_get_devnum(device);
+	if (b->drm.id < 0) {
+#else
 	sysnum = udev_device_get_sysnum(device);
 	if (sysnum)
 		b->drm.id = atoi(sysnum);
 	if (!sysnum || b->drm.id < 0) {
+#endif
 		weston_log("cannot get device sysnum\n");
 		return -1;
 	}
 
 	filename = udev_device_get_devnode(device);
+#if defined(__FreeBSD__)
+	char buf[128];
+	memset(buf, 0, sizeof(buf));
+	snprintf(buf, sizeof(buf) - 1, "%s/%s", udev_get_dev_path(b->udev), filename);
+	filename = buf;
+#endif
 	fd = weston_launcher_open(b->compositor->launcher, filename, O_RDWR);
 	if (fd < 0) {
 		/* Probably permissions error */
@@ -1483,6 +1546,20 @@
 	b->drm.fd = fd;
 	b->drm.filename = strdup(filename);
 
+	ver.drm_di_major = 1;
+	ver.drm_di_minor = 1;
+	ver.drm_dd_major = -1;
+	ver.drm_dd_minor = -1;
+	/*
+	 * This call is needed for the PCI address of the graphics device to
+	 * be appended to the hw.dri.X.name sysctl node.
+	 */
+	ret = drmSetInterfaceVersion(fd, &ver);
+	if (ret != 0) {
+		weston_log("Error: failed to set drm interface version.\n");
+		return -1;
+	}
+
 	ret = drmGetCap(fd, DRM_CAP_TIMESTAMP_MONOTONIC, &cap);
 	if (ret == 0 && cap == 1)
 		clk_id = CLOCK_MONOTONIC;
@@ -1490,8 +1567,13 @@
 		clk_id = CLOCK_REALTIME;
 
 	if (weston_compositor_set_presentation_clock(b->compositor, clk_id) < 0) {
+#if defined(__FreeBSD__)
+		weston_log("Error: failed to set presentation clock %ld.\n",
+			   clk_id);
+#else
 		weston_log("Error: failed to set presentation clock %d.\n",
 			   clk_id);
+#endif
 		return -1;
 	}
 
@@ -1666,6 +1748,7 @@
 	}
 }
 
+#if !defined(__FreeBSD__)
 /* returns a value between 0-255 range, where higher is brighter */
 static uint32_t
 drm_get_backlight(struct drm_output *output)
@@ -1701,6 +1784,7 @@
 
 	backlight_set_brightness(output->backlight, new_brightness);
 }
+#endif
 
 static drmModePropertyPtr
 drm_get_prop(int fd, drmModeConnectorPtr connector, const char *name)
@@ -2105,6 +2189,7 @@
 	return 0;
 }
 
+#if !defined(__FreeBSD__)
 static void
 setup_output_seat_constraint(struct drm_backend *b,
 			     struct weston_output *output,
@@ -2127,6 +2212,7 @@
 					     &pointer->y);
 	}
 }
+#endif
 
 static int
 get_gbm_format_from_section(struct weston_config_section *section,
@@ -2338,8 +2424,10 @@
 					&output->format) == -1)
 		output->format = b->format;
 
+#if !defined(__FreeBSD__)
 	weston_config_section_get_string(section, "seat", &s, "");
 	setup_output_seat_constraint(b, &output->base, s);
+#endif
 	free(s);
 
 	output->crtc_id = resources->crtcs[i];
@@ -2389,6 +2477,7 @@
 		goto err_output;
 	}
 
+#if !defined(__FreeBSD__)
 	output->backlight = backlight_init(drm_device,
 					   connector->connector_type);
 	if (output->backlight) {
@@ -2399,6 +2488,7 @@
 	} else {
 		weston_log("Failed to initialize backlight\n");
 	}
+#endif
 
 	weston_compositor_add_output(b->compositor, &output->base);
 
@@ -2591,6 +2681,7 @@
 	return 0;
 }
 
+#if !defined(__FreeBSD__)
 static void
 update_outputs(struct drm_backend *b, struct udev_device *drm_device)
 {
@@ -2693,6 +2784,197 @@
 
 	return 1;
 }
+#endif
+
+#if defined(__FreeBSD__)
+static int
+drm_kbd_handler(int fd, uint32_t mask, void *data)
+{
+	int i, n;
+	struct kbdev_event evs[64];
+
+	struct drm_backend *b = (struct drm_backend *)data;
+
+	fcntl(b->kbd_fd, F_SETFL, O_NONBLOCK);
+	n = kbdev_read_events(b->kbdst, evs, 64);
+	if (n <= 0)
+		return 0;
+
+	for (i = 0; i < n; i++) {
+		notify_key(&b->syscons_seat, weston_compositor_get_time(),
+		    evs[i].keycode,
+		    evs[i].pressed ? WL_KEYBOARD_KEY_STATE_PRESSED
+				   : WL_KEYBOARD_KEY_STATE_RELEASED,
+		    STATE_UPDATE_AUTOMATIC);
+	}
+
+	return 1;
+}
+
+static int
+drm_sysmouse_handler(int fd, uint32_t mask, void *data)
+{
+	char buf[128];
+	int xdelta, ydelta, zdelta;
+	wl_fixed_t xf, yf;
+	int nm;
+	static int oldmask = 7;
+	uint32_t time;
+	int k, n;
+
+	struct drm_backend *b = (struct drm_backend *)data;
+
+	k = 0;
+	if ((n = read(b->sysmouse_fd, buf, sizeof(buf))) <= 0) {
+		if (n < 0 && errno != EAGAIN)
+			weston_log("failed to read from sysmouse fd: %s\n",
+			    strerror(errno));
+		return 0;
+	}
+
+	for (k = 0; k < n; k += 8) {
+		if (n -k < 8 || (buf[0] & 0x80) == 0 || (buf[7] & 0x80) != 0)
+			continue;
+
+		xdelta = buf[k+1] + buf[k+3];
+		ydelta = buf[k+2] + buf[k+4];
+		ydelta = -ydelta;
+		zdelta = (buf[k+5] > 0 && buf[k+6] == 0) ? buf[k+5] | 0x80 : buf[k+5] + buf[k+6];
+
+		time = weston_compositor_get_time();
+
+		notify_axis(&b->syscons_seat, weston_compositor_get_time(),
+		    WL_POINTER_AXIS_VERTICAL_SCROLL,
+		    wl_fixed_from_int(((char)zdelta)*10));
+
+		if (xdelta != 0 || ydelta != 0) {
+			xf = wl_fixed_from_int(xdelta);
+			yf = wl_fixed_from_int(ydelta);
+			notify_motion(&b->syscons_seat, time, xf, yf);
+		}
+
+		nm = buf[k+0] & 7;
+		if (nm != oldmask) {
+			if ((nm & 4) != (oldmask & 4)) {
+				notify_button(&b->syscons_seat, time, BTN_LEFT,
+				    (nm & 4) ? WL_POINTER_BUTTON_STATE_RELEASED
+					     : WL_POINTER_BUTTON_STATE_PRESSED);
+			}
+			if ((nm & 2) != (oldmask & 2)) {
+				notify_button(&b->syscons_seat, time, BTN_MIDDLE,
+				    (nm & 2) ? WL_POINTER_BUTTON_STATE_RELEASED
+					     : WL_POINTER_BUTTON_STATE_PRESSED);
+			}
+			if ((nm & 1) != (oldmask & 1)) {
+				notify_button(&b->syscons_seat, time, BTN_RIGHT,
+				    (nm & 1) ? WL_POINTER_BUTTON_STATE_RELEASED
+					     : WL_POINTER_BUTTON_STATE_PRESSED);
+			}
+			oldmask = nm;
+		}
+	}
+
+	return 1;
+}
+
+static int
+drm_input_sysmouse_open(struct drm_backend *b)
+{
+	if (b->sysmouse_fd == -1) {
+		b->sysmouse_fd = weston_launcher_open(b->compositor->launcher,
+		    "/dev/sysmouse", O_RDONLY | O_CLOEXEC);
+		if (b->sysmouse_fd < 0)
+			return -1;
+	}
+
+	int lvl = 1;
+	fcntl(b->sysmouse_fd, F_SETFL, O_NONBLOCK);
+	ioctl(b->sysmouse_fd, MOUSE_SETLEVEL, &lvl);
+
+	return 0;
+}
+
+static int
+drm_input_create(struct drm_backend *b)
+{
+	if (drm_input_sysmouse_open(b) != 0)
+		return -1;
+
+	if (b->kbd_fd == -1) {
+		b->kbd_fd = weston_launcher_ttyfd(b->compositor->launcher);
+		printf("tty fd from weston-launcher: %d\n", b->kbd_fd);
+		if (b->kbd_fd < 0) {
+			close(b->sysmouse_fd);
+			return -1;
+		}
+	}
+
+	fcntl(b->kbd_fd, F_SETFL, O_NONBLOCK);
+	b->kbdst = kbdev_new_state(b->kbd_fd);
+	if (b->kbdst == NULL)
+		return -1;
+
+	weston_seat_init(&b->syscons_seat, b->compositor, "syscons");
+
+	weston_seat_init_pointer(&b->syscons_seat);
+	weston_seat_init_keyboard(&b->syscons_seat, NULL);
+
+	notify_motion_absolute(&b->syscons_seat, weston_compositor_get_time(),
+	    50, 50);
+
+	b->sysmouse_source = wl_event_loop_add_fd(
+	    wl_display_get_event_loop(b->compositor->wl_display),
+	    b->sysmouse_fd, WL_EVENT_READABLE, drm_sysmouse_handler, b);
+
+	b->kbd_source = wl_event_loop_add_fd(
+	    wl_display_get_event_loop(b->compositor->wl_display),
+	    b->kbd_fd, WL_EVENT_READABLE, drm_kbd_handler, b);
+
+	return 0;
+}
+
+static void
+drm_input_enable(struct drm_backend *b)
+{
+	struct wl_array keys;
+	struct wl_event_loop *loop =
+	    wl_display_get_event_loop(b->compositor->wl_display);
+
+	/* Re-enable keyboard input */
+	b->kbd_source = wl_event_loop_add_fd(
+	    wl_display_get_event_loop(b->compositor->wl_display),
+	    b->kbd_fd, WL_EVENT_READABLE, drm_kbd_handler, b);
+	kbdev_reset_state(b->kbdst);
+	wl_array_init(&keys);
+	notify_keyboard_focus_in(&b->syscons_seat, &keys, STATE_UPDATE_AUTOMATIC);
+	wl_array_release(&keys);
+
+	/* Re-enable mouse input */
+	if (drm_input_sysmouse_open(b) == 0) {
+		b->sysmouse_source = wl_event_loop_add_fd(loop,
+		    b->sysmouse_fd, WL_EVENT_READABLE,
+		    drm_sysmouse_handler, b);
+	} else {
+		weston_log("Re-initializing sysmouse input failed\n");
+	}
+}
+
+static void
+drm_input_disable(struct drm_backend *b)
+{
+	if (b->sysmouse_source != NULL) {
+		wl_event_source_remove(b->sysmouse_source);
+		b->sysmouse_source = NULL;
+	}
+	if (b->kbd_source != NULL) {
+		wl_event_source_remove(b->kbd_source);
+		b->kbd_source = NULL;
+	}
+	notify_keyboard_focus_out(&b->syscons_seat);
+	close(b->sysmouse_fd);
+	b->sysmouse_fd = -1;
+}
+#endif
 
 static void
 drm_restore(struct weston_compositor *ec)
@@ -2705,9 +2987,15 @@
 {
 	struct drm_backend *b = (struct drm_backend *) ec->backend;
 
+#if defined(__FreeBSD__)
+	wl_event_source_remove(b->sysmouse_source);
+	wl_event_source_remove(b->kbd_source);
+	close(b->sysmouse_fd);
+#else
 	udev_input_destroy(&b->input);
 
 	wl_event_source_remove(b->udev_drm_source);
+#endif
 	wl_event_source_remove(b->drm_source);
 
 	destroy_sprites(b);
@@ -2749,9 +3037,10 @@
 				     &drm_mode->mode_info);
 		if (ret < 0) {
 			weston_log(
-				"failed to set mode %dx%d for output at %d,%d: %m\n",
+				"failed to set mode %dx%d for output at %d,%d: %s\n",
 				drm_mode->base.width, drm_mode->base.height,
-				output->base.x, output->base.y);
+				output->base.x, output->base.y,
+				strerror(errno));
 		}
 	}
 }
@@ -2769,10 +3058,18 @@
 		compositor->state = b->prev_state;
 		drm_backend_set_modes(b);
 		weston_compositor_damage_all(compositor);
+#if defined(__FreeBSD__)
+		drm_input_enable(b);
+#else
 		udev_input_enable(&b->input);
+#endif
 	} else {
 		weston_log("deactivating session\n");
+#if defined(__FreeBSD__)
+		drm_input_disable(b);
+#else
 		udev_input_disable(&b->input);
+#endif
 
 		b->prev_state = compositor->state;
 		weston_compositor_offscreen(compositor);
@@ -2807,6 +3104,8 @@
 {
 	struct weston_compositor *compositor = data;
 
+	weston_log("trying to vt switch to %d\n", key - KEY_F1 + 1);
+
 	weston_launcher_activate_vt(compositor->launcher, key - KEY_F1 + 1);
 }
 
@@ -2818,24 +3117,42 @@
  * If no such device is found, the first DRM device reported by udev is used.
  */
 static struct udev_device*
+#if defined(__FreeBSD__)
+find_primary_gpu(struct drm_backend *b)
+#else
 find_primary_gpu(struct drm_backend *b, const char *seat)
+#endif
 {
 	struct udev_enumerate *e;
 	struct udev_list_entry *entry;
+#if defined(__FreeBSD__)
+	struct udev_device *device, *drm_device;
+#else
 	const char *path, *device_seat, *id;
 	struct udev_device *device, *drm_device, *pci;
+#endif
 
 	e = udev_enumerate_new(b->udev);
+#if defined(__FreeBSD__)
+	udev_enumerate_add_match_expr(e, "driver", "drm");
+	udev_enumerate_add_match_regex(e, "name", "card[0-9]*");
+#else
 	udev_enumerate_add_match_subsystem(e, "drm");
 	udev_enumerate_add_match_sysname(e, "card[0-9]*");
+#endif
 
 	udev_enumerate_scan_devices(e);
 	drm_device = NULL;
 	udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(e)) {
+#if defined(__FreeBSD__)
+		device = udev_list_entry_get_device(entry);
+#else
 		path = udev_list_entry_get_name(entry);
 		device = udev_device_new_from_syspath(b->udev, path);
+#endif
 		if (!device)
 			continue;
+#if !defined(__FreeBSD__)
 		device_seat = udev_device_get_property_value(device, "ID_SEAT");
 		if (!device_seat)
 			device_seat = default_seat;
@@ -2855,6 +3172,7 @@
 				break;
 			}
 		}
+#endif
 
 		if (!drm_device)
 			drm_device = device;
@@ -2925,7 +3243,7 @@
 	ret = vaapi_recorder_frame(output->recorder, fd,
 				   output->current->stride);
 	if (ret < 0) {
-		weston_log("[libva recorder] aborted: %m\n");
+		weston_log("[libva recorder] aborted: %s\n", strerror(errno));
 		recorder_destroy(output);
 	}
 }
@@ -3065,11 +3383,15 @@
 	uint32_t key;
 
 	weston_log("initializing drm backend\n");
+	loop = wl_display_get_event_loop(compositor->wl_display);
 
 	b = zalloc(sizeof *b);
 	if (b == NULL)
 		return NULL;
 
+	b->sysmouse_fd = -1;
+	b->kbd_fd = -1;
+
 	/*
 	 * KMS support for hardware planes cannot properly synchronize
 	 * without nuclear page flip. Without nuclear/atomic, hw plane
@@ -3109,12 +3431,21 @@
 	b->session_listener.notify = session_notify;
 	wl_signal_add(&compositor->session_signal, &b->session_listener);
 
+#if defined(__FreeBSD__)
+	drm_device = find_primary_gpu(b);
+#else
 	drm_device = find_primary_gpu(b, param->seat_id);
+#endif
 	if (drm_device == NULL) {
 		weston_log("no drm device found\n");
 		goto err_udev;
 	}
+#if defined(__FreeBSD__)
+	/* XXX no sysfs on DragonFly, just use devnode for log output */
+	path = udev_device_get_devnode(drm_device);
+#else
 	path = udev_device_get_syspath(drm_device);
+#endif
 
 	if (init_drm(b, drm_device) < 0) {
 		weston_log("failed to initialize kms\n");
@@ -3138,7 +3469,7 @@
 
 	b->prev_state = WESTON_COMPOSITOR_ACTIVE;
 
-	for (key = KEY_F1; key < KEY_F9; key++)
+	for (key = KEY_F1; key <= KEY_F10; key++)
 		weston_compositor_add_key_binding(compositor, key,
 						  MODIFIER_CTRL | MODIFIER_ALT,
 						  switch_vt_binding, compositor);
@@ -3146,8 +3477,12 @@
 	wl_list_init(&b->sprite_list);
 	create_sprites(b);
 
+#if defined(__FreeBSD__)
+	if (drm_input_create(b) < 0) {
+#else
 	if (udev_input_init(&b->input,
 			    compositor, b->udev, param->seat_id) < 0) {
+#endif
 		weston_log("failed to create input devices\n");
 		goto err_sprite;
 	}
@@ -3164,11 +3499,11 @@
 
 	path = NULL;
 
-	loop = wl_display_get_event_loop(compositor->wl_display);
 	b->drm_source =
 		wl_event_loop_add_fd(loop, b->drm.fd,
 				     WL_EVENT_READABLE, on_drm_input, b);
 
+#if !defined(__FreeBSD__)
 	b->udev_monitor = udev_monitor_new_from_netlink(b->udev, "udev");
 	if (b->udev_monitor == NULL) {
 		weston_log("failed to intialize udev monitor\n");
@@ -3185,6 +3520,7 @@
 		weston_log("failed to enable udev-monitor receiving\n");
 		goto err_udev_monitor;
 	}
+#endif
 
 	udev_device_unref(drm_device);
 
@@ -3209,13 +3545,17 @@
 
 	return b;
 
+#if !defined(__FreeBSD__)
 err_udev_monitor:
 	wl_event_source_remove(b->udev_drm_source);
 	udev_monitor_unref(b->udev_monitor);
 err_drm_source:
+#endif
 	wl_event_source_remove(b->drm_source);
 err_udev_input:
+#if !defined(__FreeBSD__)
 	udev_input_destroy(&b->input);
+#endif
 err_sprite:
 	gbm_device_destroy(b->gbm);
 	destroy_sprites(b);
@@ -3241,7 +3581,9 @@
 
 	const struct weston_option drm_options[] = {
 		{ WESTON_OPTION_INTEGER, "connector", 0, &param.connector },
+#if !defined(__FreeBSD__)
 		{ WESTON_OPTION_STRING, "seat", 0, &param.seat_id },
+#endif
 		{ WESTON_OPTION_INTEGER, "tty", 0, &param.tty },
 		{ WESTON_OPTION_BOOLEAN, "current-mode", 0, &option_current_mode },
 		{ WESTON_OPTION_BOOLEAN, "use-pixman", 0, &param.use_pixman },
