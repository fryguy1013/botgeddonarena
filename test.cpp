#include <GLFW/glfw3.h>

#include "ColorTriplet.h"
#include "SegmentLedState.h"
#include "ArenaStates.h"

#include <array>
#include <chrono>
#include <memory>

float tween(float t, float start, float end);

volatile bool redDown = false;
volatile bool blueDown = false;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == 'R')
        redDown = action == GLFW_PRESS;
    else if (key == 'B')
        blueDown = action == GLFW_PRESS;
}

void draw_segment(uint8_t segment)
{
    uint8_t led = SegmentLedState::GetSegmentDigits(segment);
    glLineWidth(5.0);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    if (led & (1 << 6)) {
        glVertex2d(0.1, 0.1);
        glVertex2d(0.9, 0.1);
    }
    if (led & (1 << 1)) {
        glVertex2d(0.1, 0.1);
        glVertex2d(0.1, 0.5);
    }
    if (led & (1 << 0)) {
        glVertex2d(0.1, 0.5);
        glVertex2d(0.9, 0.5);
    }
    if (led & (1 << 2)) {
        glVertex2d(0.1, 0.5);
        glVertex2d(0.1, 0.9);
    }
    if (led & (1 << 3)) {
        glVertex2d(0.1, 0.9);
        glVertex2d(0.9, 0.9);
    }
    if (led & (1 << 4)) {
        glVertex2d(0.9, 0.5);
        glVertex2d(0.9, 0.9);
    }
    if (led & (1 << 5)) {
        glVertex2d(0.9, 0.1);
        glVertex2d(0.9, 0.5);
    }
    glEnd();
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    std::unique_ptr<LedAnimation> state;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glTranslated(-1, 1, 0);
    glScaled(2, -2, 1);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        if (state == nullptr || state->IsAnimationComplete())
            state = GetAnimation(ArenaState::Staging);

        state->Tick(redDown, blueDown);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        auto colors = state->GetColors();

        glPushMatrix();
        glScalef(0.5f, 1.0f, 1.0f);
        for (int x = 0; x < LedAnimation::NumHorizontal; x++)
        {
            for (int y = 0; y < LedAnimation::NumVertical; y++)
            {
                glPushMatrix();
                glScaled(1.0 / LedAnimation::NumHorizontal, 1.0 / LedAnimation::NumVertical, 0.1);
                glTranslated(x, y, 0);

                auto color = colors[y + x * LedAnimation::NumVertical];
                glColor3b(color.r, color.g, color.b);
                glBegin(GL_QUADS);
                glVertex2d(0.4, 0.25);
                glVertex2d(0.4, 0.75);
                glVertex2d(0.6, 0.75);
                glVertex2d(0.6, 0.25);
                glEnd();
                glPopMatrix();
            }
        }
        glPopMatrix();

        for (int i = 0; i < 4; i++) {
            glPushMatrix();
            
            glScalef(0.25f, 1.0f, 1.0f);
            glTranslatef(static_cast<float>(i), 0.0f, 0.0f);

            auto timer = state->GetSegmentLed();
            draw_segment(timer.segments[i]);

            glPopMatrix();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}