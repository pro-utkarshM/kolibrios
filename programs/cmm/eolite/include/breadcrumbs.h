
//===================================================//
//                                                   //
//                      PATH                         //
//                                                   //
//===================================================//

char work_area_pointer[1024];
PathShow_data PathShow = {0, 17,250, 6, 250, 0, 0, 0x000000, 0xFFFFCC, #path, #work_area_pointer, 0};
void DrawPathBar()
{
	if (efm) {
		DrawPathBarKfm();
		return;
	}

	if (show_breadcrumb.checked) {
		DrawBreadCrumbs(); 
		return;
	}

	PathShow.start_x = 250;
	PathShow.start_y = 17;
	PathShow.area_size_x = Form.cwidth-300;
	DrawBar(PathShow.start_x-3, PathShow.start_y-6, PathShow.area_size_x+3, 19, col.odd_line);
	DrawRectangle(PathShow.start_x-4,PathShow.start_y-7,PathShow.area_size_x+4,20,sc.work_graph);
	DefineHiddenButton(PathShow.start_x-4+1,PathShow.start_y-7+1,PathShow.area_size_x+4-2,20-2,PATH_BTN);
	DrawBar(PathShow.start_x-4, PathShow.start_y+14, PathShow.area_size_x+5+18, 1, sc.work_light);

	DrawFlatButtonSmall(PathShow.start_x+PathShow.area_size_x,PathShow.start_y-7,18,20, 61, "\26");

	PathShow.font_color = col.list_gb_text;
	PathShow_prepare stdcall(#PathShow);
	PathShow_draw stdcall(#PathShow);
}

void DrawPathBarKfm()
{
	dword bgc;
	int i=0;
	if (!Form.cwidth) return;

	if (skin_is_dark()) {
		bgc = col.odd_line;
		PathShow.font_color = col.list_gb_text;
	} else {
		bgc = 0xFFFFCC; 
		PathShow.font_color = 0x222222;
	}
	if (active_panel==1) PathShow.text_pointer = #path; else PathShow.text_pointer = #inactive_path;
	PathShow.start_x = 4;
	PathShow.area_size_x = Form.cwidth/2-8;
	PathShow.start_y = Form.cheight - status_bar_h+2;

	_DRAW_BAR:
	DrawBar(PathShow.start_x-2,PathShow.start_y-3,PathShow.area_size_x+5,14,bgc);
	DrawRectangle(PathShow.start_x-3,PathShow.start_y-4,PathShow.area_size_x+6,15,sc.work_graph);
	PathShow_prepare stdcall(#PathShow);
	PathShow_draw stdcall(#PathShow);
	i++;
	if (i<2) {
		if (active_panel==1) PathShow.text_pointer = #inactive_path; else PathShow.text_pointer = #path;
		PathShow.start_x = Form.cwidth/2 + 2;
		PathShow.area_size_x = Form.cwidth - PathShow.start_x - 5;
		goto _DRAW_BAR;
	}

	DrawBar(0,PathShow.start_y-2,1,15,sc.work);
	DrawBar(Form.cwidth-1,PathShow.start_y-2,1,15,sc.work);
	DrawBar(1,PathShow.start_y+12,Form.cwidth-2,1,sc.work_light);
}

void DrawPathBarKfm_Line()
{

}


//===================================================//
//                                                   //
//                   BREADCRUMBS                     //
//                                                   //
//===================================================//

void DrawBreadCrumbs()
 collection_int breadCrumb=0;
 char PathShow_path[4096];
 block btn;
 int i;
 unsigned text_line;
 {
	strcat(#PathShow_path, #path);
	for (i=0; i<50; i++) DeleteButton(i+BREADCRUMB_ID);
	breadCrumb.drop();
	for (i=0; (PathShow_path[i]) && (i<sizeof(PathShow_path)-1); i++) 
	{
		if (PathShow_path[i]=='/') {
			PathShow_path[i] = NULL;
			breadCrumb.add(i+1);
		}
	}
	breadCrumb.add(i+1);
	btn.set_size(246,10,NULL,20);
	//area_w = Form.cwidth - btn.x - 20;
	for (i=0; i<breadCrumb.count-1; i++)
	{
		text_line = breadCrumb.get(i) + #PathShow_path;
		btn.w = strlen(text_line)*8+10;
		DrawBreadcrumbButton(btn.x, btn.y, btn.w, btn.h, i+BREADCRUMB_ID, text_line);
		btn.x += btn.w;
	}
	//DrawFavButton(btn.x);
	//btn.x+=20;
	btn.x++;
	DrawBar(btn.x,btn.y-1,Form.cwidth-btn.x-25,btn.h+3,sc.work);
}


void ClickOnBreadCrumb(unsigned clickid)
{
	int i, slashpos = #path;
	for (i=0; i!=clickid+2; i++) {
		slashpos=strchr(slashpos,'/')+1;
	}
	ESBYTE[slashpos-1] = NULL;
	Open_Dir(#path,WITH_REDRAW);
}


void DrawBreadcrumbButton(dword x,y,w,h,id,text)
{
	int i;
	DrawRectangle(x,y,w,h,sc.work_graph);
	for (i=0; (i<h-1) & (i<20); i++) DrawBar(x+1, y+i+1, w-1, 1, col.work_gradient[20-i]);
	DrawRectangle3D(x+1,y+1,w-2,h-2,sc.work_light, sc.work_dark);
	DefineHiddenButton(x+1,y+1,w-2,h-2,id);
	WriteText(-strlen(text)*8+w/2+x,h/2+y-7,0x90,sc.work_text,text);
	DrawBar(x, y+h+1, w+1, 1, sc.work_light);
}