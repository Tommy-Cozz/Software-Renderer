//Currently does not work while implimenting mouse movement it did not enjoy it however workign on fix soon enough but for practical purposes posting on github

#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>


#include "TexturesForRayCast/All_textures.ppm"
#include "TexturesForRayCast/da_sky_1.ppm"
#include "TexturesForRayCast/winning_screen.ppm"
#include "TexturesForRayCast/Lose_screen.ppm"
#include "TexturesForRayCast/titlescreen.ppm"
#include "TexturesForRayCast/sprites.ppm"

float degToRad(float a) { return a*M_PI/180.0;}
float FixAng(float a){ if(a>359){ a-=360;} if(a<0){ a+=360;} return a;}
float distance(ax,ay,bx,by,ang){ return cos(degToRad(ang))*(bx-ax)-sin(degToRad(ang))*(by-ay);}
float px,py,pdx,pdy,pa;
float frame1, frame2, fps;
int gameState=0, timer=0;
int mouse_x, mouse_y;
float fade=0;


typedef struct
{
	int w,a,d,s; //button states to be used to determine if high or lo 1 is high 0 is lo (positive logic)
}ButtonKeys; ButtonKeys Keys;

//typedef struct
//{
//	float mouseYPos, mouseYNeg, mouseXPos, mouseXNeg;
//}MouseDirection; MouseDirection mouseDir;


//-----------------------------MAP----------------------------------------------
#define mapX  8      //map width
#define mapY  8      //map height
#define mapS 64      //map cube size
int mapW[]=           //the map array. Edit to change level but keep the outer walls
{
 1,1,1,1,1,1,1,1,
 1,0,0,1,0,0,0,1,
 1,0,0,4,0,2,0,1,
 1,1,4,1,0,0,0,1,
 2,0,0,0,0,0,0,1,
 2,0,0,0,0,1,0,1,
 2,0,0,0,0,0,0,1,
 1,1,3,1,3,1,3,1,	
};


int mapF[]=           //floor
{
 0,0,0,0,1,1,0,0,
 0,0,2,0,0,0,0,0,
 0,0,2,0,0,2,0,0,
 0,0,0,2,0,2,0,0,
 0,0,2,0,0,2,0,0,
 0,0,2,0,0,0,0,0,
 0,0,2,0,2,0,0,0,
 0,0,0,0,0,0,0,0,	
};

int mapC[]=           //ceiling tile pallete
{
 0,0,0,0,1,1,0,0,
 0,0,2,0,0,0,0,0,
 0,0,2,0,0,2,0,0,
 0,0,0,2,0,2,0,0,
 0,2,2,2,0,2,0,0,
 0,0,2,0,0,0,0,0,
 0,0,2,0,2,0,0,0,
 0,0,0,0,0,0,0,0,	
};

//Sprite is just a flat plane in the world that you can have images on 

typedef struct
{
	int type; //static, type, key, enemy what type of sprite will this be
	int state; //is the sprite on or off 
	int map;
	float x,y,z; //what is the position of the sprite
}sprite; sprite sp[4];
int depth[120];


void drawSprite()
{
	int x,y,s;
	if(px<sp[0].x+30 && px>sp[0].x-30 && py<sp[0].y+30 && py>sp[0].y-30){sp[0].state=0;} //pick up key
	if(px<sp[3].x+30 && px>sp[3].x-30 && py<sp[3].y+30 && py>sp[3].y-30){gameState=4;} //enemy can one shot player
	

   //enemy attack
   int spx=(int)sp[3].x>>6,          spy=(int)sp[3].y>>6;          //normal grid position
   int spx_add=((int)sp[3].x+15)>>6, spy_add=((int)sp[3].y+15)>>6; //normal grid position plus     offset
   int spx_sub=((int)sp[3].x-15)>>6, spy_sub=((int)sp[3].y-15)>>6; //normal grid position subtract offset
   if(sp[3].x>px && mapW[spy*8+spx_sub]==0){ sp[3].x-=0.04*fps;}
   if(sp[3].x<px && mapW[spy*8+spx_add]==0){ sp[3].x+=0.04*fps;}
   if(sp[3].y>py && mapW[spy_sub*8+spx]==0){ sp[3].y-=0.04*fps;}
   if(sp[3].y<py && mapW[spy_add*8+spx]==0){ sp[3].y+=0.04*fps;}
	
	for(s=0;s<4;s++)   //create a for loioop to cycle through all the structs in the array to draw all of the sprites within one function
	{
	 float sx= sp[s].x-px;
	 float sy=sp[s].y-py;
	 float sz=sp[s].z;
	
	 float CS=cos(degToRad(pa)), SN = sin(degToRad(pa));						//need to rotate the sprite around origin(billboarding is my guess what this equation is)	
	 float a=sy*CS+sx*SN;
	 float b=sx*CS-sy*SN;
	 sx=a; sy=b;
	
	 sx=(sx*108.0/sy)+(120/2);   //convert to screen x,y
	 sy=(sz*108.0/sy)+(80/2);
	
	
	 int scale=32*80/b;
	                                           //fix for if the sprite is too small or too big so limits are set here
	 if(scale<0){scale=0;} if(scale>120){scale=120;}
	 
	 
	 //adding texture to the sprites
	 float t_x = 0, t_y =31, t_x_step=31.5/(float)scale, t_y_step=32.0/(float)scale;  //The height of the texture divided by the scale (height is 32 and width is 32 empty space around the sprite is purple as it is easy not to render out as sprites from other classic games use this technique)
	 
	 
	 for(x=sx-scale/2;x<sx+scale/2;x++)
	 {
	  t_y=31;
	  for(y=0;y<scale;y++)
	  {
	   if(sp[s].state==1 && x>0 && x<120 && b<depth[x])  //only draw if on screen and z depth is closer than the wall and if it is further than the wall not allowed to run it 
	    {
	     int pixel=((int)t_y*32+(int)t_x)*3+(sp[s].map*32*32*3);  
		 int red = sprites[pixel+0];
		 int green = sprites[pixel+1];
		 int blue = sprites[pixel+2];
		 if(red!=255,green!=0, blue!=255)   //dont draw purple area
		 {
		 glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS); glVertex2i(x*8,sy*8-y*8); glEnd(); //draw point 
	     }
		 t_y-=t_y_step; if(t_y<0){t_y=0;}
	    } 
	  }
	  t_x+=t_x_step;
	 }
    }
}
//---------------------------Draw Rays and Walls--------------------------------


void drawRays2D()
{
	 	
	
 int r,mx,my,mp,dof,side; float vx,vy,rx,ry,ra,xo,yo,disV,disH; 
 
 ra=FixAng(pa+30);                                                              //ray set back 30 degrees
 
 for(r=0;r<120;r++)
 {
	

	int vmt =0, hmt=0;  //vertical and horizontal map texture number first holding on the to the value the wall hits
  //---Vertical--- 
  dof=0; side=0; disV=100000;
  float Tan=tan(degToRad(ra));
       if(cos(degToRad(ra))> 0.001){ rx=(((int)px>>6)<<6)+64;      ry=(px-rx)*Tan+py; xo= 64; yo=-xo*Tan;}//looking left
  else if(cos(degToRad(ra))<-0.001){ rx=(((int)px>>6)<<6) -0.0001; ry=(px-rx)*Tan+py; xo=-64; yo=-xo*Tan;}//looking right
  else { rx=px; ry=py; dof=8;}                                                  //looking up or down. no hit  

  while(dof<8) 
  { 
   mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;                     
   if(mp>0 && mp<mapX*mapY && mapW[mp]>0){vmt=mapW[mp]-1; dof=8; disV=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);}//hit         
   else{ rx+=xo; ry+=yo; dof+=1;}                                               //check next horizontal
  } 
  vx=rx; vy=ry;

  //---Horizontal---
  dof=0; disH=100000;
  Tan=1.0/Tan; 
       if(sin(degToRad(ra))> 0.001){ ry=(((int)py>>6)<<6) -0.0001; rx=(py-ry)*Tan+px; yo=-64; xo=-yo*Tan;}//looking up 
  else if(sin(degToRad(ra))<-0.001){ ry=(((int)py>>6)<<6)+64;      rx=(py-ry)*Tan+px; yo= 64; xo=-yo*Tan;}//looking down
  else{ rx=px; ry=py; dof=8;}                                                   //looking straight left or right
 
  while(dof<8) 
  { 
   mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;                          
   if(mp>0 && mp<mapX*mapY && mapW[mp]>0){ hmt=mapW[mp]-1; dof=8; disH=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);}//hit         
   else{ rx+=xo; ry+=yo; dof+=1;}                                               //check next horizontal
  } 
  
  float shade =1;
  glColor3f(0,0.8,0);
  if(disV<disH){hmt=vmt; shade=0.5; rx=vx; ry=vy; disH=disV; glColor3f(0,0.6,0);}                  //horizontal hit first
  
    
  int ca=FixAng(pa-ra); disH=disH*cos(degToRad(ca));                            //fix fisheye 
  int lineH = (mapS*640)/(disH); 
  float ty_step=32.0/(float)lineH;
  float ty_off =0;
  if(lineH>640){ty_off=(lineH-640)/2.0; lineH=640;}                     //line height and limit
  int lineOff = 320 - (lineH>>1);                                               //line offset
  
  //glLineWidth(8);glBegin(GL_LINES);glVertex2i(r*8+530,lineOff);glVertex2i(r*8+530,lineOff+lineH);glEnd();//draw vertical wall  
  
	depth[r] = disH;  //save lines depth
  //Drawin the walls
  int y;
  float ty = ty_off*ty_step;// + hmt*32;  //ty is the textures y values (texture y starting value)
  float tx;
  if(shade==1){tx = (int)(rx/2.0)%32; if(ra>180){tx=31-tx;}} //the rays x value divide by 2 becuase textures are half the size then the modulous to find the remainder of the division of 32 ; If the rays angle is greater than 180 degrees subtract then flip it
  	else	  {tx = (int)(ry/2.0)%32; if(ra>90){tx=31-tx;}}

  for(y=0;y<lineH;y++)
  {
  
	int pixel=((int)ty*32+(int)tx)*3+(hmt*32*32*3);  //since each color for each pixel is defined by 3 lines in the ppm text file multiply by 3 in order to skip to each pixel
	int red = All_textures[pixel+0]*shade;
	int green = All_textures[pixel+1]*shade;
	int blue = All_textures[pixel+2]*shade;
	glPointSize(8); glColor3ub(red, green,blue); glBegin(GL_POINTS); glVertex2i(r*8, y+lineOff); glEnd();	
	ty += ty_step;
  }
  
  //Drawing the floors
	
  for(y = lineOff+lineH;y<640;y++)   //only need to start drawing at the bottom of the wall and then to the screens edge 
  {
	float dy=y-(640/2.0), deg = degToRad(ra), raFix = cos(degToRad(FixAng(pa-ra)));	
	tx=px/2+ cos(deg) * 158*2*32/dy/raFix;                                                                            //utlizing trig to find the point at which the ray cast is on the "ground" based on the caluclation of these to neccessary for both to get the resultant vector
	ty=py/2- sin(deg) * 158*2*32/dy/raFix;                                                                           //this takes the lookup for the tx and ty in the texture table
	int mp=mapF[(int)(ty/32.0)*mapX+(int)(tx/32.0)]*32*32;
	//float c=All_textures[((int)(ty)&31)*32 + ((int)(tx)&31)+mp]*0.7;                                             //Use the & operator to return the direct value in the table
	//glColor3f(c/1.7,c/2.0,c/1.3);glPointSize(8);glBegin(GL_POINTS);glVertex2i(r*8+530, y);glEnd();
  	int pixel=(((int)(ty)&31)*32 + ((int)(tx)&31))*3+mp*3;                                                              //since each color for each pixel is defined by 3 lines in the ppm text file multiply by 3 in order to skip to each pixel
	int red = All_textures[pixel+0]*0.7;
	int green = All_textures[pixel+1]*0.7;
	int blue = All_textures[pixel+2]*0.7;
	glPointSize(8); glColor3ub(red, green,blue); glBegin(GL_POINTS); glVertex2i(r*8, y); glEnd();

    
	//draw the cieling
	mp=mapC[(int)(ty/32.0)*mapX+(int)(tx/32.0)]*32*32;
	//c=All_textures[((int)(ty)&31)*32 + ((int)(tx)&31)+mp]*0.7;  //Use the & operator to return the direct value in the table
    //glColor3f(c/1.2,c,c/2.0);glPointSize(8);glBegin(GL_POINTS);glVertex2i(r*8+530, 320-y);glEnd();
	pixel=(((int)(ty)&31)*32 + ((int)(tx)&31))*3+mp*3;                                                              //since each color for each pixel is defined by 3 lines in the ppm text file multiply by 3 in order to skip to each pixel
	red = All_textures[pixel+0];
	green = All_textures[pixel+1];
	blue = All_textures[pixel+2];
	if(mp>0){glPointSize(8); glColor3ub(red, green,blue); glBegin(GL_POINTS); glVertex2i(r*8, 640-y); glEnd();}
  }
	
  
  ra=FixAng(ra-0.5);                                                              //go to next ray
 }
}//-----------------------------------------------------------------------------

void drawSky()
{int x,y;
  for(y=0;y<40;y++)
  {
	for(x=0;x<120;x++)
	{
	  int xo = (int)pa-x; if(xo<0){xo+=120;} xo=xo%120; //This will allow for the implimentation of the sky scrolling infinitely
	  int pixel=(y*120+xo)*3;  //since each color for each pixel is defined by 3 lines in the ppm text file multiply by 3 in order to skip to each pixel
	  int red = Da_sky[pixel+0];
	  int green = Da_sky[pixel+1];
	  int blue = Da_sky[pixel+2];
	  glPointSize(8); glColor3ub(red, green,blue); glBegin(GL_POINTS); glVertex2i(x*8, y*8); glEnd();	
	}
  }	
}




void screen(int v)   //since all three images are the same size we can use a pointer that just points to the cooresponding array per screen 
{
  int x,y;
  int *T;
  if(v==1){T = title_screen;}
  if(v==2){T = win_screen;}
  if(v==3){T = lose_screen;}	
  for(y=0;y<80;y++)
  {
	for(x=0;x<120;x++)
	{
	  int xo = (int)pa-x; if(xo<0){xo+=120;} xo=xo%120; //This will allow for the implimentation of the sky scrolling infinitely
	  int pixel=(y*120+xo)*3;  //since each color for each pixel is defined by 3 lines in the ppm text file multiply by 3 in order to skip to each pixel
	  int red = T[pixel+0]*fade;
	  int green = T[pixel+1]*fade;
	  int blue = T[pixel+2]*fade;
	  glPointSize(8); glColor3ub(red, green,blue); glBegin(GL_POINTS); glVertex2i(x*8, y*8); glEnd();	
	}
  }
  if(fade<1){fade+=0.001*fps;}
  if(fade>1){fade=1;}	
}


void init()
{
 glClearColor(0.3,0.3,0.3,0);
 px=150; py=400; pa=90;
 pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa)); 
 mapW[19]=4; mapW[26]=4; //close doors THATS RIGHT ITS A FUNCIONTAL DOOR(not really i get it soon they will close tho) 
 //center the mouse 
 glutWarpPointer(320,240);
 //Hide the cursor 
 glutSetCursor(GLUT_CURSOR_NONE);
 
 
 
 sp[0].type=1; sp[0].state =1; sp[0].map =0; sp[0].x =1.5*64; sp[0].y =6*64; sp[0].z=20; //key (sprite 1
 sp[1].type=2; sp[1].state =1; sp[1].map =1; sp[1].x =1.5*64; sp[1].y =5*64; sp[1].z=0; //light (sprite 2
 sp[2].type=2; sp[2].state =1; sp[2].map =1; sp[2].x =3.5*64; sp[2].y =5*64; sp[2].z=0; //light (sprite 3
 sp[3].type=3; sp[3].state =1; sp[3].map =2; sp[3].x =2.5*64; sp[3].y =2*64; sp[3].z=20; //enemy (sprite 4
 
 
}

 



void display()
{   
 
  frame2 = glutGet(GLUT_ELAPSED_TIME); fps = (frame2-frame1); frame1 = glutGet(GLUT_ELAPSED_TIME);   //hold current elapsed time subtract from preivous elapsed time then reset first elapsed time to the current
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
  
  if(gameState==0){init(); fade=0; timer=0; gameState=1;}  //game state initalize
  if(gameState==1){screen(1); timer+=1*fps; if(timer>2000){fade=0; timer=0; gameState=2;}} //start screen and setting game state to 2 which begins the game
  if(gameState==2)
  {
   //buttons

   int xo =0; if(pdx<0){xo=-20;} else{xo=20;}  //creating the x and y offset variables set to about 20 units in front of player this will create a line in order to check if the tile infront of player is avaiable space or defined as a wall
   int yo =0; if(pdy<0){yo=-20;} else{yo=20;}
   int ipx = px/64.0, ipx_add_xo=(px+xo)/64.0, ipx_sub_xo=(px-xo)/64.0;  //hold players grid pos and playpos plus the x offest and then minus the x offset
   int ipy = py/64.0, ipy_add_yo=(py+yo)/64.0, ipy_sub_yo=(py-yo)/64.0;
   
   if(Keys.a== 1)                                                             //In order to add coliison detecetion to the left and right it is caluclated based on angles 90 degreses to the left and right of the current players angle 
   { 
    float pa_left = FixAng(pa + 90);
    float pdx_left = cos(degToRad(pa_left));
    float pdy_left = -sin(degToRad(pa_left));
    int xo_left = 0; if(pdx_left < 0) { xo_left = -20; } else { xo_left = 20; }
    int yo_left = 0; if(pdy_left < 0) { yo_left = -20; } else { yo_left = 20; }
    int ipx_left = (px + pdx_left * 20) / 64.0;
    int ipy_left = (py + pdy_left * 20) / 64.0;
            
    if(mapW[ipy * mapX + ipx_left] == 0) { px += pdx_left * 0.2 * fps; }
    if(mapW[ipy_left * mapX + ipx] == 0) { py += pdy_left * 0.2 * fps; }
   }  	 
   
   
   if(Keys.d == 1)
    {
      float pa_right = FixAng(pa - 90);
      float pdx_right = cos(degToRad(pa_right));
      float pdy_right = -sin(degToRad(pa_right));
      int xo_right = 0; if(pdx_right < 0) { xo_right = -20; } else { xo_right = 20; }
      int yo_right = 0; if(pdy_right < 0) { yo_right = -20; } else { yo_right = 20; }
      int ipx_right = (px + pdx_right * 20) / 64.0;
      int ipy_right = (py + pdy_right * 20) / 64.0;
            
      if(mapW[ipy * mapX + ipx_right] == 0) { px += pdx_right * 0.2 * fps; }
      if(mapW[ipy_right * mapX + ipx] == 0) { py += pdy_right * 0.2 * fps; }
    }
  
  
   
  
   if(Keys.w== 1)
   { 
  	if(mapW[ipy*mapX + ipx_add_xo]==0){px+=pdx*0.2*fps;}   //Check the players y position then the x positon plus the x offset if good move players x position (Same thing for moving the players y position)
	if(mapW[ipy_add_yo*mapX + ipx]==0){py+=pdy*0.2*fps;}
   }
   if(Keys.s== 1)
   { 
  	if(mapW[ipy*mapX + ipx_sub_xo]==0){px-=pdx*0.2*fps;}   //Check the players y position then the x positon plus the x offset if good move players x position (Same thing for moving the players y position)
	if(mapW[ipy_sub_yo*mapX + ipx]==0){py-=pdy*0.2*fps;}
   }
  
   
  
   drawSky();
   drawRays2D();
   drawSprite();
   if((int)px>>6==1 && (int)py>>6==1){fade=0; timer=0; gameState=3;}
  }

   if(gameState==3){screen(2); timer+=1*fps; if(timer>2000){fade=0; timer=0; gameState=0;}}  //winning 
   if(gameState==4){screen(3); timer+=1*fps; if(timer>2000){fade=0; timer=0; gameState=0;}}  //Lose
 
 
 
  glutPostRedisplay();
  glutSwapBuffers();  
}


void ButtonDown(unsigned char key, int x, int y)
{
	if(key =='a'){Keys.a = 1;}
	if(key =='d'){Keys.d = 1;}
	if(key =='w'){Keys.w = 1;}
	if(key =='s'){Keys.s = 1;}
	if(key == 'e' && sp[0].state==0)     //interact button (Just open doors for now)
	{
	 int xo =0; if(pdx<0){xo=-25;} else{xo=25;}  
  	 int yo =0; if(pdy<0){yo=-25;} else{yo=25;}
	 int ipx = px/64.0, ipx_add_xo=(px+xo)/64.0;  
  	 int ipy = py/64.0, ipy_add_yo=(py+yo)/64.0;  //only opening doors in front of the player not behind (just makin sure)
	 if(mapW[ipy_add_yo*mapX+ipx_add_xo]==4){mapW[ipy_add_yo*mapX+ipx_add_xo]=0;}  //if players x and y position plus the offset in the map is 4 set the same value to 0 (Opening of the door basically 4 is the code for door if player in front and interacts with e 4 goes to 0 which is "open" animations to be added later)
	}
	glutPostRedisplay();
}

void ButtonUp(unsigned char key, int x, int y)
{
	if(key =='a'){Keys.a = 0;}
	if(key =='d'){Keys.d = 0;}
	if(key =='w'){Keys.w = 0;}
	if(key =='s'){Keys.s = 0;}
	glutPostRedisplay();
}

void mouseFunc(int x, int y)
{
	int diff_x = x - mouse_x;
	pa += diff_x* 0.1; //0.1 is the mouse sensitivity
	pa = FixAng(pa);
	pdx = -sin(degToRad(pa));
	pdy = cos(degToRad(pa));
	
	mouse_x = x;
	mouse_y = y;
	
}


void resize(int w, int h)
{
	glutReshapeWindow(960,640);	
}



int main(int argc, char* argv[])
{ 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 glutInitWindowSize(960,640);
 glutInitWindowPosition(glutGet(GLUT_SCREEN_WIDTH)/2-960/2,glutGet(GLUT_SCREEN_HEIGHT)/2-640/2);
 glutCreateWindow("BALL ITUS YOU HAVE TORSION");
 gluOrtho2D(0,960,640,0);
 init();
 glutDisplayFunc(display);
 glutReshapeFunc(resize);
 glutKeyboardFunc(ButtonDown);
 glutKeyboardUpFunc(ButtonUp);
 glutMainLoop();
}

