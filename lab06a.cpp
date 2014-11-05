/***********
0;136;0cAuthor: Mackenzie Larson
Date: 10/1/14
Class: CS315 LAB 06
Description: This program will use OCR to read text off a scanned bmp image. It will turn the input image into pure black and white, & draw bounding boxes around letters. The program will list how many lines, words, and characters are in the text. User can also specify where the OCR looks. They can also specify colors. 
To compile:
g++ EasyBMP.cpp lab06a.cpp
./a.out input.bmp output.bmp
**************/

#include "EasyBMP.h"
#include <math.h>
#include <iostream>
#include <climits>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <locale> //toupper

using namespace std;

const int MSIZE = 1000;

class point {
private:
  int x; int y;

public:

  point(int p, int q) {
    x = p; y = q;
  }

  int getx() {
    return x;
  }

  int gety() {
    return y;
  }
};

class queue {

private:
  point* Q[MSIZE];

  int front, rear, size;

public:
  queue() {
    // initialize an empty queue
    front = 0; rear = 0; size = 0;
    for (int j=0; j < MSIZE; ++j)
      Q[j] = 0;
  }

  void insert(point * x) {
    if (size != MSIZE) {
      front++; size++;
      if (front == MSIZE) front = 0;
      Q[front] = x;
    }
  }

  point del() {
    if (size != 0) {
      rear++; if (rear == MSIZE) rear = 0;
      point temp(Q[rear]->getx(), Q[rear]->gety());
      size--;
      return temp;
    };
  }

  void print() {
    for (int j = 1; j <= size; ++j) {
      int i = front - j + 1;
      cout << "x = " << Q[i]->getx() << " y = " << Q[i]->gety() << endl;
    }
    cout << "end of queue" << endl;
  }

  bool isEmpty() {
    return (size == 0);
  }
};

RGBApixel colorValue(string colour){
  RGBApixel c5;

  int i = 0; 
  while(colour[i]){
    colour[i] = toupper(colour[i]);
    i++;
  }
  if(colour == "WHITE")
    {
      c5.Red = 255;
      c5.Green = 255;
      c5.Blue = 255;
    }
  else if(colour == "SILVER")
    {
      c5.Red = 192;
      c5.Green = 192;
      c5.Blue = 192;
    }
  else if(colour == "GRAY")
    {
      c5.Red = 128;
      c5.Green = 128;
      c5.Blue = 128;
    }
  else if(colour == "BLACK")
    {
      c5.Red = 0;
      c5.Green = 0;
      c5.Blue = 0;
    }
  else if(colour == "YELLOW")
    {
      c5.Red = 255;
      c5.Green = 255;
      c5.Blue = 0;
    }
  else if(colour == "FUCHSIA")
    {
      c5.Red = 255;
      c5.Green = 0;
      c5.Blue = 255;
    }
  else if(colour == "RED")
    {
      c5.Red = 255;
      c5.Green = 0;
      c5.Blue = 0;
    }
  else if(colour == "MAROON")
    {
      c5.Red = 128;
      c5.Green = 0;
      c5.Blue = 0;
    }
  else if(colour == "LIME")
    {
      c5.Red = 0;
      c5.Green = 255;
      c5.Blue = 0;
    }
  else if(colour == "OLIVE")
    {
      c5.Red = 128;
      c5.Green = 128;
      c5.Blue = 0;
    }
  else if(colour == "GREEN")
    {
      c5.Red = 0;
      c5.Green = 128;
      c5.Blue = 0;
    }
  else if(colour == "PURPLE")
    {
      c5.Red = 128;
      c5.Green = 0;
      c5.Blue = 128;
    }
  else if(colour == "AQUA")
    {
      c5.Red = 0;
      c5.Green = 255;
      c5.Blue = 255;
    }
  else if(colour == "TEAL")
    {
      c5.Red = 0;
      c5.Green = 128;
      c5.Blue = 128;
    }
  else if(colour == "BLUE")
    {
      c5.Red = 0;
      c5.Green = 0;
      c5.Blue = 255;
    }
  else if(colour == "NAVY")
    {
      c5.Red = 0;
      c5.Green = 0;
      c5.Blue = 128;
    }
  else
    {
      cout << "Your color was: " << colour << endl;
      cout << "Your color was not valid. Please try again." << endl;
      //exit(1);
    }
  
  return c5;
}


void Preprocessing(BMP & Output, RGBApixel c1, RGBApixel c2) {
  int picWidth = Output.TellWidth();
  int picHeight = Output.TellHeight();
  
  for (int i = 0; i < picWidth - 1; ++i)
    for (int j = 0; j < picHeight - 1; ++j) {
      int colorVal = abs(Output(i,j)->Blue - c1.Blue) + abs(Output(i,j)->Green - c1.Green) + abs(Output(i,j)->Red - c1.Red);
      int colorVal2 = abs(Output(i,j)->Blue - c2.Blue) + abs(Output(i,j)->Green - c2.Green) + abs(Output(i,j)->Red - c2.Red);
      if (colorVal < colorVal2){
        Output(i, j)->Red = c1.Red;
        Output(i, j)->Blue = c1.Green;
        Output(i, j)->Green = c1.Blue;
      }
      else{
        Output(i, j)->Red = c2.Red;
        Output(i, j)->Blue = c2.Green;
        Output(i, j)->Green = c2.Blue;
      }
    }
}


int label(BMP & Output, int xpixels, int ypixels, int xlow, int xhigh, int ylow, int yhigh, RGBApixel c1, RGBApixel c2, RGBApixel c3, RGBApixel c4){
  int w = Output.TellWidth();
  int h = Output.TellHeight();
  int count = 0;
  //set visited [j][k] false for all j, k
  bool visited[h][w];
  for(int j = 0; j < h; j++){
    for (int k = 0; k < w; k++){
      visited[j][k] = false;
    }
  }
  
  //initialize Q
  queue Q;
  int x1, y1;
  int lxlow, lxhigh, lylow, lyhigh;

  for (int j = xlow; j <= xhigh; j++){
    for (int k = ylow; k <= yhigh; k++){
      
      if (Output(j,k)->Red == c2.Red && Output(j,k)->Blue == c2.Blue && Output(j,k)->Green == c2.Green){
	count++;
	lxlow = j; lylow = k; lxhigh = j; lyhigh = k;
	point * p = new point(j,k);
	Q.insert(p);
	visited[k][j] = true;

	while (!Q.isEmpty()){
	  *p = Q.del(); 
	  x1 = p->getx();
	  y1 = p->gety(); 
  
	  Output(x1,y1)->Red = c3.Red;
	  Output(x1,y1)->Green = c3.Green;
	  Output(x1,y1)->Blue = c3.Blue;

	  if (x1 < lxlow) lxlow = x1;
	  if (y1 < lylow) lylow = y1; 
	  if (x1 > lxhigh) lxhigh = x1;
	  if (y1 > lyhigh) lyhigh = y1;
  
	  for (int x = x1-xpixels; x <= x1+xpixels; x++){
	    for (int y = y1-ypixels; y <= y1+ypixels; y++){
	      if (!visited[y][x]){
		visited[y][x] = true;
		if (Output(x,y)->Red == c2.Red && Output(x,y)->Blue == c2.Blue && Output(x,y)->Green == c2.Green){
		  point * n = new point(x, y);
		  Q.insert(n);
		}
	      }
	    }
	  }
  
	}// end while

	for (int sides = lylow-1; sides < lyhigh+2; sides++) {
	  Output(lxlow-1, sides)->Red = c4.Red; 
	  Output(lxlow-1, sides)->Green = c4.Green;
	  Output(lxlow-1, sides)->Blue = c4.Blue;
	  
	  Output(lxhigh+1, sides)->Red = c4.Red;
	  Output(lxhigh+1, sides)->Green = c4.Green;
	  Output(lxhigh+1, sides)->Blue = c4.Blue;
	}

	for (int topnbot = lxlow-1; topnbot < lxhigh+2; topnbot++) {
	  Output(topnbot, lylow-1)->Red = c4.Red;
	  Output(topnbot, lylow-1)->Green = c4.Green;
	  Output(topnbot, lylow-1)->Blue = c4.Blue;
  
	  Output(topnbot, lyhigh+1)->Red = c4.Red;
	  Output(topnbot, lyhigh+1)->Green = c4.Green;
	  Output(topnbot, lyhigh+1)->Blue = c4.Blue;
	}

       
      } // end if
      
      visited[k][j] = true; 
      
    }// end for k;
  }// end for j;
  return count;
}



int main(int argc, char* argv[]) {
  BMP myImage;
  myImage.ReadFromFile(argv[1]);

  string colour1, colour2, colour3, colour4;
  RGBApixel c1, c2, c3, c4;
  int xlow, ylow, xhigh, yhigh;
  string choice;
  
 retry: // if they enter a bad choice
  cout << "Would you like to manually enter the region of the input image where preprocessing is done and the colors it will change to? (y/n/exit): ";
  getline(cin,choice);
  int i=0;
  while (choice[i]){
    choice[i] = toupper(choice[i]);
    i++;
  }
  cout << "You entered: " << choice << endl;
  
  if (choice == "Y"){
    cout << "Enter the xlow input: " << endl;
    cin >> xlow;
    cout << "Enter the ylow input: " << endl;
    cin >> ylow;
    cout << "Enter the xhigh input: " << endl;
    cin >> xhigh;
    cout << "Enter the yhigh input: " << endl;
    cin >> yhigh;
    
    cout << "Enter a color for c1: " << endl;
    cin >> colour1;
    c1 = colorValue(colour1);
    
    cout << "Enter a color for c2: " << endl;
    cin >> colour2;
    c2 = colorValue(colour2);
    
    cout << "Enter a color for c3: " << endl;
    cin >> colour3;
    c3 = colorValue(colour3);
    
    cout << "Enter a color for c4: " << endl;
    cin >> colour4;
    c4 = colorValue(colour4);
  }
  
  else if (choice == "N"){
    xlow = 0;
    ylow = 0;
    xhigh = myImage.TellWidth() -1;
    yhigh = myImage.TellHeight() - 1;
    colour1 = "WHITE";
    c1 = colorValue(colour1);
    colour2 = "BLACK";
    c2 = colorValue(colour2);
    colour3 = "GREEN";
    c3 = colorValue(colour3);
    colour4 = "RED";
    c4 = colorValue(colour4);
  }
  
  else if (choice == "EXIT"){
    cout << "Okay. Goodbye!" << endl;
    exit(1);
  }
  else{
    cout << "Not a valid choice. Please select again." << endl;
    goto retry;
  }
  
  
  
  Preprocessing(myImage, c1, c2);
  BMP WordCount = myImage;
  BMP LetterCount = myImage;
  BMP LineCount = myImage;

  int letters = label (LetterCount, 1, 6, xlow, xhigh, ylow, yhigh, c1, c2, c3, c4);
  LetterCount.WriteToFile(argv[2]);

  int lines = label (LineCount, 30, 6, xlow, xhigh, ylow, yhigh, c1, c2, c3, c4);
  //LineCount.WriteToFile(argv[2]);


  int words = label (WordCount, 6, 6, xlow, xhigh, ylow, yhigh, c1, c2, c3, c4);
  //WordCount.WriteToFile(argv[2]);



  cout << "The number of letters in the text are: " << letters << endl;
  cout << "The number of lines in the text are: " << lines <<  endl;
  cout << "The number of words in the text are " << words << endl;
  cout << "Writing new image with bounding boxes and text color changed to file."<< endl;
  
  return 0;
}
