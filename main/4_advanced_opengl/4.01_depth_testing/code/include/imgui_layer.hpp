#ifndef IMGUI_LAYER_HPP_NNY5AQ84
#define IMGUI_LAYER_HPP_NNY5AQ84

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "common/old/window.hpp"
#include "common/old/stringified_enum.hpp"
#include "common/old/scope_time_logger.hpp"

#include "scene.hpp"

struct GlslVersion
{
    int major;
    int minor;
};

class ImGuiLayer
{
private:
#define ENUM_FIELDS(M)             \
    M(SHOW_MAIN_WINDOW)            \
    M(SHOW_SCOPE_TIMER_LOG_WINDOW) \
    M(SHOW_OVERLAY_WINDOW)
    using MyImGuiWindowShown = STRINGIFIED_ENUM_FLAG(MyImGuiWindowShown, int, ENUM_FIELDS);
#undef ENUM_FIELDS

    enum class MyImGuiSortBy
    {
        NO_SORT,
        RUN_TIME,
        THREAD_ID,
    };

    enum class MyImGuiOverlayPos
    {
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT,
    };

    class MyImGuiWindowOpenHelper
    {
    private:
        std::optional<std::reference_wrapper<MyImGuiWindowShown>> m_flags;
        MyImGuiWindowShown::Enum                                  m_flagValue;
        bool                                                      m_open;

    public:
        MyImGuiWindowOpenHelper(
            const char* const name,
            ImGuiWindowFlags  windowFlags = ImGuiWindowFlags_None
        )
            : m_open{ true }
        {
            ImGui::Begin(name, &m_open, windowFlags);
        }

        MyImGuiWindowOpenHelper(
            const char* const          name,
            MyImGuiWindowShown&        flags,
            MyImGuiWindowShown::Enum&& flagValue,
            ImGuiWindowFlags           windowFlags = ImGuiWindowFlags_None
        )
            : m_flags{ flags }
            , m_flagValue{ std::move(flagValue) }
            , m_open{ flags.test(std::move(flagValue)) }
        {
            ImGui::Begin(name, &m_open, windowFlags);
        }

        ~MyImGuiWindowOpenHelper()
        {
            ImGui::End();
            if (m_flags.has_value()) { m_flags->get().setToValue(m_open, std::move(m_flagValue)); }
        }

        operator bool() const { return m_open; }
    };

private:
    window::Window&   m_window;
    Scene&            m_scene;
    ImGuiContext*     m_imguiContext;
    const GlslVersion m_glslVersion;

    ImGuiIO*           m_imguiIo;
    MyImGuiWindowShown m_windowShown{ MyImGuiWindowShown::SHOW_OVERLAY_WINDOW };
    MyImGuiSortBy      m_sortBy{ MyImGuiSortBy::NO_SORT };
    MyImGuiOverlayPos  m_overlayPosition{ MyImGuiOverlayPos::TOP_LEFT };

    struct LogData
    {
        std::map<std::string, std::tuple<double, std::size_t, int>>      m_data_accumulate;
        std::vector<std::tuple<std::string, double, std::size_t, float>> m_data_shown_active;
        std::vector<std::tuple<std::string, double, std::size_t>>        m_data_shown_inactive;
        int                                                              m_counter{ 0 };
        float                                                            m_sum{ 0 };
    } m_logData;

public:
    ImGuiLayer(window::Window& window, Scene& scene, const GlslVersion& glslVersion = { 3, 3 })
        : m_window{ window }
        , m_scene{ scene }
        , m_glslVersion{ glslVersion }
    {
        IMGUI_CHECKVERSION();
        m_imguiContext = ImGui::CreateContext();
        m_imguiIo      = &ImGui::GetIO();
        // m_imguiIo->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    // Enable Keyboard Controls
        // m_imguiIo->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer bindings
        m_window.useHere();
        ImGui_ImplGlfw_InitForOpenGL(m_window.getHandle(), true);
        auto glslVersionString{ std::format("#version {}{}0 core", m_glslVersion.major, m_glslVersion.minor) };
        ImGui_ImplOpenGL3_Init(glslVersionString.c_str());
        m_window.unUse();

        // setup actual window key mapping
        m_window
            .addKeyEventHandler(GLFW_KEY_M, GLFW_MOD_ALT, window::Window::KeyActionType::CALLBACK, [this](window::Window&) {
                m_windowShown.toggle(MyImGuiWindowShown::SHOW_MAIN_WINDOW);
            })
            .addKeyEventHandler(GLFW_KEY_L, GLFW_MOD_ALT, window::Window::KeyActionType::CALLBACK, [this](window::Window&) {
                m_windowShown.toggle(MyImGuiWindowShown::SHOW_SCOPE_TIMER_LOG_WINDOW);
            });
    }

    ~ImGuiLayer()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

public:
    void render()
    {
        PRETTY_FUNCTION_TIME_LOG();

        using enum MyImGuiWindowShown::Enum;

        // ImGui::SetCurrentContext(m_imguiContext);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // draw imgui stuff
        if (m_windowShown.test(SHOW_MAIN_WINDOW)) { showMainWindow(); }
        if (m_windowShown.test(SHOW_SCOPE_TIMER_LOG_WINDOW)) { showScopeTimerLogWindow(); }
        if (m_windowShown.test(SHOW_OVERLAY_WINDOW)) { showOverlayWindow(); }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

private:
    void showMainWindow()
    {
        MyImGuiWindowOpenHelper helper{ "Main", m_windowShown, MyImGuiWindowShown::SHOW_MAIN_WINDOW };

        bool vsync{ m_window.isVsyncEnabled() };
        if (ImGui::Checkbox("vsync", &vsync)) {
            m_window.setVsync(vsync);
        }

        bool color{ m_scene.u_enableColorOutput.m_value };
        if (ImGui::Checkbox("color", &color)) {
            m_scene.setColorOutput(color);
        }

        bool depth{ m_scene.u_enableDepthOutput.m_value };
        if (ImGui::Checkbox("depth", &depth)) {
            m_scene.setDepthOutput(depth);
        }

        ImGui::Separator();
        bool invertDepth{ m_scene.u_invertDepthOutput.m_value };
        if (ImGui::Checkbox("invert depth", &invertDepth)) {
            m_scene.invertDepthOutput(invertDepth);
        }

        ImGui::Separator();

        ImGui::Text("windows:");
        for (const auto& [val, name] : MyImGuiWindowShown::s_enums) {
            if (val == MyImGuiWindowShown::SHOW_MAIN_WINDOW) { continue; }
            ImGui::CheckboxFlags(name.c_str(), &m_windowShown.base(), val);
        }

        ImGui::Separator();

        ImGui::Text("activated lights:");
        for (auto& activatedLights{ m_scene.u_activatedLights.m_value }; const auto& [val, name] : activatedLights.s_enums) {
            ImGui::CheckboxFlags(name.c_str(), &activatedLights.base(), val);
        }

        ImGui::Separator();

        if (ImGui::TreeNode("colors")) {
            auto& clearColor{ m_scene.m_backgroundColor };
            ImGui::Text("clear color:");
            ImGui::ColorEdit3("##clear", &clearColor.r);

            ImGui::Separator();

            // directional light
            if (ImGui::TreeNode("directional light")) {
                auto& light{ m_scene.m_directionalLight };
                ImGui::ColorEdit3("amb##dir", &light.m_ambient.r);
                ImGui::ColorEdit3("diff##dir", &light.m_diffuse.r);
                ImGui::ColorEdit3("spec##dir", &light.m_specular.r);
                ImGui::TreePop();
            }

            ImGui::Separator();

            // point light
            if (ImGui::TreeNode("point light")) {
                auto light{ m_scene.m_pointLights[0] };    // all lights will be set to the same color
                ImGui::ColorEdit3("amb##point", &light.m_ambient.r);
                ImGui::ColorEdit3("diff##point", &light.m_diffuse.r);
                ImGui::ColorEdit3("spec##point", &light.m_specular.r);
                for (auto& l : m_scene.m_pointLights) {
                    l.m_ambient  = light.m_ambient;
                    l.m_diffuse  = light.m_diffuse;
                    l.m_specular = light.m_specular;
                }    // inefficient but eh
                ImGui::TreePop();
            }

            ImGui::Separator();

            // spot light
            if (ImGui::TreeNode("spot light")) {
                auto& light{ m_scene.m_spotLight };    // all lights will be set to the same color
                ImGui::ColorEdit3("amb##spot", &light.m_ambient.r);
                ImGui::ColorEdit3("diff##spot", &light.m_diffuse.r);
                ImGui::ColorEdit3("spec##spot", &light.m_specular.r);

                ImGui::Separator();

                ImGui::SliderFloat("lin##spot", &light.m_linear, 0.001f, 1.0f);
                ImGui::SliderFloat("sqr##spot", &light.m_quadratic, 0.001f, 1.0f);
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    void showScopeTimerLogWindow()
    {
        MyImGuiWindowOpenHelper helper{ "Scope Timer Log", m_windowShown, MyImGuiWindowShown::SHOW_SCOPE_TIMER_LOG_WINDOW };

        auto& l{ m_logData };

        l.m_counter++;
        l.m_sum += (float)m_window.getDeltaTime();

        if (const auto& data{ util::ScopeTimeLogger::read(util::ScopeTimeLogger::ACTIVE_AND_INACTIVE) }; data.has_value()) {
            for (auto&& [name, time, threadId, status] : data.value()) {
                auto found{ l.m_data_accumulate.find(name) };
                if (found != l.m_data_accumulate.end()) {
                    auto& [oldTime, oldThreadId, oldStatus]{ found->second };
                    oldTime     += time;
                    oldThreadId  = threadId;    // replace
                    oldStatus   += status;
                } else {
                    l.m_data_accumulate[name] = { time, threadId, status };
                }
            }
        }

        // clang-format off
        ImGui::Text("sort:");                                                            ImGui::SameLine();
        ImGui::RadioButton("no",        (int*)&m_sortBy, (int)MyImGuiSortBy::NO_SORT);   ImGui::SameLine();
        ImGui::RadioButton("run_time",  (int*)&m_sortBy, (int)MyImGuiSortBy::RUN_TIME);  ImGui::SameLine();
        ImGui::RadioButton("thread_id", (int*)&m_sortBy, (int)MyImGuiSortBy::THREAD_ID); ImGui::Separator();
        // clang-format on

        constexpr float timeInterval{ 0.250f };
        if (l.m_sum >= timeInterval) {
            l.m_data_shown_active.clear();
            l.m_data_shown_inactive.clear();

            for (auto&& [name, data] : l.m_data_accumulate) {
                auto&& [time, threadId, activity]{ data };
                if (activity != 0) {
                    l.m_data_shown_active.emplace_back(name, time / l.m_counter, threadId, (float)activity / (float)l.m_counter);
                } else {
                    l.m_data_shown_inactive.emplace_back(name, time / l.m_counter, threadId);
                }
            }
            l.m_data_accumulate.clear();

            if (auto& sortBy{ m_sortBy }; sortBy != MyImGuiSortBy::NO_SORT) {
                std::sort(l.m_data_shown_active.begin(), l.m_data_shown_active.end(), [&](auto& lhs, auto& rhs) {
                    if (sortBy == MyImGuiSortBy::RUN_TIME) {
                        return std::get<1>(lhs) > std::get<1>(rhs);
                    } else {
                        return std::get<2>(lhs) > std::get<2>(rhs);
                    }
                });
                std::sort(l.m_data_shown_inactive.begin(), l.m_data_shown_inactive.end(), [&](auto& lhs, auto& rhs) {
                    if (sortBy == MyImGuiSortBy::RUN_TIME) {
                        return std::get<1>(lhs) > std::get<1>(rhs);
                    } else {
                        return std::get<2>(lhs) > std::get<2>(rhs);
                    }
                });
            }
        }

        if (util::ScopeTimeLogger::getInstance() != nullptr) {
            for (const auto& e : l.m_data_shown_active) {
                const auto& [name, time, threadId, activity]{ e };
                ImGui::Text("%.3fms | [%zu] %s (%d%%)", time, threadId, name.c_str(), int(activity * 100));
            }
            ImGui::Separator();
            for (const auto& e : l.m_data_shown_inactive) {
                const auto& [name, time, threadId]{ e };
                ImGui::Text("%.3fms | [%zu] %s (inactive)", time, threadId, name.c_str());
            }
        } else {
            ImGui::Text("logger not started");
        }

        if (l.m_sum >= timeInterval) {
            l.m_counter = 0;
            l.m_sum     = 0;
        }
    }

    void showOverlayWindow()
    {
        using enum MyImGuiOverlayPos;

        const bool isNearRight{ m_overlayPosition == TOP_RIGHT || m_overlayPosition == BOTTOM_RIGHT };
        const bool isNearBottom{ m_overlayPosition == BOTTOM_LEFT || m_overlayPosition == BOTTOM_RIGHT };

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
                                        | ImGuiWindowFlags_AlwaysAutoResize
                                        | ImGuiWindowFlags_NoSavedSettings
                                        | ImGuiWindowFlags_NoFocusOnAppearing
                                        | ImGuiWindowFlags_NoNav
                                        | ImGuiWindowFlags_NoMove;

        constexpr float      padding{ 10.0f };
        const ImGuiViewport* viewport{ ImGui::GetMainViewport() };
        const ImVec2&        work_pos{ viewport->WorkPos };    // Use work area to avoid menu-bar/task-bar, if any!
        const ImVec2&        work_size{ viewport->WorkSize };

        const ImVec2 window_pos{
            (isNearRight) ? (work_pos.x + work_size.x - padding) : (work_pos.x + padding),
            (isNearBottom) ? (work_pos.y + work_size.y - padding) : (work_pos.y + padding),
        };
        const ImVec2 window_pos_pivot{
            (isNearRight) ? 1.0f : 0.0f,
            (isNearBottom) ? 1.0f : 0.0f
        };

        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowBgAlpha(0.35f);    // Transparent background

        {
            MyImGuiWindowOpenHelper helper{ "Overlay", window_flags };

            ImGui::Text("main window: %s", m_windowShown.test(MyImGuiWindowShown::SHOW_MAIN_WINDOW) ? "shown" : "hidden");
            ImGui::Separator();

            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / m_imguiIo->Framerate, m_imguiIo->Framerate);
            ImGui::Separator();

            const auto& camPos{ m_scene.m_camera.m_position };
            const auto& camDir{ m_scene.m_camera.m_front };
            ImGui::Text("camera pos: (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);
            ImGui::Text("camera dir: (%.2f, %.2f, %.2f)", camDir.x, camDir.y, camDir.z);
            ImGui::Text("cam yaw   : %.2f", m_scene.m_camera.m_yaw);
            ImGui::Text("cam pitch : %.2f", m_scene.m_camera.m_pitch);

            const auto& pos{ m_overlayPosition };
            if (ImGui::BeginPopupContextWindow()) {
                ImGui::MenuItem("Top-left", NULL, pos == TOP_LEFT);
                ImGui::MenuItem("Top-right", NULL, pos == TOP_RIGHT);
                ImGui::MenuItem("Bottom-left", NULL, pos == BOTTOM_LEFT);
                ImGui::MenuItem("Bottom-right", NULL, pos == BOTTOM_RIGHT);
                ImGui::EndPopup();
            }
        }
    }
};

#endif /* end of include guard: IMGUI_LAYER_HPP_NNY5AQ84 */
