/**
 * @file src/text.h
 * 
 * Renders a text in a character-by-character manner
 */
#ifndef __TEXT_H__
#define __TEXT_H__

typedef struct stText text;

int txt_getNew(text **ppTxt);
void txt_free(text **ppTxt);
void txt_setText(text *pTxt, int num);
void txt_update(text *pTxt, int ms);
void txt_draw(text *pTxt);

#endif /* __TEXT_H__ */

