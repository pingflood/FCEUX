
// Externals
extern Config *g_config;

/* MENU COMMANDS */

// Overclock enabled
static void ocenabled_update(unsigned long key)
{
	int val;
	g_config->getOption("SDL.OverclockEnabled", &val);

	if (key == DINGOO_RIGHT) val = 1;
	if (key == DINGOO_LEFT) val = 0;

	g_config->setOption("SDL.OverclockEnabled", val);
}

// Skip 7bit overlocking
static void skip7bitoc_update(unsigned long key)
{
	int val;
	g_config->getOption("SDL.Skip7BitOverclocking", &val);

	if (key == DINGOO_RIGHT) val = 1;
	if (key == DINGOO_LEFT) val = 0;

	g_config->setOption("SDL.Skip7BitOverclocking", val);
}

// Vblank Scanlines
static void vblanksls_update(unsigned long key)
{
	int val;
	g_config->getOption("SDL.VBlankScanLines", &val);

	if (key == DINGOO_RIGHT) val = val < 520 ? val+10 : 520;
	if (key == DINGOO_LEFT) val = val > 0 ? val-10 : 0;

	g_config->setOption("SDL.VBlankScanLines", val);
}

// Post-render Scanlines
static void postrendersls_update(unsigned long key)
{
	int val;
	g_config->getOption("SDL.PostRenderScanLines", &val);

	if (key == DINGOO_RIGHT) val = val < 520 ? val+10 : 520;
	if (key == DINGOO_LEFT) val = val > 0 ? val-10 : 0;

	g_config->setOption("SDL.PostRenderScanLines", val);
}

/* TIMING SETTINGS MENU */
static SettingEntry oc_menu[] =
{
	{ "Overclock Enabled", "Enables Overclock settings", "SDL.OverclockEnabled", ocenabled_update },
	{ "Skip 7bit overclocking", "Don't Overclock 7bit samples", "SDL.Skip7BitOverclocking", skip7bitoc_update },
	{ "VBlank Scanlines", "# of extra VBlank scanlines", "SDL.VBlankScanLines", vblanksls_update },
	{ "Post-rend Scanlines", "# of post-render scanlines", "SDL.PostRenderScanLines", postrendersls_update },
};

int RunOverclockSettings()
{
	static int index = 0;
	static int spy = 72;
	int done = 0, y, i;

	char tmp[32];
	int  itmp;

	static const int menu_size = sizeof(oc_menu) / sizeof(oc_menu[0]);
	static const int max_entries = 8;
	static int offset_start = 0;
	static int offset_end = menu_size > max_entries ? max_entries : menu_size;

	g_dirty = 1;
	while (!done) {
		// Parse input
		readkey();
		if (parsekey(DINGOO_B)) done = 1;

		if (parsekey(DINGOO_UP, 1)) {
			if (index > 0) {
				index--;

				if (index >= offset_start)
					spy -= 15;

				if ((offset_start > 0) && (index < offset_start)) {
					offset_start--;
					offset_end--;
				}
			} else {
				index = menu_size-1;
				offset_end = menu_size;
				offset_start = menu_size <= max_entries ? 0 : offset_end - max_entries;
				spy = 72 + 15*(index - offset_start);
			}
		}

		if (parsekey(DINGOO_DOWN, 1)) {
			if (index < (menu_size - 1)) {
				index++;

				if (index < offset_end)
					spy += 15;

				if ((offset_end < menu_size) && (index >= offset_end)) {
					offset_end++;
					offset_start++;
				}
			} else {
				index = 0;
				offset_start = 0;
				offset_end = menu_size <= max_entries ? menu_size : max_entries;
				spy = 72;
			}
		}

		if (parsekey(DINGOO_RIGHT, 1) || parsekey(DINGOO_LEFT, 1)) {
			oc_menu[index].update(g_key);
		}

		// Draw stuff
		if( g_dirty )
		{
			draw_bg(g_bg);

			//Draw Top and Bottom Bars
			DrawChar(gui_screen, SP_SELECTOR, 0, 37);
			DrawChar(gui_screen, SP_SELECTOR, 81, 37);
			DrawChar(gui_screen, SP_SELECTOR, 0, 225);
			DrawChar(gui_screen, SP_SELECTOR, 81, 225);
			DrawText(gui_screen, "B - Go Back", 235, 225);
			DrawChar(gui_screen, SP_LOGO, 12, 9);

			// Draw selector
			DrawChar(gui_screen, SP_SELECTOR, 56, spy);
			DrawChar(gui_screen, SP_SELECTOR, 77, spy);

			DrawText(gui_screen, "Overclock Settings", 8, 37);

			// Draw menu
			// for(i=0,y=72;i <= menu_size;i++,y+=15) {
			for (i = offset_start, y = 72; i < offset_end; i++, y += 15) {
				DrawText(gui_screen, oc_menu[i].name, 60, y);

				g_config->getOption(oc_menu[i].option, &itmp);
				if (!strncmp(oc_menu[i].name, "Overclock Enabled", 17)) {
					sprintf(tmp, "%s", itmp ? "on" : "off");
				} else if(!strncmp(oc_menu[i].name, "Skip 7bit overclocking", 22)) {
					sprintf(tmp, "%s", itmp ? "on" : "off");
                }
				else sprintf(tmp, "%d", itmp);

				DrawText(gui_screen, tmp, 210, y);
			}

			// Draw info
			DrawText(gui_screen, oc_menu[index].info, 8, 225);

			// Draw offset marks
			if (offset_start > 0)
				DrawChar(gui_screen, SP_UPARROW, 157, 62);
			if (offset_end < menu_size)
				DrawChar(gui_screen, SP_DOWNARROW, 157, 203);

			g_dirty = 0;
		}

		SDL_Delay(16);

		// Update real screen
		FCEUGUI_Flip();
	}

	// Clear screen
	dingoo_clear_video();

	g_dirty = 1;
	return 0;
}
