/**
 * @file src/text.h
 * 
 * Renders a text in a character-by-character manner
 */
#include <GFraMe/GFraMe_spriteset.h>

#include <stdlib.h>
#include <string.h>

#include "audio.h"
#include "global.h"
#include "text.h"

struct stText {
    char *curText;
    int length;
    int cooldown;
    int maxLength;
    int numLines;
    int didFinish;
};

static char _str_line0[] = 
"MOVE WITH AD, QD OR THE LEFT STICK\n"
"JUMP WITH THE SPACEBAR OR THE \n"
"SHOULDER BUTTONS";

static char _str_line1[] = 
"AIM WITH THE MOUSE OR THE RIGHT STICK\n \n"
"FIRE WITH THE MOUSE OR THE TRIGGER \n"
"BUTTONS";

/*
static char _str_line2[] = 
"THERE'S AN OLD LEGEND... IT SAYS THAT\n"
"IF ONE WERE TO OBTAIN THE 7 STONES OF\n"
"POWER, IT WOULD HAVE ONE OF ITS\n"
"WISHES REALISED...";

static char _str_line3[] =
""

static char _strLine4[] = 
*/

static char _str_line7[] = 
"YOU DID IT!!\n"
"YOU WERE ABLE TO FIND EVERY STONE OF\n"
"POWER!!                    \n"
" \n"
" \n"
" \n"
"GAME OVER\n"
" \n"
"PLEASE CLOSE THE GAME"
"                    \n \n"
"(OR WALK AROUND WITH THAT RAINBOW *-*)";

int txt_getNew(text **ppTxt) {
    int rv;
    
    // Sanitize arguments
    ASSERT(ppTxt, 1);
    ASSERT(!(*ppTxt), 1);
    
    // Alloc the object
    *ppTxt = (text*)malloc(sizeof(text));
    ASSERT(*ppTxt, 1);
    
    memset(*ppTxt, 0, sizeof(text));
    
    rv = 0;
__ret:
    return rv;
}

void txt_free(text **ppTxt) {
    // Sanitize arguments
    ASSERT_NR(ppTxt);
    ASSERT_NR(*ppTxt);
    
    free(*ppTxt);
    *ppTxt = 0;
__ret:
    return;
}

void txt_setText(text *pTxt, int num) {
    // Clean up the text
    memset(pTxt, 0, sizeof(text));
    
    switch (num) {
        case 0: {
            pTxt->curText = _str_line0;
            pTxt->maxLength = sizeof(_str_line0) - 1;
        } break;
        case 1: {
            pTxt->curText = _str_line1;
            pTxt->maxLength = sizeof(_str_line1) - 1;
        } break;
        case 7: {
            pTxt->curText = _str_line7;
            pTxt->maxLength = sizeof(_str_line7) - 1;
        } break;
    }
    pTxt->numLines = 1;
}

void txt_update(text *pTxt, int ms) {
    if (!pTxt->curText) {
        return;
    }
    
    if (pTxt->cooldown > 0)
        pTxt->cooldown -= ms;
    if (pTxt->cooldown <= 0) {
        // Stop if we reached the end
        if (pTxt->length == pTxt->maxLength) {
            pTxt->didFinish = 1;
            if (pTxt->curText != _str_line7)
                pTxt->curText = 0;
        }
        // Otherwise, go to the next char
        else {
            pTxt->length++;
            // Stop for a while, if it's the last char
            if (pTxt->length == pTxt->maxLength) {
                pTxt->cooldown = TXT_COMPLETE_DELAY;
            }
            else {
                if (pTxt->curText[pTxt->length - 1] == '\n') {
                    pTxt->numLines++;
                    pTxt->length++;
                }
                pTxt->cooldown = TXT_CHAR_DELAY;
                if (pTxt->curText[pTxt->length - 1] != ' ')
                    aud_playText();
            }
        }
    }
}

void txt_draw(text *pTxt) {
    int i, lines, x, y;
    int tile;
    
    if (!pTxt->curText) {
        return;
    }
    
    x = 8;
    y = 24;
    
    // Draw the window (on the BG)
    
    // Draw the center
    i = 0;
    tile = 114;
    while (i < (SCRW-8)*4) {
        int j, k;
        
        j = i % (SCRW-8);
        k = i / (SCRW-8);
        GFraMe_spriteset_draw(gl_sset8x8, tile, x-8 + j*8, y-8 + k*8, 0/*flipped*/);
        i++;
    }
    // Draw the top and bottom lines
    i = 0;
    while (i < SCRW/8) {
        tile = 82;
        GFraMe_spriteset_draw(gl_sset8x8, tile, x-8 + i*8, y-8, 0/*flipped*/);
        tile = 146;
        GFraMe_spriteset_draw(gl_sset8x8, tile, x-8 + i*8, y-8 + 4*8, 0/*flipped*/);
        i++;
    }
    // Draw the lateral lines
    i = 0;
    while (i < 5) {
        tile = 113;
        GFraMe_spriteset_draw(gl_sset8x8, tile, x-8, y-8 + i*8, 0/*flipped*/);
        tile = 115;
        GFraMe_spriteset_draw(gl_sset8x8, tile, x-8 + SCRW-8, y-8 + i*8, 0/*flipped*/);
        i++;
    }
    
    // Draw the corners
    tile = 81;
    GFraMe_spriteset_draw(gl_sset8x8, tile, x-8, y-8, 0/*flipped*/);
    tile = 83;
    GFraMe_spriteset_draw(gl_sset8x8, tile, x-8 + SCRW-8, y-8, 0/*flipped*/);
    tile = 145;
    GFraMe_spriteset_draw(gl_sset8x8, tile, x-8, y-8 + 4*8, 0/*flipped*/);
    tile = 147;
    GFraMe_spriteset_draw(gl_sset8x8, tile, x-8 + SCRW-8, y-8 + 4*8, 0/*flipped*/);
    
    i = 0;
    lines = pTxt->numLines - TXT_MAX_LINES;
    
    // Draw the text
    while (i < pTxt->length) {
        
        tile = pTxt->curText[i] - '!';
        
        if (pTxt->curText[i] != '\n' && lines <= 0) {
            // Draw the current tile
            GFraMe_spriteset_draw(gl_sset8x8, tile, x, y, 0/*flipped*/);
            
            x += 8;
        }
        else if (pTxt->curText[i] == '\n') {
            if (lines <= 0) {
                x = 8;
                y += 8;
            }
            lines--;
        }
        i++;
    }
}

