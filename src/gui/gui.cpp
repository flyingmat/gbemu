#include "gui.hpp"

namespace Gui {

    SDL_Window* window = nullptr;
    ImGuiIO* p_io = nullptr;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool InitInterface() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
            printf("Error: %s\n", SDL_GetError());
            return 1;
        }

        const char* glsl_version = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
        window = SDL_CreateWindow("gbemu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256, 128, window_flags);
        if (window == NULL) {
            printf("Error: %s\n", SDL_GetError());
            return 1;
        }
        SDL_GLContext gl_context = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, gl_context);

        if (gl3wInit() != 0) {
            printf("Failed to initialize gl3w!\n");
            return 1;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void) io;
        p_io = &io;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void ImGuiFrameRender(Cpu::Cpu* const cpu, uint8_t* const memory, std::shared_ptr<Cpu::Operations::Instruction> instruction) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        bool p_open = true;
        ImGui::Begin("debug", &p_open, 0);

        std::stringstream hexss;

        std::string AF = std::bitset<16>(Cpu::Helpers::JoinBytes(cpu->A, cpu->F)).to_string();
        std::string BC = std::bitset<16>(Cpu::Helpers::JoinBytes(cpu->B, cpu->C)).to_string();
        std::string DE = std::bitset<16>(Cpu::Helpers::JoinBytes(cpu->D, cpu->E)).to_string();
        std::string HL = std::bitset<16>(Cpu::Helpers::JoinBytes(cpu->H, cpu->L)).to_string();
        std::string SP = std::bitset<16>(Cpu::Helpers::JoinBytes(cpu->S, cpu->P)).to_string();
        std::string PC = std::bitset<16>(cpu->PC).to_string();

        uint8_t opcode = 0;
        if (instruction != nullptr)
            opcode = instruction->opcode;
        hexss << std::hex << std::setfill('0') << std::setw(2) << std::bitset<8>(opcode).to_ulong();

        uint8_t argn = Cpu::Helpers::GetArgsNumber(opcode);
        for (uint8_t i = 0; i < argn; i++)
            hexss << " " <<  std::hex << std::setfill('0') << std::setw(2) << std::bitset<8>(instruction->args[i]).to_ulong();

        std::string instr = hexss.str();

        ImGui::Text("registers");
        ImGui::Text("AF: ");
        ImGui::SameLine();
        ImGui::Text(AF.c_str());
        ImGui::Text("BC: ");
        ImGui::SameLine();
        ImGui::Text(BC.c_str());
        ImGui::Text("DE: ");
        ImGui::SameLine();
        ImGui::Text(DE.c_str());
        ImGui::Text("HL: ");
        ImGui::SameLine();
        ImGui::Text(HL.c_str());
        ImGui::Text("");
        ImGui::Text("SP: ");
        ImGui::SameLine();
        ImGui::Text(SP.c_str());
        ImGui::Text("PC: ");
        ImGui::SameLine();
        ImGui::Text(PC.c_str());
        ImGui::Text("@ ");
        ImGui::SameLine();
        ImGui::Text(instr.c_str());

        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, (int)p_io->DisplaySize.x, (int)p_io->DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

        SDL_GL_SwapWindow(window);
    }

    bool ShouldDestroy(SDL_Event event) {
        if (event.type == SDL_QUIT)
            return true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            return true;

        return false;
    }

    void DestroyInterface() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(SDL_GL_GetCurrentContext());
        SDL_DestroyWindow(SDL_GL_GetCurrentWindow());
        SDL_Quit();
    }
}
