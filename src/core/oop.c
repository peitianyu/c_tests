#include "oop.h"


static float g_length;
static float g_width;

// 实现一个三角形面积公式
void triangle_init(float length, float width)
{
    g_length = length;
    g_width = width;
}

// 长方形面积
static float get_rectangle_area()
{
    return g_length*g_width;
}

float get_area()
{
    return get_rectangle_area()/2.f;
}


