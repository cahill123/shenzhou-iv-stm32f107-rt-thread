#include "demo_view.h"
#include <rtgui/rtgui_system.h>
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/progressbar.h>

static rtgui_progressbar_t* hbar;
static rtgui_progressbar_t* vbar;
static rtgui_timer_t *bar_timer = RT_NULL;

void progressbar_timeout(struct rtgui_timer* timer, void* parameter)
{
	static rt_uint32_t value = 0;

	value++;
	if (value == 100)
		value = 0;

	rtgui_progressbar_set_value(hbar, value);
	rtgui_progressbar_set_value(vbar, value);
}

static rt_bool_t start_timer(struct rtgui_object *object, struct rtgui_event *event)
{
	if (bar_timer != RT_NULL)
		rtgui_timer_start(bar_timer);
	return RT_TRUE;
}

static rt_bool_t stop_timer(struct rtgui_object *object, struct rtgui_event *event)
{
	if (bar_timer != RT_NULL)
		rtgui_timer_stop(bar_timer);
	return RT_TRUE;
}

rtgui_container_t *demo_view_progressbar(void)
{
	rtgui_container_t *container;
	rtgui_rect_t rect;
	rtgui_label_t *label;

	/* create a demo container */
	container = demo_view("ProgressBar View");

	/* get demo container rect */
	demo_view_get_rect(container, &rect);
	label = rtgui_label_create("ˮƽ������:");
	rtgui_container_add_child(container, RTGUI_WIDGET(label));
	rect.x1 += 5; rect.x2 -= 5;
	rect.y1 += 5; rect.y2 = rect.y1 + 18;
	rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
	rect.y1 += 20; rect.y2 = rect.y1 + 18;
	hbar = rtgui_progressbar_create(RTGUI_HORIZONTAL, 100, &rect);
	rtgui_container_add_child(container, RTGUI_WIDGET(hbar));

	/* get demo container rect */
	demo_view_get_rect(container, &rect);
	label = rtgui_label_create("��ֱ������:");
	rtgui_container_add_child(container, RTGUI_WIDGET(label));
	rect.x1 += 5; rect.x2 -= 5;
	rect.y1 += 45; rect.y2 = rect.y1 + 18;
	rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
	rect.x1 += 110; rect.x2 = rect.x1 + 20;
	rect.y1 += 18 + 5; rect.y2 = rect.y1 + 150;
	vbar = rtgui_progressbar_create(RTGUI_VERTICAL, 100, &rect);
	rtgui_container_add_child(container, RTGUI_WIDGET(vbar));

	bar_timer = rtgui_timer_create(50, RT_TIMER_FLAG_PERIODIC,
										progressbar_timeout, RT_NULL);

	rtgui_widget_set_onshow(RTGUI_WIDGET(container), start_timer);
	rtgui_widget_set_onhide(RTGUI_WIDGET(container), stop_timer);

	return container;
}

