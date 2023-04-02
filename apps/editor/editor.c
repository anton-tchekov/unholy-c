#include "editor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* --- PRIVATE --- */
static void editor_line_delete(Editor *ed, i32 line)
{
	free(ed->Lines[line]);
	--ed->NumLines;
	memmove(&ed->Lines[line], &ed->Lines[line + 1],
		ed->NumLines - line);
}

static void editor_line_insert(Editor *ed, i32 line)
{

}

/* --- PUBLIC --- */
void editor_init(Editor *ed)
{
	ed->TabSize = 4;
	ed->LineNumbers = 4;

	ed->Capacity = 128;
	ed->Lines = _malloc(ed->Capacity * sizeof(Line *));
	ed->NumLines = 1;
	ed->Lines[0] = line_new(8);

	ed->CursorX = 0;
	ed->CursorY = 0;

	ed->PageW = 30;
	ed->PageH = 50;
	ed->PageY = 0;
	ed->PageX = 0;

	ed->Screen = _malloc((ed->PageW + 1 + ed->LineNumbers) * ed->PageH);
}

/* --- EDITING --- */
void editor_char(Editor *ed, char c)
{
	line_insert(ed, &c, 1);
}

void editor_backspace(Editor *ed)
{
	if(ed->CursorX == 0)
	{
		/* merge with previous line */
		if(ed->CursorY > 0)
		{
			Line *cur = ed->Lines[ed->CursorY];
			--ed->CursorY;
			ed->CursorX = ed->Lines[ed->CursorY]->Length;
			editor_line_insert(ed, cur->Buffer, cur->Length);
			editor_line_delete(ed, ed->CursorY + 1);
		}
	}
	else
	{
		/* delete prev char */
		--ed->CursorX;
		line_delete(&ed->Lines[ed->CursorY], ed->CursorX, 1);
	}
}

void editor_delete(Editor *ed)
{
	if(ed->CursorX >= ed->Lines[ed->CursorY]->Length)
	{
		if(ed->CursorY < ed->NumLines)
		{
			/* merge with next line */
			line_insert(
				&ed->Lines[ed->CursorY],
				ed->Lines[ed->CursorY + 1]->Buffer,
				ed->Lines[ed->CursorY + 1]->Length);

			editor_line_delete(ed, ed->CursorY + 1);
		}
	}
	else
	{
		/* delete next char */
		line_delete(&ed->Lines[ed->CursorY], ed->CursorX, 1);
	}
}

void editor_new_line(Editor *ed)
{
	i32 len;
	char *str;
	Line *cur;

	cur = ed->Lines[ed->CursorY];
	str = cur->Buffer + ed->CursorX;
	len = cur->Length - ed->CursorX;

	editor_line_insert(ed, ed->CursorY + 1);
	line_insert(&ed->Lines[ed->CursorY + 1], 0, str, len);
	line_delete(&ed->Lines[ed->CursorY], ed->CursorX, len);

	++ed->CursorY;
	ed->CursorX = 0;
}

/* --- CURSOR MOVEMENT --- */
void editor_home(Editor *ed)
{
	int i = 0;
	if(ed->CursorX == 0)
	{
		char *buf = ed->Lines[ed->CursorY]->Buffer;
		while(isspace(buf[i]))
		{
			++i;
		}
	}

	ed->CursorX = i;
}

void editor_end(Editor *ed)
{
	ed->CursorX = ed->Lines[ed->CursorY]->Length;
}

void editor_page_up(Editor *ed)
{
	ed->CursorY -= ed->PageH;
	if(ed->CursorY < 0)
	{
		ed->CursorY = 0;
		ed->CursorX = 0;
	}
}

void editor_page_down(Editor *ed)
{
	ed->CursorY += ed->PageY;
	if(ed->CursorY >= ed->NumLines)
	{
		ed->CursorY = ed->NumLines - 1;
		ed->CursorX = ed->Lines[ed->NumLines - 1]->Length;
	}
}

void editor_left(Editor *ed)
{
	if(ed->CursorX == 0)
	{
		--ed->CursorY;
		ed->CursorX = ed->Lines[ed->CursorY]->Length;
	}
	else
	{
		--ed->CursorX;
	}
}

void editor_right(Editor *ed)
{
	if(ed->CursorX == ed->Lines[ed->CursorY]->Length)
	{
		++ed->CursorY;
		ed->CursorX = 0;
	}
	else
	{
		++ed->CursorX;
	}
}

void editor_up(Editor *ed)
{
	if(ed->CursorY == 0)
	{
		ed->CursorX = 0;
	}
	else
	{
		--ed->CursorY;
		if(ed->CursorY < ed->PageY)
		{
			ed->PageY = ed->CursorY;
		}
	}
}

void editor_down(Editor *ed)
{
	if(ed->CursorY >= ed->NumLines - 1)
	{
		ed->CursorX = ed->Lines[ed->NumLines - 1]->Length;
	}
	else
	{
		++ed->CursorY;
		if(ed->CursorY >= ed->PageY + ed->PageH)
		{
			ed->PageY = ed->CursorY - ed->PageH;
		}
	}
}
