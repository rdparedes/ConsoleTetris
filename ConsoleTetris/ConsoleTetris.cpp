/// <summary>
/// A tetris terminal game. Based on https://www.youtube.com/watch?v=8OK8_tHeCIA&list=RDCMUC-yuWVUplUJZvieEligKBkA&index=25
/// </summary>

#include <iostream>
#include <thread>
#include <vector>
#include <Windows.h>
using namespace std;

const int PIECE_SIZE = 4;

wstring tetromino[7];
int field_width = 14;
int field_height = 18;
unsigned char* p_field = nullptr;

int screen_width = 80;       // Console screen size (cols)
int screen_height = 30;      // Console screen size (rows)
int screen_offset_x = (screen_width / 2) - (field_width / 2);

int rotate(int px, int py, int r) {
    switch (r % PIECE_SIZE)
    {
    case 0: return py * PIECE_SIZE + px;         // 0 degrees
    case 1: return 12 + py - (px * PIECE_SIZE);  // 90 degrees
    case 2: return 15 - (py * PIECE_SIZE) - px;  // 180 degrees
    case 3: return 3 - py + (px * PIECE_SIZE);   // 270 degrees
    }
    return 0;
}

int does_piece_fit(int n_tetromino, int rotation_id, int pos_x, int pos_y)
{
    for (int px = 0; px < PIECE_SIZE; px++)
    {
        for (int py = 0; py < PIECE_SIZE; py++)
        {
            int piece_index = rotate(px, py, rotation_id);
            int field_index = (pos_y + py) * field_width + (pos_x + px);

            if (pos_x + px >= 0 && pos_x + px < field_width) {
                if (pos_y + py >= 0 && pos_x + py < field_height) {
                    if (tetromino[n_tetromino][piece_index] != L'.' && p_field[field_index] != 0) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

int main()
{
    // Create assets
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"..X.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L"..X.");
    tetromino[1].append(L"....");

    tetromino[2].append(L".X..");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"..X.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"....");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L"..X.");

    tetromino[5].append(L"....");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L".X..");
    tetromino[5].append(L".X..");

    tetromino[6].append(L"..X.");
    tetromino[6].append(L".XX.");
    tetromino[6].append(L".X..");
    tetromino[6].append(L"....");

    p_field = new unsigned char[field_width * field_height];
    // Field borders
    for (int x = 0; x < field_width; x++)
    {
        for (int y = 0; y < field_height; y++)
        {
            if (y == field_height - 1 && x > 1 && x < field_width - 2) {
                p_field[y * field_width + x] = 12;
            }
            else if (y == field_height - 1 && (x <= 1 || x >= field_width - 2)) {
                p_field[y * field_width + x] = 13;
            }
            else if (y == field_height - 2 && x > 1 && x < field_width - 2) {
                p_field[y * field_width + x] = 8;
            }
            else if (x == 0) {
                p_field[y * field_width + x] = 9;
            }
            else if (x == 1 || x == field_width - 2) {
                p_field[y * field_width + x] = 11;
            }
            else if (x == field_width - 1) {
                p_field[y * field_width + x] = 10;
            }
            else {
                p_field[y * field_width + x] = 0;
            }
        }
    }

    wchar_t *screen = new wchar_t[screen_width * screen_height];
    for (int i = 0; i < screen_width * screen_height; i++) screen[i] = L' ';
    HANDLE h_console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(h_console);
    DWORD dw_bytes_written = 0;

    bool is_running = true;
    int current_piece = 0;
    int current_rotation_id = 0;
    int current_x = field_width / 2;
    int current_y = 0;
    bool key[4];
    bool is_rotate_pressed = false;

    int speed = 20;
    int speed_counter = 0;
    bool force_down = false;
    vector<int> lines;
    int piece_count = 0;
    int score = 0;

    while (is_running) {
        // Game timing
        this_thread::sleep_for(50ms);
        speed_counter++;
        force_down = (speed_counter == speed);

        // Input handling 
        for (int k = 0; k < 4; k++)
        {
            // Check if left(A), right(D), down(S) or rotate(R) keys are pressed
            key[k] = (0x8000 & GetAsyncKeyState((unsigned char)("DASR"[k]))) != 0;
        }

        // Game logic
        if (key[1] && does_piece_fit(current_piece, current_rotation_id, current_x - 1, current_y)) {
            current_x -= 1;
        }
        if (key[0]  && does_piece_fit(current_piece, current_rotation_id, current_x + 1, current_y)) {
            current_x += 1;
        }
        if (key[2] && does_piece_fit(current_piece, current_rotation_id, current_x, current_y + 1)) {
            current_y += 1;
        }
        if (key[3]) {
            if (!is_rotate_pressed && does_piece_fit(current_piece, current_rotation_id + 1, current_x, current_y)) {
                current_rotation_id += 1;
            }
            is_rotate_pressed = true;
        }
        else
        {
            is_rotate_pressed = false;
        }

        if (force_down) {
            if (does_piece_fit(current_piece, current_rotation_id, current_x, current_y + 1)) {
                current_y++;
            }
            else {
                // Lock the current piece into the field
                for (int px = 0; px < PIECE_SIZE; px++) {
                    for (int py = 0; py < PIECE_SIZE; py++) {
                        if (tetromino[current_piece][rotate(px, py, current_rotation_id)] == L'X') {
                            p_field[(current_y + py) * field_width + (current_x + px)] = current_piece + 1; // current_piece + 1 == .ABCDEFG
                        }
                    }
                }

                piece_count++;
                if (piece_count % 10 == 0) {
                    if (speed >= 10) {
                        speed--;
                    }
                }

                // Check we have a full horizontal line
                for (int py = 0; py < PIECE_SIZE; py++)
                {
                    if (current_y + py < field_height - 2) {
                        bool line = true;
                        for (int px = 2; px < field_width-2; px++)
                        {
                            line &= (p_field[(current_y + py) * field_width + px]) != 0;
                        }

                        if (line) {
                            for (int px = 2; px < field_width - 2; px++)
                            {
                                p_field[(current_y + py) * field_width + px] = 8;
                            }
                            lines.push_back(current_y + py);
                        }
                    }
                }

                score += 25;
                if (!lines.empty()) {
                    score += (1 << lines.size()) * 100;
                }

                // Choose next piece
                current_x = field_width / 2;
                current_y = 0;
                current_rotation_id = 0;
                current_piece = rand() % 7;

                // if new piece doesn't fit
                is_running = does_piece_fit(current_piece, current_rotation_id, current_x, current_y);

            }
            speed_counter = 0;
        }

        // Render output ---

        // Draw field
        for (int x = 0; x < field_width; x++) {
            for (int y = 0; y < field_height; y++) {
                screen[(y + 2) * screen_width + (x + screen_offset_x)] = L".ABCDEFG=<>!V "[p_field[y * field_width + x]];
            }
        }

        // Draw current piece
        for (int px = 0; px < PIECE_SIZE; px++) {
            for (int py = 0; py < PIECE_SIZE; py++) {
                if (tetromino[current_piece][rotate(px, py, current_rotation_id)] != L'.') {
                    screen[(current_y + py + 2) * screen_width + (current_x + px + screen_offset_x)] = current_piece + 65;
                }
            }
        }

        // Draw score
        swprintf_s(&screen[2 * screen_width + 6], 16, L"SCORE: %8d", score);

        if (!lines.empty()) {
            WriteConsoleOutputCharacter(h_console, screen, screen_width * screen_height, { 0,0 }, &dw_bytes_written);
            this_thread::sleep_for(400ms);

            // After a short delay of displaying the ='s of the line, remove it and move everything down
            for (auto &v : lines) {
                for (int px = 2; px < field_width - 2; px++) {
                    for (int py = v; py > 0; py--) {
                        p_field[py * field_width + px] = p_field[(py - 1) * field_width + px];
                    }
                    p_field[px] = 0;
                }
            }

            lines.clear();
        }

        WriteConsoleOutputCharacter(h_console, screen, screen_width * screen_height, { 0, 0 }, &dw_bytes_written);
    }

    CloseHandle(h_console);
    cout << "GAME OVER!! Score: " << score << endl;
    system("pause");
    return 0;
}
