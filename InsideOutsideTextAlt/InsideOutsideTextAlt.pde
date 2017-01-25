
/*
 * This sketch uses the text drawn to an offscreen PGraphics to determine
 * what color and shape it will draw to each xy-coordinate on the main screen
 *
 * USAGE:
 * - move the mouse around to change the grid dimensions
 * - click the mouse to flip the drawing method used for respectively inside and outside the text  
 */

// SKETCH SETTINGS
color SHAPE_COLOR = color(0); // color of drawn shapes
color LINE_COLOR = color(0, 125); // color of drawn lines
color PGRAPHICS_COLOR = color(0); // won't impact this sketch's visual outcome
int LINE_LENGTH = 25; // length of drawn lines
boolean reverseDrawing = false; // boolean to flip the drawing method (toggle with mouse)
boolean drawCircle = true;
boolean drawTriangle = false;

PGraphics pg;

void setup() {
  size(1280, 720); 
  
}

void draw() {
  // create and draw to PPraphics (see Getting Started > UsingPGraphics example)
  pg = createGraphics(width, height, JAVA2D);
  pg.beginDraw();
  pg.textSize(500);
  pg.textAlign(CENTER, CENTER);
  pg.fill(PGRAPHICS_COLOR);
  pg.text("TYPE", mouseX, mouseY); 
  pg.endDraw();
  
  // determine grid dimensions based on the mouse position and calculate resulting grid settings
  int gridHorizontal = (int) map(mouseX, 0, width, 30, 200); // number of horizontal grid cells (based on mouseX)
  int gridVertical = (int) map(mouseY, 0, height, 15, 100); // number of vertical grid cells (based on mouseY)
  float w = float(width) / gridHorizontal;
  float h = float(height) / gridVertical;
  float r = min(w, h);

  // draw shapes to the screen
  background(255);
  strokeWeight(0.5);
  for (int y=0; y<gridVertical; y++) {
    for (int x=0; x<gridHorizontal; x++) {
      float s_x = x * w;
      float s_y = y * h;
      color c = pg.get(int(s_x), int(s_y)); // get PGraphics color at this coordinate
      boolean textDrawn = (c == PGRAPHICS_COLOR); // is the color equal to PGRAPHICS_COLOR (aka is there text here)
      // use the reverseDrawing boolean to flip the textDrawn boolean
      // thus in fact flipping the resulting displayed shapes
      if (reverseDrawing ? !textDrawn : textDrawn) {
        noStroke();
        fill(SHAPE_COLOR);
        if (drawCircle) ellipse(s_x, s_y, r, r);
        if (drawTriangle) triangle(s_x, s_y, s_x+w/2, s_y+h, s_x+w, s_y);
      } else {
        stroke(LINE_COLOR);
        line(s_x, s_y, s_x + LINE_LENGTH, s_y + LINE_LENGTH);
      }
    }
  }
}

void mousePressed() {
  reverseDrawing = !reverseDrawing; // toggle boolean for drawing method
}

void keyPressed() {
  if (key == 'c') {
    drawCircle = true;
    drawTriangle = false;
  }
  if (key == 't') {
    drawCircle = false;
    drawTriangle = true;
  }
}