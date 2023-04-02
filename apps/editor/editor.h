#ifndef __EDITOR_H__
#define __EDITOR_H__

typedef struct EDITOR
{
	int CursorX, CursorY;

	char *Screen;
	int PageW, PageH, PageX, PageY;

	int LineNumbers;
	int TabSize;

	int Capacity, NumLines;
	Line **Lines;
} Editor;

void editor_init(Editor *ed);
void editor_render(Editor *ed);
void editor_action(Editor *ed, int action);
void editor_char(Editor *ed, char c);
void editor_insert(Editor *ed, char *s, int len);

#endif /* __EDITOR_H__ */
