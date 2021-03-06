/*
 * File      : rtgui_application.h
 * This file is part of RTGUI in RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-01-13     Grissiom     first version
 */
#ifndef __RTGUI_APPLICATION_H__
#define __RTGUI_APPLICATION_H__

#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/event.h>
#include <rtgui/rtgui_system.h>

DECLARE_CLASS_TYPE(application);

/** Gets the type of a application */
#define RTGUI_APP_TYPE       (RTGUI_TYPE(application))
/** Casts the object to an rtgui_workbench */
#define RTGUI_APP(obj)       (RTGUI_OBJECT_CAST((obj), RTGUI_APP_TYPE, struct rtgui_app))
/** Checks if the object is an rtgui_workbench */
#define RTGUI_IS_APP(obj)    (RTGUI_OBJECT_CHECK_TYPE((obj), RTGUI_APP_TYPE))

enum rtgui_app_flag
{
	RTGUI_APP_FLAG_EXITED  = 0x04,
	RTGUI_APP_FLAG_SHOWN   = 0x08
};

typedef void (*rtgui_idle_func_t)(struct rtgui_object* obj, struct rtgui_event *event);

struct rtgui_app
{
	struct rtgui_object parent;

	/* application name */
	unsigned char *name;
	struct rtgui_image* icon;

	enum rtgui_app_flag state_flag;

	rt_uint16_t ref_count;
	rt_uint16_t exit_code;

	/* the thread id */
	rt_thread_t tid;
	/* the RTGUI server id */
    rt_thread_t server;

	/* the message queue of thread */
	rt_mq_t mq;
	/* event buffer */
	rt_uint8_t event_buffer[sizeof(union rtgui_event_generic)];

	/* if not RT_NULL, the application is in modal state by modal_object. If is
	 * RT_NULL, nothing modal windows. */
	struct rtgui_object *modal_object;
	struct rtgui_object *main_object;

	/* on idle event handler */
	rtgui_idle_func_t on_idle;
};

/**
 * create an application named @myname on thread @param tid
 */
struct rtgui_app* rtgui_app_create(rt_thread_t tid, const char *title);
void rtgui_app_destroy(struct rtgui_app *app);
rt_bool_t rtgui_app_event_handler(struct rtgui_object* obj, rtgui_event_t* event);

rt_base_t rtgui_app_run(struct rtgui_app *app);
void rtgui_app_exit(struct rtgui_app *app, rt_uint16_t code);

void rtgui_app_set_onidle(rtgui_idle_func_t onidle);
rtgui_idle_func_t rtgui_app_get_onidle(void);
struct rtgui_app* rtgui_app_self(void);

rt_err_t rtgui_app_set_as_wm(void);
void rtgui_app_set_main_win(struct rtgui_win* win);

#endif /* end of include guard: RTGUI_APPLICATION_H */
