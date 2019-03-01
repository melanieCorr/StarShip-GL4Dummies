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

  assimpInit("models/Falcon/millenium-falcon.obj");
  
  _pId  = gl4duCreateProgram("<vs>shaders/depTex.vs", "<fs>shaders/depTex.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/model.vs", "<fs>shaders/model.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_wW, _wH);  
  
  _sphere = gl4dgGenSpheref(30, 30);
  
   glGenTextures(sizeof _tex / sizeof *_tex, _tex);
  /* chargement et transfert des images dans des textures OpenGL */
  
  loadTexture(_tex[0], "images/tatooine.jpg");
  
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


/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {

  GLboolean dt = glIsEnabled(GL_DEPTH_TEST);
  GLfloat lum[4] = {0.0, 0.0, 5.0, 1.0};
  glClearColor(0.11f, 0.16f, 0.32f, 1.0f);
  
  static GLfloat a = 0;
  static GLfloat b = 0;
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
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
  glDepthMask(GL_TRUE);

  glEnable(GL_DEPTH_TEST);
  
  glUseProgram(_pId);
  
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  
  gl4duTranslatef(0, 0, -3.0f);
  gl4duPushMatrix();
  gl4duRotatef(a, 0, 1, 0);
  gl4duPushMatrix(); {
    gl4duScalef(0.8f, 0.8f, 0.8f);
    gl4duSendMatrices();
  }
  gl4duPopMatrix();
  gl4duPushMatrix();
  glBindTexture(GL_TEXTURE_2D, _tex[0]);
  gl4dgDraw(_sphere);
  gl4duPopMatrix();
  a = a + 1;
  
  
  gl4duLoadIdentityf();
  gl4duTranslatef(0.6f, 0, -3.0f);
  gl4duPushMatrix();
  gl4duRotatef(b, 0, 1, 0);
  gl4duTranslatef(2.3f	, 0, 0);
  gl4duPushMatrix(); {
    gl4duScalef(0.3f, 0.3f, 0.3f);
    gl4duSendMatrices();
  }  
  gl4duPopMatrix();
  glUseProgram(_pId2);
  b = b + 1;
  assimpDrawScene();
  //glEnable(GL_DEPTH_TEST);
  if(!dt)
    glDisable(GL_DEPTH_TEST);
}


/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  /* suppression de plusieurs identifiants de texture */
  glDeleteTextures(sizeof _tex / sizeof *_tex, _tex);
}


void falcon(int state) {
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
