#ifndef CS247_PROG_H
#define CS247_PROG_H

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

// framework includes
#include "glslprogram.h"
#include "vboquad.h"


////////////////
// Structures //
////////////////

// window size
const unsigned int gWindowWidth = 512;
const unsigned int gWindowHeight = 512;

int current_scalar_field;
int data_size;
bool en_arrow;
bool en_streamline;
bool en_pathline;

int sampling_rate;
float dt;
bool useRK2;




//////////////////////
//  Global defines  //
//////////////////////
#define TIMER_FREQUENCY_MILLIS  50

//////////////////////
// Global variables //
//////////////////////

// Handle of the window we're rendering to
static GLFWwindow* window;

char bmModifiers;	// keyboard modifiers (e.g. ctrl,...)

int clearColor;

// data handling
char* filenames[ 3 ];
bool grid_data_loaded;
bool scalar_data_loaded;
unsigned short vol_dim[ 3 ];
float* vector_array;
float* scalar_fields;
float* scalar_bounds;

GLuint scalar_field_texture;

int num_scalar_fields;
int num_timesteps; //stores number of time steps

int loaded_file;
int loaded_timestep;
float timestep;

int view_width, view_height; // height and width of entire view

GLuint displayList_idx;

int toggle_xy;

////////////////
// Prototypes //
////////////////

void drawGlyphs();

void computeStreamline(int x, int y);

void computePathline(int x, int y, int t);

void loadNextTimestep( void );
void recomputeAllStreamlines( void );

void LoadData( char* base_filename );
void LoadVectorData( const char* filename );

void DownloadScalarFieldAsTexture( void );
void initGL( void );

void reset_rendering_props( void );

// TODO: define data arrays, VAO and VBO
// Hint: you need one for the glyphs, streamlines, pathlines
GLuint glyphVAO, glyphVBO;

std::vector<GLuint> streamlineVAOs, streamlineVBOs;
std::vector<int>    streamlineVertexCounts;
std::vector<int>    streamlineSeedsX, streamlineSeedsY;
std::vector<GLuint> pathlineVAOs,   pathlineVBOs;
std::vector<int>    pathlineVertexCounts;

// TODO: define colormap variables
// Hint: you need a colormap mode (off/rainbow/cool-warm) and a blend factor
int   colormapMode;
float blendFactor;

// make quad to load texture to
VBOQuad quad;

// GLSL
GLSLProgram vectorProgram;
glm::mat4 model;


#endif //CS247_PROG_H
