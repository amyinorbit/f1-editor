//===--------------------------------------------------------------------------------------------===
// main.c - F1 - Demo text editor, uses most of TermUtils functionality
// This source is part of F1 Editor
//
// Created on 2019-07-30 by Amy Parent <amy@amyparent.com>
// Copyright (c) 2018 Amy Parent
// Licensed under the MIT License
// =^•.•^=
//===--------------------------------------------------------------------------------------------===
#include <term/editor.h>
#include <term/printing.h>
#include <stdio.h>
#include <stdlib.h>

char* readFile(FILE* in) {
    if(!in) return NULL;
    fseek(in, 0, SEEK_END);
    int length = ftell(in);
    fseek(in, 0, SEEK_SET);
    
    char* data = malloc(length + 1);
    fread(data, 1, length, in);
    data[length] = '\0';
    fclose(in);
    return data;
}

int save(FILE* out) {
    out = freopen(NULL, "wb", out);
    int length = 0;
    const char* data = termEditorBuffer(&length);
    fseek(out, 0, SEEK_SET);
    fwrite(data, 1, length, out);
    fflush(out);
    return length;
}

int main(int argc, const char** argv) {
    if(argc != 2) termError("f1", 1, "not enough arguments (f1 [file])");
    const char* fname = argv[1];
    
    char* existing = readFile(fopen(fname, "rb"));
    FILE* out = fopen(fname, "wb");
    
    if(!out) termError("f1", 1, "cannot open '%s' for writing", fname);
    termEditorInit(fname);
    if(existing) {
        termEditorReplace(existing);
        free(existing);
        save(out);
    }
    
    bool isDirty = false;
    bool ctrlqTimes = 1;
    int key = -1;
    for(;;) {
        termEditorRender();
        key = termEditorUpdate();
        switch(key) {
        case KEY_CTRL_S: {
            int written = save(out);
            termEditorOut("%d bytes written to %s", written, fname);
            isDirty = false;
        } break;
            
        case KEY_CTRL_C:
        case KEY_CTRL_D:
            termEditorStatus("press CTRL-Q to quit");
            break;
            
        case KEY_TAB:
            for(int i = 0; i < 4; ++i) termEditorInsert(' ');
            isDirty = true;
            break;
            
            case KEY_CTRL_Q:
            if(isDirty && ctrlqTimes) {
                termEditorStatus("unsaved changes, press CTRL-Q again to confirm");
                ctrlqTimes -= 1;
                continue;
            }
            goto cleanup;
            break;
            
            
        default:
            isDirty = true;
            break;
        }
        ctrlqTimes = 1;
        termEditorStatus("");
    }
    
cleanup:
    termEditorDeinit();
    fclose(out);
}