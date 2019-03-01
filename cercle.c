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

/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.*/
 
extern void assimpInit(const char * filename);
extern void assimpDrawScene(void);
extern void assimpQuit(void);

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void loadTexture(GLuint id, const char * filename);
static void resize(int w, int h);
static void draw(void);
static void quit(void);
static void br65(int x0, int y0, int x1, int y1);

/*!\brief dimensions de la fenêtre */
static int _wW = 800, _wH = 600;

/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0, _pId2 = 0;

/*!\brief quelques objets géométriques */
static GLuint _sphere = 0, _tex[2] = {0};


typedef struct Etoile{
    int x;
    int y;
}Etoile;

static Etoile etoiles[100];
static GLuint _screen = 0;

/*!\brief initialise les paramètres OpenGL et les données */
static void init(void) {
  glEnable(GL_TEXTURE_2D);
  _screen = gl4dpInitScreen();
  assimpInit("models/Falcon/millenium-falcon.obj");
  
  _pId  = gl4duCreateProgram("<vs>shaders/depTex.vs", "<fs>shaders/depTex.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/model.vs", "<fs>shaders/model.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_wW, _wH);  
  
  //_sphere = gl4dgGenSpheref(30, 30);
  
   glGenTextures(sizeof _tex / sizeof *_tex, _tex);
  
  int i;
  _screen = gl4dpInitScreen();
  srand(time(NULL));
  for(i=0; i< sizeof etoiles / sizeof etoiles[0]; i++){
  	etoiles[i].x = rand()%_wW;
  	etoiles[i].y = rand()%_wH;
  }
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


static void br65(int x0, int y0, int x1, int y1) {
  int u = x1 - x0, v = y1 - y0, del, incH, incO, au, av, pasX, pasY;
  if(u < 0) {
    au = -u; pasX = -1;
  } else {
    au = u;  pasX = 1;
  }
  if(v < 0) {
    av = -v; pasY = -1;
  } else {
    av = v;  pasY = 1;
  }
  if(au > av) { /* premier octant */
    del = (incH = -2 * av) + au;
    incO = 2 * au + incH;
    for(x1 += pasX; x0 != x1; x0 += pasX) {
      gl4dpPutPixel(x0, y0); 
      if(del < 0) {
	del += incO;
	y0 += pasY;
      } else 
	del += incH;
    }
  } else {  /* deuxième octant */
    del = (incH = -2 * au) + av;
    incO = 2 * av + incH;
    for(y1 += pasY; y0 != y1; y0 += pasY) {
      gl4dpPutPixel(x0, y0);
      if(del < 0) {
	del += incO;
	x0 += pasX;
      } else 
	del += incH;
    }
  }
}


/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GLfloat lum[4] = {0.0, 0.0, 5.0, 1.0};
  
  glDepthMask(GL_FALSE);
  //GLubyte r = 255, g = 255, b = 255;
   GLubyte red = rand() % 256, green = rand() % 256, blue = rand() % 256;
  gl4dpSetScreen(_screen);
    
  gl4dpClearScreenWith(RGB(11, 6, 52));
  int i =0; 
  for(i=0; i<sizeof etoiles / sizeof etoiles[0]; i++){
  	gl4dpSetColor(RGB(red, green, blue));
    gl4dpPutPixel(etoiles[i].x, etoiles[i].y);
  }
  gl4dpUpdateScreen(NULL);
  
  
  float a;
  gl4dpSetScreen(_screen);
  for(a = 0; a < 2.0f * M_PI; a += 0.02f) {
    GLubyte r = rand() % 255, g = rand() % 255, b = 255;
    gl4dpSetColor(RGB(r, g, b));
    br65(gl4dpGetWidth() / 2, gl4dpGetHeight() / 2, 
	    gl4dpGetWidth() / 2  + (gl4dpGetWidth() / 2  - 1) * cos(a), 
	    gl4dpGetHeight() / 2 + (gl4dpGetHeight() / 2 - 1) * sin(a));
  }
  gl4dpUpdateScreen(NULL);
  glDepthMask(GL_TRUE);
  
  
  static GLfloat b = 1;
  glUseProgram(_pId);
  
  gl4duBindMatrix("modelViewMatrix");  
  
  gl4duLoadIdentityf();
  gl4duTranslatef(0, 0, b);
  gl4duPushMatrix();
  gl4duRotatef(0, 0, 1, 0);
  gl4duPushMatrix(); {
    gl4duScalef(0.3f, 0.3f, 0.3f);
    gl4duSendMatrices();
  }  
  gl4duPopMatrix();
  glUseProgram(_pId2);
  b = b - 0.08;
  assimpDrawScene();
  glEnable(GL_DEPTH_TEST);
}


/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  /* suppression de plusieurs identifiants de texture */
  glDeleteTextures(sizeof _tex / sizeof *_tex, _tex);
}


void cercle(int state) {
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
