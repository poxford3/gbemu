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
                    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == NULL) {
        printf("error initializing window. SDL error: %s\n", SDL_GetError());
    }

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

    running = true;
}

void Emulator::createGameboyTexture() {
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
    }
}

void Emulator::run() {
    while (running) {
        SDL_Event event;
    
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) {
                    running = false;
                }
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.scancode == SDL_SCANCODE_F) {
                        FileHandler file = getFileFromUser();
                        // gameboy = std::make_unique<Gameboy>(file.readFile());
                        gameboy.emplace(file.readFile());
                        createGameboyTexture();
                    }
                }
            }
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
        // handleInput();
        static int counter = 0;
        if (++counter % 60 == 0) {
            std::cout << "status: " << gameboy->checksumPassed << '\n';
            if (gameboy->checksumPassed) {
                std::cout << "gameboy filesize: " << gameboy->mmu.entireRom.size() << std::endl;
            }
        }
        // SDL_Delay(16);
    }
}

void Emulator::handleInput() {
    const Byte* state = SDL_GetKeyboardState(NULL);

    if (state[SDL_SCANCODE_F]) {
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
    // return FileHandler(filePath);
    return FileHandler("");
}