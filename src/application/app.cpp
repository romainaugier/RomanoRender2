#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include <GL/glew.h>            // Initialize with gl3wInit()

#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

#include "application/app.h"

#define FLYTHROUGH_CAMERA_IMPLEMENTATION
#include "scene/flythrough_camera.h"

int application(int argc, char** argv)
{
    // Setup embree scene
    const std::string path = "D:/dev/Utils/Models/dragon_2_scene.obj";

    // Setup OSL
    RomanoRenderer renderer;
    OSL::ErrorHandler oslErrHandler;

    OSL::ShadingSystem* oslShadingSys = new OSL::ShadingSystem(&renderer, nullptr, &oslErrHandler);
    oslShadingSys->attribute("allow_shader_replacement", 1);
    OSL::ustring outputs[] = { OSL::ustring("Cout") };
    oslShadingSys->attribute("renderer_outputs", OSL::TypeDesc(OSL::TypeDesc::STRING, 1), &outputs);

    // Registering closures in the OSL shading system
    OSL::RegisterClosures(oslShadingSys);

    // Load shaders
    std::vector<OSL::ShaderGroupRef> shaders;

    OSL::ShaderGroupRef group = oslShadingSys->ShaderGroupBegin("test");
    oslShadingSys->Shader(*group, "surface", "D:/dev/RomanoRender2/src/shaders/uv.oso", nullptr);
    oslShadingSys->ShaderGroupEnd(*group);

    shaders.push_back(group);

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    constexpr int xres = 1280;
    constexpr int yres = 720;

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(xres, yres, "RomanoRender++", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // Enable vsync

    // Initialize OpenGL loader
    bool err = glewInit() != 0;

    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGuiStyle* style = &ImGui::GetStyle();

    style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.06f, 0.05f, 0.04f, 0.00f);


    Camera cam(embree::Vec3f(0.0f, 30.0f, 100.0f), embree::Vec3f(0.0f, 0.0f, 0.0f), 85, xres, yres);
    cam.SetTransform();

    Settings settings;
    settings.xres = xres;
    settings.yres = yres;


    color* accumBuffer = new color[xres * yres];
    color* renderBuffer = new color[xres * yres];

    Tiles tiles;
    GenerateTiles(tiles, settings);
    
    GLuint render_view_texture;

    glGenTextures(1, &render_view_texture);
    glBindTexture(GL_TEXTURE_2D, render_view_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xres, yres, 0, GL_RGB, GL_FLOAT, renderBuffer);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint myFbo{};
    glGenFramebuffers(1, &myFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, myFbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_view_texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Shader shader;

    uint32_t samples = 1;
    bool edited = 0;
    bool render = false;
    static bool drawBvh = false;
    static bool doTiles = true;
    float elapsedBvh = 0.0f;
    float elapsed = 0.0f;
    float renderSeconds = 0.0f;

    // Flythrough camera
    float pos[3] = { 0.0f, 0.0f, 0.0f };
    float look[3] = { 0.0f, 0.0f, 1.0f };
    const float up[3] = { 0.0f, 1.0f, 0.0f };

    LARGE_INTEGER then, now, freq;
    QueryPerformanceCounter(&freq);
    QueryPerformanceCounter(&then);

    POINT oldCursor;
    GetCursorPos(&oldCursor);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Reset the render accumulation buffer if something has been edited
        if (edited)
        {
            samples = 1;
            renderSeconds = 0.0f;
            // memset(renderBuffer, 0.0f, xres * yres * sizeof(color));
            edited = false;
        }

        // Update flythrough camera
        QueryPerformanceCounter(&now);
        float delta_time_sec = (float)(now.QuadPart - then.QuadPart) / freq.QuadPart;

        POINT cursor;
        GetCursorPos(&cursor);

        float activated = GetAsyncKeyState(VK_RBUTTON) ? 1.0f : 0.0f;

        if (activated) edited = true;

        float view[16];
        flythrough_camera_update(
            pos, look, up, view,
            delta_time_sec,
            1000000.0f * (GetAsyncKeyState(VK_LSHIFT) ? 2.0f : 1.0f) * activated,
            0.5f * activated,
            80.0f,
            cursor.x - oldCursor.x, cursor.y - oldCursor.y,
            GetAsyncKeyState('Z'), GetAsyncKeyState('Q'), GetAsyncKeyState('S'), GetAsyncKeyState('D'),
            GetAsyncKeyState(VK_SPACE), GetAsyncKeyState(VK_LCONTROL),
            0);

        cam.pos = embree::Vec3f(pos[0], pos[1], pos[2]);

        cam.SetTransformFromCam(mat44(view[0], view[1], view[2], view[3],
            view[4], view[5], view[6], view[7],
            view[8], view[9], view[10], view[11],
            view[12], view[13], view[14], view[15]));


        ////////////////////////////////

        glfwPollEvents();
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        if(render)
        {
            /*auto start = get_time();

            RenderTiles(embreeScene, oslShadingSys, renderBuffer, objects, shaders, samples, ImGui::GetFrameCount(), tiles, cam, settings);

            auto end = get_time();

            elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();*/

            renderSeconds += elapsed;

            glBindTexture(GL_TEXTURE_2D, render_view_texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, xres, yres, GL_RGB, GL_FLOAT, renderBuffer);
            glBindTexture(GL_TEXTURE_2D, 0);

        }
        
        glBindFramebuffer(GL_READ_FRAMEBUFFER, myFbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // default fbo  
        glBlitFramebuffer(0, 0, xres, yres,
                          0, display_h, display_w, 0, GL_COLOR_BUFFER_BIT, GL_LINEAR); // or GL_NEAREST

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        // Info Window
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::Begin("Debug");
        {
            ImGui::Text("FPS : %0.3f", ImGui::GetIO().Framerate);
            ImGui::Text("Frame time : %0.3f ms", elapsed);
            ImGui::Text("Render time : %0.1f s", renderSeconds / 1000.0f);
            ImGui::Text("Samples : %d", samples);
            if (ImGui::Button("Render"))
            {
                if (render) render = false;
                else render = true;
            }
            ImGui::End();
        }
        ImGui::PopStyleColor();

        if (render)
        {
            samples++;
            then = now;
            oldCursor = cursor;
        }

        if (show_demo_window) ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ImGui::EndFrame();
        
        glfwSwapBuffers(window);
    }

    ReleaseTiles(tiles);

    delete[] accumBuffer;
    delete[] renderBuffer;

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}