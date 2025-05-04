#include "My_GUI.h"

/* 全局变量 */
static lv_obj_t * label_counter;
static uint32_t counter = 0;

void Gui_Init(void)
{
	    /* 创建基础界面 */
    lv_obj_t * scr = lv_scr_act();

    /* 1. 创建Hello World标签 */
    lv_obj_t * label = lv_label_create(scr);
    lv_label_set_text(label, "Hello LVGL!");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    /* 2. 创建计数器标签 */
    label_counter = lv_label_create(scr);
    lv_label_set_text_fmt(label_counter, "Count: %lu", counter);
    lv_obj_align_to(label_counter, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    /* 3. 创建按钮 */
    lv_obj_t * btn = lv_btn_create(scr);
    lv_obj_set_size(btn, 100, 40);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    
    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Click Me!");
    lv_obj_center(btn_label);
		
}

void Gui_Change(void)
{
		counter++;
    lv_label_set_text_fmt(label_counter, "Count: %lu", counter);
}
