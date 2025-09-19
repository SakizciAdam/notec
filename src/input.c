#include "input.h"

void switch_mode(){
    if(mode>1){
        mode=0;
    }
                    
    mode=1-mode;
                    
    
}

#ifdef _WIN32
void copy_clipboard() {
    if (selStart == -1 || selEnd == -1 || selStart == selEnd) return;
    set_status_text("Error");
    int start = selStart < selEnd ? selStart : selEnd;
    int end = selStart > selEnd ? selStart : selEnd;
    int lengthToCopy = end - start;
    if (lengthToCopy <= 0) return;
    if (!OpenClipboard(NULL)) return;
    EmptyClipboard();
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, lengthToCopy + 1);
    if (!hMem) { CloseClipboard(); return; }
    memcpy(GlobalLock(hMem), text + start, lengthToCopy);
    GlobalUnlock(hMem);
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
    set_status_text("Copied");
}

void paste_clipboard() {
    if (!OpenClipboard(NULL)) { set_status_text("Error"); return; }
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (!hData) { set_status_text("No text in clipboard"); return; }
    wchar_t *wText = (wchar_t *)GlobalLock(hData);
    if (!wText) { set_status_text("Error"); CloseClipboard(); return; }
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wText, -1, NULL, 0, NULL, NULL);
    char *buffer = malloc(sizeNeeded);
    if (buffer) {
        set_status_text("Pasted");
        int oldIndex = get_text_index();
        WideCharToMultiByte(CP_UTF8, 0, wText, -1, buffer, sizeNeeded, NULL, NULL);
        for (int i = 0; i < strlen(buffer); i++) {
            char ch = buffer[i];
            if ((int)ch == '\n') {on_key(RETURN,WRITING_MODE);}
            else if ((int)ch == 9) on_key('#',WRITING_MODE);
            else if ((unsigned char)ch <= 127) on_key(ch,WRITING_MODE);
        }
        goto_index(oldIndex);
    }
    GlobalUnlock(hData);
    CloseClipboard();
}
#else
void copy_clipboard() {
    if (selStart == -1 || selEnd == -1 || selStart == selEnd) return;

    int start = selStart < selEnd ? selStart : selEnd;
    int end   = selStart > selEnd ? selStart : selEnd;
    int lengthToCopy = end - start;
    if (lengthToCopy <= 0) return;

    char *buffer = malloc(lengthToCopy + 1);
    if (!buffer) { set_status_text("Error allocating memory"); return; }
    memcpy(buffer, text + start, lengthToCopy);
    buffer[lengthToCopy] = '\0';

    FILE *pipe = popen("xclip -selection clipboard", "w");
    if (!pipe) { set_status_text("Error: xclip not available"); free(buffer); return; }
    fwrite(buffer, 1, lengthToCopy, pipe);
    pclose(pipe);
    free(buffer);

    set_status_text("Copied");
}

void paste_clipboard() {
    FILE *pipe = popen("xclip -selection clipboard -o", "r");
    if (!pipe) { set_status_text("Error: xclip not available"); return; }

    char buffer[4096];
    int oldIndex = get_text_index();
    set_status_text("Pasted");

    while (fgets(buffer, sizeof(buffer), pipe)) {
        for (int i = 0; i < strlen(buffer); i++) {
            char ch = buffer[i];
            if ((int)ch == '\n') {on_key(RETURN,WRITING_MODE);}
            else if ((int)ch == 9) on_key('#',WRITING_MODE);
            else if ((unsigned char)ch <= 127) on_key(ch,WRITING_MODE);
        }
    }
    pclose(pipe);

    goto_index(oldIndex);
}
#endif

void on_key(char c,int md){
    if((int)c==27){
        switch_mode();
        c=-9999;
    }
    if ((int)c == -9999) return;
    
    if(md==WRITING_MODE){
       
        
        if ((int)c == -32) { arrow = true; return; }
        if (readOnly) return;

        int idx = get_text_index();

        if ((int)c == BACKSPACE) {
            if (selStart != -1 && selEnd != -1 && selStart != selEnd) {
                int start = selStart < selEnd ? selStart : selEnd;
                int end = selStart > selEnd ? selStart : selEnd;
                int delLength = end - start;

                for (int i = start; i < length - delLength; i++) {
                    text[i] = text[i + delLength];
                }
                length -= delLength;
                text = realloc(text, length + 1);
                if (text) text[length] = '\0';

                goto_index(start);

                selStart = selEnd = -1;
            } else if (idx > 0 && length > 0) {
                for (int i = idx - 1; i < length - 1; i++) {
                    text[i] = text[i + 1];
                }
                length--;
                text = realloc(text, length + 1);
                if (text) text[length] = '\0';

                if (cursorX > 0) cursorX--;
                else if (cursorY > 0) {
                    cursorY--;
                    cursorX = get_line_length(cursorY);
                }
            }
            return;
        }
        #ifndef _WIN32 
        arrow=true;
        #endif
        if (arrow) {
            switch ((int)c) {
                case DOWN_ARROW: move_cursor_down(); return;
                case UP_ARROW: move_cursor_up(); return;
                case LEFT_ARROW: move_cursor_left(); return;
                case RIGHT_ARROW: move_cursor_right(); return;
            }
        }

        switch ((int)c) {
            case 9: 
                for (int i = 0; i < 4; i++) {
                    add_char_at(' ', idx++);
                    cursorX++;
                }
                break;
            case RETURN:
                add_char_at('\n', idx);
                cursorY++;
                cursorX = 0;
                break;
            default:
                if ((unsigned char)c <= 127) {
                    add_char_at(c, idx);
                    saved=0;
                    cursorX++;
                }
                break;
        }
    } else {
        char lower = tolower(c);
        arrow = false;

        if (lower == 'q') quit();
        if (lower == 's') toggle_selection_mode();
        else if ((int)c == UP_ARROW) move_cursor_or_selection_up();
        else if ((int)c == DOWN_ARROW) move_cursor_or_selection_down();
        else if ((int)c == RIGHT_ARROW) move_cursor_or_selection_right();
        else if ((int)c == LEFT_ARROW) move_cursor_or_selection_left();
        else if (lower == 'p') clear_selection();
        else if (lower == 'a') select_all();
        else if (lower == 'c') copy_clipboard();
        else if (lower == 'v') paste_clipboard();
        else if (lower == 'g') goto_line();
        else if (lower == 'w') save_file();
        else if (lower == 'f') find_text();
    }
}