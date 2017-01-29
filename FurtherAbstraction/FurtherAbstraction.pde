import processing.video.*;

/*
 * This sketch uses the text drawn to an offscreen PGraphics to determine
 * what color and shape it will draw to each xy-coordinate on the main screen
 * User can choose shape and activate device camera to take a picture and use
 * image as the fill color for each shape
 * 
 * USAGE:
 * - move the mouse around to change the grid dimensions
 * - click the mouse to flip the drawing method used for inside and outside of the text
 * - press 's' to draw shapes (either elipse or triangle)
 * - press 'e' to draw elipses
 * - press 't' to draw triangles
 * - press spacebar to take picture and use as fill color for each shape in grid
 * - press 'c' to clear picture and switch the fill color back to black
 * - press 'g' to toggle gradient
 * - press 'f' to toggle flux gradient
 * - press 'o' to toggle stroke display 
 *
 * CREDIT:
 * This sketch is built off of the excellent Processing tutorials and sample code of 
 * Amnon Owed's "Generative Typography with Processing Tutorial"
 * and Daniel Schiffman's "Capture and Live Video - Processing Tutorial". 
 */

Capture video;
// want camera to read a lower resolution for performance
// used when reading image to get values for shape color
// display at a high resolutoin, but capture (image) at a low resolutoin
// int videoScale = 2;

color SHAPE_COLOR = color(0); // color of drawn shapes
color IMAGE_COLOR; // determined by color of pixel in photo
color LINE_COLOR = color(0, 125); // color of drawn lines
color PGRAPHICS_COLOR = color(0); // won't impact this sketch's visual outcome
int LINE_LENGTH = 25; // length of drawn lines
boolean reverseDrawing = false; // boolean to flip the drawing method (toggle with mouse)
boolean ellipseSelected = true; // boolean to determine shape (select with keyPress 'c')
boolean triangleSelected = false; // boolean to determine shape (select with keyPress 't')
boolean colorDefaults = true; // boolean to turn fill color of shapes to black
boolean drawShapesSelected = true; // boolean to trigger drawShapes function (select with keyPress 's')

// Globals for creating gradients
color FOREGROUND_COLOR = color(255, 0, 0);
color BACKGROUND_COLOR = color(0, 0, 255);
int margin; // variable to determine when the right or topside have been reached with gradient
int gradientCounter = 0;
int selectGradientCounter = 0; // gradient background and foreground colors randomize after first selection
boolean changeGradientSize = true;
boolean displayStroke = false;
boolean drawGradientSelected = false; // boolean to trigger drawGradients function (select with keyPress 'g')

// PGraphics class can be used for offscreen rendering
// Can use all of Processing's default drawing commands on a PGraphics
PGraphics pg; // initialize PGraphics instance



void setup() {
  size(1280, 720, P2D); // per vertex coloring requires an OpenGL renderer
  smooth(16); // for better results
  
  video = new Capture(this, width, height, 30); //(parent, requestWidth, requestHeight, frameRate)
  video.start();
  
  pg = createGraphics(width, height, JAVA2D); // create a PGraphics the same size as the main sketch display window
  // Draw something to the created PGraphics instance called 'pg'.
  // Note 1: Always put all drawing commands to a PGraphics between beginDraw/endDraw calls
  // Note 2: Always put the name of the PGraphics(in this case 'pg') plus a . (aka dot) before the drawing commands to a PGraphics
  pg.beginDraw(); // start drawing to the PGraphics
  pg.textSize(500); // set the text rendering size (of the PGraphics) to 500
  pg.textAlign(CENTER, CENTER); // center the text (of the PGraphics) horizontally and vertically
  pg.fill(PGRAPHICS_COLOR);
  pg.text("TYPE", pg.width/2, pg.height/2); // display the String "TYPE" in the center of the PGraphics
  pg.endDraw(); // finish drawing to the PGraphics
}



void draw() { 
  // draw shapes to the screen
  if (displayStroke) { strokeWeight(0.5); stroke(0);} else { noStroke(); } // toggle with 'o' key 
  if (drawShapesSelected) {drawShapes(); } // toggle with 's' key
  if (drawGradientSelected) {drawGradient(); } // toggle with 'g' key
}



void mousePressed() {
  reverseDrawing = !reverseDrawing; // toggle boolean for drawing method
}



void keyPressed() {
  // draw ellipses if 'e' is pressed
  if (key == 'e') {
    ellipseSelected = true;
    triangleSelected = false;
  }
  // draw triangles if 't' is pressed
  if (key == 't') {
    ellipseSelected = false;
    triangleSelected = true;
  }
  // capture video frame if spacebar is pressed
  if (key == ' ') {
    video.read();
    colorDefaults = false;
  }
  // revert back to default colors 'c' is pressed
  if (key == 'c') {
    colorDefaults = true;
    revertToColorDefaults();
  }
  // add or remove stroke if 's' is pressed
  if (key == 'o') displayStroke = !displayStroke;
  // draw gradient if 'g' is pressed
  if (key == 'g') {
    drawGradientSelected = true;
    drawShapesSelected = false;
    // randomize gradient colors after the first time gradient is selected
    if ( selectGradientCounter > 0 ) {
      FOREGROUND_COLOR = color(int(random(0, 256)), int(random(0, 256)), int(random(0, 256)));
      BACKGROUND_COLOR = color(int(random(0, 256)), int(random(0, 256)), int(random(0, 256)));
    }
    selectGradientCounter++; 
  }
  // draw shapes if 's' is pressed
  if (key == 's') {
    drawShapesSelected = true;
    drawGradientSelected = false;
  }
  // trigger flux gradient if 'f' is pressed
  if (key == 'f') changeGradientSize = !changeGradientSize;
}



// check if point is inside text
boolean inText(int x, int y) {
  color c = pg.get(x, y); // get PGraphics color at this coordinate
  // NOTE: if pixel image color = PGRAPHICS_COLOR it will read as if the text is in that position
  return (c == PGRAPHICS_COLOR); // is the color equal to PGRAPHICS_COLOR (aka is there text here)
}


void revertToColorDefaults() {
  SHAPE_COLOR = color(0);
  FOREGROUND_COLOR = color(255, 0, 0);
  BACKGROUND_COLOR = color(0, 0, 255);
}



void drawShapes() {
  // determine grid dimensions based on the mouse position and calculate resulting grid settings
  int gridHorizontal = (int) map(mouseX, 0, width, 30, 200); // number of horizontal grid cells (based on mouseX)
  int gridVertical = (int) map(mouseY, 0, height, 15, 100); // number of vertical grid cells (based on mouseY)
  background(255);
  // assign the width and heigth of each cell to a variables 'w' and 'h' based on mouse position
  float w = float(width)/gridHorizontal;
  float h = float(height)/gridVertical;
  float r = min(w,h); // assign the radius of the shape based on height or width depending on which is less in value
  // create the grid
  for (int y=0; y<gridVertical; y++) {
    for (int x=0; x<gridHorizontal; x++) {
      float s_x = x * w; // x position of shape on grid
      float s_y = y * h; // y position of shape on grid
      //color c = pg.get(int(s_x), int(s_y)); // get PGraphics color at this coordinate
      //boolean textDrawn = (c == PGRAPHICS_COLOR); // is the color equal to PGRAPHICS_COLOR (aka is there text here)
      // use the reverseDrawing boolean to flip the textDrawn boolean
      // thus in fact flipping the resulting displayed shapes
      if (reverseDrawing ? !inText(int(s_x), int(s_y)) : inText(int(s_x), int(s_y))) {
        noStroke();
        // if new video frame is availabe change SHAPE_COLOR to the pixel color value at the position of the shape on the grid
        if (video.available() == true) {
          IMAGE_COLOR = video.get(int(s_x), int(s_y));
        } 
        if (colorDefaults == true) {
          revertToColorDefaults();
          fill(SHAPE_COLOR);
        } else {
          fill(IMAGE_COLOR);
        }
        // determine which shape to draw based on user input. ellipse is default.
        if (ellipseSelected) ellipse(s_x, s_y, r, r);
        if (triangleSelected) triangle(s_x, s_y, s_x+w/2, s_y+h, s_x+w, s_y);
      } else {
        stroke(LINE_COLOR);
        //line(s_x, s_y, s_x + LINE_LENGTH, s_y + LINE_LENGTH);
        beginShape(LINES);
          vertex(s_x, s_y);
          vertex(s_x + LINE_LENGTH, s_y + LINE_LENGTH);
        endShape();
      }
    }
  }
}



void drawGradient() {
  // determine grid dimensions based on the mouse position and calculate resulting grid settings
  int gridHorizontal = (int) map(mouseX, 0, width, 10, 100); // number of horizontal grid cells (based on mouseX)
  int gridVertical = (int) map(mouseY, 0, height, 10, 100); // number of vertical grid cells (based on mouseY)
  background(BACKGROUND_COLOR);
  int margin = max(gridVertical, gridHorizontal) *2;
  
  // dynamic gradient size (toggle with 'g' key)
  if (changeGradientSize) gradientCounter++;
  float gradientSize = 100 + sin(gradientCounter * 0.01) * 100;
  
  // create diagonal lines covering the whole screen using two while loops
  // some initial variables needed within the while loops
  int y = 0;
  boolean done = false;
  boolean insideShape = false;
  
  while (!done) {
    // start somewhere left of the screen, each time a little bit further down
    y += gridVertical;
    int vx = -margin;
    int vy = y;
    beginShape(QUAD_STRIP);
    // keep going while the right or top side hasn't been reached
    while (vx < width+margin && vy > -margin) {
      // check if point is inside text
      // use the reverseDrawing boolean to flip the textDrawn boolean
      // thus in fact flipping the resulting displayed shapes
      if (reverseDrawing ? !inText(vx, vy) : inText(vx, vy)) {
        if (insideShape) {
          // start a new Shape when exiting the text
          beginShape(QUAD_STRIP);
          insideShape = false;
        }
        // when outside text, add two vertices
        fill (FOREGROUND_COLOR);
        vertex(vx, vy);
        fill(BACKGROUND_COLOR);
        vertex(vx + gradientSize, vy + gradientSize);
      } else {   // not inText
        if (!insideShape) {
          // end the current Shape when first entering the text
          endShape();
          insideShape = true;
        }
      }
      //move right and upwards
      vx += gridHorizontal;
      vy -= gridVertical;
      // if we are beyond the right and beyond the bottom of the screen, stop the main while loop
      if (vx > width && vy > height) done = true; // escape!
    }
    endShape();
  } 
}