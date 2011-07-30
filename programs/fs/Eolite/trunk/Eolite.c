//Leency & Veliant -=- KolibriOS Team -=- 2011
//GNU GPL licence.


//�� ��������, ���� ������������� � /rd/1/ -----   ������� 392
//��� �������� ��������� � ���������� �������� ��������� ���� � �����

#codesize  
#include "lib\kolibri.h--"
#include "lib\memory.h--"
#include "lib\edit_box_lib.h--"
#include "lib\file_system.h--" 
#include "imgs\toolbar.txt"
#include "imgs\left_p.txt"

//
#define NOTIFY_PATH	"@notify"
#define INI_PATH	"/sys/File managers/Eolite.ini"


//����������
#define title "Eolite File Manager v0.98.9"
#define videlenie 0x94AECE //0xFEA4B7,0x8BCDFF,0xB8C9B8}; //���� ����������� �������� �� ������ ������
byte toolbar_buttons_x[6]={9,46,85,134,167,203};
//
int  but_num,
     count,
     za_kadrom,
     curbtn,
     scroll_size;
byte cut_active,
     rename_active,
     del_active;
byte show_actions=1,
     sort_num=2,
     isdir;
char path[4096]="/rd/1/",
     edit_path[4096],
     file_path[4096],
     file_name[4096],
     copy_file[4096],
     temp[4096];
byte PathHistory[4096];

dword file_mas[6898];
int j, i, mouse_dd;

dword stak[100]=0; //���� About 

edit_box edit1= {250,247,13,0xffffff,videlenie,0xD3DDEB,0xffffff,0,248,#edit_path,#mouse_dd,64,6,6};
edit_box edit2= {250,213,80,0xFFFFCC,videlenie,0xFFFFCC,0xffffff,0,248,#file_name,#mouse_dd,64,6,6};

proc_info Form;
dword buf, off; //��� ������ � ������
dword devbuf, dev_num;
#include "include\LVabout.h--"
#include "include\sorting.h--"
#include "include\icons_f.h--"
#include "include\ini.h--"


void main() 
word key, id; 
mouse m;
int pressed_y;
{
	GetIni(1);
	load_editbox_lib();
	devbuf= malloc(3112); //����� ���-�� �� 10 �������� � ����� ������
	ReadDir(10, devbuf, "/");
	dev_num=EBX;
	IF (param[0]<>'')
	{
		copystr(#param,#edit_path);
		Goto_edit_path();
	} ELSE Open_Dir(#path,2); //��� �� ������ � ����������
	loop()
	{
		switch(WaitEvent())
		{
			CASE evMouse:
					IF (del_active==1) break;
					m.get();

					if (m.x>=Form.width-21) && (m.x<=Form.width-6) && (m.y>40) && (m.y<57)
					{
						IF (m.lkm==1) DrawRegion_3D(onLeft(26,0),41,14,14,0xC7C7C7,0xFFFFFF);
						WHILE (m.lkm==1) && (za_kadrom>0)
						{
							Pause(10);
							za_kadrom--;
							List_ReDraw();
							m.get();
						}
						DrawRegion_3D(onLeft(26,0),41,14,14,0xFFFFFF,0xC7C7C7);
					}

					if (m.x>=Form.width-21) && (m.x<=Form.width-6) && (m.y>onTop(22,0)+1) && (m.y<onTop(22,0)+16)
					{
						IF (m.lkm==1) DrawRegion_3D(onLeft(26,0),onTop(21,0),14,14,0xC7C7C7,0xFFFFFF);
						while (m.lkm==1) && (za_kadrom<count-but_num)
						{
							Pause(10);
							za_kadrom++;
							List_ReDraw();
							m.get();
						}
						DrawRegion_3D(onLeft(26,0),onTop(21,0),14,14,0xFFFFFF,0xC7C7C7);
					}

					//������� ����
					IF (m.vert==65535) IF (curbtn==0) FileList_ReDraw(-3); ELSE FileList_ReDraw(-1);
					IF (m.vert==1) IF (curbtn==but_num-1) FileList_ReDraw(3); ELSE FileList_ReDraw(1);
					//������
					if (m.x>=Form.width-21) && (m.x<=Form.width-6) while (m.lkm)
					{
						IF (scroll_size/2+57>m.y) || (m.y<0) || (m.y>4000) m.y=scroll_size/2+57; //���� ������ ��� �����
						id=za_kadrom; //��������� ������ ����������
						j= scroll_size/2;
						za_kadrom = m.y -j -57 * count;	za_kadrom /= onTop(22,57);
						IF (but_num+za_kadrom>count) za_kadrom=count-but_num;
						IF (id<>za_kadrom) List_ReDraw(); //���� ������ ��� �� ��������������
						m.get();
					}
					break;  
//Button pressed-----------------------------------------------------------------------------
			case evButton:
				IF (edit1.flags<>64) {edit1.flags=64; edit_box_draw stdcall(#edit1);} //���������� ��������� ��� ������� �� �����
				id=GetButtonID();
				IF (id==1) ExitProcess();
				IF (del_active==1) {IF (id==301) || (id==302) Del_File(302-id); break;}
				switch(id) 
				{
					CASE 21: GoBack(); break;//�����
					CASE 23: IF (strcmp(#path,"/")<>0) Dir_Up(); break;//up!
					CASE 24: key=24; //cut
					CASE 25: goto CTRLC_MARK; //copy
					CASE 26: Paste(); break;//paste
					CASE 27: Goto_edit_path(); break; //goto edit_path
					CASE 31...33: //sort
							IF (sort_num==id-30) break;
							IF(sort_num==1)DrawBar(onLeft(192,168)/2+210,45,6,9,0xE4DFE1);
							IF(sort_num==2)DrawBar(onLeft(115,0),45,6,9,0xE4DFE1);
							IF(sort_num==3)DrawBar(onLeft(44,0),45,6,9,0xE4DFE1);
							sort_num=id-30;
							Open_Dir(#path,1);
							break; 
					case 30: //about
						CreateThread(#authors,#stak); 
						break;
					case 77: //actions
						IF (show_actions==1) show_actions=0; ELSE show_actions=1;
						Actions();
						break;
					case 80: //rename
						goto REN_MARK; 
					case 81: //Delete file
						Del_Form();
						break;
					case 82: //create folder
						NEW_FOLDER_MARK:
						copystr(#path, #temp);
						copystr("New folder", #temp+strlen(#temp));
						CreateFolder(#temp);
						IF (EAX==0){
							SelectFile("New folder");
							goto REN_MARK;
						}
						ELSE  ShowMessage("Folder can not be created.");
						break;
					case 100...120:
						DEVICE_MARK:
						copystr(id-100*304+ devbuf+72, #path);
						IF (path[0]=='r') copystr("/rd/1",#file_path);
						ELSE {
								copystr("/", #file_path);
								copystr(#path, #file_path+strlen(#file_path));
							 }
						GOTO OPEN_DEV;
					default:
						if (id<200) break; //������ �� ������ ������
						IF (curbtn!=id-201) {FileList_ReDraw(id-201-curbtn); break;}
						else OPEN_MARK:
						if (!isdir) GetIni(0); ELSE
						IF (strcmp(#file_name,"..")==0) Dir_Up(); ELSE
						{	OPEN_DEV:
								copystr(#file_path, #path);
								copystr("/", #path+strlen(#path));
								za_kadrom=curbtn=0;
								Open_Dir(#path,1);
						}
				}
				break;
//Key pressed-----------------------------------------------------------------------------
			case evKey:
				key = GetKey();
				IF (del_active==1)
					{
					IF (key==013) Del_File(true);
					IF (key==027) Del_File(false);
				 	break;
				 	}
				IF (edit1.flags<>64) && (edit1.flags<>0) && (key<>13) {EAX=key<<8; edit_box_key stdcall (#edit1); break;} 
                IF (edit2.flags<>64) && (key<>13) && (key<>27) {EAX=key<<8; edit_box_key stdcall (#edit2); break;}
				switch (key)
				{
						case 209...217:
								id=key-110;
								IF (id-100>=dev_num) break;
								GOTO DEVICE_MARK;
						case   8: GoBack(); break; //�����
						case 014: MoveSize(80,80,OLD,OLD); RunProgram("/sys/File Managers/Eolite", #path); break; //����� ����
						case 024: //Ctrl+X cut
						case 003: //Ctrl+C copy
								CTRLC_MARK:
								IF (isdir) break; //����� ���� ��� ���������� �� �����
								copystr(#file_path,#copy_file); //��������� ����� ���� ����������
								IF (key==24) cut_active=1; ELSE cut_active=0; 
								break;
						case 022: Paste(); break;//Ctrl+V paste
						case 027: //Esc
								IF (rename_active==1) ReName(false);
								break;
						case 013: //Enter
								IF (rename_active==1) {ReName(true); break;}
								IF (strcmp(#path,#edit_path)<>0) Goto_edit_path();
								ELSE GOTO OPEN_MARK;
								break; 
						case 55: //F6 - new folder
								goto NEW_FOLDER_MARK;
						case 56:  //IF (rename_active==1) break;//up
						case 178: //up
								FileList_ReDraw(-1);
								break;
						case 177: //down
								FileList_ReDraw(1);
								break;
						case 180: //home
								FileList_ReDraw(-za_kadrom-curbtn);
								break;
						case 181: //end
								FileList_ReDraw(count-za_kadrom-curbtn+but_num-1);
								break;
						case 183: //Page Down
								FileList_ReDraw(but_num-1);
								break;
						case 184: //Page Up
								FileList_ReDraw(-but_num+1);
								break;
						case 051: //������ F2
								REN_MARK:
								DeleteButton(curbtn+201); //��� ���� ����� ���� �������� �����
								edit2.flags=66; //������ ��������� ��������
								edit2.width=onLeft(24,217);
								edit2.top=curbtn*18+59;
								edit2.size=edit2.pos=strlen(#file_name);
								edit_box_draw  stdcall (#edit2);
								DrawBar(213,curbtn*18+58,edit2.width+1,1,0xFFFFCC); //������ ������ ������ ��� ������������
								rename_active=1;
								break;
						case 052: //������ F3
								IF (isdir==false) RunProgram("/sys/tinypad", #file_path);
								break;
						case 053: //������ F4
								IF (isdir==false) RunProgram("/sys/develop/heed", #file_path);
								break;
						case 054: //F5
								Open_Dir(#path,1);
								break;
						case 182: //delete file
								Del_Form();
								break; 
						default:    
								//FOR (i=curbtn+za_kadrom+1; i<count; i++)
								for (i=curbtn+za_kadrom+1; i<count; i++)
								{
									copystr(file_mas[i]*304+buf+72,#temp);
									AL=DSBYTE[#temp]; 
									IF(AL>='A')&&(AL<='Z')DSBYTE[#temp]=AL|0x20;
									IF (temp[0]==key) {FileList_ReDraw(i-curbtn-za_kadrom); break;}
								}
				}                         
				BREAK;
				case evReDraw:	draw_window();
		}
		IF (rename_active==0) edit_box_mouse stdcall(#edit1); ELSE edit_box_mouse stdcall(#edit2);
	}
}


inline fastcall void draw_window()
{                
	WindowRedrawStatus(1);
	DefineAndDrawWindow(100,100,600,410,0x73,0x10E4DFE1,0,0,title); 
	WindowRedrawStatus(2);
	Form.GetInfo(SelfInfo);
	IF (Form.height==GetSkinWidth()+3) return; //������ �� ������ ���� ���� ��������� � ���������
	  IF (Form.height<280) MoveSize(OLD,OLD,OLD,280);
	  IF (Form.width<480) MoveSize(OLD,OLD,480,OLD);
	//toolbar buttons
	PutPaletteImage(#toolbar,246,39,0,0,#toolbar_pal);
	FOR (j=0; j<6; j++) DefineButton(toolbar_buttons_x[j],5,31,29,21+j+BT_HIDE,0xE4DFE1);
	//������ ������
	edit1.width=Form.width-314;
	DrawBar(246,0,onLeft(246,60),12,0xE4DFE1); //��� ��� ������� ������
	DrawBar(246,29,onLeft(246,60),10,0xE4DFE1); //��� ��� ������� ������
	DrawRegion_3D(246,12,onLeft(66,246),16,0x94AECE,0x94AECE);	//������
	DefineButton(onLeft(34,0),6,27,28,30+BT_HIDE+BT_NOFRAME,0xE4DFE1); //about
	DefineButton(onLeft(66,0),12,18,16,27+BT_HIDE,0xE4DFE1); //������ ��������
	PutPaletteImage(#goto_about,56,40,Form.width-65,0,#goto_about_pal);
	//�������������� ������
	DrawRegion_3D(1,40,Form.width-12,onTop(46,0),0x94AECE,0x94AECE); //����� ������
	DrawRegion_3D(0,39,Form.width-10,onTop(44,0),0xE4DFE1,0xE4DFE1); //���
	Devices(); //������ ����� �� ����� ����
	//SortButtons
	DrawFlatButton(192,40,onLeft(192,168),16,31,0xE4DFE1,"File");
	DrawFlatButton(onLeft(168,0),40,73,16,32,0xE4DFE1,"Type");
	DrawFlatButton(onLeft(95,0),40,68,16,33,0xE4DFE1,"Size");
	//�������������� ������
	Open_Dir(#path,ONLY_SHOW);
	//���������
	DrawBar(onLeft(27,0),57,1,onTop(22,57),0x94AECE); //����� ����� �� ��������� 
	DrawFlatButton(onLeft(27,0),40,16,16,0,0xE4DFE1,"\x18");		//��������� �����
	DrawFlatButton(onLeft(27,0),onTop(22,0),16,16,0,0xE4DFE1,"\x19");//��������� ����
}


void KEdit()
{
	edit1.size=edit1.pos=strlen(#edit_path);
	edit_box_draw  stdcall (#edit1);
}


inline fastcall void TVScroll() { //���������
	dword on_y;
	IF (count<=0) {DrawFlatButton(onLeft(27,0),57,16,onTop(22,58),0,0xE4DFE1,""); return;}
	on_y = za_kadrom * onTop(22,57) / count +57;
	scroll_size=onTop(22,57) * but_num - but_num / count;
	IF (scroll_size<20) scroll_size = 20; //������������� ����������� ������ �������
	IF (scroll_size>onTop(22,57)-on_y+56) || (za_kadrom+but_num>=count) on_y=onTop(23+scroll_size,0); //��� �������� ������ 
	DrawFlatButton(onLeft(27,0),on_y,16,scroll_size,0,0xE4DFE1,"");//��������
	DrawBar(onLeft(26,0),57,15,on_y-57,0xCED0D0);//���� �� ��������
	DrawBar(onLeft(26,0),on_y+scroll_size+1,15,onTop(22,57)-scroll_size-on_y+56,0xCED0D0); //���� ����� ��������
}


void FileList_ReDraw(int curbtn_)
{
	if (curbtn_<=0) //�����
	{
		IF (za_kadrom==0) && (curbtn<=0) return;
		IF (-curbtn_-1<curbtn)
		{
			Line_ReDraw(0xFFFFFF, curbtn); //����� ������
			curbtn+=curbtn_;
			Line_ReDraw(videlenie, curbtn); //���������
			return;
		}
		ELSE
		{
			IF (-curbtn_<za_kadrom) za_kadrom+=curbtn_; ELSE za_kadrom=0;
			curbtn=0;
			List_ReDraw();
			return;
		}
	}
	else  //����
	{
		IF (za_kadrom==count-but_num) && (curbtn==but_num-1) return;
		IF (but_num-curbtn>curbtn_) // 18-0>33?
		{
			Line_ReDraw(0xFFFFFF, curbtn); //����� ������
			curbtn+=curbtn_;
			Line_ReDraw(videlenie, curbtn); //���������
			return;
		}
		//WriteDebug(IntToStr(curbtn)); =0
		//WriteDebug(IntToStr(za_kadrom)); =0
		//WriteDebug(IntToStr(count)); =41
		ELSE
		{
			IF(but_num+za_kadrom+curbtn_>=count) //18+0+33>=41
			{
				za_kadrom=count-but_num; //41-18=23
				//curbtn=but_num+curbtn_-count; //11=33-18
				}
			ELSE
			{
				za_kadrom+=curbtn_+curbtn-but_num+1;
				//curbtn=but_num-1;
			}
			curbtn=but_num-1;
			List_ReDraw();
		}
	}
}


void List_ReDraw()
{
	int paint_x=but_num*18+57;
	IF (count-za_kadrom<but_num) || (curbtn>but_num-1) //���� �� � ����� ������ ������ ��������� ���� ��������� ������ ����� ������
	{ za_kadrom=count-but_num; curbtn=but_num-1; } //��� ���� ��������� ����� ����������� ���� �� ������
	//
	FOR (j=0; j<but_num; j++) IF (curbtn<>j) Line_ReDraw(0xFFFFFF, j); ELSE Line_ReDraw(videlenie, curbtn);
		DrawBar(192,paint_x,onLeft(27,192),onTop(paint_x,6),0xFFFFFF); //������� ����� �������
		DrawBar(onLeft(168,0),paint_x,1,onTop(paint_x,6),0xE4DFE1); //������ ����� ������������ 1
		DrawBar(onLeft(95,0),paint_x,1,onTop(paint_x,6),0xE4DFE1); //������ ����� ������������ 2
	TVScroll();
}


//puticon+icon_fairing!!!
void Line_ReDraw(dword color, filenum){
	dword text_col=0, temp_int,
	y=filenum*18+57; //��������� ������ �� Y
	IF (rename_active==1) ReName(false);
	//��, � �� ����� ��� ������, �� ����������� ���� ���������
	DrawBar(192,y,3,18,color); DrawBar(195,y,16,2,color); 
	DrawBar(192+19,y,onLeft(46,192),18,color); DrawBar(195,y+17,16,1,color);
	//
	off=file_mas[filenum+za_kadrom]*304 + buf+72;
	if (TestBit(ESDWORD[off-40],1)) || (TestBit(ESDWORD[off-40],2)) text_col=0xA6A6B7;
	if (!TestBit(ESDWORD[off-40],4))
	{
		copystr(off,#temp);
		temp_int = Put_icon(#temp+find_symbol(#temp,'.'), y+2);
		WriteText(7-strlen(ConvertSize(ESDWORD[off-8]))*6+onLeft(75,0),y+6,0x80,0,ConvertSize(ESDWORD[off-8]),0); //size
	} ELSE IF (!strcmp("..",off)) temp_int=Put_icon("..", y+2); ELSE temp_int=Put_icon("<DIR>", y+2);
	if (color==videlenie)
	{
		isdir=TestBit(ESDWORD[off-40], 4);
		IconFairing(temp_int, y+2); //����������� ������
		copystr(off,#file_name);
		copystr(#path,#file_path);
		copystr(#file_name,#file_path+strlen(#file_path)); //������ ���� � �����
		IF (text_col==0xA6A6B7) text_col=0xFFFFFF;
	}
	temp_int = onLeft(215,165)/6;
	IF (strlen(off)<temp_int) temp_int = strlen(off);  //������ �������� �����
		WriteText(215,y+6,0,text_col,off,temp_int);  //��� �����
	DrawBar(onLeft(168,0),y,1,18,0xE4DFE1); //������ ����� ������������ 1
	DrawBar(onLeft(95,0),y,1,18,0xE4DFE1); //������ ����� ������������ 2
}


void Open_Dir(dword temp_, redraw){
	int errornum;
	byte path_[256],
	somelen=strlen(temp_)-1;
	if (redraw<>ONLY_SHOW)
	{
		copystr(temp_, #path_);
		IF (somelen) path_[somelen]=0x00;
		//
		IF (buf) free(buf);
    		buf = malloc(32);
		errornum=ReadDir(0, buf, #path_);
		if (errornum<>0) //������ ��� ������ �����
		{
			//WriteDebug(#path_);
			Write_Debug_Error(errornum);
			HistoryPath(add_new_path);
			GoBack();
			return;
		}
    		count = ESDWORD[buf+8];
    		buf = realloc(count * 304 + 32, buf);
		ReadDir(count, buf, #path_);
		IF (EBX>6897) && (EBX<>-1) EBX=6897; //1 �� ����� ;)
		count=EBX;
	}  
	if (count<>-1)
	{
		copystr(temp_,#edit_path);
		KEdit();
		HistoryPath(add_new_path);
		IF (!strcmp(".",buf+72)) {memmov(buf,buf+304,count-1*304); count--;} //��������� ������� "."
		FOR (j=0;j<but_num;j++) DeleteButton(201+j); //������� ������
		but_num=onTop(6,57)/18;                                                                                             
		IF (count<but_num) but_num=count;
		FOR (j=0;j<but_num;j++) DefineButton(192,j*18+57,onLeft(27,192),18,201+j+BT_HIDE+BT_NOFRAME,0xFFFFFF); //����� �����
		//��������� ����������
		IF (sort_num==1) WriteText(Form.width+60/2,45,0x80,0x4E78AC,"\x19",0);
		IF (sort_num==2) WriteText(Form.width-115,45,0x80,0x4E78AC,"\x19",0);
		IF (sort_num==3) WriteText(Form.width-44,45,0x80,0x4E78AC,"\x19",0);
		IF (redraw<>ONLY_SHOW) Sorting(); //��� ������� ����� ��� �������� ����
		IF (redraw<>ONLY_OPEN) List_ReDraw();
	}
	IF (count==-1) && (redraw<>ONLY_OPEN) {but_num=count=0; List_ReDraw();}
}



inline Sorting()
{
	dword k=0, l=1;
	IF (!strcmp(#path,"/")) {FOR(k=1;k<count;k++;)file_mas[k]=k; return;} //�� ����������� ����� � �� ������ ������� � "/"
	FOR (j=count-1, off=count-1*304+buf+32; j>=0; j--, off-=304;)  //����� �����, ����� ����
	{
		ChangeCase(off+40);
		IF (TestBit(ESDWORD[off],4)) {file_mas[k]=j; k++;}
			ELSE {file_mas[count-l]=j; l++;}
	}
	//���������� ����������: ������� �����, ����� �����
	Sort_by_Name(0,k-1);
	IF (sort_num==1) Sort_by_Name(k,count-1);
	IF (sort_num==2) Sort_by_Type(k, count-1);
	IF (sort_num==3) Sort_by_Size(k,count-1);
	//���� ����� ".." �� ������, ������ � ����
	IF (k>0) && (strcmp(file_mas[0]*304+buf+72,"..")<>0) FOR(k=k-1; k>0; k--;) IF (!strcmp(file_mas[k]*304+buf+72,"..")) file_mas[k]><file_mas[0];
}


void Del_Form()
{
	int dform_x=Form.width/2-13;
	//���� ����
	FOR (i=5;i<11;i++) DeleteButton(201+i); //������� ������ ��� ������
	DrawFlatButton(dform_x,160,200,80,0,0xE4DFE1, ""); //�����
	WriteText(dform_x+19,175,0x80,0,"Do you really want to delete",0);
	IF (strlen(#file_name)<28) 
		{
			WriteText(strlen(#file_name)*6+dform_x+20,190,0x80,0,"?",0);
			WriteText(dform_x+20,190,0x80,0,#file_name,0); //����� ���
		}
	ELSE
		{
			WriteText(164+dform_x,190,0x80,0,"...?",0);
			WriteText(dform_x+20,190,0,0,#file_name,24); //����� ���
		}
	//��������
	DrawFlatButton(dform_x+20,208,70,20,301,0xFFB6B5,"Yes");
	DrawFlatButton(dform_x+111,208,70,20,302,0xC6DFC6,"No");
	del_active=1;
}

	
void Del_File(byte dodel)
{    
	int del_file_rez;
	IF (dodel==true)
	{
		del_file_rez=DeleleFile(#file_path);
		IF (del_file_rez<>0)
		{
			Write_Debug_Error(del_file_rez);
		IF (isdir==true) ShowMessage("Error. Folder isn't empty.");
			IF (isdir==false) ShowMessage("Error. Filesystem read-only.");
		}
 	}
	del_active=0;
	DeleteButton(301); DeleteButton(302); //������� �������� Yes/No
	Open_Dir(#path,1);
}    


void Paste()
{
	IF (!copy_file) return; //������, ��� ��� ���� �� �����������
	copystr(#path,#temp);
	copystr(#copy_file+find_symbol(#copy_file,'/'),#temp+strlen(#temp));
	if (strcmp(#copy_file,#temp)==0) //���� �� �������� � ��������� � ����� � ��� �� �����
		{
			copystr(#path,#temp);
			copystr("new_",#temp+strlen(#temp));
			copystr(#copy_file+find_symbol(#copy_file,'/'),#temp+strlen(#temp));
		}
	CopyFile(#copy_file,#temp);
	IF (EAX<>0) //������
	{
		Write_Debug_Error(EAX);
		DrawFlatButton(Form.width/2-13,160,200,80,0,0xFFB6B5, "Error. You can't paste here.");
		Pause(150);
	}
	IF (cut_active==1) //���� �� ������� ��������
		{
			copystr(#copy_file,#file_path);
			Del_File(true);
			copy_file='';
			cut_active=0;
		}
	SelectFile(#copy_file+find_symbol(#copy_file,'/'));
}


void Tip(int y, dword caption, id, arrow)
{
	DrawBar(17,y,160,17,0xE4DFE1); //����� ������
	WriteText(25,y+5,0x80,0,caption,0);		//text Goto:
	IF (id<>0) DefineButton(159,y+1,16,16,id+BT_HIDE+BT_NOFRAME,0xE4DFE1); //����� ��� �������
	WriteText(165,y+5,0x80,0,arrow,0); //������� ����
	DrawBar(17,y+17,160,1,0x94AECE);		//�������������
}


void ReName(byte rename)
{
	rename_active=0;
	edit2.flags=64;
	DefineButton(192,curbtn*18+57,onLeft(27,192),18,curbtn+201+BT_HIDE+BT_NOFRAME,0xFFFFFF);
	if (rename==true)
	{
		copystr(#path,#temp);
		copystr(#file_name,#temp+strlen(#temp));
		if (strcmp(#file_path,#temp)<>0) && (file_name)
		IF (isdir)
		{
			IF (DeleleFile(#file_path)<>0) ShowMessage("Error. Folder isn't empty.");
			ELSE CreateFolder(#temp);
			Open_Dir(#path,1);
		}
		ELSE
		{
			CopyFile(#file_path,#temp);
			Del_File(true);
		}
		SelectFile(#temp+find_symbol(#path,'/'));
	}
	Line_ReDraw(videlenie,curbtn);
}


void Devices()
{          
	byte dev_icon; dword drive_name[30]; char dev_name[4];
	DrawBar(2,41,190,15,0x00699C);		//����� ������������� - ������
	DrawBar(2,56,15,onTop(21,41),0x00699C);	//����� ������������� - �����       
	DrawBar(177,56,15,onTop(21,41),0x00699C);	//����� ������������� - ������
	//������ ������
	Tip(56, "Goto:", 0, "");
	for (i=0;i<dev_num;i++)
	{
		DrawBar(17,i*16+74,160,17,0xFFFFFF); //�����
		DefineButton(17,i*16+74,159,16,100+i+BT_HIDE,0xFFFFFF); //������ ������, � ����� ������� �������� ������
		copystr("Unknown drive",#drive_name); //���������� ���������� ��������
		dev_icon=3; //��-��������� ���������� �������� ��� ������
		copystr(i*304+ devbuf+72, #dev_name);
		IF (dev_name[0]=='r')  { dev_icon=0; copystr("RAM-disk /rd/1",#drive_name); }
		IF (dev_name[0]=='c')  { dev_icon=1; copystr("CD-drive /",#drive_name); }
		IF (dev_name[0]=='f')  { dev_icon=2; copystr("Floppy disk /",#drive_name); }
		IF (dev_name[0]=='h')   copystr("Hard disk drive /",#drive_name); 
		IF (dev_name[0]=='b')   copystr("SATA disk drive /",#drive_name); 
		//
		IF (dev_icon<>0) copystr(#dev_name,#drive_name+strlen(#drive_name));
		copystr("/",#drive_name+strlen(#drive_name));
		//
		WriteText(45,i*16+79,0x80,0,#drive_name,0);
		PutImage(dev_icon*14*13*3+#devices,14,13,21,i*16+76);
	}
	Actions();  //������� ������ � �����
}


void Actions()
{
	int actions_y=dev_num*16;
	DrawBar(17,actions_y+75,160,15,0x00699C); //����� ������������� - ��� ���������
	if (show_actions==1)
	{
		Tip(actions_y+90, "Actions", 77, "\x19");
		DrawBar(17,actions_y+108,160,51,0xFFFFFF); //�����
		PutImage(#factions,16,44,21,actions_y+113); //�����������
		//rename file 
		DefineButton(22,actions_y+108,159,16,80+BT_HIDE,0xE4DFE1);
		WriteText(42,actions_y+113,0x80,0,"Rename file <F2>",0);
		//delete file
		DefineButton(17,actions_y+125,159,16,81+BT_HIDE,0xE4DFE1);
		WriteText(42,actions_y+130,0x80,0,"Delete file <Del>",0);
		//create folder
		DefineButton(17,actions_y+142,159,16,82+BT_HIDE,0xE4DFE1);
		WriteText(42,actions_y+147,0x80,0,"Create folder <F6>",0);
	}
	ELSE
	{
		DeleteButton(80);	DeleteButton(81);	DeleteButton(82);
		Tip(actions_y+90, "Actions", 77, "\x18");
	}
	DrawBar(17,show_actions*51+actions_y+108,160,onTop(show_actions*51+actions_y+108,6),0x00699C); //����� � �����
}



		/*WriteDebug(""); 
		WriteDebug(#path);
		WriteDebug("Number of files:"); WriteDebug(IntToStr(count)); 
		WriteDebug("but_num:");	 		WriteDebug(IntToStr(but_num));
		WriteDebug("curbtn");		 	WriteDebug(IntToStr(curbtn));
		WriteDebug("ra_kadrom:");	 	WriteDebug(IntToStr(za_kadrom));*/



void Goto_edit_path()
{
	IF (strcmp(#path,#edit_path)==0) return; //�������� �� � ���� �� �� �����
 	IF (strcmp(#edit_path+strlen(#edit_path)-1,"/")<>0) copystr("/",#edit_path+strlen(#edit_path)); //���� ���, + "/"
	Open_Dir(#edit_path,2); //��������� ���� ����� � �����, ��� �� ����� ���������� �� ���
	IF (count<1) KEdit(); ELSE //������ ������� ������ ������
		{za_kadrom=curbtn=0; copystr(#edit_path,#path);}//������ ������
	Open_Dir(#path,1);
}

void SelectFile(dword that_file)
{
	za_kadrom=curbtn=0; //����� ������
   	Open_Dir(#path,2);
	ChangeCase(that_file);
	FOR (i=count-1; i>=0; i--;) IF(strcmp(file_mas[i]*304+buf+72,that_file)==0) BREAK;
	IF (i>but_num-1) && (i>count-but_num)
	{
		za_kadrom=count-but_num;
		curbtn=i-count+but_num;
	}
	ELSE FileList_ReDraw(i);
	List_ReDraw();
}


void Dir_Up()
{
	byte temp_[256];
	i=strlen(#path)-1;
	path[i]=0x00;
	do i--; while (path[i]<>'/'); copystr(#path+i+1,#temp_); 
	path[i+1]=0x00;
	SelectFile(#temp_);
}


inline fastcall void GoBack()   //������� ������� ������� ����, � ����� �������� ��, ��� ��������
{
	i=strlen(#PathHistory)-1;
	IF (i<7) RETURN;
	PathHistory[i]=0x00;
	copystr(#PathHistory+find_symbol(#PathHistory,'/'),#temp); //����� � ������� ����
	HistoryPath(go_back); 
	SelectFile(#temp);
}


stop:
