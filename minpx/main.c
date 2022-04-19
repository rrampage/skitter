/*
   Minpx - A Pixel editor
   Adapted from https://github.com/Datavorous/minpx
   - Original project was windows only and in C++
   - Have removed C++ parts. It now works with C.
     - Can make slight changes to make it compile with C++ as well
   - Have tried to make this cross-platform. Currently tested on Linux
   - Some functionality working in browser. Needs more debugging
   - Current effort has been mostly on porting it. Have not made any design/logic changes.
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> // mkdir
#include "raylib.h"

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#define SCREEN_H 675
#define SCREEN_W 840
#define NO_PIXEL 32

const int canvas_side = 640;
const int grid_x = 10;
const int grid_y = 10;
const int colorPalette_x = 662; 
const int colorPalette_y = 180;

int img_no = 0;
int pencil_size = 1;
int pixel_size_in_palette = 8;
int switch_palette = 1;
int currentFrameEditing = 0;
int currentFrame = 0;
int framesCounter = 0;
int framesSpeed = 2;
int count = 0;
int session = 0;
int framesSize = 0;

int pixel_size=canvas_side/NO_PIXEL;

short m_x,m_y;

char status[50];
char **droppedFiles = { 0 };

bool mirror = false;
bool grid_lines = false;
bool show_centre = false;
bool playAnim = false;
bool chosen = false;

Color frames[NO_PIXEL][NO_PIXEL][NO_PIXEL];

Color present_color;
Color* color_list1;
Color* color_list2;
Color* color_list3;
Color* img_col;
Image color_palette_img1;
Image color_palette_img2;
Image color_palette_img3;
Image checked; 
Image checked2; 
Texture2D checkedTex;
Texture2D checkedTex2;

void drawAnimation(int da_frame) {
  for(int row=0; row<NO_PIXEL; row++) {
    for(int column=0; column<NO_PIXEL; column++) {
      DrawRectangle(
        (row*(float)(canvas_side/4)/NO_PIXEL)+grid_x+canvas_side+10,
        (column*(float)(canvas_side/4)/NO_PIXEL)+grid_y,
        NO_PIXEL==64?3:(canvas_side/(4*NO_PIXEL)),
        NO_PIXEL==64?3:(canvas_side/(4*NO_PIXEL)),
        frames[da_frame][row][column]);
    }
  }                   
}

void drawColors(int no_rows, int size) {
  for(int row=0;row<32;row++) {
      if (switch_palette == 1)
          DrawRectangle(colorPalette_x+size*(row>=no_rows?row-no_rows*(row/no_rows):row),colorPalette_y+size*(row/no_rows), size, size, color_list1[row*pixel_size_in_palette]);
      if (switch_palette == 2)
          DrawRectangle(colorPalette_x+size*(row>=no_rows?row-no_rows*(row/no_rows):row),colorPalette_y+size*(row/no_rows), size, size, color_list2[row*pixel_size_in_palette]);
      if (switch_palette == 3)
          DrawRectangle(colorPalette_x+size*(row>=no_rows?row-no_rows*(row/no_rows):row),colorPalette_y+size*(row/no_rows), size, size, color_list3[row*pixel_size_in_palette]);
  }
}

void printData() {
  if ((GetMouseX()<=canvas_side+grid_x) && (GetMouseY()<=canvas_side+grid_y) && (GetMouseX()>=grid_x) && (GetMouseY()>=grid_y))
  {
      m_x = (int)((GetMouseX()-grid_x)/pixel_size)+1;
      m_y = (int)((GetMouseY()-grid_y)/pixel_size)+1;
  }

  DrawText(TextFormat("%03i,%03i",m_x,m_y),10,660, 12,(Color){255,255,255,255});
  DrawText(status,70,660, 12,(Color){255,255,255,255});
  
  if (mirror)DrawText("M",canvas_side+grid_x-42,660,12,(Color){255,255,255,255});

  DrawText(TextFormat("%0i",pencil_size),canvas_side+grid_x-30,660,12,(Color){255,255,255,255});
  DrawRectangle(canvas_side+grid_x-16,660,15,10,present_color);
  
  DrawText(TextFormat("Paused: %0s",(playAnim?"No":"Yes")),grid_x+canvas_side+10,270+40,12+2,(Color){255,255,255,255});
  DrawText(TextFormat("Editing Frame: %0i",currentFrameEditing+1),grid_x+canvas_side+10,290+40,12+2,(Color){255,255,255,255});
  DrawText(TextFormat("FPS: %0i",framesSpeed),grid_x+canvas_side+10,330+40,12+2,(Color){255,255,255,255});
  DrawText(TextFormat("Total Frames: %0i",framesSize),grid_x+canvas_side+10,310+40,12+2,(Color){255,255,255,255});
}

void drawGrid() {
    for(int row=0; row<NO_PIXEL; row++) {
      for(int column=0; column<NO_PIXEL; column++) {
        DrawRectangle((row*pixel_size)+grid_x, (column*pixel_size)+grid_y, pixel_size, pixel_size, frames[currentFrameEditing][row][column]); 
      }
    }
    DrawRectangleLines(grid_x, grid_y, canvas_side, canvas_side, (Color){50,50,50,255}); 
    if (grid_lines) {
        for(int i=1; i<NO_PIXEL; i++)
            DrawLine(grid_x, (i*pixel_size)+grid_y, grid_x+canvas_side, ((i*pixel_size)+grid_y), (Color){50,50,50,255});
        for(int i=1; i<NO_PIXEL; i++)
            DrawLine((i*pixel_size)+grid_x, grid_y, (i*pixel_size)+grid_x, grid_y+canvas_side, (Color){50,50,50,255});
    }
    if (show_centre) {
        DrawLine((grid_x+canvas_side/2), grid_y, (grid_x+canvas_side/2), grid_y+canvas_side, (Color){255,100,100,255});
        DrawLine(grid_x, (grid_y+canvas_side/2), grid_x+canvas_side, (grid_y+canvas_side/2), (Color){255,100,100,255});
    }

    if (framesCounter >= 60/framesSpeed){
      framesCounter = 0;
      if (framesSize >= 1) {
        currentFrame++;
        if (currentFrame == framesSize) {
          currentFrame = 0;
        }   
      }
    }
    if (!(playAnim)) {
      currentFrame=currentFrameEditing; 
    }
    if (framesSize > 0) {
      drawAnimation(currentFrame);
    }
    DrawRectangleLines(grid_x+canvas_side+10, grid_y, canvas_side/4, canvas_side/4, (Color){50,50,50,255});  
}

void processFile() {
    if (IsFileDropped() && (count==0)) {
      droppedFiles = GetDroppedFiles(&count);
      Image img = LoadImage(droppedFiles[0]); 
      ImageResizeNN(&img, NO_PIXEL, NO_PIXEL);
      ExportImage(img, "minpx/assets/temp.png"); 
      img_col = LoadImageColors(img);
      UnloadImage(img);
      for(int row=0; row<NO_PIXEL; row++)
          for(int column=0; column<NO_PIXEL; column++)
              frames[currentFrameEditing][row][column] = img_col[row+column*NO_PIXEL];
    }
    if (count==0) {
        DrawText("F",canvas_side+grid_x-54,660,12,(Color){255,255,255,255});
    } 
}

void processInputs(void) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      if ((GetMouseX()<=canvas_side+grid_x) && (GetMouseY()<=canvas_side+grid_y) && (GetMouseX()>=grid_x) && (GetMouseY()>=grid_y)) {
          for (int i = -pencil_size/2; i <= pencil_size/2; i++) {
            for (int y = -pencil_size/2; y <= pencil_size/2; y++) {
              if (((
                  (int)((GetMouseX()-grid_x)/pixel_size)+i)>-1 && 
                  ((int)((GetMouseX()-grid_x)/pixel_size)+i)<NO_PIXEL) && 
                  (((int)((GetMouseY()-grid_y)/pixel_size)+y)>-1 && 
                  ((int)((GetMouseY()-grid_y)/pixel_size)+y)<NO_PIXEL)) {
                  if (mirror) {
                      frames[currentFrameEditing][((int)((GetMouseX()-grid_x)/pixel_size)+i)][((int)((GetMouseY()-grid_y)/pixel_size)+y)] = present_color;
                          frames[currentFrameEditing][(NO_PIXEL - 1 - ((int)((GetMouseX()-grid_x)/pixel_size)+i))][((int)((GetMouseY()-grid_y)/pixel_size)+y)] = present_color;
                  } else {
                      frames[currentFrameEditing][((int)((GetMouseX()-grid_x)/pixel_size)+i)][((int)((GetMouseY()-grid_y)/pixel_size)+y)] = present_color;
                  }
              }
              sprintf(status, "Pencil");
            }
          }
      }
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        if ((GetMouseX()<=canvas_side+grid_x) && (GetMouseY()<=canvas_side+grid_y) && (GetMouseX()>=grid_x) && (GetMouseY()>=grid_y)) {
            for (int i = -pencil_size/2; i <= pencil_size/2; i++) {
                for (int y = -pencil_size/2; y <= pencil_size/2; y++) {
                    if ((((int)((GetMouseX()-grid_x)/pixel_size)+i)>-1 && ((int)((GetMouseX()-grid_x)/pixel_size)+i)<NO_PIXEL) && (((int)((GetMouseY()-grid_y)/pixel_size)+y)>-1 && ((int)((GetMouseY()-grid_y)/pixel_size)+y)<NO_PIXEL)) {
                        if (mirror) {
                            frames[currentFrameEditing][((int)((GetMouseX()-grid_x)/pixel_size)+i)][((int)((GetMouseY()-grid_y)/pixel_size)+y)] = (Color){0,0,0,0};
                            frames[currentFrameEditing][(NO_PIXEL - 1 - ((int)((GetMouseX()-grid_x)/pixel_size)+i))][((int)((GetMouseY()-grid_y)/pixel_size)+y)] = (Color){0,0,0,0};
                        } else {
                          frames[currentFrameEditing][((int)((GetMouseX()-grid_x)/pixel_size)+i)][((int)((GetMouseY()-grid_y)/pixel_size)+y)] = (Color){0,0,0,0};
                        }
                    }
                    sprintf(status, "Eraser");
                }
            }
        }
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      if ((GetMouseX()<=colorPalette_x+22*7) && (GetMouseY()<=colorPalette_y+22*5) && (GetMouseX()>=colorPalette_x) && (GetMouseY()>=colorPalette_y)) {
        if (switch_palette == 1) {
          present_color = color_list1[(((GetMouseX()-colorPalette_x)/22)+((GetMouseY()-colorPalette_y)/22)*7)*pixel_size_in_palette];
        }
        if (switch_palette == 2) {
          present_color = color_list2[(((GetMouseX()-colorPalette_x)/22)+((GetMouseY()-colorPalette_y)/22)*7)*pixel_size_in_palette];
        }
        if (switch_palette == 3) {
          present_color = color_list3[(((GetMouseX()-colorPalette_x)/22)+((GetMouseY()-colorPalette_y)/22)*7)*pixel_size_in_palette];
        }
        sprintf(status, "New Color Picked");
      }
    }

    if (IsKeyPressed(KEY_G)) {
      sprintf(status, "Grid");
      grid_lines = !grid_lines;
    }

    if (IsKeyDown(KEY_A)) {
      if(IsKeyPressed(KEY_I)) {
        present_color.a += 5;
      }
      if(IsKeyPressed(KEY_D)) {
        present_color.a -= 5;
      }
      sprintf(status, "Alpha Changed");
    }

    if (IsKeyPressed(KEY_X)) {
      sprintf(status, "Centre");
      show_centre = !show_centre;
    }

    if (IsKeyPressed(KEY_M)) {
      mirror = !mirror;
    }

    if (IsKeyPressed(KEY_LEFT_BRACKET)) {
      pencil_size = (pencil_size>2) ? pencil_size -2 : pencil_size;
    }

    if (IsKeyPressed(KEY_RIGHT_BRACKET)) {
      pencil_size +=2;
    }

    if (IsKeyPressed(KEY_F)) {
      ClearDroppedFiles();
      count = 0;
    }

    if (IsKeyPressed(KEY_C)) {
      for(int row=0; row<NO_PIXEL; row++) {
        for(int column=0; column<NO_PIXEL; column++) {
          frames[currentFrameEditing][row][column]= (Color){0,0,0,0};
        }
      }         
    }

    if((IsKeyDown(KEY_LEFT_SHIFT)) && (IsKeyPressed(KEY_N))) {
      // Copy current frame data to new frame and switch to it
      if (framesSize >= NO_PIXEL || currentFrameEditing >= NO_PIXEL-1) {
        return;
      }
      for (int i = 0; i < NO_PIXEL; i++) {
        for (int j = 0; j < NO_PIXEL; j++) {
          frames[currentFrameEditing+1][i][j] = frames[currentFrameEditing][i][j];
        }
      }
      framesSize++;
      currentFrameEditing++;
      sprintf(status, "New Frame");
    }

    if (IsKeyPressed(KEY_P)) {
      switch_palette = (switch_palette > 2 ) ? 0 : switch_palette+1; 
    }

    if (IsKeyPressed(KEY_RIGHT)) {
      if (currentFrameEditing < framesSize-1) {
        currentFrameEditing++;
      }
    }
        
    if (IsKeyPressed(KEY_LEFT)) {
      if (currentFrameEditing>0) {
        currentFrameEditing--; 
      }
    }
        
    if (IsKeyPressed(KEY_UP)) {
      framesSpeed++;
    }
        
    if (IsKeyPressed(KEY_DOWN)) {
      if (framesSpeed > 1) {
        framesSpeed--;
      }
    }
        
    if (IsKeyPressed(KEY_SPACE)) {
      playAnim = !playAnim;
    }
        
    if ((IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) && ((GetMouseX()<=canvas_side+grid_x) && (GetMouseY()<=canvas_side+grid_y) && (GetMouseX()>=grid_x) && (GetMouseY()>=grid_y))) {
      present_color = frames[currentFrameEditing][((int)((GetMouseX()-grid_x)/pixel_size))][((int)((GetMouseY()-grid_y)/pixel_size))];
    }

    if((IsKeyDown(KEY_LEFT_CONTROL)) && (IsKeyPressed(KEY_E))) {
      for (int imgs=0; imgs<framesSize; imgs++) {
        Image p_img = GenImageColor(NO_PIXEL, NO_PIXEL, ((Color){0,0,0,0}));
        for(int row=0; row<NO_PIXEL; row++) {
          for(int column=0; column<NO_PIXEL; column++) {
            ImageDrawPixel(&p_img, row, column, frames[imgs][row][column]);
          }
        }
        //ImageResizeNN(&p_img, 256, 256);
        ExportImage(p_img, TextFormat("out/session_%i/%03i.png",session,img_no));
        img_no++;
        UnloadImage(p_img);
      }
      sprintf(status, "Image Exported");
    }
}

void runMainLoop(void) {
  BeginDrawing();
    ClearBackground((Color){25,25,25});
    DrawTexture(checkedTex, grid_x, grid_y, WHITE);
    DrawTexture(checkedTex2, grid_x+canvas_side+10, grid_y, WHITE);
    drawGrid();
    drawColors(7,22);
    printData();
    processFile();
  EndDrawing();
  framesCounter++;
  processInputs();
}

int main(void) {
    color_palette_img1 = LoadImage("minpx/assets/palette1.png"); 
    color_palette_img2 = LoadImage("minpx/assets/palette2.png"); 
    color_palette_img3 = LoadImage("minpx/assets/palette3.png");
    color_list1 = LoadImageColors(color_palette_img1);
    color_list2 = LoadImageColors(color_palette_img2); 
    color_list3 = LoadImageColors(color_palette_img3);
    UnloadImage(color_palette_img1);
    UnloadImage(color_palette_img2);
    UnloadImage(color_palette_img3);
    present_color = (Color){20,20,20,255};

    for(int row=0;row<=NO_PIXEL;row++) {
        for (int column = 0; column <= NO_PIXEL; column++) {
            //frames[0][row][column] = {0,0,0,0};
        }
    }

    framesSize++;

    InitWindow(SCREEN_W, SCREEN_H, "minpx");
    //SetTargetFPS(60);

    checked = LoadImage("minpx/assets/checked.png"); 
    checked2 = LoadImage("minpx/assets/checked2.png"); 
    checkedTex = LoadTextureFromImage(checked);
    checkedTex2 = LoadTextureFromImage(checked2);
    UnloadImage(checked);
    UnloadImage(checked2);
    session = LoadStorageValue(0) + 1;
    SaveStorageValue(0, session);
    mkdir(TextFormat("out/session_%i",session), 0755);
    #ifndef __EMSCRIPTEN__
    while (!WindowShouldClose()) {
        runMainLoop();
    }
    #endif
    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(runMainLoop, 0, 1);
    #endif
    UnloadTexture(checkedTex);
    UnloadTexture(checkedTex2);
    UnloadImageColors(color_list1);
    UnloadImageColors(color_list2);
    UnloadImageColors(color_list3);
    UnloadImageColors(img_col);
    ClearDroppedFiles();
    CloseWindow();
}
