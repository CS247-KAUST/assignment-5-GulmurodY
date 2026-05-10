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
            case GLFW_KEY_C:
                colormapMode = (colormapMode + 1) % 3;
                fprintf(stderr, "Colormap mode: %d (0=off, 1=rainbow, 2=cool-warm).\n", colormapMode);
                break;
            case GLFW_KEY_V:
                blendFactor = std::min(blendFactor + 0.1f, 1.0f);
                fprintf(stderr, "Blend factor: %.1f.\n", blendFactor);
                break;
            case GLFW_KEY_X:
                blendFactor = std::max(blendFactor - 0.1f, 0.0f);
                fprintf(stderr, "Blend factor: %.1f.\n", blendFactor);
                break;
            case GLFW_KEY_M:
                useRK2 = !useRK2;
                fprintf(stderr, "Integration method: %s.\n", useRK2 ? "RK2" : "Euler");
                break;
            case GLFW_KEY_D:
                for (int i = 0; i < (int)streamlineVAOs.size(); i++) {
                    glDeleteVertexArrays(1, &streamlineVAOs[i]);
                    glDeleteBuffers(1, &streamlineVBOs[i]);
                }
                streamlineVAOs.clear();
                streamlineVBOs.clear();
                streamlineVertexCounts.clear();
                for (int i = 0; i < (int)pathlineVAOs.size(); i++) {
                    glDeleteVertexArrays(1, &pathlineVAOs[i]);
                    glDeleteBuffers(1, &pathlineVBOs[i]);
                }
                pathlineVAOs.clear();
                pathlineVBOs.clear();
                pathlineVertexCounts.clear();
                fprintf(stderr, "Cleared all seeds.\n");
                break;
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
                                 "c, cycle colormap (off/rainbow/cool-warm).\n"
                                 "v/x, increase/decrease blend factor.\n"
                                 "m, toggle Euler/RK2 integration.\n"
                                 "d, clear all seeds.\n"
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
        int win_w, win_h;
        glfwGetWindowSize(window, &win_w, &win_h);

        // screen → grid: y is flipped (screen y=0 is top, grid y=0 is bottom)
        int gx = (int)(xpos / (double)win_w * vol_dim[0]);
        int gy = (int)((1.0 - ypos / (double)win_h) * vol_dim[1]);
        gx = std::max(0, std::min(gx, (int)vol_dim[0] - 1));
        gy = std::max(0, std::min(gy, (int)vol_dim[1] - 1));
        fprintf(stderr, "Seed at grid (%d, %d), timestep %d\n", gx, gy, loaded_timestep);

        if (en_streamline) computeStreamline(gx, gy);
        if (en_pathline)   computePathline(gx, gy, loaded_timestep);
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
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    int datasize = vol_dim[0] * vol_dim[1];
    glTexImage2D( GL_TEXTURE_2D, 0, GL_R32F, vol_dim[ 0 ], vol_dim[ 1 ], 0, GL_RED, GL_FLOAT, &scalar_fields[ (loaded_timestep + current_scalar_field * num_timesteps)*datasize ] );


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
    glGenVertexArrays(1, &glyphVAO);
    glGenBuffers(1, &glyphVBO);
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
    vectorProgram.setUniform("colormapMode", colormapMode);
    vectorProgram.setUniform("blendFactor",  blendFactor);

    quad.render();
    glDisable( GL_TEXTURE_2D );

    // TODO: reset colormap mode to 0 before drawing overlays
    // so that glyphs/streamlines/pathlines use solid colors
    vectorProgram.setUniform("colormapMode", 0);
    vectorProgram.setUniform("blendFactor",  1.0f);

    // TODO: draw glyphs, streamlines, pathlines
    if (en_arrow)
        drawGlyphs();

    if (en_streamline && !streamlineVAOs.empty()) {
        vectorProgram.setUniform("vertexColor", glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
        vectorProgram.setUniform("model", glm::mat4(1.0f));
        for (int i = 0; i < (int)streamlineVAOs.size(); i++) {
            glBindVertexArray(streamlineVAOs[i]);
            glDrawArrays(GL_LINE_STRIP, 0, streamlineVertexCounts[i]);
        }
        glBindVertexArray(0);
    }

    if (en_pathline && !pathlineVAOs.empty()) {
        vectorProgram.setUniform("vertexColor", glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
        vectorProgram.setUniform("model", glm::mat4(1.0f));
        for (int i = 0; i < (int)pathlineVAOs.size(); i++) {
            glBindVertexArray(pathlineVAOs[i]);
            glDrawArrays(GL_LINE_STRIP, 0, pathlineVertexCounts[i]);
        }
        glBindVertexArray(0);
    }

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
    useRK2 = false;
    colormapMode = 0;
    blendFactor = 1.0f;

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

glm::vec2 getVector(int x, int y, int t) {
    int j = y * vol_dim[0] + x;
    float vx = vector_array[3 * j + 0 + 3 * t * data_size];
    float vy = vector_array[3 * j + 1 + 3 * t * data_size];
    return glm::vec2(vx, vy);
}


glm::vec2 bilinearInterp(float fx, float fy, int t) {
    // clamp to valid grid range
    fx = std::max(0.0f, std::min(fx, (float)(vol_dim[0] - 1)));
    fy = std::max(0.0f, std::min(fy, (float)(vol_dim[1] - 1)));

    int x0 = (int)fx,  x1 = std::min(x0 + 1, (int)vol_dim[0] - 1);
    int y0 = (int)fy,  y1 = std::min(y0 + 1, (int)vol_dim[1] - 1);
    float sx = fx - x0,  sy = fy - y0;   // fractional parts [0,1]

    glm::vec2 v00 = getVector(x0, y0, t);
    glm::vec2 v10 = getVector(x1, y0, t);
    glm::vec2 v01 = getVector(x0, y1, t);
    glm::vec2 v11 = getVector(x1, y1, t);

    glm::vec2 bot = glm::mix(v00, v10, sx);   
    glm::vec2 top = glm::mix(v01, v11, sx);  
    return glm::mix(bot, top, sy);            
}

glm::vec2 trilinearInterp(float fx, float fy, float ft) {
    ft = std::max(0.0f, std::min(ft, (float)(num_timesteps - 1)));

    int t0 = (int)ft,  t1 = std::min(t0 + 1, num_timesteps - 1);
    float st = ft - t0;   // fractional time [0,1]

    glm::vec2 v_t0 = bilinearInterp(fx, fy, t0);
    glm::vec2 v_t1 = bilinearInterp(fx, fy, t1);
    return glm::mix(v_t0, v_t1, st);        
}


void computeStreamline(int x, int y)
{
    // TODO: compute streamlines starting from x,y position. enable switching between euler and runge kutta
    // Hint: implement bilinear interpolation of vectors, forward+backward integration,
    //       and stopping conditions (boundary, zero vector, max accumulated length)
    if (!scalar_data_loaded) return;

    int   t      = loaded_timestep;
    float maxLen = sqrtf((float)(vol_dim[0]*vol_dim[0] + vol_dim[1]*vol_dim[1]));

    // integrate in one direction (sign = +1 forward, -1 backward)
    auto integrate = [&](float sx, float sy, float sign) -> std::vector<glm::vec2> {
        std::vector<glm::vec2> pts;
        pts.push_back({sx, sy});
        float cx = sx, cy = sy, accLen = 0.0f;

        while (accLen < maxLen) {
            glm::vec2 v;
            if (useRK2) {
                glm::vec2 k1 = bilinearInterp(cx, cy, t);
                if (glm::length(k1) < 1e-6f) break;
                k1 = glm::normalize(k1);
                float mx = cx + sign * dt * k1.x;
                float my = cy + sign * dt * k1.y;
                if (mx < 0 || mx >= vol_dim[0] || my < 0 || my >= vol_dim[1]) break;
                glm::vec2 k2 = bilinearInterp(mx, my, t);
                if (glm::length(k2) < 1e-6f) break;
                v = glm::normalize((k1 + glm::normalize(k2)) * 0.5f);
            } else {
                glm::vec2 k1 = bilinearInterp(cx, cy, t);
                if (glm::length(k1) < 1e-6f) break;
                v = glm::normalize(k1);
            }

            float nx = cx + sign * dt * v.x;
            float ny = cy + sign * dt * v.y;
            if (nx < 0 || nx >= vol_dim[0] || ny < 0 || ny >= vol_dim[1]) break;

            accLen += dt;
            cx = nx;  cy = ny;
            pts.push_back({cx, cy});
        }
        return pts;
    };

    std::vector<glm::vec2> fwd = integrate((float)x, (float)y, +1.0f);
    std::vector<glm::vec2> bwd = integrate((float)x, (float)y, -1.0f);

    // build vertex buffer: reversed backward (tail) + forward (head)
    // giving one continuous LINE_STRIP through the seed point
    std::vector<float> verts;
    auto pushPt = [&](float gx, float gy) {
        float nx = gx / (vol_dim[0] - 1) * 2.0f - 1.0f;
        float ny = gy / (vol_dim[1] - 1) * 2.0f - 1.0f;
        verts.insert(verts.end(), { nx, ny, 0, 0, 0, 0 });
    };

    for (int i = (int)bwd.size() - 1; i >= 1; i--)
        pushPt(bwd[i].x, bwd[i].y);
    for (auto& p : fwd)
        pushPt(p.x, p.y);

    if ((int)verts.size() < 12) return;  // need at least 2 vertices

    // TODO: set any useful uniforms & update VBO & draw
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    streamlineVAOs.push_back(vao);
    streamlineVBOs.push_back(vbo);
    streamlineVertexCounts.push_back((int)verts.size() / 6);

    fprintf(stderr, "  streamline: %d verts (fwd=%d, bwd=%d), method=%s, dt=%.3f\n",
            (int)verts.size() / 6, (int)fwd.size(), (int)bwd.size(),
            useRK2 ? "RK2" : "Euler", dt);
}

void computePathline(int x, int y, int t)
{
    // TODO: compute pathlines starting from x,y position and time step t. enable switching between euler and runge kutta
    // Hint: implement trilinear interpolation (bilinear in space + linear in time),
    //       forward+backward integration advancing in both space and time
    if (!scalar_data_loaded) return;

    float maxTime = (float)(num_timesteps - 1);
    int   maxSteps = (num_timesteps > 1) ? (int)(maxTime / dt) + 1 : 500;

    auto integrate = [&](float sx, float sy, float st, float sign) -> std::vector<glm::vec2> {
        std::vector<glm::vec2> pts;
        pts.push_back({sx, sy});
        float cx = sx, cy = sy, ct = st;

        for (int step = 0; step < maxSteps; step++) {
            glm::vec2 v;
            if (useRK2) {
                glm::vec2 k1 = trilinearInterp(cx, cy, ct);
                if (glm::length(k1) < 1e-6f) break;
                float mx = cx + sign * dt * k1.x;
                float my = cy + sign * dt * k1.y;
                float mt = ct + sign * dt;
                glm::vec2 k2 = trilinearInterp(mx, my, mt);
                if (glm::length(k2) < 1e-6f) break;
                v = (k1 + k2) * 0.5f;
            } else {
                v = trilinearInterp(cx, cy, ct);
                if (glm::length(v) < 1e-6f) break;
            }

            float nx = cx + sign * dt * v.x;
            float ny = cy + sign * dt * v.y;
            float nt = ct + sign * dt;
            if (nx < 0 || nx >= vol_dim[0] || ny < 0 || ny >= vol_dim[1]) break;
            if (nt < 0 || nt > maxTime) break;

            cx = nx;  cy = ny;  ct = nt;
            pts.push_back({cx, cy});
        }
        return pts;
    };

    std::vector<glm::vec2> fwd = integrate((float)x, (float)y, (float)t, +1.0f);
    std::vector<glm::vec2> bwd = integrate((float)x, (float)y, (float)t, -1.0f);

    std::vector<float> verts;
    auto pushPt = [&](float gx, float gy) {
        float nx = gx / (vol_dim[0] - 1) * 2.0f - 1.0f;
        float ny = gy / (vol_dim[1] - 1) * 2.0f - 1.0f;
        verts.insert(verts.end(), { nx, ny, 0, 0, 0, 0 });
    };

    for (int i = (int)bwd.size() - 1; i >= 1; i--)
        pushPt(bwd[i].x, bwd[i].y);
    for (auto& p : fwd)
        pushPt(p.x, p.y);

    if ((int)verts.size() < 12) return;

    // TODO: set any useful uniforms & update VBO & draw
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    pathlineVAOs.push_back(vao);
    pathlineVBOs.push_back(vbo);
    pathlineVertexCounts.push_back((int)verts.size() / 6);

    fprintf(stderr, "  pathline: %d verts (fwd=%d, bwd=%d), method=%s, dt=%.3f, num_timesteps=%d\n",
            (int)verts.size() / 6, (int)fwd.size(), (int)bwd.size(),
            useRK2 ? "RK2" : "Euler", dt, num_timesteps);
}

void drawGlyphs() {
    // TODO: draw arrows/glyphs
    // Hint: iterate over grid with sampling_rate stride, compute arrow geometry
    //       (shaft + arrowhead) in NDC, upload to VBO, draw with GL_LINES
    if (!en_arrow || !scalar_data_loaded) return;

    std::vector<float> verts;

    float cellW   = (float)sampling_rate / (vol_dim[0] - 1) * 2.0f;
    float cellH   = (float)sampling_rate / (vol_dim[1] - 1) * 2.0f;
    float maxLen  = std::min(cellW, cellH) * 0.5f;
    float headAngle = 0.6f;

    float maxMag = 1e-6f;
    for (int y = 0; y < (int)vol_dim[1]; y += sampling_rate)
        for (int x = 0; x < (int)vol_dim[0]; x += sampling_rate)
            maxMag = std::max(maxMag, glm::length(getVector(x, y, loaded_timestep)));

    for (int y = 0; y < (int)vol_dim[1]; y += sampling_rate) {
        for (int x = 0; x < (int)vol_dim[0]; x += sampling_rate) {

            glm::vec2 v   = getVector(x, y, loaded_timestep);
            float     mag = glm::length(v);
            if (mag < 1e-6f) continue;

            glm::vec2 dir = glm::normalize(v);

            float scale    = mag / maxMag;
            float arrowLen = maxLen * scale;
            float headLen  = arrowLen * 0.45f;

            float cx = (float)x / (vol_dim[0] - 1) * 2.0f - 1.0f;
            float cy = (float)y / (vol_dim[1] - 1) * 2.0f - 1.0f;

            float tailX = cx - dir.x * arrowLen * 0.5f;
            float tailY = cy - dir.y * arrowLen * 0.5f;
            float tipX  = cx + dir.x * arrowLen * 0.5f;
            float tipY  = cy + dir.y * arrowLen * 0.5f;

            float cosA = cosf(headAngle), sinA = sinf(headAngle);
            float rx = -dir.x, ry = -dir.y;

            float lx =  rx*cosA - ry*sinA,  ly =  rx*sinA + ry*cosA;
            float wx =  rx*cosA + ry*sinA,  wy = -rx*sinA + ry*cosA;

            verts.insert(verts.end(), { tailX, tailY, 0, 0,0,0 });
            verts.insert(verts.end(), { tipX,  tipY,  0, 0,0,0 });
            verts.insert(verts.end(), { tipX, tipY, 0, 0,0,0 });
            verts.insert(verts.end(), { tipX + lx*headLen, tipY + ly*headLen, 0, 0,0,0 });
            verts.insert(verts.end(), { tipX, tipY, 0, 0,0,0 });
            verts.insert(verts.end(), { tipX + wx*headLen, tipY + wy*headLen, 0, 0,0,0 });
        }
    }

    if (verts.empty()) return;

    glBindVertexArray(glyphVAO);
    glBindBuffer(GL_ARRAY_BUFFER, glyphVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    vectorProgram.setUniform("vertexColor", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    vectorProgram.setUniform("model", glm::mat4(1.0f));

    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, (int)verts.size() / 6);
    glBindVertexArray(0);
}
