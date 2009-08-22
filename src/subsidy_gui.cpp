/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file subsidy_gui.cpp GUI for subsidies. */

#include "stdafx.h"
#include "station_base.h"
#include "industry.h"
#include "town.h"
#include "economy_func.h"
#include "cargotype.h"
#include "window_gui.h"
#include "strings_func.h"
#include "date_func.h"
#include "viewport_func.h"
#include "gfx_func.h"
#include "gui.h"
#include "subsidy_func.h"
#include "subsidy_base.h"

#include "table/strings.h"

/** Widget numbers for the subsidy list window. */
enum SubsidyListWidgets {
	SLW_CLOSEBOX,
	SLW_CAPTION,
	SLW_STICKYBOX,
	SLW_PANEL,
	SLW_SCROLLBAR,
	SLW_RESIZEBOX,
};

struct SubsidyListWindow : Window {
	SubsidyListWindow(const WindowDesc *desc, WindowNumber window_number) : Window(desc, window_number)
	{
		this->FindWindowPlacementAndResize(desc);
	}

	virtual void OnClick(Point pt, int widget)
	{
		if (widget != SLW_PANEL) return;

		int y = pt.y - this->widget[SLW_PANEL].top - FONT_HEIGHT_NORMAL - 1; // Skip 'subsidies on offer' line

		if (y < 0) return;

		uint num = 0;

		const Subsidy *s;
		FOR_ALL_SUBSIDIES(s) {
			if (!s->IsAwarded()) {
				y -= FONT_HEIGHT_NORMAL;
				if (y < 0) {
					this->HandleClick(s);
					return;
				}
				num++;
			}
		}

		if (num == 0) {
			y -= FONT_HEIGHT_NORMAL; // "None"
			if (y < 0) return;
		}

		y -= 11; // "Services already subsidised:"
		if (y < 0) return;

		FOR_ALL_SUBSIDIES(s) {
			if (s->IsAwarded()) {
				y -= FONT_HEIGHT_NORMAL;
				if (y < 0) {
					this->HandleClick(s);
					return;
				}
			}
		}
	}

	void HandleClick(const Subsidy *s)
	{
		/* determine src coordinate for subsidy and try to scroll to it */
		TileIndex xy;
		switch (s->src_type) {
			case ST_INDUSTRY: xy = Industry::Get(s->src)->xy; break;
			case ST_TOWN:     xy =     Town::Get(s->src)->xy; break;
			default: NOT_REACHED();
		}

		if (_ctrl_pressed || !ScrollMainWindowToTile(xy)) {
			if (_ctrl_pressed) ShowExtraViewPortWindow(xy);

			/* otherwise determine dst coordinate for subsidy and scroll to it */
			switch (s->dst_type) {
				case ST_INDUSTRY: xy = Industry::Get(s->dst)->xy; break;
				case ST_TOWN:     xy =     Town::Get(s->dst)->xy; break;
				default: NOT_REACHED();
			}

			if (_ctrl_pressed) {
				ShowExtraViewPortWindow(xy);
			} else {
				ScrollMainWindowToTile(xy);
			}
		}
	}

	virtual void OnPaint()
	{
		YearMonthDay ymd;
		const Subsidy *s;

		this->DrawWidgets();

		ConvertDateToYMD(_date, &ymd);

		int right = this->widget[SLW_PANEL].right;
		int y = this->widget[SLW_PANEL].top + 1;
		int x = this->widget[SLW_PANEL].left + 1;

		/* Section for drawing the offered subisidies */
		DrawString(x, right, y, STR_SUBSIDIES_OFFERED_TITLE);
		y += FONT_HEIGHT_NORMAL;
		uint num = 0;

		FOR_ALL_SUBSIDIES(s) {
			if (!s->IsAwarded()) {
				/* Displays the two offered towns */
				SetupSubsidyDecodeParam(s, 1);
				SetDParam(7, _date - ymd.day + s->remaining * 32);
				DrawString(x + 2, right - 2, y, STR_SUBSIDIES_OFFERED_FROM_TO);

				y += FONT_HEIGHT_NORMAL;
				num++;
			}
		}

		if (num == 0) {
			DrawString(x + 2, right - 2, y, STR_SUBSIDIES_NONE);
			y += FONT_HEIGHT_NORMAL;
		}

		/* Section for drawing the already granted subisidies */
		DrawString(x, right, y + 1, STR_SUBSIDIES_SUBSIDISED_TITLE);
		y += FONT_HEIGHT_NORMAL;
		num = 0;

		FOR_ALL_SUBSIDIES(s) {
			if (s->IsAwarded()) {
				SetupSubsidyDecodeParam(s, 1);
				SetDParam(7, s->awarded);
				SetDParam(8, _date - ymd.day + s->remaining * 32);

				/* Displays the two connected stations */
				DrawString(x + 2, right - 2, y, STR_SUBSIDIES_SUBSIDISED_FROM_TO);

				y += FONT_HEIGHT_NORMAL;
				num++;
			}
		}

		if (num == 0) DrawString(x + 2, right - 2, y, STR_SUBSIDIES_NONE);
	}
};

static const Widget _subsidies_list_widgets[] = {
{   WWT_CLOSEBOX, RESIZE_NONE,   COLOUR_BROWN,   0,  10,   0,  13, STR_BLACK_CROSS,       STR_TOOLTIP_CLOSE_WINDOW},                       // SLW_CLOSEBOX
{    WWT_CAPTION, RESIZE_RIGHT,  COLOUR_BROWN,  11, 307,   0,  13, STR_SUBSIDIES_CAPTION, STR_TOOLTIP_WINDOW_TITLE_DRAG_THIS},             // SLW_CAPTION
{  WWT_STICKYBOX, RESIZE_LR,     COLOUR_BROWN, 308, 319,   0,  13, STR_NULL,              STR_TOOLTIP_STICKY},                              // SLW_STICKYBOX
{      WWT_PANEL, RESIZE_RB,     COLOUR_BROWN,   0, 307,  14, 126, 0x0,                   STR_SUBSIDIES_TOOLTIP_CLICK_ON_SERVICE_TO_CENTER}, // SLW_PANEL
{  WWT_SCROLLBAR, RESIZE_LRB,    COLOUR_BROWN, 308, 319,  14, 114, 0x0,                   STR_TOOLTIP_VSCROLL_BAR_SCROLLS_LIST},           // SLW_SCROLLBAR
{  WWT_RESIZEBOX, RESIZE_LRTB,   COLOUR_BROWN, 308, 319, 115, 126, 0x0,                   STR_TOOLTIP_RESIZE},                              // SLW_RESIZEBOX

{   WIDGETS_END},
};

static const NWidgetPart _nested_subsidies_list_widgets[] = {
	NWidget(NWID_HORIZONTAL),
		NWidget(WWT_CLOSEBOX, COLOUR_BROWN, SLW_CLOSEBOX),
		NWidget(WWT_CAPTION, COLOUR_BROWN, SLW_CAPTION), SetDataTip(STR_SUBSIDIES_CAPTION, STR_TOOLTIP_WINDOW_TITLE_DRAG_THIS),
		NWidget(WWT_STICKYBOX, COLOUR_BROWN, SLW_STICKYBOX),
	EndContainer(),
	NWidget(NWID_HORIZONTAL),
		NWidget(WWT_PANEL, COLOUR_BROWN, SLW_PANEL), SetMinimalSize(308, 113), SetDataTip(0x0, STR_SUBSIDIES_TOOLTIP_CLICK_ON_SERVICE_TO_CENTER), SetResize(1, 1), EndContainer(),
		NWidget(NWID_VERTICAL),
			NWidget(WWT_SCROLLBAR, COLOUR_BROWN, SLW_SCROLLBAR),
			NWidget(WWT_RESIZEBOX, COLOUR_BROWN, SLW_RESIZEBOX),
		EndContainer(),
	EndContainer(),
};

static const WindowDesc _subsidies_list_desc(
	WDP_AUTO, WDP_AUTO, 320, 127, 320, 127,
	WC_SUBSIDIES_LIST, WC_NONE,
	WDF_STD_TOOLTIPS | WDF_STD_BTN | WDF_DEF_WIDGET | WDF_STICKY_BUTTON | WDF_RESIZABLE,
	_subsidies_list_widgets, _nested_subsidies_list_widgets, lengthof(_nested_subsidies_list_widgets)
);


void ShowSubsidiesList()
{
	AllocateWindowDescFront<SubsidyListWindow>(&_subsidies_list_desc, 0);
}
