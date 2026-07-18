#include "emulator.hpp"

Emulator::Emulator() {
    window = nullptr;
    renderer = nullptr;
    font = nullptr;
    gbTexture = nullptr;

    init();
}


Emulator::~Emulator() {
    TTF_CloseFont(font);    
    TTF_Quit();
    SDL_DestroyTexture(gbTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    gbTexture = NULL;
    window = NULL;
    renderer = NULL;

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_Quit();
}


void Emulator::init() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be intiialized, SDL error: %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow(emulatorTitle,
                    SDL_WINDOWPOS_CENTERED,
                    SDL_WINDOWPOS_CENTERED,
                    emulatorScreenWidth,
                    emulatorScreenHeight,
                    SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("error initializing window. SDL error: %s\n", SDL_GetError());
    }

    // renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // vsync (needs research)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL) {
        printf("error initializing renderer. SDL error: %s\n", SDL_GetError());
    }


    if (TTF_Init()) {
        printf("error initializing font, SDL error: %s\n", TTF_GetError());
    }

    font = TTF_OpenFont("/Users/poxford3/Documents/coding/cpp/gbemu/assets/arial/ARIAL.TTF", 24);
    if (font == NULL) {
        printf("error loading font: %s\n", TTF_GetError());
    }

    // Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    running = true;
}


SDL_Renderer* Emulator::GetRenderer() {
    return renderer;
}


void Emulator::createGameboyTextures() {
    if (gameboy.has_value()) {
        gbTexture = SDL_CreateTexture(
                            renderer,
                            SDL_PIXELFORMAT_RGB24,
                            SDL_TEXTUREACCESS_STREAMING,
                            gameboy->ppu.GAMEBOY_WIDTH,
                            gameboy->ppu.GAMEBOY_HEIGHT
                        );

        if (gbTexture == NULL) {
            printf("error initializing texture, SDL error %s\n", SDL_GetError());
        }

        tileDataTexture = SDL_CreateTexture(
                            renderer,
                            SDL_PIXELFORMAT_RGB24,
                            SDL_TEXTUREACCESS_STREAMING,
                            gameboy->ppu.TILEDATA_WIDTH,
                            gameboy->ppu.TILEDATA_HEIGHT
                        );

        if (tileDataTexture == NULL) {
            printf("error initializing texture, SDL error %s\n", SDL_GetError());
        }
    }
    SDL_SetWindowSize(window, gameboy->ppu.EMULATOR_SCREEN_WIDTH(), gameboy->ppu.EMULATOR_SCREEN_HEIGHT());
}

void Emulator::run() {
    while (running) {

        // imgui menu section
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // UI
        renderMenuBar();
        ImGui::Render();

        // events
        SDL_Event event;
    
        while (SDL_PollEvent(&event) != 0) {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_P) {
                    if (gameboy.has_value()) {
                        paused = !paused;
                    }
                } else if (event.key.keysym.scancode == SDL_SCANCODE_S) {
                    if (gameboy.has_value()) {
                        if (gameboy->ppu.palette.selectedPalette == PaletteOptions::GameboyGreen) {
                            gameboy->ppu.palette.selectedPalette = PaletteOptions::BlackWhite;
                        } else {
                            gameboy->ppu.palette.selectedPalette = PaletteOptions::GameboyGreen;
                        }
                    }
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        // gameboy rendering
        if (gameboy.has_value()) {
            if (!paused) {
                gameboy->runFrame();
                gameboy->ppu.loadTileData(gameboy->mmu);
            }

            uint gameboyWidth = gameboy->ppu.EMULATOR_SCREEN_WIDTH();
            uint gameboyHeight = gameboy->ppu.EMULATOR_SCREEN_HEIGHT();
            ImVec2 menuSize = ImGui::GetMainViewport()->WorkPos;

            if (showTileData) {
                int newWindowWidth = gameboy->ppu.EMULATOR_SCREEN_WIDTH() + gameboy->ppu.EMULATOR_TILEDATA_WIDTH();
                int newWindowHeight = gameboy->ppu.EMULATOR_TILEDATA_HEIGHT();
                SDL_SetWindowSize(window, newWindowWidth, newWindowHeight);

                // section off the tile data from the screen
                SDL_Rect separatorV = {static_cast<int>(gameboyWidth), 0, 1, static_cast<int>(gameboyHeight)};
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
                SDL_RenderFillRect(renderer, &separatorV);

                SDL_Rect tileDataSection = {static_cast<int>(gameboyWidth + 1), static_cast<int>(menuSize.y), static_cast<int>(gameboy->ppu.EMULATOR_TILEDATA_WIDTH()), static_cast<int>(gameboy->ppu.EMULATOR_TILEDATA_HEIGHT())};
                SDL_UpdateTexture(tileDataTexture, NULL, gameboy->ppu.tileData.data(), gameboy->ppu.TILEDATA_WIDTH * 3);
                SDL_RenderCopy(renderer, tileDataTexture, NULL, &tileDataSection);
            } else {
                SDL_SetWindowSize(window, gameboy->ppu.EMULATOR_SCREEN_WIDTH(), gameboy->ppu.EMULATOR_SCREEN_HEIGHT());
            }

            SDL_Rect gameboySection = {0, static_cast<int>(menuSize.y), static_cast<int>(gameboyWidth), static_cast<int>(gameboyHeight)};
            SDL_UpdateTexture(gbTexture, NULL, gameboy->ppu.frameBuffer.data(), gameboy->ppu.GAMEBOY_WIDTH * 3);
            SDL_RenderCopy(renderer, gbTexture, NULL, &gameboySection);
        }

        // Draw ImGui
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // 16 ms = 60 fps
    }
}


void Emulator::handleInput() {
    const Byte* state = SDL_GetKeyboardState(NULL);
    // needs some work
}

void Emulator::renderMenuBar() {
    if (ImGui::BeginMainMenuBar()){
        if (ImGui::BeginMenu("File")){
            if (ImGui::Button("Open File")) {
                FileHandler file = getFileFromUser();
                if (sizeof(file) > 0) { // todo, check what happens when a user hits cancel, and if file isn't the right type (extension)
                    gameboy.emplace(file.readFile());
                    createGameboyTextures();
                } else {
                    printf("no file selected, please select a file to run the emulator\n");
                }
            }
            ImGui::EndMenu();
        }
        if (gameboy.has_value()) {
            static int winScale = gameboy->ppu.winScale;

            if (winScale > 4) {
                winScale = 4;
            } else if (winScale < 1) {
                winScale = 1;
            }

            if (ImGui::BeginMenu("Window")) {
                ImGui::SetNextItemWidth(40);
               if (ImGui::InputInt("Gameboy Scale (1-4)", &winScale, 1)) {
                gameboy->ppu.winScale = std::clamp(winScale, 1, 4); // set limit to 1-4
               }
               ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Debug")) {
                if (ImGui::Button("Open Gameboy Stats")) {
                    showDebugMenu = true;
                }
                if (ImGui::Button("Show Tile Data")) {
                    showTileData = !showTileData;
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }

    if (showDebugMenu) {
        ImGui::SetNextWindowSize(ImVec2(100, 200), ImGuiCond_FirstUseEver);
        // ImGui::SetNextWindowPos(ImVec2(10, 10)); // todo find a way to make this allow for movement

        if (ImGui::Begin("Debug", &showDebugMenu)) {
            Byte lcdcBin = gameboy->mmu.readByte(Mmu::LCDC);
            Byte statBin = gameboy->mmu.readByte(Mmu::STAT);
            Byte ly = gameboy->mmu.readByte(Mmu::LY);
            ImGui::Text("A: 0x%02x\tF: 0x%02x", gameboy->cpu.A, gameboy->cpu.F);
            ImGui::Text("B: 0x%02x\tC: 0x%02x", gameboy->cpu.B, gameboy->cpu.C);
            ImGui::Text("D: 0x%02x\tE: 0x%02x", gameboy->cpu.D, gameboy->cpu.E);
            ImGui::Text("H: 0x%02x\tL: 0x%02x", gameboy->cpu.H, gameboy->cpu.L);
            ImGui::Text("PC: 0x%04x\tSP: 0x%02x", gameboy->cpu.PC, gameboy->cpu.SP);
            ImGui::Text("LCDC: 0b%s", std::bitset<8>(lcdcBin).to_string().c_str());
            ImGui::Text("STAT: 0b%s", std::bitset<8>(statBin).to_string().c_str());
            ImGui::Text("LY: 0x%02x", ly);
        }
        ImGui::End();
    }
}

FileHandler Emulator::getFileFromUser() {
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(&outPath, NULL, NULL, NULL);

    if (result == NFD_OKAY) {
        // std::string filePath(outPath);
        std::cout << "successfully got file path: " << outPath << std::endl;
        puts(outPath);
        std::string str_filePath(outPath);
        free(outPath);
        return FileHandler(str_filePath);

    } else if (result == NFD_CANCEL) {
        std::cout << "User canceled the dialog." << std::endl;
    } else {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
    return FileHandler("");
}