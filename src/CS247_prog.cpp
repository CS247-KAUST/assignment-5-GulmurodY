// CS 247 - Scientific Visualization, KAUST
//
// Programming Assignment #5
#include <cstring>
#include "CS247_prog.h"

// cycle clear colors
static void nextClearColor()
{
    clearColor = (++clearColor) % 3;

    switch(clearColor)
    {
        case 0:
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            break;
        case 1:
            glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
            break;
        default:
            glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
            break;
    }
}


// callbacks
// framebuffer to fix viewport
void frameBufferCallback(GLFWwindow* window, int width, int height)
{
    view_width = width;
    view_height = height;
    glViewport(0, 0, width, height);
}

// key callback to take user inputs for both windows
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_RELEASE) {
        char* status[ 2 ];
        status[ 0 ] = "disabled";
        status[ 1 ] = "enabled";

        switch (key) {
            case '1':
                toggle_xy = 0;
                LoadData( filenames[ 0 ] );
                loaded_file = 0;
                fprintf( stderr, "Loading " );
                fprintf( stderr, filenames[ 0 ] );
                fprintf( stderr, " dataset.\n");
                break;
            case '2':
                toggle_xy = 0;
                LoadData(filenames[ 1 ] );
                loaded_file = 1;
                fprintf( stderr, "Loading " );
                fprintf( stderr, filenames[ 1 ] );
                fprintf( stderr, " dataset.\n");
                break;
            case '3':
                toggle_xy = 1;
                LoadData( filenames[ 2 ] );
                loaded_file = 2;
                fprintf( stderr, "Loading " );
                fprintf( stderr, filenames[ 2 ] );
                fprintf( stderr, " dataset.\n");
                break;
            case '0':
                if( num_timesteps > 1 ){
                    loadNextTimestep();
                    fprintf( stderr, "Timestep %d.\n", loaded_timestep );
                }
                break;
            case GLFW_KEY_A:
                en_arrow = !en_arrow;
                fprintf(stderr, "%s drawing arrows.\n", en_arrow? "enabling" : "disabling");
                break;
            case GLFW_KEY_S:
                current_scalar_field = (current_scalar_field + 1)%num_scalar_fields;
                DownloadScalarFieldAsTexture();
                fprintf( stderr, "Scalar field changed.\n");
                break;
            case GLFW_KEY_B:
                nextClearColor();
                fprintf( stderr, "Next clear color.\n");
                break;
            case GLFW_KEY_EQUAL:
                sampling_rate = std::min(sampling_rate + 5, 100);
                fprintf(stderr, "Increasing sampling rate to %d.\n", sampling_rate);
                break;
            case GLFW_KEY_MINUS:
                sampling_rate = std::max(sampling_rate - 5, 5);
                fprintf(stderr, "Decreasing sampling rate to: %d.\n", sampling_rate);
                break;
            case GLFW_KEY_I:
                dt = std::min(dt + 0.005, 1.);
                fprintf(stderr, "Increase dt: %.2f\n", dt);
                break;
            case GLFW_KEY_K:
                dt = std::max(dt - 0.005, 0.0001);
                fprintf(stderr, "Decrease dt: %.2f\n", dt);
                break;
            case GLFW_KEY_T:
                en_streamline = !en_streamline;
                fprintf(stderr, "%s drawing streamlines.\n", en_streamline? "enabling" : "disabling");
                break;
            case GLFW_KEY_P:
                en_pathline = !en_pathline;
                fprintf(stderr, "%s drawing pathlines.\n", en_pathline? "enabling" : "disabling");
                break;
            // TODO: add keyboard controls for:
            //   - toggle colormap mode (cycle off/rainbow/cool-warm)
            //   - adjust blend factor (increase/decrease between 0.0 and 1.0)
            //   - toggle integration method (Euler/RK2)
            //   - clear all seeds
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                exit( 0 );
                break;
            default:
                fprintf( stderr, "\nKeyboard commands:\n\n"
                                 "1, load %s dataset\n"
                                 "2, load %s dataset\n"
                                 "3, load %s dataset\n"
                                 "0, cycle through timesteps\n"
                                 "b, switch backgropund color\n"
                                 "a, en-/disable arrows.\n"
                                 "t, en-/disable streamlines.\n"
                                 "p, en-/disable pathlines.\n"
                                 "+, increase sampling rate.\n"
                                 "-, decrease sampling rate.\n"
                                 "i, increase dt.\n"
                                 "k, decrease dt.\n"
                                 "q, <esc> - Quit\n",
                         filenames[0], filenames[1], filenames[2] );
                break;
        }
    }
}

// mouse callback to seed streamlines/pathlines
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        //getting cursor position
        glfwGetCursorPos(window, &xpos, &ypos);
        // TODO: seed streamlines & pathlines using mouse clicks
        // Hint: convert screen coords to grid coords (y-flip needed),
        //       then call computeStreamline/computePathline when enabled
    }
}

// glfw error callback
static void errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// data

void loadNextTimestep( void )
{
    loaded_timestep = ( loaded_timestep + 1 ) % num_timesteps;
    DownloadScalarFieldAsTexture();
}


/*
 * load .gri dataset
 * This only reads the header information and calls the dat loader
 * For now we ignore the grid data and assume a rectangular grid
 */
void LoadData( char* base_filename )
{
    //reset
    reset_rendering_props();

    char filename[ 80 ];
    strcpy( filename, base_filename );
    strcat( filename, ".gri");

    fprintf( stderr, "loading grid file %s\n", filename );

    // open grid file, read only, binary mode
    FILE* fp = fopen( filename, "rb" );
    if ( fp == NULL ) {
        fprintf( stderr, "Cannot open file %s for reading.\n", filename );
        return;
    }

    // read header
    char header[ 40 ];
    fread( header, sizeof( char ), 40, fp );
    sscanf( header, "SN4DB %d %d %d %d %d %f",
            &vol_dim[ 0 ], &vol_dim[ 1 ], &vol_dim[ 2 ],
            &num_scalar_fields, &num_timesteps ,&timestep );

    fprintf( stderr, "dimensions: x: %d, y: %d, z: %d.\n", vol_dim[ 0 ], vol_dim[ 1 ], vol_dim[ 2 ] );
    fprintf( stderr, "additional info: # scalar fields: %d, # timesteps: %d, timestep: %f.\n", num_scalar_fields, num_timesteps, timestep );

    // read data
    char dat_filename[ 80 ];
    strcpy( dat_filename, base_filename );

    if( num_timesteps <= 1 ){

        strcat( dat_filename, ".dat");

    } else {

        strcat( dat_filename, ".00000.dat");

    }

    loaded_timestep = 0;
    LoadVectorData( base_filename );

    glfwSetWindowSize(window, vol_dim[ 0 ], vol_dim[ 1 ] );
    grid_data_loaded = true;
}

/*
 * load .dat dataset
 * loads vector and scalar fields
 */
void LoadVectorData( const char* filename )
{
    fprintf( stderr, "loading scalar file %s\n", filename );

    // open data file, read only, binary mode
    char ts_name[ 80 ];
    if( num_timesteps > 1 )
    {
        sprintf( ts_name, "%s.%.5d.dat", filename, 0 );
    }
    else
        sprintf( ts_name, "%s.dat",filename);

    FILE* fp = fopen( ts_name, "rb" );
    if ( fp == NULL ) {
        fprintf( stderr, "Cannot open file %s for reading.\n", filename );
        return;
    }
    else
    {
        fclose( fp );
    }

    data_size = vol_dim[ 0 ] * vol_dim[ 1 ] * vol_dim[ 2 ];

    if (!vector_array) {
        delete[] vector_array;
        vector_array = NULL;
    }
    // dim.xyz * vector.xyz * timesteps
    vector_array = new float[ data_size * 3 * num_timesteps];

    // read data
    if (!scalar_fields) {
        delete[] scalar_fields;
        scalar_fields = NULL;
        delete[] scalar_bounds;
        scalar_bounds = NULL;
    }
    // dim.xyz * scalarfields(2) * timesteps
    scalar_fields = new float[ data_size * num_scalar_fields * num_timesteps ];
    scalar_bounds = new float[ 2 * num_scalar_fields * num_timesteps ];

    int num_total_fields = num_scalar_fields + 3; // scalar fields + vec.xyz
    float *tmp = new float[ data_size * num_total_fields * num_timesteps ];

    for( int k = 0 ; k < num_timesteps; k++ )
    {
        char times_name[ 80 ];
        if( num_timesteps > 1 )
        {
            sprintf( times_name, "%s.%.5d.dat", filename, k );
            fp = fopen( times_name, "rb" );
        }
        else
            fp = fopen( ts_name, "rb" );
        // read scalar data
        fread( &tmp[k*data_size*num_total_fields], sizeof( float ), ( data_size * num_total_fields ), fp );

        // close file
        fclose( fp );

        // copy and scan for min and max values
        for( int  i = 0; i < num_scalar_fields; i++ ){

            float min_val = 99999.9f;
            float max_val = 0.0f;

            float avg = 0.0;

            int offset = i * data_size * num_timesteps;

            for( int j = 0; j < data_size; j++ ){

                float val = tmp[ j * num_total_fields + 3 + i + k*data_size*num_total_fields ];

                scalar_fields[ j + k*data_size + offset ] = val;

                if( toggle_xy ) {
                    // overwrite
                    if( i == 0 ){
                        vector_array[ 3*j + 0 + 3*k*data_size ] = tmp[ j * num_total_fields + 1 + k*data_size*num_total_fields ];//toggle x and y components in vector field
                        vector_array[ 3*j + 1 + 3*k*data_size ] = tmp[ j * num_total_fields + 0 + k*data_size*num_total_fields ];
                        vector_array[ 3*j + 2 + 3*k*data_size ] = tmp[ j * num_total_fields + 2 + k*data_size*num_total_fields ];
                    }
                } else {
                    // overwrite
                    if( i == 0 ){
                        vector_array[ 3*j + 0 + 3*k*data_size ] = tmp[ j * num_total_fields + 0 + k*data_size*num_total_fields ];
                        vector_array[ 3*j + 1 + 3*k*data_size ] = tmp[ j * num_total_fields + 1 + k*data_size*num_total_fields ];
                        vector_array[ 3*j + 2 + 3*k*data_size ] = tmp[ j * num_total_fields + 2 + k*data_size*num_total_fields ];
                    }
                }

                min_val = std::min( val, min_val );
                max_val = std::max( val, max_val );

                avg += scalar_fields[ offset + j + k*data_size ] / data_size;
            }
            scalar_bounds[ 2 * i     + k*num_scalar_fields*2 ] = min_val;
            scalar_bounds[ 2 * i + 1 + k*num_scalar_fields*2 ] = max_val;
        }

        // normalize
        for( int  i = 0; i < num_scalar_fields; i++ ){

            int offset = i * data_size * num_timesteps;

            float lower_bound = scalar_bounds[ 2 * i     + k*num_scalar_fields*2 ];
            float upper_bound = scalar_bounds[ 2 * i + 1 + k*num_scalar_fields*2 ];

            // scale between [0..1] where 1 is original zero
            // the boundary of the bigger abs border will be used to scale
            // meaning one boundary will likely not be hit i.e real scale
            // for -50..100 will be [0.25..1.0]
            if( lower_bound < 0.0 && upper_bound > 0.0 ){

                float scale = 0.5f / std::max( -lower_bound, upper_bound );

                for( int j = 0; j < data_size; j++ ){

                    scalar_fields[ offset + j + k*data_size ] = 0.5f + scalar_fields[ offset + j + k*data_size ] * scale;
                }
                scalar_bounds[ 2 * i     + k*num_scalar_fields*2 ] = 0.5f + scalar_bounds[ 2 * i     + k*num_scalar_fields*2 ] * scale;
                scalar_bounds[ 2 * i + 1 + k*num_scalar_fields*2 ] = 0.5f + scalar_bounds[ 2 * i + 1 + k*num_scalar_fields*2 ] * scale;


                // scale between [0..1]
            } else {

                float sign = upper_bound <= 0.0 ? -1.0f : 1.0f;

                float scale = 1.0f / ( upper_bound - lower_bound ) * sign;

                for( int j = 0; j < data_size; j++ ){

                    scalar_fields[ offset + j + k*data_size ] = ( scalar_fields[ offset + j + k*data_size ] - lower_bound ) * scale;
                }
                scalar_bounds[ 2 * i     + k*num_scalar_fields*2 ] = ( scalar_bounds[ 2 * i     + k*num_scalar_fields*2 ] + lower_bound ) * scale; //should be 0.0
                scalar_bounds[ 2 * i + 1 + k*num_scalar_fields*2 ] = ( scalar_bounds[ 2 * i + 1 + k*num_scalar_fields*2 ] + lower_bound ) * scale; //should be 1.0
            }
        }
    }
    delete[] tmp;
    DownloadScalarFieldAsTexture();

    scalar_data_loaded = true;
}


void DownloadScalarFieldAsTexture( void )
{
    fprintf( stderr, "downloading scalar field to 2D texture\n" );

    glEnable( GL_TEXTURE_2D );

    // generate and bind 2D texture
    glGenTextures( 1, &scalar_field_texture );
    glBindTexture( GL_TEXTURE_2D, scalar_field_texture );

    // set necessary texture parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    int datasize = vol_dim[0] * vol_dim[1];
    //download texture in correct format
    glTexImage2D( GL_TEXTURE_2D, 0,  GL_INTENSITY16, vol_dim[ 0 ], vol_dim[ 1 ], 0, GL_LUMINANCE, GL_FLOAT, &scalar_fields[ (loaded_timestep + current_scalar_field * num_timesteps)*datasize ] );


    glDisable( GL_TEXTURE_2D );
}

char *textFileRead( char *fn ){

    FILE *fp;
    char *content = NULL;

    int count=0;

    if (!fn) {
        fp = fopen(fn,"rt");

        if (!fp) {

            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);

            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = fread(content,sizeof(char),count,fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}


// initializations
// init application
bool initApplication(int argc, char **argv)
{

    std::string version((const char *)glGetString(GL_VERSION));
    std::stringstream stream(version);
    unsigned major, minor;
    char dot;

    stream >> major >> dot >> minor;

    assert(dot == '.');
    if (major > 3 || (major == 2 && minor >= 0)) {
        std::cout << "OpenGL Version " << major << "." << minor << std::endl;
    } else {
        std::cout << "The minimum required OpenGL version is not supported on this machine. Supported is only " << major << "." << minor << std::endl;
        return false;
    }

    return true;
}

void reset_rendering_props( void )
{
    num_scalar_fields = 0;
}

// set up the scene
void setup() {
    LoadData( filenames[ 0 ] );
    loaded_file = 0;

    DownloadScalarFieldAsTexture();


    // compile & link shader
    vectorProgram.compileShader("../shaders/vertex.vs");
    vectorProgram.compileShader("../shaders/fragment.fs");
    vectorProgram.link();

    // make quad to render texture
    // see: vboquad.h and vboquad.cpp
    quad.init();

    // TODO: glyph/streamlines/pathlines VAO and VBO
}

// rendering
void render() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_TEXTURE_2D );

    // draw the texture
    glBindTexture(GL_TEXTURE_2D, scalar_field_texture);
    vectorProgram.use();

    model = mat4(1);

    vectorProgram.setUniform("vertexColor", glm::vec4(0));
    vectorProgram.setUniform("model", model);

    // TODO: pass colormap uniforms to shader before drawing the quad
    // Hint: set colormapMode and blendFactor uniforms here

    quad.render();
    glDisable( GL_TEXTURE_2D );

    // TODO: reset colormap mode to 0 before drawing overlays
    // so that glyphs/streamlines/pathlines use solid colors

    // TODO: draw glyphs, streamlines, pathlines

}

// entry point
int main(int argc, char** argv)
{
    // init variables
    view_width = 0;
    view_height = 0;

    toggle_xy = 0;

    en_arrow = false;
    en_streamline = false;
    en_pathline = false;
    sampling_rate = 15;
    dt = 0.1;

    reset_rendering_props();

    vector_array = NULL;
    scalar_fields = NULL;
    scalar_bounds = NULL;
    grid_data_loaded = false;
    scalar_data_loaded = false;
    current_scalar_field = 0;
    clearColor = 0;


    filenames[ 0 ] = "../data/block/c_block";
    filenames[ 1 ] = "../data/tube/tube";
    filenames[ 2 ] = "../data/hurricane/hurricane_p_tc";



    // set glfw error callback
    glfwSetErrorCallback(errorCallback);

    // init glfw
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    // request OpenGL 4.1 Core Profile (highest macOS supports)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // init glfw window
    window = glfwCreateWindow(gWindowWidth, gWindowHeight, "AMCS/CS247 Scientific Visualization", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // set GLFW callback functions
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetFramebufferSizeCallback(window, frameBufferCallback);

    // make context current (once is sufficient)
    glfwMakeContextCurrent(window);

    // get the frame buffer size
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // init the OpenGL API (we need to do this once before any calls to the OpenGL API)
    gladLoadGL();

    // init our application
    if (!initApplication(argc, argv)) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }


    // set up the scene
    setup();

    // print menu
    keyCallback(window, GLFW_KEY_BACKSLASH, 0, GLFW_PRESS, 0);

    // start traversing the main loop
    // loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // clear frame buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render one frame
        render();

        // swap front and back buffers
        glfwSwapBuffers(window);

        // poll and process input events (keyboard, mouse, window, ...)
        glfwPollEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}

// TODO: define any useful functions you might need.
//  e.g., indexing, linear interpolation ..etc


void computeStreamline(int x, int y)
{
    // TODO: compute streamlines starting from x,y position. enable switching between euler and runge kutta
    // Hint: implement bilinear interpolation of vectors, forward+backward integration,
    //       and stopping conditions (boundary, zero vector, max accumulated length)

    // TODO: set any useful uniforms & update VBO & draw
}

void computePathline(int x, int y, int t)
{
    // TODO: compute pathlines starting from x,y position and time step t. enable switching between euler and runge kutta
    // Hint: implement trilinear interpolation (bilinear in space + linear in time),
    //       forward+backward integration advancing in both space and time

    // TODO: set any useful uniforms & update VBO & draw
}

void drawGlyphs() {
    // TODO: draw arrows/glyphs
    // Hint: iterate over grid with sampling_rate stride, compute arrow geometry
    //       (shaft + arrowhead) in NDC, upload to VBO, draw with GL_LINES
}
