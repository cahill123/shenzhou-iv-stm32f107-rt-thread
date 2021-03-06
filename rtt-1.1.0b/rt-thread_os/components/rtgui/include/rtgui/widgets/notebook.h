#ifndef __RTGUI_NOTEBOOK_H__
#define __RTGUI_NOTEBOOK_H__

#include <rtgui/rtgui.h>
#include <rtgui/widgets/widget.h>

DECLARE_CLASS_TYPE(notebook);
/** Gets the type of a notebook */
#define RTGUI_NOTEBOOK_TYPE       (RTGUI_TYPE(notebook))
/** Casts the object to a notebook control */
#define RTGUI_NOTEBOOK(obj)       (RTGUI_OBJECT_CAST((obj), RTGUI_NOTEBOOK_TYPE, struct rtgui_notebook))
/** Checks if the object is a notebook control */
#define RTGUI_IS_NOTEBOOK(obj)    (RTGUI_OBJECT_CHECK_TYPE((obj), RTGUI_NOTEBOOK_TYPE))

#define RTGUI_NOTEBOOK_TOP			0x00
#define RTGUI_NOTEBOOK_BOTTOM		0x01
#define RTGUI_NOTEBOOK_NOTAB		0x02

struct rtgui_notebook_tab;

struct rtgui_notebook
{
	struct rtgui_widget parent;

	rt_uint8_t flag;

	/* widget private data */
	struct rtgui_notebook_tab *childs;
	rt_uint16_t count;
	rt_int16_t current;
};

struct rtgui_notebook* rtgui_notebook_create(const rtgui_rect_t* rect, rt_uint8_t style);
void rtgui_notebook_destroy(struct rtgui_notebook* notebook);

void rtgui_notebook_add(struct rtgui_notebook* notebook, const char* label, struct rtgui_widget* child);
void rtgui_notebook_remove(struct rtgui_notebook* notebook, rt_uint16_t index);
struct rtgui_widget* rtgui_notebook_get_current(struct rtgui_notebook* notebook);
rt_int16_t rtgui_notebook_get_current_index(struct rtgui_notebook* notebook);

int rtgui_notebook_get_count(struct rtgui_notebook* notebook);

void rtgui_notebook_set_current(struct rtgui_notebook* notebook, struct rtgui_widget* child);
void rtgui_notebook_set_current_by_index(struct rtgui_notebook* notebook, rt_uint16_t index);

struct rtgui_widget* rtgui_notebook_get_widget_at(struct rtgui_notebook* notebook, rt_uint16_t index);

rt_bool_t rtgui_notebook_event_handler(struct rtgui_object* widget, struct rtgui_event* event);

#endif
