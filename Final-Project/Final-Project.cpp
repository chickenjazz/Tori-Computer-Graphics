#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>
#define TIMER_ID_FLIGHT 1 // Timer ID for the animation loop

using namespace std;

// PROTOTYPES
void displayBackground();
void displayToriGate();
void snowpile();
void displayGodzilla();
void displayFire();
void displayBoy();
void initVBOs();
void displayBirds();
void mouseCallback(int button, int state, int x, int y);
void animateGodzilla(int value);
void animateFlock(int value);
void birdMovement(int key, int x, int y);
void displayMountains();
void animateBoyRun(int value);
void drawBoyText();
void setSnowColor(float r, float g, float b, float a);
void drawSnowBalls();
void animateClouds(int value);

const int NUM_BIRDS = 15;
const GLfloat BIRD_COLOR[] = { 0.0f, 0.0f, 0.0f };
const GLfloat BIRD_SCALE = 0.020f;
GLuint boyVBO[2];
GLfloat birdOffsets[NUM_BIRDS * 2];
GLfloat birdFlock_X = 50.0f; //Current X-position
const GLfloat BIRD_FLOCK_SPEED = 0.005f; //Movement speed per frame
int birdFlockDirection = 0; //Flag to control movement: -1 (Left), 1 (Right), 0 (Idle)

float transition = 0.0f;
bool isTransitioning = false;
float targetMode = 0;
bool isFiring = false;
bool isSnowing = false;

float snowAlpha = 0.0f;         // Current opacity (0.0 to TARGET)
const float SNOW_MAX_ALPHA = 1.0f; // Max opacity for snow
const float SNOW_FADE_SPEED = 0.01f; // How fast it fades

// --- ANIMATION VARIABLES ---
float godzillaX = 1.0f;
float godzillaY = 0.0f;
float groundShake = 0.0f;

//boy animations
bool isBoyRunning = false;
float boyScale = 1.0f; // Scale factor for the boy
float boyPosX = 0.0f;// X-translation for boy's position
float boyPosY = -0.40f;// Y-translation for boy's position
float legRotateAngle = 0.0f;// For cartoon run animation

// Cloud Animation Variables (Different speeds for parallax effect)
float cloudOffset1 = 0.0f; // Horizon (Slowest)
float cloudOffset2 = 0.0f; // Mid (Medium)
float cloudOffset3 = 0.0f; // High (Fastest)

inline float mixf(float a, float b, float t);//for intepolation (transition)

const int totalSnowBalls = 200; // number of snowballs
float snowXs[200];
float snowYs[200];


// ----------------------------------------------------------------
// HELPER: DRAW A CLOUD (Procedural)
// ----------------------------------------------------------------
void drawCloud(float cx, float cy, float size, float offset) {
    // 1. Apply Animation Offset
    float finalX = cx + offset;

    // 2. Infinite Wrapping Logic
    // If cloud goes too far right (> 1.3), move it to far left (-1.3)
    // The screen width is 2.0 (-1 to 1), so 2.6 covers the whole width plus buffer
    while (finalX > 1.3f) {
        finalX -= 2.6f;
    }
    while (finalX < -1.3f) {
        finalX += 2.6f;
    }

    // Enable blending for soft edges
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float dayR = 0.7f, dayG = 0.8f, dayB = 1.0f;
    float dawnR = 0.65f, dawnG = 0.55f, dawnB = 0.80f;

    float R = mixf(dayR, dawnR, transition);
    float G = mixf(dayG, dawnG, transition);
    float B = mixf(dayB, dawnB, transition);

    glColor4f(R, G, B, 0.15f);

    // Draw 6 overlapping circles (Top, Sides, and a Bottom base)
    float offsets[6][3] = {
        { 0.0f,  0.5f,  1.0f},   // 1. Main Top Center
        { 0.0f,  -0.1f,  0.6f},   // 1. Main Top Center
        {-0.75f, 0.0f,  0.75f},  // 2. Left Middle
        { 0.75f, 0.0f,  0.75f},  // 3. Right Middle
        {-1.5f, -0.4f, 0.3f},  // 4. Bottom Left (Creates the oblong base)
        {1.5f, -0.4f, 0.3f},  // 5. Bottom RIght (Creates the oblong base)
    };

    for (int j = 0; j < 6; j++) {
        glBegin(GL_TRIANGLE_FAN);
        float xPos = finalX + (offsets[j][0] * size);
        float yPos = cy + (offsets[j][1] * size);
        float radius = size * offsets[j][2];

        glVertex2f(xPos, yPos); // Center of circle
        for (int i = 0; i <= 20; i++) {
            float angle = 2.0f * 3.14159f * i / 20;
            float dx = radius * cosf(angle);
            float dy = radius * sinf(angle);
            glVertex2f(xPos + dx, yPos + dy);
        }
        glEnd();
    }
    glDisable(GL_BLEND);
}

// ----------------------------------------------------------------
// KEyBOARD MONITOR
// ----------------------------------------------------------------

void keyboardMonitor(unsigned char key, int x, int y) {
    if (key == '1') {
        targetMode = 0.0f; // Day
        isTransitioning = true;
    }
    if (key == '2') {
        targetMode = 1.0f; // Dawn
        isTransitioning = true;
    }

    if (key == '3') {
        isSnowing = !isSnowing;  // flip ON/OFF
    }
    if (key == '4') {
        // If the boy's scale is 0.0f, he has already finished his run 
        if (boyScale == 0.0f) {
            return;

        }
        // This block executes if the boy is currently visible/active (boyScale > 0.0f).
        else {

            // If the boy is currently running, reset the leg angle for a smooth stop.
            if (isBoyRunning) {
                legRotateAngle = 0.0f;
            }
            // Toggle the running state
            isBoyRunning = !isBoyRunning;
        }
        glutPostRedisplay();
    }
}


void animateClouds(int value) {
    cloudOffset1 += 0.00005f; // lower/farther = slower mv
    cloudOffset2 += 0.00008f; // medium mv
    cloudOffset3 += 0.0002f;  // fast mv

    glutPostRedisplay();
    glutTimerFunc(16, animateClouds, 0);
}
// ----------------------------------------------------------------
// TIMER FUNCTION (ANIMATION)
// ----------------------------------------------------------------
void animateGodzilla(int value) {
    

    // --- GODZILLA STARTS MOVING ONLY AFTER 10 SECONDS ---
    if (value == 1) {
        if (godzillaX > -2.5f) {
            godzillaX -= 0.002f;
            godzillaY = 0.007f * abs(sin(godzillaX * 20.0f));
            groundShake = 0.002f * abs(sin(godzillaX * 20.0f));
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, animateGodzilla, value);
}

// ----------------------------------------------------------------------
// CHANGE FROM DAY TO DAWN (& VICE VERSA)
// ----------------------------------------------------------------------

void animateBackground(int value) {
    if (isTransitioning) {
        float speed = 0.0045f; // adjust for slower/faster transition

        if (transition < targetMode)
            transition += speed;
        else if (transition > targetMode)
            transition -= speed;

        if (fabs(transition - targetMode) < 0.004f) {
            transition = targetMode;
            isTransitioning = false;
        }
        glutPostRedisplay();
    }

    glutTimerFunc(16, animateBackground, 0);
}

// -----------------------------------------------------------------------
// ANIMATE SNOWING AND ITS FADING WHEN TOGGLED OFF
// -----------------------------------------------------------------------

void animateSnowfall(int value) {
    // 1. HANDLE FADING
    if (isSnowing) {
        // Fade In
        if (snowAlpha < SNOW_MAX_ALPHA) {
            snowAlpha += SNOW_FADE_SPEED;
            if (snowAlpha > SNOW_MAX_ALPHA) snowAlpha = SNOW_MAX_ALPHA;
        }
    }
    else {
        // Fade Out
        if (snowAlpha > 0.0f) {
            snowAlpha -= SNOW_FADE_SPEED;
            if (snowAlpha < 0.0f) snowAlpha = 0.0f;
        }
    }

    // 2. UPDATE POSITIONS (Keep falling if visible at all)
    if (snowAlpha > 0.0f) {
        for (int i = 0; i < totalSnowBalls; i++) {
            snowYs[i] -= 0.02f;

            if (snowYs[i] < -1.0f) { //if snow exceeds bottom bring back to the top
                snowYs[i] = 1.0f;
                snowXs[i] = ((float)(rand() % 100) / 50.0f) - 1.0f;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, animateSnowfall, 0);
}
void drawSnowBalls() {
    if (snowAlpha <= 0.0f) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(5.0f);
    glBegin(GL_POINTS);

    for (int i = 0; i < totalSnowBalls; i++) {
        // Use snowAlpha for the alpha channel
        setSnowColor(1.0f, 1.0f, 1.0f, snowAlpha);
        glVertex2f(snowXs[i], snowYs[i]);
    }

    glEnd();
    glDisable(GL_BLEND);
}
 //---------------------------------------------------------------
// INITIALIZE SNOWBALLS
 //---------------------------------------------------------------
void initSnowBalls() {
    for (int i = 0; i < totalSnowBalls; i++) {
        snowXs[i] = ((float)(rand() % 100) / 50.0f) - 1.0f;   // random X (-1 to 1)
        snowYs[i] = ((float)(rand() % 100) / 50.0f);          // random Y (0 to 2)
    }
}


// ----------------------------------------------------------------
// MOUSE CALLBACK
// ----------------------------------------------------------------
void mouseCallback(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) isFiring = true;
        else if (state == GLUT_UP) isFiring = false;
        glutPostRedisplay();
    }
}

//------------------------------------------------------------------
// SNOW FIELD COLOR HELPER
//------------------------------------------------------------------

void applySnowFieldColor(float r, float g, float b) {
    float dawnFactor = transition;

    float dr = mixf(r, r * 0.45f, dawnFactor);
    float dg = mixf(g, g * 0.45f, dawnFactor);
    float db = mixf(b, b * 0.55f, dawnFactor);

    glColor3f(dr, dg, db);
}

inline float mixf(float a, float b, float t) {
    return a * (1.0f - t) + b * t;
}


// ----------------------------------------------------------------
// DISPLAY BACKGROUND (Sky, Clouds, Blended Sea, Snow)
// ----------------------------------------------------------------
void displayBackground() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Settings
    GLfloat snowFieldHeight = -0.58f;
    GLfloat seaLevel = -0.25f;

    // ------------------------------------
    // 1. SKY GRADIENT
    // ------------------------------------
    GLfloat skyVertices[] = {
        -1.0f,  1.0f, 0.0f,      // Top-Left 
         1.0f,  1.0f, 0.0f,      // Top-Right
         1.0f,  seaLevel, 0.0f,  // Bottom boundary
        -1.0f,  seaLevel, 0.0f   // Bottom boundary
    };

    GLfloat skyColors[12];

    // DAY TOP → DAWN TOP
    float dayTopR = 0.1f, dayTopG = 0.2f, dayTopB = 0.5f;
    float dawnTopR = 0.45f, dawnTopG = 0.05f, dawnTopB = 0.55f;

    // DAY BOTTOM → DAWN BOTTOM
    float dayBotR = 0.6f, dayBotG = 0.7f, dayBotB = 0.95f;
    float dawnBotR = 1.0f, dawnBotG = 0.45f, dawnBotB = 0.1f;

    skyColors[0] = skyColors[3] = mixf(dayTopR, dawnTopR, transition);
    skyColors[1] = skyColors[4] = mixf(dayTopG, dawnTopG, transition);
    skyColors[2] = skyColors[5] = mixf(dayTopB, dawnTopB, transition);

    skyColors[6] = skyColors[9] = mixf(dayBotR, dawnBotR, transition);
    skyColors[7] = skyColors[10] = mixf(dayBotG, dawnBotG, transition);
    skyColors[8] = skyColors[11] = mixf(dayBotB, dawnBotB, transition);

    glVertexPointer(3, GL_FLOAT, 0, skyVertices);
    glColorPointer(3, GL_FLOAT, 0, skyColors);
    glDrawArrays(GL_QUADS, 0, 4);

    // ------------------------------------
    // 2. CLOUDS (Procedural)
    // ------------------------------------
    // We draw these before the sea so they appear behind the horizon if low
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // LAYER 1: HORIZON (Smallest, Furthest Back)
       // Y-positions: -0.2 to 0.1 | Size: 0.04 to 0.07
    drawCloud(-0.95f, -0.15f, 0.04f, cloudOffset1);
    drawCloud(0.85f, -0.12f, 0.05f, cloudOffset1);
    drawCloud(-0.60f, -0.05f, 0.06f, cloudOffset1);
    drawCloud(0.40f, 0.00f, 0.05f, cloudOffset1);
    drawCloud(0.10f, -0.08f, 0.06f, cloudOffset1);
    drawCloud(-0.25f, 0.05f, 0.07f, cloudOffset1);
    drawCloud(0.65f, 0.08f, 0.06f, cloudOffset1);
    drawCloud(0.30f, -0.15f, 0.05f, cloudOffset1); // Low Center-Right
    drawCloud(-0.45f, 0.02f, 0.06f, cloudOffset1); // Mid Horizon Left
    drawCloud(-0.80f, -0.10f, 0.05f, cloudOffset1); // Low Far Left
    drawCloud(-0.98f, -0.05f, 0.04f, cloudOffset1); // Far edge left
    drawCloud(0.95f, 0.02f, 0.05f, cloudOffset1); // Far edge right
    drawCloud(-0.15f, -0.18f, 0.06f, cloudOffset1); // Low center
    drawCloud(-0.50f, 0.32f, 0.04f, cloudOffset1); // Mid-left horizon
    drawCloud(0.55f, -0.10f, 0.05f, cloudOffset1); // Mid-right horizon
    drawCloud(-0.90f, -0.20f, 0.04f, cloudOffset1); // Far left bottom
    drawCloud(0.92f, -0.18f, 0.05f, cloudOffset1); // Far right bottom
    drawCloud(0.75f, -0.05f, 0.04f, cloudOffset1); // Mid right
    drawCloud(-0.70f, 0.23f, 0.05f, cloudOffset1); // Mid left
    drawCloud(1.25f, 0.18f, 0.06f, cloudOffset1); // Big Far Right
    drawCloud(-1.25f, 0.13f, 0.08f, cloudOffset1); // Big Far Right
    drawCloud(1.75f, -0.06f, 0.056f, cloudOffset1); // Big Far Right
    drawCloud(-1.85f, 0.11f, 0.058f, cloudOffset1); // Big Far Right

    // LAYER 2: MID-SKY (Medium Size)
    // Y-positions: 0.2 to 0.5 | Size: 0.09 to 0.14
    drawCloud(-0.85f, 0.30f, 0.10f, cloudOffset2);
    drawCloud(0.90f, 0.35f, 0.11f, cloudOffset2);
    drawCloud(0.20f, 0.40f, 0.13f, cloudOffset2);
    drawCloud(-0.45f, 0.25f, 0.09f, cloudOffset2);
    drawCloud(0.55f, 0.50f, 0.14f, cloudOffset2);
    drawCloud(0.00f, 0.30f, 0.11f, cloudOffset2); // Center Mid
    drawCloud(-0.20f, 0.45f, 0.12f, cloudOffset2); // Mid-High Left
    drawCloud(0.70f, 0.25f, 0.10f, cloudOffset2); // Low-Mid Right
    drawCloud(-0.68f, 0.08f, 0.12f, cloudOffset2); // Low-Mid Right
    drawCloud(0.68f, 0.08f, 0.12f, cloudOffset2); // Low-Mid Right
    drawCloud(-0.65f, 0.35f, 0.11f, cloudOffset2); // Left side filler
    drawCloud(0.35f, 0.28f, 0.10f, cloudOffset2); // Right side filler
    drawCloud(1.25f, 0.24f, 0.15f, cloudOffset3); // Big Far Right
    drawCloud(-1.25f, 0.39f, 0.13f, cloudOffset2); // Big Far Right
    drawCloud(1.75f, 0.41f, 0.14f, cloudOffset2); // Big Far Right
    drawCloud(-1.85f, 0.33f, 0.11f, cloudOffset2); // Big Far Right


    // LAYER 3: HIGH SKY (Largest, Closest/Front)
    // Y-positions: 0.6 to 0.9 | Size: 0.18 to 0.25
    drawCloud(-0.60f, 0.70f, 0.18f, cloudOffset3); // Big Left
    drawCloud(0.50f, 0.80f, 0.20f, cloudOffset3); // Big Right
    drawCloud(-0.10f, 0.85f, 0.22f, cloudOffset3); // Huge Top Center
    drawCloud(0.85f, 0.75f, 0.19f, cloudOffset3); // Big Far Right
    drawCloud(-0.90f, 0.60f, 0.16f, cloudOffset3); // Big Far Left
    drawCloud(0.20f, 0.90f, 0.21f, cloudOffset3); // Huge Top Right
    drawCloud(-0.35f, 0.55f, 0.19f, cloudOffset3); // Big Mid-Left
    drawCloud(0.0f, 0.55f, 0.19f, cloudOffset3); // Big Mid-Left
    drawCloud(-0.75f, 0.80f, 0.23f, cloudOffset3); // Huge Top Far Left
    drawCloud(1.25f, 0.7f, 0.21f, cloudOffset3); // Big Far Right
    drawCloud(-1.25f, 0.65f, 0.19f, cloudOffset3); // Big Far Right

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // ------------------------------------
    // 3. SEA GRADIENT (Seamless Blend)
    // ------------------------------------
    GLfloat seaVertices[] = {
        -1.0f,  seaLevel, 0.0f,         // Top-Left 
         1.0f,  seaLevel, 0.0f,         // Top-Right
         1.0f,  snowFieldHeight - 0.1f, 0.0f,  // Bottom Right
        -1.0f,  snowFieldHeight - 0.1f, 0.0f   // Bottom Left
    };

    // UPDATED COLORS: The Top of the sea now matches the Bottom of the sky
    GLfloat seaColors[12];

    // DAY TOP → DAWN TOP
    float sDayTopR = 0.5f, sDayTopG = 0.7f, sDayTopB = 0.95f;
    float sDawnTopR = 0.85f, sDawnTopG = 0.35f, sDawnTopB = 0.15f;

    // DAY BOTTOM → DAWN BOTTOM
    float sDayBotR = 0.1f, sDayBotG = 0.2f, sDayBotB = 0.5f;
    float sDawnBotR = 0.55f, sDawnBotG = 0.2f, sDawnBotB = 0.65f;

    seaColors[0] = seaColors[3] = mixf(sDayTopR, sDawnTopR, transition);
    seaColors[1] = seaColors[4] = mixf(sDayTopG, sDawnTopG, transition);
    seaColors[2] = seaColors[5] = mixf(sDayTopB, sDawnTopB, transition);

    seaColors[6] = seaColors[9] = mixf(sDayBotR, sDawnBotR, transition);
    seaColors[7] = seaColors[10] = mixf(sDayBotG, sDawnBotG, transition);
    seaColors[8] = seaColors[11] = mixf(sDayBotB, sDawnBotB, transition);

    glVertexPointer(3, GL_FLOAT, 0, seaVertices);
    glColorPointer(3, GL_FLOAT, 0, seaColors);
    glDrawArrays(GL_QUADS, 0, 4);

  // ------------------------------------
  // 4. TEXTURED SNOW FIELD
  // ------------------------------------
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    int numSegments = 60;
    GLfloat segmentWidth = 2.0f / numSegments;
    GLfloat snowBottom = -1.0f;

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numSegments; ++i) {
        GLfloat x = -1.0f + (i * segmentWidth);

        // Complex Noise for rugged snow
        GLfloat yBumpy = snowFieldHeight
            + 0.05f * sinf(x * 3.5f)
            + 0.02f * cosf(x * 15.0f)
            + 0.005f * sinf(x * 50.0f);

        yBumpy += 0.02f;

        if (yBumpy < snowBottom + 0.01f)
            yBumpy = snowBottom + 0.01f;

        // Height-based shading
        float heightFactor = (yBumpy - snowFieldHeight + 0.05f) * 10.0f;
        heightFactor = fminf(fmaxf(heightFactor, 0.0f), 1.0f);

        // Surface snow color
        float r = 0.9f + (0.15f * heightFactor);
        float g = 0.95f + (0.10f * heightFactor);
        float b = 1.0f + (0.05f * heightFactor);

        // Apply darkening automatically
        applySnowFieldColor(r, g, b);
        glVertex3f(x, yBumpy, 0.0f);

        // Underground color
        applySnowFieldColor(0.50f, 0.55f, 0.75f);
        glVertex3f(x, snowBottom, 0.0f);
    }
    glEnd();
}

// ----------------------------------------------------------------
// DISPLAY CALLBACK
// ----------------------------------------------------------------
void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Apply ground shake to the background
    glPushMatrix();
    glTranslatef(0.0f, groundShake, 0.0f);
    displayBackground();
    displayMountains();
    
    
    // Draw Godzilla (He shakes independently)
    glPushMatrix();
    glTranslatef(godzillaX, godzillaY, 0.0f);
    displayGodzilla();
    if (isFiring) {
        displayFire();
    }
    glPopMatrix();

    // Gate and Boy also shake with the ground
    displayToriGate();
    snowpile();
    displayBoy();
    drawBoyText();
    glPopMatrix(); // End ground shake

    // Birds fly smoothly (No shake)
    displayBirds();
    drawSnowBalls();

    glutSwapBuffers();
}

// ----------------------------------------------------------------
// MAIN FUNCTION
// ----------------------------------------------------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Manuzon - Sandiego - Computer Graphics");
    glutFullScreen();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    GLenum err = glewInit();
    if (GLEW_OK != err) return 1;

    initVBOs();

    // Initialize Birds
    GLfloat flockSpreadX = 0.25f;
    GLfloat flockSpreadY = 0.2f;
    for (int i = 0; i < NUM_BIRDS; ++i) {
        birdOffsets[i * 2] = ((float)rand() / RAND_MAX * flockSpreadX) - (flockSpreadX / 2.0f);
        birdOffsets[i * 2 + 1] = ((float)rand() / RAND_MAX * flockSpreadY) - (flockSpreadY / 2.0f);
    }
    initSnowBalls();
    glutSpecialFunc(birdMovement);
    glutTimerFunc(0, animateFlock, TIMER_ID_FLIGHT);
    glutTimerFunc(16, animateSnowfall, 0);
    glutTimerFunc(16, animateBackground, 0);
    glutTimerFunc(16, animateGodzilla, 0);
    glutTimerFunc(10000, animateGodzilla, 1);
    glutTimerFunc(16, animateBoyRun, 0);
    glutTimerFunc(16, animateClouds, 0);
    glutKeyboardFunc(keyboardMonitor);
    glutMouseFunc(mouseCallback);
    glutDisplayFunc(Display);
    glutMainLoop();
    return 0;
}

// ----------------------------------------------------------------
// DATA SECTION - TORII GATE GEOMETRY
// ----------------------------------------------------------------
GLfloat toriGateVertices[] = {
    // ---------------------------------------------------------
    // 3. LEFT PILLAR (Hashira)
    // ---------------------------------------------------------
    -0.2f,   0.19f,  0.0f,  // Top-Left
    -0.13f,  0.19f,  0.0f,  // Top-Right
    -0.13f, -0.6f,   0.0f,  // Bottom-Right
    -0.2f,  -0.62f,   0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 4. RIGHT PILLAR (Hashira)
    // ---------------------------------------------------------
     0.13f,  0.19f,  0.0f,  // Top-Left
     0.2f,   0.19f,  0.0f,  // Top-Right
     0.2f,  -0.62f,   0.0f,  // Bottom-Right
     0.13f, -0.6f,   0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 5. LOWER RED BEAM (Nuki)
     // ---------------------------------------------------------
     -0.28f,  0.1f,   0.0f,  // Top-Left
      0.28f,  0.1f,   0.0f,  // Top-Right
      0.26f,  0.02f,  0.0f,  // Bottom-Right
     -0.26f,  0.02f,  0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 6. MIDDLE STRUT (Gakuzuka)
     // ---------------------------------------------------------
     -0.05f,  0.19f,  0.0f,  // Top-Left
      0.05f,  0.19f,  0.0f,  // Top-Right
      0.05f,  0.1f,   0.0f,  // Bottom-Right
     -0.05f,  0.1f,   0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 9. UPPER CONNECTOR RED BEAM LEFT
     // ---------------------------------------------------------
     -0.3f,  -0.18f,  0.0f,  // Top-Left
     -0.13f, -0.12f,  0.0f,  // Top-Right
     -0.13f, -0.24f,  0.0f,  // Bottom-Right
     -0.3f,  -0.3f,   0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 10. LOWER RED BEAM LEFT
     // ---------------------------------------------------------
     -0.3f,  -0.43f,  0.0f,  // Top-Left
     -0.13f, -0.37f,  0.0f,  // Top-Right
     -0.13f, -0.49f,  0.0f,  // Bottom-Right
     -0.3f,  -0.55f,  0.0f,  // Bottom-Left

     // ---------------------------------------------------------
     // 11. UPPER CONNECTOR RED BEAM RIGHT
     // ---------------------------------------------------------
      0.3f,  -0.18f,  0.0f,  // Top-Left
      0.13f, -0.12f,  0.0f,  // Top-Right
      0.13f, -0.24f,  0.0f,  // Bottom-Right
      0.3f,  -0.3f,   0.0f,  // Bottom-Left

      // ---------------------------------------------------------
      // 12. LOWER RED BEAM RIGHT
      // ---------------------------------------------------------
       0.3f,  -0.43f,  0.0f,  // Top-Left
       0.13f, -0.37f,  0.0f,  // Top-Right
       0.13f, -0.49f,  0.0f,  // Bottom-Right
       0.3f,  -0.55f,  0.0f,  // Bottom-Left

       // ---------------------------------------------------------
       // 7. SMALL LEFT PILLAR 
       // ---------------------------------------------------------
       -0.3f,  -0.08f,  0.0f,  // Top-Left
       -0.23f, -0.08f,  0.0f,  // Top-Right
       -0.23f, -0.7f,   0.0f,  // Bottom-Right
       -0.3f,  -0.7f,   0.0f,  // Bottom-Left

       // ---------------------------------------------------------
       // 8. SMALL RIGHT PILLAR 
       // ---------------------------------------------------------
        0.23f, -0.08f,  0.0f,  // Top-Left
        0.3f,  -0.08f,  0.0f,  // Top-Right
        0.3f,  -0.7f,   0.0f,  // Bottom-Right
        0.23f, -0.7f,   0.0f,  // Bottom-Left

        // ---------------------------------------------------------
        // 13. Pillar detail lower left
        // ---------------------------------------------------------
        -0.22f,  0.13f,  0.0f,  // Top-Left
        -0.11f,  0.13f,  0.0f,  // Top-Right
        -0.13f,  0.09f,  0.0f,  // Bottom-Right
        -0.2f,   0.09f,  0.0f,  // Bottom-Left

        // ---------------------------------------------------------
        // 14. Pillar detail lower right
        // ---------------------------------------------------------
         0.22f,  0.13f,  0.0f,  // Top-Left
         0.11f,  0.13f,  0.0f,  // Top-Right
         0.13f,  0.09f,  0.0f,  // Bottom-Right
         0.2f,   0.09f,  0.0f,  // Bottom-Left

         // ---------------------------------------------------------
         // 15. Pillar detail upper left
         // ---------------------------------------------------------
         -0.21f,  0.2f,   0.0f,  // Top-Left
         -0.12f,  0.2f,   0.0f,  // Top-Right
         -0.12f,  0.16f,  0.0f,  // Bottom-Right
         -0.21f,  0.16f,  0.0f,  // Bottom-Left

         // ---------------------------------------------------------
         // 16. Pillar detail upper right
         // ---------------------------------------------------------
          0.21f,  0.2f,   0.0f,  // Top-Left
          0.12f,  0.2f,   0.0f,  // Top-Right
          0.12f,  0.16f,  0.0f,  // Bottom-Right
          0.21f,  0.16f,  0.0f,  // Bottom-Left

          // ---------------------------------------------------------
          // 17. left side pillar roof
          // ---------------------------------------------------------
          -0.32f, -0.1f,   0.0f,  // Top-Left
          -0.21f, -0.1f,   0.0f,  // Top-Right
          -0.23f, -0.06f,  0.0f,  // Bottom-Right
          -0.3f,  -0.06f,  0.0f,  // Bottom-Left

          // ---------------------------------------------------------
          // 17. right side pillar roof
          // ---------------------------------------------------------
           0.32f, -0.1f,   0.0f,  // Top-Left
           0.21f, -0.1f,   0.0f,  // Top-Right
           0.23f, -0.06f,  0.0f,  // Bottom-Right
           0.3f,  -0.06f,  0.0f,  // Bottom-Left

           // ---------------------------------------------------------
           // 1. TOP BLACK ROOF (Kasagi)
           // ---------------------------------------------------------
           -0.39f,  0.3f,   0.0f,  // Top-Left
            0.39f,  0.3f,   0.0f,  // Top-Right
            0.37f,  0.25f,  0.0f,  // Bottom-Right
           -0.37f,  0.25f,  0.0f,  // Bottom-Left

           // ---------------------------------------------------------
           // 2. UPPER RED BEAM (Shimaki)
           // ---------------------------------------------------------
           -0.35f,  0.25f,  0.0f,  // Top-Left
            0.35f,  0.25f,  0.0f,  // Top-Right
            0.33f,  0.19f,  0.0f,  // Bottom-Right
           -0.33f,  0.19f,  0.0f,  // Bottom-Left
};

GLfloat colors[] = {
    // ---------------------------------------------------------
    // 3. LEFT PILLAR (Hashira)
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 4. RIGHT PILLAR (Hashira)
    // ---------------------------------------------------------
    0.8f, 0.1f, 0.1f,  // Top-Left
    0.3f, 0.0f, 0.0f,  // Top-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Left

    // ---------------------------------------------------------
    // 5. LOWER BEAM (Nuki)
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.3f, 0.0f, 0.0f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Left

    // ---------------------------------------------------------
    // 6. MIDDLE STRUT (Gakuzuka)
    // ---------------------------------------------------------
    0.6f, 0.0f, 0.0f,  // Top-Left
    0.5f, 0.0f, 0.0f,  // Top-Right
    0.6f, 0.0f, 0.0f,  // Bottom-Right
    0.5f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 9. CONNECTOR LEFT TOP
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 10. CONNECTOR LEFT BOTTOM
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 11. CONNECTOR RIGHT TOP
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 12. CONNECTOR RIGHT BOTTOM
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 7. SMALL LEFT PILLAR
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 8. SMALL RIGHT PILLAR
    // ---------------------------------------------------------
    0.8f, 0.1f, 0.1f,  // Top-Left
    0.3f, 0.0f, 0.0f,  // Top-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Left

    // ---------------------------------------------------------
    // 13. LEFT LOWER DETAIL
    // ---------------------------------------------------------
    0.8f, 0.1f, 0.1f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 14. RIGHT LOWER DETAIL
    // ---------------------------------------------------------
    0.8f, 0.1f, 0.1f,  // Top-Left
    0.8f, 0.1f, 0.1f,  // Top-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Right
    0.3f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 15. LEFT UPPER DETAIL
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.3f, 0.0f, 0.0f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Left

    // ---------------------------------------------------------
    // 16. RIGHT UPPER DETAIL
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.3f, 0.0f, 0.0f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Left

    // ---------------------------------------------------------
    // 17. ROOF SIDE PILLAR LEFT (Black)
    // ---------------------------------------------------------
    0.0f, 0.0f, 0.0f,  // Top-Left
    0.0f, 0.0f, 0.0f,  // Top-Right
    0.0f, 0.0f, 0.0f,  // Bottom-Right
    0.0f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 18. ROOF SIDE PILLAR RIGHT (Black)
    // ---------------------------------------------------------
    0.0f, 0.0f, 0.0f,  // Top-Left
    0.0f, 0.0f, 0.0f,  // Top-Right
    0.0f, 0.0f, 0.0f,  // Bottom-Right
    0.0f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 1. TOP BLACK ROOF (Kasagi)
    // ---------------------------------------------------------
    0.0f, 0.0f, 0.0f,  // Top-Left
    0.0f, 0.0f, 0.0f,  // Top-Right
    0.0f, 0.0f, 0.0f,  // Bottom-Right
    0.0f, 0.0f, 0.0f,  // Bottom-Left

    // ---------------------------------------------------------
    // 2. UPPER RED BEAM (Shimaki)
    // ---------------------------------------------------------
    0.3f, 0.0f, 0.0f,  // Top-Left
    0.3f, 0.0f, 0.0f,  // Top-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Right
    0.8f, 0.1f, 0.1f,  // Bottom-Left
};

// ----------------------------------------------------------------
// DISPLAY TORII GATE (Geometry Only)
// ----------------------------------------------------------------
void displayToriGate() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, toriGateVertices);
    glColorPointer(3, GL_FLOAT, 0, colors);
    glDrawArrays(GL_QUADS, 0, 72);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void setSnowColor(float r, float g, float b, float a) {
    float t = transition; // 0 = day, 1 = dawn

    // Dawn snow is darker & slightly bluish
    float r2 = r * 0.45f;
    float g2 = g * 0.45f;
    float b2 = b * 0.55f;

    float R = mixf(r, r2, t);
    float G = mixf(g, g2, t);
    float B = mixf(b, b2, t);

    glColor4f(R, G, B, a);
}

//-----------------------------------------------------------------
// PILE OF SNOW
//-----------------------------------------------------------------
void snowpile() {
    // Enable Blending for soft edges/transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable Point Smoothing to make the GL_POINTS look like round snowballs
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    // =========================================================
    // LAYER 2: SNOW PILES (Foreground)
    // Updated to use glColor4f for blending
    // =========================================================
   
    // PILE 1: FAR LEFT
        glBegin(GL_POLYGON);

        setSnowColor(0.85f, 0.85f, 0.95f, 0.9f);   // Base
        glVertex3f(-0.95f, -0.90f, 0.0f);
        glVertex3f(-0.85f, -0.92f, 0.0f);
        glVertex3f(-0.70f, -0.90f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.39f);      // Top highlight
        glVertex3f(-0.65f, -0.80f, 0.0f);
        glVertex3f(-0.72f, -0.70f, 0.0f);
        glVertex3f(-0.80f, -0.75f, 0.0f);
        glVertex3f(-0.88f, -0.68f, 0.0f);

        setSnowColor(0.95f, 0.95f, 1.0f, 0.59f);
        glVertex3f(-0.98f, -0.80f, 0.0f);
        glEnd();


        // PILE 2: MID LEFT
        glBegin(GL_POLYGON);

        setSnowColor(0.85f, 0.85f, 0.95f, 0.9f);
        glVertex3f(-0.60f, -0.85f, 0.0f);
        glVertex3f(-0.50f, -0.88f, 0.0f);
        glVertex3f(-0.35f, -0.85f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.49f);
        glVertex3f(-0.38f, -0.75f, 0.0f);
        glVertex3f(-0.42f, -0.65f, 0.0f);
        glVertex3f(-0.48f, -0.70f, 0.0f);
        glVertex3f(-0.52f, -0.62f, 0.0f);
        glVertex3f(-0.58f, -0.72f, 0.0f);
        glEnd();


        // PILE 3: CENTER LEFT
        glBegin(GL_POLYGON);

        setSnowColor(0.85f, 0.85f, 0.95f, 0.98f);
        glVertex3f(-0.3f, -0.75f, 0.0f);
        glVertex3f(-0.2f, -0.78f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.98f);
        glVertex3f(-0.15f, -0.65f, 0.0f);
        glVertex3f(-0.19f, -0.68f, 0.0f);
        glVertex3f(-0.23f, -0.62f, 0.0f);
        glVertex3f(-0.27f, -0.59f, 0.0f);
        glVertex3f(-0.31f, -0.65f, 0.0f);
        glVertex3f(-0.33f, -0.70f, 0.0f);
        glEnd();


        // PILE 4: CENTER RIGHT
        glBegin(GL_POLYGON);

        setSnowColor(0.85f, 0.85f, 0.95f, 0.98f);
        glVertex3f(0.15f, -0.70f, 0.0f);
        glVertex3f(0.25f, -0.72f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.98f);
        glVertex3f(0.35f, -0.70f, 0.0f);
        glVertex3f(0.45f, -0.65f, 0.0f);
        glVertex3f(0.38f, -0.57f, 0.0f);
        glVertex3f(0.30f, -0.63f, 0.0f);
        glVertex3f(0.22f, -0.6f, 0.0f);
        glVertex3f(0.15f, -0.65f, 0.0f);
        glEnd();


        // PILE 5: MID RIGHT
        glBegin(GL_POLYGON);

        setSnowColor(0.85f, 0.85f, 0.95f, 0.49f);
        glVertex3f(0.50f, -0.85f, 0.0f);
        glVertex3f(0.65f, -0.85f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.4f);
        glVertex3f(0.70f, -0.75f, 0.0f);
        glVertex3f(0.62f, -0.65f, 0.0f);
        glVertex3f(0.58f, -0.70f, 0.0f);
        glVertex3f(0.55f, -0.68f, 0.0f);
        glVertex3f(0.52f, -0.75f, 0.0f);
        glVertex3f(0.48f, -0.80f, 0.0f);
        glEnd();


        // PILE 6: FAR RIGHT
        glBegin(GL_POLYGON);

        setSnowColor(0.85f, 0.85f, 0.95f, 0.8f);
        glVertex3f(0.75f, -0.90f, 0.0f);
        glVertex3f(0.95f, -0.90f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.8f);
        glVertex3f(0.98f, -0.80f, 0.0f);
        glVertex3f(0.92f, -0.72f, 0.0f);
        glVertex3f(0.88f, -0.75f, 0.0f);
        glVertex3f(0.85f, -0.70f, 0.0f);
        glVertex3f(0.80f, -0.78f, 0.0f);
        glVertex3f(0.72f, -0.85f, 0.0f);
        glEnd();

        // PILE 6
        glBegin(GL_POLYGON);

        setSnowColor(0.85f, 0.85f, 0.95f, 0.5f);
        glVertex3f(0.4f, -1.00f, 0.0f);
        glVertex3f(0.45f, -1.00f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.5f);
        glVertex3f(0.50f, -0.95f, 0.0f);
        glVertex3f(0.42f, -0.85f, 0.0f);
        glVertex3f(0.38f, -0.90f, 0.0f);
        glVertex3f(0.35f, -0.88f, 0.0f);
        glVertex3f(0.32f, -0.95f, 0.0f);
        glVertex3f(0.28f, -1.0f, 0.0f);
        glEnd();

        // PILE 7: LOW LEFT FILLER
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.589f);
        glVertex3f(-0.85f, -0.95f, 0.0f);
        glVertex3f(-0.75f, -0.96f, 0.0f);
        glVertex3f(-0.65f, -0.95f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.59f);
        glVertex3f(-0.68f, -0.88f, 0.0f);
        glVertex3f(-0.75f, -0.85f, 0.0f);
        glVertex3f(-0.82f, -0.88f, 0.0f);
        glEnd();

        // PILE 9: RIGHT SIDE GAP FILL
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.59f);
        glVertex3f(0.35f, -0.90f, 0.0f);
        glVertex3f(0.45f, -0.92f, 0.0f);
        glVertex3f(0.55f, -0.90f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.59f);
        glVertex3f(0.52f, -0.82f, 0.0f);
        glVertex3f(0.45f, -0.78f, 0.0f);
        glVertex3f(0.38f, -0.82f, 0.0f);
        glEnd();

        // PILE 10: FAR LEFT EDGE (SMALL)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.5f);
        glVertex3f(-0.99f, -0.85f, 0.0f);
        glVertex3f(-0.95f, -0.87f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.5f);
        glVertex3f(-0.94f, -0.78f, 0.0f);
        glVertex3f(-0.97f, -0.75f, 0.0f);
        glEnd();


        // PILE 12: BACKGROUND RIGHT (BEHIND GATE)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.4f);
        glVertex3f(0.15f, -0.92f, 0.0f);
        glVertex3f(0.25f, -0.94f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.5f);
        glVertex3f(0.22f, -0.85f, 0.0f);
        glVertex3f(0.18f, -0.85f, 0.0f);
        glEnd();

        // PILE 13: MID-RIGHT FOREGROUND
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.39f);
        glVertex3f(0.60f, -0.88f, 0.0f);
        glVertex3f(0.70f, -0.90f, 0.0f);
        glVertex3f(0.80f, -0.88f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.2f);
        glVertex3f(0.75f, -0.82f, 0.0f);
        glVertex3f(0.70f, -0.78f, 0.0f);
        glVertex3f(0.65f, -0.82f, 0.0f);
        glEnd();

        // PILE 14: VERY FAR RIGHT
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.52f);
        glVertex3f(0.85f, -0.70f, 0.0f);
        glVertex3f(0.92f, -0.72f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.21f);
        glVertex3f(0.95f, -0.65f, 0.0f);
        glVertex3f(0.88f, -0.62f, 0.0f);
        glEnd();

        // PILE 15: RANDOM BUMP CENTER LEFT 
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.6f);
        glVertex3f(-0.30f, -0.88f, 0.0f);
        glVertex3f(-0.10f, -0.90f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.6f);
        glVertex3f(-0.12f, -0.80f, 0.0f);
        glVertex3f(-0.18f, -0.76f, 0.0f); 
        glVertex3f(-0.24f, -0.76f, 0.0f); 
        glVertex3f(-0.28f, -0.82f, 0.0f);
        glEnd();

        // PILE 16: LEFT CENTER GAP (Curvy irregular)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.51f);
        glVertex3f(-0.45f, -0.88f, 0.0f);
        glVertex3f(-0.30f, -0.90f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.51f);
        glVertex3f(-0.28f, -0.82f, 0.0f);
        glVertex3f(-0.32f, -0.76f, 0.0f); 
        glVertex3f(-0.42f, -0.78f, 0.0f); 
        glVertex3f(-0.48f, -0.80f, 0.0f);
        glEnd();

        // PILE 17: RIGHT CENTER GAP (Curvy irregular)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.1f);
        glVertex3f(0.30f, -0.90f, 0.0f);
        glVertex3f(0.48f, -0.88f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.1f);
        glVertex3f(0.50f, -0.80f, 0.0f);
        glVertex3f(0.44f, -0.76f, 0.0f); // Curve
        glVertex3f(0.36f, -0.76f, 0.0f); // Curve
        glVertex3f(0.28f, -0.82f, 0.0f);
        glEnd();

        // PILE 18: CENTER FILLER (Small Mound)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.1f);
        glVertex3f(-0.05f, -1.0f, 0.0f);
        glVertex3f(0.12f, -1.0f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.1f);
        glVertex3f(0.10f, -0.98f, 0.0f);
        glVertex3f(0.04f, -0.94f, 0.0f); // Peak
        glVertex3f(-0.02f, -0.94f, 0.0f); // Peak
        glVertex3f(-0.08f, -0.96f, 0.0f);
        glEnd();

        // PILE 19: DEEP BACKGROUND LEFT (Round Hill)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.81f);
        glVertex3f(-0.65f, -0.68f, 0.0f);
        glVertex3f(-0.50f, -0.65f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.72f);
        glVertex3f(-0.48f, -0.58f, 0.0f);
        glVertex3f(-0.55f, -0.54f, 0.0f); // Round Top
        glVertex3f(-0.62f, -0.54f, 0.0f); // Round Top
        glVertex3f(-0.68f, -0.60f, 0.0f);
        glEnd();

        // PILE 22: LONG FOREGROUND RIGHT (Wavy)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.95f);
        glVertex3f(0.65f, -0.65f, 0.0f);
        glVertex3f(0.85f, -0.55f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.85f);
        glVertex3f(1.3f, -1.0f, 0.0f);
        glVertex3f(0.85f, -0.73f, 0.0f); // Wave
        glVertex3f(0.80f, -0.73f, 0.0f); // Wave
        glVertex3f(0.60f, -1.2f, 0.0f);
        glEnd();

        // PILE 20: DEEP BACKGROUND RIGHT (Round Hill)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.1f);
        glVertex3f(0.50f, -0.65f, 0.0f);
        glVertex3f(0.65f, -0.68f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.15f);
        glVertex3f(0.68f, -0.60f, 0.0f);
        glVertex3f(0.60f, -0.54f, 0.0f); // Round Top
        glVertex3f(0.55f, -0.54f, 0.0f); // Round Top
        glVertex3f(0.48f, -0.58f, 0.0f);
        glEnd();

        // PILE 21: LONG FOREGROUND LEFT (Wavy)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.53f);
        glVertex3f(-0.85f, -1.0f, 0.0f);
        glVertex3f(-0.45f, -1.2f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.7f);
        glVertex3f(-0.40f, -1.2f, 0.0f);
        glVertex3f(-0.55f, -0.78f, 0.0f); // Wave up
        glVertex3f(-1.0f, -0.58f, 0.0f); // Wave
        glVertex3f(-1.20f, -1.0f, 0.0f);
        glEnd();

        // PILE 22: LONG FOREGROUND RIGHT (Wavy)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.75f);
        glVertex3f(0.45f, -1.0f, 0.0f);
        glVertex3f(0.85f, -1.0f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.85f);
        glVertex3f(1.3f, -1.0f, 0.0f);
        glVertex3f(0.85f, -0.73f, 0.0f); // Wave
        glVertex3f(0.80f, -0.73f, 0.0f); // Wave
        glVertex3f(0.60f, -1.2f, 0.0f);
        glEnd();



        // PILE 23: FAR LEFT GAP (Round Wedge)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.75f);
        glVertex3f(-0.95f, -0.72f, 0.0f);
        glVertex3f(-0.82f, -0.75f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.75f);
        glVertex3f(-0.80f, -0.65f, 0.0f);
        glVertex3f(-0.86f, -0.60f, 0.0f); // Round Tip
        glVertex3f(-0.92f, -0.60f, 0.0f); // Round Tip
        glVertex3f(-0.98f, -0.65f, 0.0f);
        glEnd();

        // PILE 24: FAR RIGHT GAP (Round Wedge)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.75f);
        glVertex3f(0.82f, -0.75f, 0.0f);
        glVertex3f(0.95f, -0.72f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.75f);
        glVertex3f(0.98f, -0.65f, 0.0f);
        glVertex3f(0.92f, -0.60f, 0.0f); // Round Tip
        glVertex3f(0.86f, -0.60f, 0.0f); // Round Tip
        glVertex3f(0.80f, -0.65f, 0.0f);
        glEnd();

        // PILE 25: BEHIND PILE 1 (Round Mound)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.55f);
        glVertex3f(-0.90f, -0.68f, 0.0f);
        glVertex3f(-0.70f, -0.66f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.55f);
        glVertex3f(-0.68f, -0.58f, 0.0f);
        glVertex3f(-0.75f, -0.54f, 0.0f); // Curve
        glVertex3f(-0.85f, -0.54f, 0.0f); // Curve
        glVertex3f(-0.95f, -0.60f, 0.0f);
        glEnd();

        // PILE 26: BEHIND PILE 6 (Round Mound)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.15f);
        glVertex3f(0.70f, -0.66f, 0.0f);
        glVertex3f(0.90f, -0.68f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.75f);
        glVertex3f(0.95f, -0.60f, 0.0f);
        glVertex3f(0.88f, -0.54f, 0.0f); // Curve
        glVertex3f(0.78f, -0.54f, 0.0f); // Curve
        glVertex3f(0.68f, -0.58f, 0.0f);
        glEnd();

        // PILE 27: TINY FOREGROUND LEFT (Round Bump)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.95f);
        glVertex3f(-0.20f, -0.90f, 0.0f);
        glVertex3f(-0.05f, -0.91f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.95f);
        glVertex3f(-0.02f, -0.85f, 0.0f);
        glVertex3f(-0.08f, -0.82f, 0.0f); // Top
        glVertex3f(-0.16f, -0.82f, 0.0f); // Top
        glVertex3f(-0.22f, -0.85f, 0.0f);
        glEnd();

        // PILE 28: TINY FOREGROUND RIGHT (Round Bump)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.05f);
        glVertex3f(0.05f, -0.91f, 0.0f);
        glVertex3f(0.20f, -0.90f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.15f);
        glVertex3f(0.22f, -0.85f, 0.0f);
        glVertex3f(0.16f, -0.82f, 0.0f); // Top
        glVertex3f(0.08f, -0.82f, 0.0f); // Top
        glVertex3f(0.02f, -0.85f, 0.0f);
        glEnd();

    

        // PILE 30: LOW CRAGGY RIGHT (Rounded)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.95f);
        glVertex3f(0.62f, -0.64f, 0.0f);
        glVertex3f(0.92f, -0.62f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.95f);
        glVertex3f(0.95f, -0.55f, 0.0f);
        glVertex3f(0.86f, -0.52f, 0.0f); // Top
        glVertex3f(0.78f, -0.52f, 0.0f); // Top
        glVertex3f(0.70f, -0.55f, 0.0f);
        glEnd();

        // PILE 31: LARGE FAR RIGHT BACKGROUND (High Opacity)
        glBegin(GL_POLYGON);
        setSnowColor(0.90f, 0.90f, 0.98f, 0.95f); // High alpha Base
        glVertex3f(0.75f, -0.65f, 0.0f);
        glVertex3f(1.05f, -0.68f, 0.0f); // Wide base going off-screen

        setSnowColor(1.0f, 1.0f, 1.0f, 1.0f); // Solid white top
        glVertex3f(1.1f, -0.58f, 0.0f);
        glVertex3f(1.0f, -0.52f, 0.0f); // High peak
        glVertex3f(0.85f, -0.54f, 0.0f); // Lumpy top
        glVertex3f(0.72f, -0.60f, 0.0f);
        glEnd();

        // PILE 32: MID-RIGHT MIDGROUND FILLER (High Opacity)
        glBegin(GL_POLYGON);
        setSnowColor(0.88f, 0.88f, 0.96f, 0.9f); // Base
        glVertex3f(0.55f, -0.72f, 0.0f);
        glVertex3f(0.75f, -0.74f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.95f); // Top
        glVertex3f(0.78f, -0.65f, 0.0f);
        glVertex3f(0.70f, -0.62f, 0.0f); // Lumpy
        glVertex3f(0.60f, -0.62f, 0.0f); // Lumpy
        glVertex3f(0.52f, -0.68f, 0.0f);
        glEnd();

        // PILE 33: LARGE RIGHT FOREGROUND (Lower Opacity - closer)
        glBegin(GL_POLYGON);
        setSnowColor(0.85f, 0.85f, 0.95f, 0.6f); // Lower alpha base
        glVertex3f(0.65f, -0.95f, 0.0f);
        glVertex3f(0.95f, -0.98f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 0.7f); // Lower alpha top
        glVertex3f(0.98f, -0.85f, 0.0f);
        glVertex3f(0.88f, -0.82f, 0.0f); // Round top
        glVertex3f(0.75f, -0.82f, 0.0f); // Round top
        glVertex3f(0.62f, -0.88f, 0.0f);
        glEnd();

        // PILE 34: EXTRA FAR RIGHT EDGE (Solid Opacity - background)
        glBegin(GL_POLYGON);
        setSnowColor(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, -0.50f, 0.0f);
        glVertex3f(1.2f, -0.52f, 0.0f);

        setSnowColor(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3f(1.2f, -0.40f, 0.0f);
        glVertex3f(1.1f, -0.35f, 0.0f);
        glVertex3f(1.0f, -0.42f, 0.0f);
        glEnd();

        glDisable(GL_POINT_SMOOTH);
        glDisable(GL_BLEND);
        glFlush();
    }


// ----------------------------------------------------------------
// GODZILLA (Vertex Arrays + Procedural Spikes)
// ----------------------------------------------------------------
GLfloat godzillaVertices[] = {
    // 1. MAIN BODY
     0.45f,  0.67f,  0.0f,  // Top-Left
     0.65f,  0.5f,  0.0f,  // Top-Right
     0.7f,  -0.45f,  0.0f,  // Bottom-Right
     0.45f,  -0.45f,  0.0f,  // Bottom-Left

     // 2. HEAD
      0.4f,  0.68f,  0.0f,  // Top-Left
      0.6f,  0.65f,  0.0f,  // Top-Right
      0.6f,  0.45f,  0.0f,  // Bottom-Right
      0.4f,  0.45f,  0.0f,  // Bottom-Left

      // 3. EYE
       0.47f,  0.6f,  0.0f,  // Top-Left
       0.52f,  0.64f,  0.0f,  // Top-Right
       0.52f,  0.58f,  0.0f,  // Bottom-Right
       0.47f,  0.58f,  0.0f,  // Bottom-Left

       // 4. BACK TRIANGLE
        0.65f,  0.45f,  0.0f,  // Top-Left
        0.65f,  -0.45f,  0.0f,  // Top-Right
        1.0f,  -0.45f,  0.0f,  // Bottom-Right

        // 5. LEG
         0.65f,  -0.05f,  0.0f,  // peak
         0.8f,  -0.15f,  0.0f,  // Top-Right
         0.75f,  -0.5f,  0.0f,  // Bottom-Right
         0.5f,  -0.5f,  0.0f,  // Bottom-Left
         0.5f,  -0.25f,  0.0f,  // Top-left

         // 6. ARM
          0.5f,  0.3f,  0.0f,  // 1 ledt top
          0.6f,  0.3f,  0.0f,  // 1 right top
          0.6f,  0.15f,  0.0f,  // 1 right bottom
          0.4f,  0.15f,  0.0f,  // 1 left bottom
          0.45f,  0.25f,  0.0f,  // 1 left middle
          0.5f,  0.25f,  0.0f,  // 1 right middle

          // 7. OPEN MOUTH (rectangle to match fire base)
       0.4f, 0.46f, 0.0f,  // Bottom-Left
       0.4f, 0.58f, 0.0f,  // Top-Left
       0.44f, 0.57f, 0.0f,  // Top-Right
       0.44f, 0.47f, 0.0f   // Bottom-Right
};

GLfloat godzillaColors[] = {
    // 1. MAIN BODY
    0.25f, 0.2f, 0.25f, 1.0f,   // Top-Left
    0.25f, 0.2f, 0.75f, 1.0f,  // Top-Right
    0.25f, 0.2f, 0.25f, 0.9f,  // Bottom-Right
    0.25f, 0.2f, 0.25f, 0.9f, // Bottom-Left

    // 2. HEAD
    0.25f, 0.2f, 0.25f, 1.0f, // Top-Left
    0.25f, 0.2f, 0.75f, 1.0f,  // Top-Right
    0.25f, 0.2f, 0.25f, 1.0f,  // Bottom-Right
    0.25f, 0.2f, 0.25f, 1.0f,  // Bottom-Left

    // 3. EYE
    1.0f, 1.0f, 1.0f, 1.0f,  // Top-Left
    1.0f, 1.0f, 1.0f, 1.0f,    // Top-Right
    1.0f, 1.0f, 1.0f, 1.0f,    // Bottom-Right
    1.0f, 1.0f, 1.0f, 1.0f,    // Bottom-Left

    // 4. BACK TRIANGLE
    0.25f, 0.2f, 0.65f, 1.0f,   // Top-Left
    0.25f, 0.2f, 0.25f, 0.9f,  // bottem-left
    0.25f, 0.2f, 0.75f, 0.9f, // Bottom-Right

    // 5. LEG
    0.25f, 0.2f, 0.65f, 1.0f,  // Vertex 1
    0.25f, 0.2f, 0.55f, 1.0f,  // Vertex 2
    0.25f, 0.2f, 0.45f, 0.95f,  // Vertex 3
    0.25f, 0.2f, 0.25f, 0.95f,  // Vertex 4
    0.25f, 0.2f, 0.25f, 1.0f,  // Vertex 5

    // 6. ARM
    0.25f, 0.2f, 0.35f,1.0f,   // Vertex 1
    0.25f, 0.2f, 0.75f, 1.0f,  // Vertex 2
    0.25f, 0.2f, 0.35f, 1.0f,  // Vertex 3
    0.25f, 0.2f, 0.25f, 1.0f,  // Vertex 4
    0.25f, 0.2f, 0.25f, 1.0f,  // Vertex 5
    0.0f, 0.0f, 0.0f,  1.0f,   // Vertex 6

     // 7. OPEN MOUTH
    0.4f, 0.1f, 0.0f, 1.0f,  // Bottom-Left
    0.4f, 0.1f, 0.0f, 1.0f,  // Top-Left
    0.73f, 0.1f, 0.0f, 1.0f,  // Top-Right
    0.65, 0.1f, 0.0f, 1.0f   // Bottom-Right
};

void displayGodzilla() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, godzillaVertices);
    glColorPointer(4, GL_FLOAT, 0, godzillaColors);

    glDrawArrays(GL_QUADS, 0, 12);
    glDrawArrays(GL_TRIANGLES, 12, 3);
    glDrawArrays(GL_POLYGON, 15, 5);
    glDrawArrays(GL_POLYGON, 20, 6);
    glDrawArrays(GL_POLYGON, 26, 4);

    // Draw Spikes
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    float startX = 0.65f, startY = 0.45f;
    float endX = 1.0f, endY = -0.45f;
    int numSpikes = 6;

    GLfloat neonTipColor[] = { 0.7f, 0.85f, 1.0f };
    GLfloat neonBaseColor[] = { 0.1f, 0.2f, 0.9f };

    for (int i = 0; i < numSpikes; i++) {
        float t = (float)i / (float)numSpikes;
        float t_next = (float)(i + 0.75) / (float)numSpikes;

        float baseX = startX + (endX - startX) * t;
        float baseY = startY + (endY - startY) * t;
        float nextBaseX = startX + (endX - startX) * t_next;
        float nextBaseY = startY + (endY - startY) * t_next;

        float size = 0.32f * (1.0f - (t * 0.3f));
        float tipX = (baseX + nextBaseX) / 2.0f + 0.08f;
        float tipY = (baseY + nextBaseY) / 2.0f + size;

        glBegin(GL_TRIANGLE_FAN);
        glColor3fv(neonTipColor);
        glVertex3f(tipX, tipY, 0.0f);
        glColor3fv(neonBaseColor);
        glVertex3f(baseX, baseY, 0.0f);
        glColor3fv(neonBaseColor);
        glVertex3f(nextBaseX, nextBaseY, 0.0f);

        glDisable(GL_BLEND);
        glEnd();
    }
}

// ----------------------------------------------------------------
// BOY OBJECT (Layered Hair, Thinner Body, Shorter Legs - 52 Vertices)
// ----------------------------------------------------------------
GLfloat boyVertices[] = {
    // 1. HEAD (Skin)
    -0.035f,  0.05f, 0.0f,  // Top-Left 
     0.035f,  0.05f, 0.0f,  // Top-Right
     0.035f, -0.02f, 0.0f,  // Bottom-Right
    -0.035f, -0.02f, 0.0f,  // Bottom-Left

    // 3. HAIR BACK LAYER
-0.040f,  0.06f, 0.0f,  // Top-Left (Slightly overlaps main hair)
 0.040f,  0.06f, 0.0f,  // Top-Right
 0.025f, 0.01f, 0.0f,  // Bottom-Right (Tapered in towards the center)
-0.025f, 0.01f, 0.0f,  // Bottom-Left


// 2. HAIR TOP VOLUME - Main block
-0.045f,  0.1f, 0.0f,  // Top-Left 
 0.045f,  0.1f, 0.0f,  // Top-Right
 0.045f,  0.05f, 0.0f,  // Bottom-Right (Aligns with head top)
-0.045f,  0.05f, 0.0f,  // Bottom-Left

// 4. NECK (Skin)
-0.015f, -0.02f, 0.0f,  // Top-Left
 0.015f, -0.02f, 0.0f,  // Top-Right
 0.015f, -0.04f, 0.0f,  // Bottom-Right
-0.015f, -0.04f, 0.0f,  // Bottom-Left

// 5. BODY
-0.04f,  -0.04f, 0.0f,  // Top-Left 
 0.04f,  -0.04f, 0.0f,  // Top-Right
 0.04f,  -0.16f, 0.0f,  // Bottom-Right 
-0.04f,  -0.16f, 0.0f,  // Bottom-Left

// 6. LEFT ARM 
-0.07f,  -0.04f, 0.0f,  // Top-Left 
-0.04f,  -0.04f, 0.0f,  // Top-Right 
-0.04f,  -0.11f, 0.0f,  // Bottom-Right
-0.07f,  -0.11f, 0.0f,  // Bottom-Left

// 7. RIGHT ARM 
 0.04f,  -0.04f, 0.0f,  // Top-Left 
 0.07f,  -0.04f, 0.0f,  // Top-Right 
 0.07f,  -0.11f, 0.0f,  // Bottom-Right
 0.04f,  -0.11f, 0.0f,  // Bottom-Left

 // 8. LEFT HAND (Skin)
 -0.07f,  -0.11f, 0.0f,  // Top-Left
 -0.05f,  -0.11f, 0.0f,  // Top-Right
 -0.05f,  -0.13f, 0.0f,  // Bottom-Right
 -0.07f,  -0.13f, 0.0f,  // Bottom-Left

 // 9. RIGHT HAND (Skin)
  0.05f,  -0.11f, 0.0f,  // Top-Left
  0.07f,  -0.11f, 0.0f,  // Top-Right
  0.07f,  -0.13f, 0.0f,  // Bottom-Right
  0.05f,  -0.13f, 0.0f,  // Bottom-Left

  // 10. LEFT LEG (Shorter)
  -0.04f, -0.16f, 0.0f, // Top-Left 
  -0.01f, -0.16f, 0.0f, // Top-Right 
  -0.01f, -0.26f, 0.0f, // Bottom-Right 
  -0.04f, -0.26f, 0.0f, // Bottom-Left

  // 11. RIGHT LEG (Shorter)
   0.01f, -0.16f, 0.0f, // Top-Left 
   0.04f, -0.16f, 0.0f, // Top-Right 
   0.04f, -0.26f, 0.0f, // Bottom-Right
   0.01f, -0.26f, 0.0f, // Bottom-Left

   // 12. LEFT SHOE 
   -0.04f, -0.26f, 0.0f, // Top-Left
   -0.01f, -0.26f, 0.0f, // Top-Right
   -0.01f, -0.28f, 0.0f, // Bottom-Right
   -0.05f, -0.28f, 0.0f, // Bottom-Left

   // 13. RIGHT SHOE 
    0.01f, -0.26f, 0.0f, // Top-Left
    0.04f, -0.26f, 0.0f, // Top-Right
    0.05f, -0.28f, 0.0f, // Bottom-Right
    0.01f, -0.28f, 0.0f, // Bottom-Left
};

GLfloat boyColors[] = {
    // 1. HEAD (Skin Tone)
    0.96f, 0.73f, 0.58f,  // Top-Left
    0.96f, 0.73f, 0.58f,  // Top-Right
    0.78f, 0.6f, 0.55f,  // Bottom-Right
    0.78f, 0.6f, 0.55f,  // Bottom-Left

    // 3. HAIR (BACK LAYER - Same Dark Hair Color)
    0.2f, 0.1f, 0.2f,   // Top-Left
    0.2f, 0.1f, 0.2f,   // Top-Right
    0.3f, 0.2f, 0.3f,   // Bottom-Right
    0.3f, 0.2f, 0.3f,   // Bottom-Left

    // 2. HAIR (TOP VOLUME - Dark Purple/Brown)
    0.2f, 0.1f, 0.2f,   // Top-Left
    0.2f, 0.1f, 0.2f,   // Top-Right
    0.2f, 0.1f, 0.2f,   // Bottom-Right
    0.2f, 0.1f, 0.2f,   // Bottom-Left

    // 4. NECK (Skin Tone)
    0.76f, 0.63f, 0.58f,  // Top-Left
    0.76f, 0.63f, 0.58f,  // Top-Right
    0.96f, 0.73f, 0.58f,  // Bottom-Right
    0.96f, 0.73f, 0.58f,  // Bottom-Left

    // 5. BODY (Maroon/Reddish-Purple Shirt)
    0.49f, 0.22f, 0.35f,  // Top-Left
    0.69f, 0.22f, 0.35f,  // Top-Right
    0.49f, 0.22f, 0.35f,  // Bottom-Right
    0.69f, 0.22f, 0.35f,  // Bottom-Left

    // 6. LEFT ARM (Maroon/Reddish-Purple Shirt)
    0.69f, 0.22f, 0.35f,  // Top-Left
    0.49f, 0.22f, 0.35f,  // Top-Right
    0.49f, 0.22f, 0.35f,  // Bottom-Right
    0.69f, 0.22f, 0.35f,  // Bottom-Left

    // 7. RIGHT ARM (Maroon/Reddish-Purple Shirt)
    0.49f, 0.22f, 0.35f,  // Top-Left
    0.69f, 0.22f, 0.35f,  // Top-Right
    0.69f, 0.22f, 0.35f,  // Bottom-Right
    0.49f, 0.22f, 0.35f,  // Bottom-Left

    // 8. LEFT HAND (Skin Tone)
    0.96f, 0.73f, 0.58f,  // Top-Left
    0.96f, 0.73f, 0.58f,  // Top-Right
    0.96f, 0.73f, 0.58f,  // Bottom-Right
    0.96f, 0.73f, 0.58f,  // Bottom-Left

    // 9. RIGHT HAND (Skin Tone)
    0.96f, 0.73f, 0.58f,  // Top-Left
    0.96f, 0.73f, 0.58f,  // Top-Right
    0.96f, 0.73f, 0.58f,  // Bottom-Right
    0.96f, 0.73f, 0.58f,  // Bottom-Left

    // 10. LEFT LEG (Blue Jeans)
    0.2f, 0.25f, 0.4f,   // Top-Left
    0.3f, 0.45f, 0.6f,   // Top-Right
    0.2f, 0.25f, 0.4f,   // Bottom-Right
    0.3f, 0.45f, 0.6f,   // Bottom-Left

    // 11. RIGHT LEG (Blue Jeans)
    0.3f, 0.45f, 0.6f,   // Top-Left
    0.2f, 0.25f, 0.4f,   // Top-Right
    0.3f, 0.45f, 0.6f,   // Bottom-Right
    0.2f, 0.25f, 0.4f,   // Bottom-Left

    // 12. LEFT SHOE (Dark Red/Purple)
    0.4f, 0.15f, 0.2f,   // Top-Left
    0.4f, 0.15f, 0.2f,   // Top-Right
    0.4f, 0.15f, 0.2f,   // Bottom-Right
    0.4f, 0.15f, 0.2f,   // Bottom-Left

    // 13. RIGHT SHOE (Dark Red/Purple)
    0.4f, 0.15f, 0.2f,   // Top-Left
    0.4f, 0.15f, 0.2f,   // Top-Right
    0.4f, 0.15f, 0.2f,   // Bottom-Right
    0.4f, 0.15f, 0.2f,   // Bottom-Left
};

void initVBOs() {
    glGenBuffers(2, boyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, boyVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boyVertices), boyVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, boyVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boyColors), boyColors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void displayBoy() {
    //Overall Transformation for the Boy Object
    glPushMatrix();

    // Position the boy on the screen
    glTranslatef(boyPosX, boyPosY, 0.0f);

    // Scale the boy to make him appear bigger/closer
    glScalef(boyScale, boyScale, 1.0f);

    // VBO setup (unchanged)
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, boyVBO[0]);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, boyVBO[1]);
    glColorPointer(3, GL_FLOAT, 0, 0);

    // --- 1. Draw Body/Arms/Head (first 9 Quads = 36 Vertices) ---
    glDrawArrays(GL_QUADS, 0, 36);

    // --- 2. Draw LEFT LEG and LEFT SHOE (Continuous Rotation) ---
    glPushMatrix();
    // Translate to the hip pivot point
    glTranslatef(-0.025f, -0.16f, 0.0f);
    //Only apply rotation if the boy is running
    if (isBoyRunning) {
        glRotatef(legRotateAngle, 0.0f, 0.0f, 1.0f);
    }
    // Translate back
    glTranslatef(0.025f, 0.16f, 0.0f);

    // Draw LEFT LEG and LEFT SHOE (indices 36 & 44)
    glDrawArrays(GL_QUADS, 36, 4);
    glDrawArrays(GL_QUADS, 44, 4);
    glPopMatrix();

    // --- 3. Draw RIGHT LEG and RIGHT SHOE (Continuous Rotation) ---
    glPushMatrix();
    // Translate to the hip pivot point
    glTranslatef(0.025f, -0.16f, 0.0f);
    // Apply SAME continuous rotation (since it's a wheel effect)
    if (isBoyRunning) {
        // Apply SAME continuous rotation (since it's a wheel effect)
        glRotatef(legRotateAngle, 0.0f, 0.0f, 1.0f);
    }
    // Translate back
    glTranslatef(-0.025f, 0.16f, 0.0f);

    // Draw RIGHT LEG and RIGHT SHOE (indices 40 & 48)
    glDrawArrays(GL_QUADS, 40, 4);
    glDrawArrays(GL_QUADS, 48, 4);
    glPopMatrix();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glPopMatrix(); // Restore the matrix state before overall Boy transformations
}

void displayBirds() {
    glColor3fv(BIRD_COLOR);
    GLfloat flockCenterY = 0.8f;
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < NUM_BIRDS; ++i) {
        GLfloat xOffset = birdOffsets[i * 2];
        GLfloat yOffset = birdOffsets[i * 2 + 1];
        GLfloat birdX = birdFlock_X + xOffset;
        GLfloat birdY = flockCenterY + yOffset;
        glVertex3f(birdX - BIRD_SCALE * 1.5f, birdY, 0.0f);
        glVertex3f(birdX, birdY + BIRD_SCALE, 0.0f);
        glVertex3f(birdX - BIRD_SCALE * 0.5f, birdY + BIRD_SCALE * 0.5f, 0.0f);

        glVertex3f(birdX + BIRD_SCALE * 1.5f, birdY, 0.0f);
        glVertex3f(birdX, birdY + BIRD_SCALE, 0.0f);
        glVertex3f(birdX + BIRD_SCALE * 0.5f, birdY + BIRD_SCALE * 0.5f, 0.0f);
    }
    glEnd();
}

// ----------------------------------------------------------------
// ATOMIC BREATH
// ----------------------------------------------------------------
GLfloat fireVertices[] = {
    // 1. START POINT
    0.44f,   0.47f,   0.0f,   // Center/Base
    // 2. MOUTH FLARING
    0.44f,   0.57f,   0.0f,   // Top corner
    // 3. THE JAGGED BEAM
    -0.50f, 0.82f,  0.0f,   // Far Left
    -0.35f, 0.62f,  0.0f,   // Mid Left
    -0.70f, 0.47f,  0.0f,   // Furthest Left
    -0.35f, 0.42f,  0.0f,   // Mid Left
    // 4. BOTTOM FLARE
    -0.50f, 0.27f,  0.0f,   // Far Left
};

GLfloat fireColors[] = {
    0.4f, 0.5f, 0.9f,        // Pale Blue)
    0.05f, 0.1f, 0.4f,       // Dark Deep Blue
    0.1f, 0.2f, 0.6f,        // Mid Blue
    0.05f, 0.1f, 0.4f,       // Deep Blue
    0.05f, 0.1f, 0.4f,       // Deep Blue
    0.1f, 0.2f, 0.6f,        // Mid Blue
    0.05f, 0.1f, 0.4f,       // Deep Blue
};

void displayFire() {
    // 1. ENABLE LIGHTING
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    // 2. CONFIGURE THE LIGHT (Atomic Blue Glow)
    // Position: Mid-air in the path of the beam 
    GLfloat lightPos[] = { -0.2f, 0.6f, 0.5f, 1.0f };

    GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.3f, 1.0f };  // Deep Blue ambient
    GLfloat lightDiffuse[] = { 0.4f, 0.8f, 1.0f, 1.0f };  // Cyan/White main light
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Bright white highlights

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // 3. CONFIGURE MATERIAL (Glowing Effect)
    // Emission allows the object to glow without an external light source
    GLfloat matEmission[] = { 0.2f, 0.4f, 0.9f, 1.0f };
    GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat matShininess[] = { 50.0f };

    glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    // vertex color array (fireColors) to tint the light
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // 4. SET NORMAL (Face the camera)
    glNormal3f(0.0f, 0.0f, 1.0f);

    // 5. DRAW THE BEAM 
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, fireVertices);
    glColorPointer(3, GL_FLOAT, 0, fireColors);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArrays(GL_POLYGON, 0, 7);

    glDisable(GL_BLEND);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // 6. CLEANUP (Reset Emission and Disable Lighting)
    GLfloat noEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

void animateFlock(int value) {
    if (value != TIMER_ID_FLIGHT) return;
    if (birdFlockDirection != 0) {
        // Update the position: decreases X when direction is -1 (Left), increases when 1 (Right)
        birdFlock_X += birdFlockDirection * BIRD_FLOCK_SPEED;

        // Boundary Check: Loop the birds back when they go off-screen
        if (birdFlock_X < -1.0f) {
            // When birds disappear far left, reposition them far right
            birdFlock_X = 1.2f;
        }
        else if (birdFlock_X > 1.2f) {
            // When birds disappear far right, reposition them far left
            birdFlock_X = -1.0f;
        }
        glutPostRedisplay();
    }

    //16 == 60fps
    glutTimerFunc(16, animateFlock, TIMER_ID_FLIGHT);
}

void birdMovement(int key, int x, int y) {
    // Variable to store the intended new direction
    int newDirection = 0;
    if (key == GLUT_KEY_LEFT) {
        newDirection = -1;
    }
    else if (key == GLUT_KEY_RIGHT) {
        newDirection = 1;
    }
    // Only proceed if a valid directional key was pressed
    if (newDirection != 0) {
        //CHECK FOR INITIAL SPAWN: 
        if (birdFlockDirection == 0) {
            // Set the direction
            birdFlockDirection = newDirection;
            // Set the off-screen spawn point based on the direction
            if (birdFlockDirection == -1) {
                // Fly Left: Spawn off-screen right (e.g., 1.2f)
                birdFlock_X = 1.2f;
            }
            else if (birdFlockDirection == 1) {
                // Fly Right: Spawn off-screen left (e.g., -1.2f)
                birdFlock_X = -1.2f;
            }

        }
        else {
            // 2. MID-FLIGHT CHANGE: If the birds are already flying, just update direction.
            birdFlockDirection = newDirection;
        }
    }
    // Request a redraw immediately to start or change the visual movement
    glutPostRedisplay();
}

// ----------------------------------------------------------------
// 8 SMALL MOUNTAIN SILHOUETTES (Vertex Arrays + Elements)
// ----------------------------------------------------------------
// -------------------------------------------------------------
// CONNECTED MOUNTAIN RIDGE (smooth silhouette from left to right)
// -------------------------------------------------------------
GLfloat ridgeVertices[] = {
    // --- Left to Right Peaks ---
    -1.10f, -0.25f, 0.0f,   // 0 Base Left
    -0.95f, -0.12f, 0.0f,   // 1 Peak 1
    -0.75f, -0.25f, 0.0f,   // 2 Valley 1
    -0.62f, -0.08f, 0.0f,   // 3 Peak 2
    -0.48f, -0.25f, 0.0f,   // 4 Valley 2
    -0.38f, -0.16f, 0.0f,   // 5 Peak 3
    -0.25f, -0.25f, 0.0f,   // 6 Valley 3
    -0.10f, -0.10f, 0.0f,   // 7 Peak 4
     0.08f, -0.25f, 0.0f,   // 8 Valley 4
     0.25f, -0.14f, 0.0f,   // 9 Peak 5
     0.42f, -0.25f, 0.0f,   //10 Valley 5
     0.58f, -0.09f, 0.0f,   //11 Peak 6
     0.72f, -0.25f, 0.0f,   //12 Valley 6
     0.85f, -0.16f, 0.0f,   //13 Peak 7
     1.15f, -0.25f, 0.0f    //14 Base Right
};

GLfloat ridgeColors[] = {
    // Base - deep distant blue
    0.18f, 0.23f, 0.35f, 0.5f,

    // Peak 1 - snowy highlight (dimmed)
    0.50f, 0.58f, 0.88f, 0.5f,
    0.18f, 0.23f, 0.65f, 0.5f,

    // Peak 2
    0.62f, 0.70f, 0.80f, 0.5f,
    0.18f, 0.23f, 0.65f, 0.5f,

    // Peak 3
    0.54f, 0.62f, 0.72f, 0.5f,
    0.18f, 0.23f, 0.35f, 0.5f,

    // Peak 4 (taller)
    0.66f, 0.74f, 0.84f, 0.5f,
    0.18f, 0.23f, 0.85f, 0.5f,

    // Peak 5
    0.63f, 0.71f, 0.81f, 0.5f,
    0.18f, 0.23f, 0.55f, 0.5f,

    // Peak 6
    0.64f, 0.72f, 0.82f, 0.5f,
    0.18f, 0.23f, 0.35f, 0.5f,

    // Peak 7
    0.52f, 0.60f, 0.70f, 0.5f,
    0.18f, 0.23f, 0.45f, 0.5f
};

GLubyte ridgeIndices[] = {
    // Top ridge outline
    0,  1,
    2,  3,
    4,  5,
    6,  7,
    8,  9,
    10, 11,
    12, 13,
    14, // end

    // Close shape down to the bottom
    14,  // last top vertex
    0    // back to first base vertex
};

void displayMountains() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // --- Draw original mountains ---
    glVertexPointer(3, GL_FLOAT, 0, ridgeVertices);
    glColorPointer(4, GL_FLOAT, 0, ridgeColors);
    glDrawElements(GL_TRIANGLE_STRIP, sizeof(ridgeIndices), GL_UNSIGNED_BYTE, ridgeIndices);

    // --- Draw squished reflection ---
    glPushMatrix();
    // Move to water level first
    glTranslatef(0.07f, -0.375f, 0.0f);
    // Mirror and squish vertically (Y-axis)
    glScalef(1.0f, -0.5f, 1.0f);

    // Dimmed colors for reflection
    GLfloat reflectionColors[15 * 4];
    for (int i = 0; i < 15 * 4; i++) {
        reflectionColors[i] = ridgeColors[i] * 0.1f; // darker & semi-transparent
    }

    // Draw reflection using same vertices
    glVertexPointer(3, GL_FLOAT, 0, ridgeVertices);
    glColorPointer(4, GL_FLOAT, 0, reflectionColors);
    glDrawElements(GL_TRIANGLE_STRIP, sizeof(ridgeIndices), GL_UNSIGNED_BYTE, ridgeIndices);

    glPopMatrix(); // restore original matrix so mountains are not deformed

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glDisable(GL_BLEND);
}

// ----------------------------------------------------------------
// TIMER FUNCTION (BOY ANIMATION)
// ----------------------------------------------------------------
void animateBoyRun(int value) {
    glutTimerFunc(30, animateBoyRun, 0);

    if (isBoyRunning) {
        //1. Horizontal Movement (Running Across Screen)
        // Move from center (0.0f) to the left (-1.0f)
        if (boyPosX > -1.2f) {
            boyPosX -= 0.02f;
        }
        else {
            // Loop the movement
            boyScale = 0.0f;
            isBoyRunning = false;
            glutPostRedisplay(); // Draw the final (invisible) frame
            return; // Exit the function to prevent further updates
        }

        // 2. Scaling (Approaching the User/Getting Bigger) 
        // Scale from 1.0f up to 3.0f (stop when approaching)
        if (boyScale < 3.0f) {
            boyScale += 0.02f;
        }
        else {
            boyScale = 3.0f; // Stop scaling
        }

        // --- 3. Cartoon Leg Rotation (Continuous) ---
        legRotateAngle -= 100.0f;

        glutPostRedisplay();
    }
}

void drawBoyText() {
    // Only draw the text if the boy is currently running
    if (isBoyRunning) {
        glColor3f(1.0f, 1.0f, 1.0f); // Set text color to red
        glRasterPos3f(boyPosX - -0.1f, - 0.3f, 0.0f);
        const char* text = "HELPPPPP!!";
        // Loop through the string and draw each character
        for (int i = 0; text[i] != '\0'; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
        }
    }
}
