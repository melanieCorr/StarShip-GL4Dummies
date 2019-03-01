/*!\file window.c
 * \brief géométries lumière diffuse, transformations de base et chargement de textures en GL4Dummies
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date May 13 2018 */
#include <stdio.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4dh.h>
#include <SDL_image.h>
#include <GL4D/gl4dp.h>
#include <time.h>
#include <assert.h>


/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void loadTexture(GLuint id, const char * filename);
static void resize(void);
static void draw(void);
static void quit(void);

/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;

/*!\brief quelques objets géométriques */
static GLuint _sphere = 0, _tex[3] = {0};

//static GLfloat _alpha = 0;

typedef struct Etoile{
    int x;
    int y;
}Etoile;

static Etoile etoiles[100];
static GLuint _screen = 0;

 

/*!\brief initialise les paramètres OpenGL et les données */
static void init(void) {  
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  _pId  = gl4duCreateProgram("<vs>shaders/depTex.vs", "<fs>shaders/depTex.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  _sphere = gl4dgGenSpheref(30, 30);
   glGenTextures(sizeof _tex / sizeof *_tex, _tex);
  /* chargement et transfert des images dans des textures OpenGL */
  loadTexture(_tex[0], "images/tatooine.jpg");
  loadTexture(_tex[1], "images/tatoo1.jpg");
  loadTexture(_tex[2], "images/tatoo2.jpg");
  
  int i;
  _screen = gl4dpInitScreen();
  srand(time(NULL));
  for(i=0; i< sizeof etoiles / sizeof etoiles[0]; i++){
    etoiles[i].x = rand()%vp[2];
    etoiles[i].y = rand()%vp[3];
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
static void resize(void) {
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5f, 0.5f, -0.5f * vp[3] / vp[2], 0.5 * vp[3] / vp[2], 1.0f, 1000.0f);
  gl4duBindMatrix("modelViewMatrix");
}


/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  GLboolean dt = glIsEnabled(GL_DEPTH_TEST);
  glClearColor(0.11f, 0.16f, 0.32f, 1.0f);
  
  static GLfloat a = 0;
  static GLfloat b = 0;
  
  static GLuint t0 = 0, otc = 0, firstTime = 1;
  GLuint tc;
  GLfloat dt1;
  if(firstTime){
  	t0 = SDL_GetTicks();
  	firstTime = 0;
  }
  tc = SDL_GetTicks() - t0;
  dt1 = (tc - otc) / 1000.0f;
  otc = tc;
  
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
  resize();
  glUseProgram(_pId);
  
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(0, 0, -3);
  gl4duPushMatrix();
  gl4duRotatef(a, 0, 1, 0);
  gl4duPushMatrix(); {
    gl4duScalef(0.4f, 0.4f, 0.4f);
    gl4duSendMatrices();
  }
  gl4duPopMatrix();
  glBindTexture(GL_TEXTURE_2D, _tex[0]);
  gl4dgDraw(_sphere);
  a = a + 0.3f;
  
  
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(0.1f, 0, -3.0f);
  gl4duPushMatrix();
  gl4duRotatef(b, 0, 1, 0);
  gl4duTranslatef(0.9f, 0, 0);
  gl4duPushMatrix(); {
    gl4duScalef(0.09f, 0.09f, 0.09f);
    gl4duSendMatrices();
  }
  gl4duPopMatrix();
  glBindTexture(GL_TEXTURE_2D, _tex[2]);
  gl4dgDraw(_sphere);
  
  
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(0.1f, 0, -3.0f);
  gl4duPushMatrix();
  gl4duRotatef(b, 0, 1, 0);
  gl4duTranslatef(1.1f, 0.1f, 0);
  gl4duPushMatrix(); {
    gl4duScalef(0.02f, 0.02f, 0.02f);
    gl4duSendMatrices();
  }
  gl4duPopMatrix();
  glBindTexture(GL_TEXTURE_2D, _tex[1]);
  gl4dgDraw(_sphere);
  
  b = b + 0.2f;

  static GLfloat count = 0;
  GLint cfbo, ctex = 0;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &cfbo);
  if(cfbo)
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &ctex);
  
  if(tc > 10000){
  	gl4dfBlur(ctex, ctex, count, 1, 0, GL_FALSE);
  	count++;
  }
  /*gl4dfScattering(ctex, ctex, 5, 0, 0, GL_FALSE);
  gl4dfMedian(ctex, ctex, 3, GL_FALSE);
  gl4dfSobelSetResultMode(GL4DF_SOBEL_RESULT_INV_LUMINANCE);*/
  gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
  gl4dfSobel(ctex, ctex, GL_FALSE);
 
 if(tc > 13000)
 	Mix_FadeOutMusic(2000);
 
  if(!dt)
    glDisable(GL_DEPTH_TEST);
}


/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  /* suppression de plusieurs identifiants de texture */
  glDeleteTextures(sizeof _tex / sizeof *_tex, _tex);
  //gl4duClean(GL4DU_ALL);
}


void tatooine(int state) {
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
