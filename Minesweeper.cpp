
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

bool Game = true;
class Tile {
public:
    sf::RectangleShape* Tilep;
    sf::RectangleShape Square;
    sf::Text Number;
    sf::Text* Text;
    bool hasBomb = false;
    bool isClosed = true, isFlagged = false;
    int BombsAround = 0, posx, posy, row, column;



    void TileDesign() {
        Tilep = new sf::RectangleShape(sf::Vector2f(48, 48));
        posy = row * 50;
        posx = column * 50;
        Tilep->setPosition(posx, posy);
        Tilep->setFillColor(sf::Color::Blue);
        Tilep->setOutlineThickness(2);
        Tilep->setOutlineColor(sf::Color::Black);
        Square = *Tilep;
    }

    void isHighlighted() {
        if (isClosed && isFlagged == false)
            Square.setFillColor(sf::Color::Green);
    }
    void isNotHighlighted() {
        if (isClosed && isFlagged == false)
            Square.setFillColor(sf::Color::Blue);
    }
    void flag() {
        if (isFlagged)
            Square.setFillColor(sf::Color::Yellow);

    }

    void open() {
        if (hasBomb) {
            Square.setFillColor(sf::Color::Red);
            Game = false;
        }
        else {
            Square.setFillColor(sf::Color::White);
        }

    }
    void clicked() {
        isClosed = false;
        open();

    }
    void checkAround(int TileNumber, int TilesPerSide, std::vector<Tile>& TileBoard) {
        int TotalTiles = (TilesPerSide * TilesPerSide);
        for (int i = std::max(column - 1, 0); i < std::min(column + 2, TilesPerSide); i++) {
            for (int j = std::max(row - 1, 0); j < std::min(row + 2, TilesPerSide); j++) {
                // Skip the current tile (no bomb around itself)
                if (i == column && j == row) {
                    continue;
                }

                // Check if neighbor is within the board and has a bomb
                int neighborIndex = j * TilesPerSide + i;
                if (neighborIndex >= 0 && neighborIndex < TileBoard.size() && TileBoard[neighborIndex].hasBomb) {
                    BombsAround++;
                }
            }
        }
    }






};

class Board {
public:
    int TilesPerSide, bombs, TotalTiles, videores, openedTiles = 0, flags;
    Board(int aux, int bombs) {
        TilesPerSide = aux;
        flags = bombs;
        int counter = 0, bombAssigner = 0;
        videores = TilesPerSide * 50;
        TotalTiles = TilesPerSide * TilesPerSide;
        std::vector<int> AssignedBombs;
        AssignBombs(TotalTiles, bombs, AssignedBombs);

        std::vector<Tile> TileBoard(TotalTiles);

        for (int i = 0; i < TilesPerSide; i++) {
            for (int j = 0; j < TilesPerSide; j++) {
                TileBoard[counter].column = j;
                TileBoard[counter].row = i;
                if (bombAssigner < bombs) {
                    if (counter == AssignedBombs[bombAssigner]) {
                        TileBoard[counter].hasBomb = true;
                        bombAssigner++;
                    }
                }
                TileBoard[counter].TileDesign();
                counter++;
            }

        }
        for (int i = 0; i < TotalTiles; i++) {
            TileBoard[i].checkAround(i, TilesPerSide, TileBoard);
        }
        drawTiles(TileBoard, bombs);


    }
    void Flusher(int TileNumber, std::vector<Tile>& TileBoard) {
        int TotalTiles = TilesPerSide * TilesPerSide;
        for (int i = std::max(TileBoard[TileNumber].column - 1, 0); i < std::min(TileBoard[TileNumber].column + 2, TilesPerSide); i++) {
            for (int j = std::max(TileBoard[TileNumber].row - 1, 0); j < std::min(TileBoard[TileNumber].row + 2, TilesPerSide); j++) {
                // Skip the current tile (no bomb around itself)
                if (i == TileBoard[TileNumber].column && j == TileBoard[TileNumber].row) {
                    continue;
                }
                // Check if neighbor is within the board and has a bomb
                int neighborIndex = j * TilesPerSide + i;
                if (TileBoard[neighborIndex].isClosed) {
                    TileBoard[neighborIndex].clicked();
                    if (neighborIndex >= 0 && neighborIndex < TileBoard.size() && TileBoard[neighborIndex].BombsAround == 0)
                        Flusher(neighborIndex, TileBoard);
                }
            }
        }
    }



    void drawTiles(std::vector<Tile> TileBoard, int bombs) {
        sf::RenderWindow window(sf::VideoMode(videores, videores+50), "Buscaminas :)");
        window.setFramerateLimit(60);
        sf::Mouse::setPosition(sf::Vector2i(10, 50), window);
        sf::Vector2i localPosition;
        int TileUnderMouse = 0, mouseX, mouseY;
        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font arial.ttf" << std::endl;
        }
        int numTexts = TotalTiles;
        int fontSize = 30;
        sf::Color textColor(sf::Color::Black), textColor1(33, 53, 166), textColor2(7, 133, 3), textColor3(250, 37, 37), textColor4(158, 3, 19);

        std::vector<sf::Text> texts;
        for (int i = 0; i < numTexts; ++i) {
            std::string message = std::to_string(TileBoard[i].BombsAround);
            sf::Text text(message, font, fontSize);
            switch (TileBoard[i].BombsAround) {
            case 1: {
                text.setFillColor(textColor1);
                break;
            }
            case 2: {
                text.setFillColor(textColor2);
                break;
            }
            case 3: {
                text.setFillColor(textColor3);
                break;
            }
            default: {
                text.setFillColor(textColor4);
                break;
            }

            }
            text.setPosition(TileBoard[i].posx + 15, TileBoard[i].posy + 7);
            texts.push_back(text);
        }

        sf::RectangleShape bar(sf::Vector2f(videores, 50));
        sf::Color Gray(192, 192, 192);
        bar.setPosition(0, videores);
        bar.setFillColor(Gray);
        sf::Text flagCount("Bombs left: " + std::to_string(flags), font, fontSize);
        flagCount.setFillColor(textColor);
        flagCount.setPosition(5, videores+7);
        sf::Text timeText;
        timeText.setFont(font);
        timeText.setCharacterSize(30);
        timeText.setPosition(videores - 100, videores + 7);
        timeText.setFillColor(sf::Color::Black);
        sf::Clock clock;


        while (window.isOpen())
        {
            while (Game) {
                sf::Event event;
                sf::Time elapsedTime = clock.getElapsedTime();
                int minutes = elapsedTime.asSeconds() / 60;
                int seconds = static_cast<int>(elapsedTime.asSeconds()) % 60;
                timeText.setString(std::to_string(minutes) + ":" + std::to_string(seconds));
                window.draw(timeText);
                while (window.pollEvent(event))
                {
                    timeText.setString(std::to_string(minutes)+":"+std::to_string(seconds));
                    localPosition = sf::Mouse::getPosition(window);
                    mouseX = localPosition.x / 50;
                    mouseY = localPosition.y / 50;
                    window.draw(timeText);
                    if (localPosition.x < videores && localPosition.x>0) {
                        if (localPosition.y < videores && localPosition.y>0) {
                            for (int i = 0; i < TotalTiles; i++) {
                                if (mouseY == TileBoard[i].row && mouseX == TileBoard[i].column) {
                                    TileBoard[i].isHighlighted();
                                    TileUnderMouse = i;
                                }
                                else {
                                    TileBoard[i].isNotHighlighted();
                                }
                            }
                        }
                    }

                    if (event.type == sf::Event::MouseButtonPressed)
                    {
                        if (event.mouseButton.button == sf::Mouse::Right && TileBoard[TileUnderMouse].isClosed)
                        {
                            if (flags > 0 || TileBoard[TileUnderMouse].isFlagged) {
                                TileBoard[TileUnderMouse].isFlagged = !TileBoard[TileUnderMouse].isFlagged;
                                TileBoard[TileUnderMouse].flag();
                                if (TileBoard[TileUnderMouse].isFlagged) {
                                    flags--;
                                }
                                else { flags++; }
                                flagCount.setString("Bombs left: " + std::to_string(flags));
                            }

                        }
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            if (!TileBoard[TileUnderMouse].isFlagged) {
                                TileBoard[TileUnderMouse].clicked();
                                if (TileBoard[TileUnderMouse].BombsAround == 0 && !TileBoard[TileUnderMouse].hasBomb)
                                    Flusher(TileUnderMouse, TileBoard);
                                openedTiles = 0;
                                for (int i = 0; i < TotalTiles; i++) {
                                    if (!TileBoard[i].isClosed && !TileBoard[i].hasBomb)
                                        openedTiles++;

                                }
                                if (openedTiles == (TotalTiles - bombs)) {
                                    Game = false;
                                }
                            }
                        }
                    }

                    if (event.type == sf::Event::Closed) {
                        window.close();
                        Game = false;
                    }
                }

                window.clear();
                for (int i = 0; i < (TotalTiles); i++) {
                    window.draw(TileBoard[i].Square);
                    if (TileBoard[i].isClosed == false && TileBoard[i].BombsAround > 0 && !TileBoard[i].hasBomb) {
                        window.draw(texts[i]);
                    }
                }
                window.draw(bar);
                window.draw(flagCount);
                window.draw(timeText);
                window.display();

            }
            if (openedTiles == (TotalTiles - bombs)) {
                std::cout << "Felicidades! Ganaste!!" << std::endl;
                system("pause");
                window.close();
            }
            else {
                std::cout << "Perdiste :(" << std::endl;
                system("pause");
                window.close();

            }
        }

    }




    void AssignBombs(int TotalTiles, int bombs, std::vector<int>& AssignedBombs) {

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, TotalTiles - 1);

        for (int i = 0; i < bombs; i++) {
            int random_number;
            do {
                random_number = dis(gen);
            } while (find(AssignedBombs.begin(), AssignedBombs.end(), random_number) != AssignedBombs.end());
            AssignedBombs.push_back(random_number);
        }
        sort(AssignedBombs.begin(), AssignedBombs.end());



    }
};

class Menu {
public:
    int choice;
   static int getValidInt() {
        int number;

        while (!(std::cin >> number)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return 0;
        }

        return number;
    }

    static void run() {
        int choice = 0;
        while (choice != 4) {
            system("cls");
            Game = true;
            std::cout << "Bienvenido al buscaminas\n";
            std::cout << "1.- Nivel facil\n";
            std::cout << "2.- Nivel Intermedio\n";
            std::cout << "3.- Nivel experto\n";
            std::cout << "4.- Salir\n";
            choice = getValidInt();
            switch (choice) {
            case 1: {
                Board easy(8, 6);
                break;
            }
            case 2: {
                Board medium(13, 21);
                break;
            }
            case 3: {
                Board hard(18, 50);
                break;
            }
            case 4:
                std::cout << "Hasta pronto :)\n";
                break;
            default:
                std::cout << "Opcion no valida\n";
                system("pause");
                break;
            }
        }
    }
};



int main()
{
    Menu::run();


    return 0;
}
