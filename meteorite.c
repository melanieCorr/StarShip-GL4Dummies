/*!\file window.c
 * \brief géométries lumière diffuse, transformations de base et chargement de textures en GL4Dummies
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date May 13 2018 */
#include <stdio.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <GL4D/gl4dp.h>
#include <time.h>
#include <assert.h>
#include <GL4D/gl4dh.h>
#include <SDL_mixer.h>
#include "assimp.h"
#include "assimp2.h"

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void loadTexture(GLuint id, const char * filename);
static void resize(int w, int h);
static void draw(void);
static void quit(void);

/*!\brief dimensions de la fenêtre */
static int _wW = 800, _wH = 600;

/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0, _pId2 = 0;;

/*!\brief quelques objets géométriques */
static GLuint _sphere = 0, _tex[2] = {0};

//static GLfloat _alpha = 0;

/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.*/
 
typedef struct Etoile{
    int x;
    int y;
}Etoile;

static Etoile etoiles[100];
static GLuint _screen = 0;


/*!\brief initialise les paramètres OpenGL et les données */
static void init(void) {
  glClearColor(0.11f, 0.16f, 0.32f, 1.0f);
//  assimpInit("models/meteor/Meteor.obj");
  _pId  = gl4duCreateProgram("<vs>shaders/depTex.vs", "<fs>shaders/depTex.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/model.vs", "<fs>shaders/model.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_wW, _wH);  
  
  _sphere = gl4dgGenSpheref(30, 30);
  
  glGenTextures(sizeof _tex / sizeof *_tex, _tex);
  
  int i;
	_screen = gl4dpInitScreen();
	srand(time(NULL));
 	for(i=0; i< sizeof etoiles / sizeof etoiles[0]; i++){
 		etoiles[i].x = rand()%_wW;
    	etoiles[i].y = rand()%_wH;
  }
  
  assimpInit("models/Falcon/millenium-falcon.obj");
  assimpInit2("models/meteor/Meteor.obj");
  //glEnable(GL_DEPTH_TEST);
}


static void loadTexture(GLuint id, const char * filename) {
  SDL_Surface * t;
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  if( (t = IMG_Load(filename)) != NULL ) {
#ifdef __APPLE__
    int mode = t->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
#else
    int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;
#endif       
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
    SDL_FreeSurface(t);
  } else {
    fprintf(stderr, "can't open file %s : %s\n", filename, SDL_GetError());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  }
}


/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.*/
static void resize(int w, int h) {
  _wW  = w; _wH = h;
  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _wH / _wW, 0.5 * _wH / _wW, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
}


/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
 
  GLfloat lum[4] = {0.0, 0.0, 5.0, 1.0};

  static GLfloat c = -20;
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthMask(GL_FALSE);
   //GLubyte r = 255, g = 255, b = 255;
   GLubyte r = rand() % 256, g = rand() % 256, b = rand() % 256;
    gl4dpSetScreen(_screen);
    
	gl4dpClearScreenWith(RGB(11, 6, 52));
    int i =0; 
    for(i=0; i<sizeof etoiles / sizeof etoiles[0]; i++){
    	gl4dpSetColor(RGB(r, g, b));
      gl4dpPutPixel(etoiles[i].x, etoiles[i].y);
    }
  gl4dpUpdateScreen(NULL);
  glDepthMask(GL_TRUE);
  glUseProgram(_pId2);
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lum);
  
  // Chargement du vaisseau
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  
  gl4duPushMatrix();
  gl4duTranslatef(0, 0, -3);
  gl4duRotatef(0, 10, 0, 0);
  
  gl4duPushMatrix(); {
    gl4duScalef(1.0f, 1.0f, 1.0f);
    gl4duSendMatrices();
  } 
  assimpDrawScene();
  gl4duPopMatrix(); 
  

  // Chargement des meterorites
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  
  gl4duPushMatrix();
  gl4duTranslatef(-7, 0, c);
  c = c + 0.09;
  if (c >= -5)	c = -20;
  assimpDrawScene2();
  gl4duPopMatrix();
  
  gl4duPushMatrix();
  gl4duTranslatef(7, 0, c);
  if (c >= -5)	c = -20;
  assimpDrawScene2();
  gl4duPopMatrix();
  
  gl4duPushMatrix();
  gl4duTranslatef(0, -3, c);
  if (c >= -5)	c = -20; 
  assimpDrawScene2();
  gl4duPopMatrix();
  
  gl4duPushMatrix();
  gl4duTranslatef(0, 3, c);
  if (c >= -5)	c = -20; 
  assimpDrawScene2();
  gl4duPopMatrix();
  glEnable(GL_DEPTH_TEST);
  
  gl4duPushMatrix();
  gl4duTranslatef(-7, -5, c);
  if (c >= -5)	c = -20; 
  assimpDrawScene2();
  gl4duPopMatrix();
  glEnable(GL_DEPTH_TEST);
  
  gl4duPushMatrix();
  gl4duTranslatef(-5, 4, c);
  if (c >= -5)	c = -20; 
  assimpDrawScene2();
  gl4duPopMatrix();
  glEnable(GL_DEPTH_TEST);
  
  gl4duPushMatrix();
  gl4duTranslatef(6, -3, c);
  if (c >= -5)	c = -20; 
  assimpDrawScene2();
  gl4duPopMatrix();
  glEnable(GL_DEPTH_TEST);
  
  gl4duPushMatrix();
  gl4duTranslatef(8, 6, c);
  if (c >= -5)	c = -20; 
  assimpDrawScene2();
  gl4duPopMatrix();
  glEnable(GL_DEPTH_TEST);
  
  gl4duPushMatrix();
  gl4duTranslatef(5, 4, c);
  if (c >= -5)	c = -20; 
  assimpDrawScene2();
  gl4duPopMatrix();
  glEnable(GL_DEPTH_TEST);
  
  gl4duPushMatrix();
  gl4duTranslatef(0, 6, c);
  if (c >= -5)	c = -20; 
  assimpDrawScene2();
  gl4duPopMatrix();
  glEnable(GL_DEPTH_TEST);
  
  gl4duPushMatrix();
  gl4duTranslatef(3, -6, c);
  if (c >= -5)	c = -20; 
  assimpDrawScene2();
  gl4duPopMatrix();
  glEnable(GL_DEPTH_TEST);
  
  gl4duPushMatrix();
  gl4duTranslatef(-8, 6, c);
  if (c >= -5)	c = -20; 
  assimpDrawScene2();
  gl4duPopMatrix();
  glEnable(GL_DEPTH_TEST);
  
  gl4duPushMatrix();
  gl4duTranslatef(-3, -5, c);
  if (c >= -5)	c = -20; 
  assimpDrawScene2();
  gl4duPopMatrix();
  glEnable(GL_DEPTH_TEST);
}


/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  /* suppression de plusieurs identifiants de texture */
  glDeleteTextures(sizeof _tex / sizeof *_tex, _tex);
  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
  assimpQuit();
  assimpQuit2();
}


void meteorite(int state) {
  switch(state) {
  case GL4DH_INIT:
    init();
    return;
  case GL4DH_FREE:
    quit();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    return;
  default: /* GL4DH_DRAW */
    draw();
    return;
  }
}
