#pragma once
#include"LibZJ.h"

void SetWorkingImage(Image * img);
void ReleaseWorkingImage();

void SetColor(D2D1_COLOR_F color);
void SetColor(float r, float g, float b, float a = 1.0);
void PutPixel(int x, int y);
void DrawLine(int x0, int y0, int x1, int y1);
void DrawCircle(int x, int y, int r);
void FloodFill8(int x, int y);
void FloodFill4(int x, int y);
