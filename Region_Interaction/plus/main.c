#include "header.h"
#include "resource.h"
#include "globals.h"


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPTSTR lpCmdLine, int nCmdShow)

{
	MSG			msg;
	HWND		hWnd;
	WNDCLASS	wc;
	int			i;

	wc.style=CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc=(WNDPROC)WndProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hInstance;
	wc.hIcon=LoadIcon(hInstance,"ID_PLUS_ICON");
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+6);
	wc.lpszMenuName="ID_MAIN_MENU";
	wc.lpszClassName="PLUS";

	if (!RegisterClass(&wc))
		return(FALSE);

	hWnd=CreateWindow("PLUS","Region Growing",
		PLUS_CUSTOM_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT,0,400,400,NULL,NULL,hInstance,NULL);
	if (!hWnd)
		return(FALSE);

	ShowScrollBar(hWnd,SB_BOTH,FALSE);
	ShowWindow(hWnd,nCmdShow|SW_MAXIMIZE);
	UpdateWindow(hWnd);
	MainWnd=hWnd;

	ShowPixelCoords=0;

	strcpy(filename,"");
	OriginalImage=NULL;
	ROWS=COLS=0;

	/*intialze all the thread states and variables to zero */
	for(i=0;i<255;i++)
	{
		ThreadRun[i]=0;/*total threads = total regions possible = 255*/
		StepModeState[i]=0;
	}

	InvalidateRect(hWnd,NULL,TRUE);
	UpdateWindow(hWnd);

	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	return(msg.wParam);
}

BOOL CALLBACK PredicateDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	BOOL bSuccess;
	switch(Message)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON_OK:
			IntensityPredicate = GetDlgItemInt(hwnd, IDC_NUM_INT, &bSuccess, FALSE);
			DistancePredicate  = GetDlgItemInt(hwnd, IDC_NUM_DIST, &bSuccess, FALSE);
			DestroyWindow(hwnd);
			break;
		case IDC_BUTTON_CANCEL:/*no need to update the predicate values*/
			DestroyWindow(hwnd);
			break;
		}
		break;
	default:
		return TRUE;
	}
	return TRUE;
}

LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg,
						  WPARAM wParam, LPARAM lParam)

{
	HMENU				hMenu;
	HWND				hPredicateDlg;
	MSG					msg;
	OPENFILENAME		ofn;
	FILE				*fpt;
	HDC					hDC;
	char				header[320],text[320];
	int					i,BYTES,xPos,yPos;

	hMenu=GetMenu(MainWnd);
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case ID_OPTIONS_RESET:

			/*exit all the child threads and variables to zero */
			for(i=0;i<255;i++)
			{
				ThreadRun[i]=0;/*total threads = total regions possible = 255*/
				StepModeState[i]=0;
			}
			/*reset the lables of previous results*/
			if(labels)
			{
				memset(labels,0,ROWS*COLS);
			}
			ShowPixelCoords = 0;/*default disabled*/
			PixelColour = 0;/*default red*/
			DisplayMode = 0;/*default play*/
			IntensityPredicate = DEFAULT_INTENSITY_PREDICATE;/*default value = 15*/
			DistancePredicate = DEFAULT_CENTROID_DISTANCE_PREDICATE; /*default vlaue = 500*/
			PaintImage();
			CheckMenuItem(hMenu,ID_SHOWPIXELCOORDS,MF_UNCHECKED);
			CheckMenuItem(hMenu,ID_MODE_PLAY,MF_UNCHECKED);
			CheckMenuItem(hMenu,ID_MODE_STEP,MF_UNCHECKED);
			CheckMenuItem(hMenu,ID_COLOUR_RED,MF_UNCHECKED);
			CheckMenuItem(hMenu,ID_COLOUR_GREEN,MF_UNCHECKED);
			CheckMenuItem(hMenu,ID_COLOUR_BLUE,MF_UNCHECKED);
			break;
		case ID_MODE_PLAY:
			DisplayMode = ePlayMode; 
			CheckMenuItem(hMenu,ID_MODE_PLAY,MF_CHECKED);
			CheckMenuItem(hMenu,ID_MODE_STEP,MF_UNCHECKED);
			break;
		case ID_MODE_STEP:
			DisplayMode = eStepMode;
			CheckMenuItem(hMenu,ID_MODE_PLAY,MF_UNCHECKED);
			CheckMenuItem(hMenu,ID_MODE_STEP,MF_CHECKED);
			break;
		case ID_COLOUR_RED:
			PixelColour = eRed;
			CheckMenuItem(hMenu,ID_COLOUR_RED,MF_CHECKED);
			CheckMenuItem(hMenu,ID_COLOUR_GREEN,MF_UNCHECKED);
			CheckMenuItem(hMenu,ID_COLOUR_BLUE,MF_UNCHECKED);
			break;
		case ID_COLOUR_GREEN:
			PixelColour = eGreen;
			CheckMenuItem(hMenu,ID_COLOUR_RED,MF_UNCHECKED);
			CheckMenuItem(hMenu,ID_COLOUR_GREEN,MF_CHECKED);
			CheckMenuItem(hMenu,ID_COLOUR_BLUE,MF_UNCHECKED);
			break;
		case ID_COLOUR_BLUE:
			PixelColour = eBlue;
			CheckMenuItem(hMenu,ID_COLOUR_RED,MF_UNCHECKED);
			CheckMenuItem(hMenu,ID_COLOUR_GREEN,MF_UNCHECKED);
			CheckMenuItem(hMenu,ID_COLOUR_BLUE,MF_CHECKED);
			break;
		case ID_PREDICATES_PIXELINTENSITY:
			hPredicateDlg = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1),hWnd, PredicateDialogProc);
			if(hPredicateDlg != NULL)
			{
				ShowWindow(hPredicateDlg, SW_SHOW);
				SetDlgItemInt(hPredicateDlg, IDC_NUM_INT,IntensityPredicate, FALSE);
				SetDlgItemInt(hPredicateDlg, IDC_NUM_DIST,DistancePredicate, FALSE);
				UpdateWindow(hPredicateDlg);
			}
			else
			{
				MessageBox(hWnd, "CreateDialog returned NULL", "Warning!",  
					MB_OK | MB_ICONINFORMATION);
			}
			break;
		case ID_PREDICATES_PIXELDISTANCE:
			hPredicateDlg = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1),hWnd, PredicateDialogProc);
			if(hPredicateDlg != NULL)
			{
				ShowWindow(hPredicateDlg, SW_SHOW);
				SetDlgItemInt(hPredicateDlg, IDC_NUM_INT,IntensityPredicate, FALSE);
				SetDlgItemInt(hPredicateDlg, IDC_NUM_DIST,DistancePredicate, FALSE);
				UpdateWindow(hPredicateDlg);
			}
			else
			{
				MessageBox(hWnd, "CreateDialog returned NULL", "Warning!",  
					MB_OK | MB_ICONINFORMATION);
			}
			break;
		case ID_SHOWPIXELCOORDS:
			ShowPixelCoords=(ShowPixelCoords+1)%2;
			//PaintImage();
			break;
		case ID_FILE_LOAD:
			if (OriginalImage != NULL)
			{
				free(OriginalImage);
				OriginalImage=NULL;
			}
			memset(&(ofn),0,sizeof(ofn));
			ofn.lStructSize=sizeof(ofn);
			ofn.lpstrFile=filename;
			filename[0]=0;
			ofn.nMaxFile=MAX_FILENAME_CHARS;
			ofn.Flags=OFN_EXPLORER | OFN_HIDEREADONLY;
			ofn.lpstrFilter = "PPM files\0*.ppm\0All files\0*.*\0\0";
			if (!( GetOpenFileName(&ofn))  ||  filename[0] == '\0')
				break;		/* user cancelled load */
			if ((fpt=fopen(filename,"rb")) == NULL)
			{
				MessageBox(NULL,"Unable to open file",filename,MB_OK | MB_APPLMODAL);
				break;
			}
			fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
			if (strcmp(header,"P5") != 0  ||  BYTES != 255)
			{
				MessageBox(NULL,"Not a PPM (P5 greyscale) image",filename,MB_OK | MB_APPLMODAL);
				fclose(fpt);
				break;
			}
			OriginalImage=(unsigned char *)calloc(ROWS*COLS,1);
			/* segmentation image = labels; calloc initializes all labels to 0 */
			labels=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
			
			header[0]=fgetc(fpt);	/* whitespace character after header */
			fread(OriginalImage,1,ROWS*COLS,fpt);
			fclose(fpt);
			SetWindowText(hWnd,filename);
			PaintImage();
			break;
		case ID_FILE_QUIT:
			/*exit all the child threads*/
			for(i=0;i<255;i++)
			{
				ThreadRun[i]=0;/*total threads = total regions possible = 255*/
				StepModeState[i]=0;
			}

			DestroyWindow(hWnd);
			break;
		}
		break;
	case WM_SIZE:		  /* could be used to detect when window size changes */
		PaintImage();
		return(DefWindowProc(hWnd,uMsg,wParam,lParam));
		break;
	case WM_PAINT:
		PaintImage();
		return(DefWindowProc(hWnd,uMsg,wParam,lParam));
		break;
	case WM_RBUTTONDOWN:
		return(DefWindowProc(hWnd,uMsg,wParam,lParam));
		break;
	case WM_LBUTTONDOWN:
		xPos=LOWORD(lParam);
		yPos=HIWORD(lParam);
		if (xPos >= 0  &&  xPos < COLS  &&  yPos >= 0  &&  yPos < ROWS)
		{
			int r=0,c=0,r2=0,c2=0,i=0,RegionSize=0,index =0;
			double		avg=0,var=0;
			user_options *options = NULL;

			//sprintf(text,"%d,%d=>%d     ",xPos,yPos,OriginalImage[yPos*COLS+xPos]);
			//hDC=GetDC(MainWnd);
			//TextOut(hDC,0,0,text,strlen(text));		/* draw text on the window */

			r = yPos;
			c = xPos;
			if (labels[r*COLS+c] != 0)
			{
				MessageBox(hWnd, "Region Already selected;Select new region", "Warning!",  
					MB_OK | MB_ICONINFORMATION);
				break;
			}
			//avg=var=0.0;	/* compute average and variance in 7x7 window */
			//for (r2=-3; r2<=3; r2++)
			//	for (c2=-3; c2<=3; c2++)
			//		avg+=(double)(OriginalImage[(r+r2)*COLS+(c+c2)]);
			//avg/=49.0;
			//for (r2=-3; r2<=3; r2++)
			//	for (c2=-3; c2<=3; c2++)
			//		var+=SQR(avg-(double)OriginalImage[(r+r2)*COLS+(c+c2)]);
			//var=sqrt(var)/49.0;
			//if (var < 1.0)	/* condition for seeding a new region is low var */
			//{
				TotalRegions++;
				if (TotalRegions == 255)
				{
					MessageBox(hWnd, "Max selections 255 used  ;Use Reset option", "Warning!",  
						MB_OK | MB_ICONINFORMATION);
					break;
				}

				options = (user_options *)malloc(sizeof(user_options));
				options->Count = TotalRegions;
				options->col = c;
				options->row = r;
				options->Colour = PixelColour;
				options->IntensityPredicate = IntensityPredicate ; 
				options->DistancePredicate = DistancePredicate;
				ThreadRun[options->Count] = TRUE;
				_beginthread(RegionGrowThread,0,(void*)options);
			/*}
			else
			{
				MessageBox(hWnd, "Low Variance Condition for Seeding Not satisfied;Select new region", "Warning!",  
					MB_OK | MB_ICONINFORMATION);
				break;
			}*/
		}

		break;
	case WM_MOUSEMOVE:
		if (ShowPixelCoords == 1)
		{
			xPos=LOWORD(lParam);
			yPos=HIWORD(lParam);
			if (xPos >= 0  &&  xPos < COLS  &&  yPos >= 0  &&  yPos < ROWS)
			{
				sprintf(text,"%d,%d=>%d     ",xPos,yPos,OriginalImage[yPos*COLS+xPos]);
				hDC=GetDC(MainWnd);
				TextOut(hDC,0,0,text,strlen(text));		/* draw text on the window */
				//switch (PixelColour)
				//{
				//case eRed:
				//	SetPixel(hDC,xPos,yPos,RGB(255,0,0));	/* color the cursor position red */
				//	break;
				//case eGreen:
				//	SetPixel(hDC,xPos,yPos,RGB(0,255,0));	/* color the cursor position green */
				//	break;
				//case eBlue:
				//	SetPixel(hDC,xPos,yPos,RGB(0,0,255));	/* color the cursor position blue */
				//	break;
				//}
				ReleaseDC(MainWnd,hDC);
			}
		}
		return(DefWindowProc(hWnd,uMsg,wParam,lParam));
		break;
	case WM_KEYDOWN:
		if ((wParam == 'j'  ||  wParam == 'J')&& (eStepMode == DisplayMode))
			for(i=0;i<255;i++)
			{
				StepModeState[i] = TRUE;
			}
		break;
		//	if (wParam == 's'  ||  wParam == 'S')
		//		PostMessage(MainWnd,WM_COMMAND,ID_SHOWPIXELCOORDS,0);	  /* send message to self */
		//	if ((TCHAR)wParam == '1')
		//	{
		//		TimerRow=TimerCol=0;
		//		SetTimer(MainWnd,TIMER_SECOND,10,NULL);	/* start up 10 ms timer */
		//	}
		//	if ((TCHAR)wParam == '2')
		//	{
		//		KillTimer(MainWnd,TIMER_SECOND);			/* halt timer, stopping generation of WM_TIME events */
		//		PaintImage();								/* redraw original image, erasing animation */
		//	}
		//	if ((TCHAR)wParam == '3')
		//	{
		//		ThreadRunning=1;
		//		_beginthread(AnimationThread,0,MainWnd);	/* start up a child thread to do other work while this thread continues GUI */
		//	}
		//	if ((TCHAR)wParam == '4')
		//	{
		//		ThreadRunning=0;							/* this is used to stop the child thread (see its code below) */
		//	}
		//	return(DefWindowProc(hWnd,uMsg,wParam,lParam));
		//	break;
	case WM_TIMER:	  /* this event gets triggered every time the timer goes off */
		hDC=GetDC(MainWnd);
		SetPixel(hDC,TimerCol,TimerRow,RGB(0,0,255));	/* color the animation pixel blue */
		ReleaseDC(MainWnd,hDC);
		TimerRow++;
		TimerCol+=2;
		break;
	case WM_HSCROLL:	  /* this event could be used to change what part of the image to draw */
		PaintImage();	  /* direct PaintImage calls eliminate flicker; the alternative is InvalidateRect(hWnd,NULL,TRUE); UpdateWindow(hWnd); */
		return(DefWindowProc(hWnd,uMsg,wParam,lParam));
		break;
	case WM_VSCROLL:	  /* this event could be used to change what part of the image to draw */
		PaintImage();
		return(DefWindowProc(hWnd,uMsg,wParam,lParam));
		break;
	case WM_DESTROY:
		/*exit all the child threads*/
		for(i=0;i<255;i++)
		{
			ThreadRun[i]=0;/*total threads = total regions possible = 255*/
			StepModeState[i]=0;
		}
		PostQuitMessage(0);
		break;
	default:
		return(DefWindowProc(hWnd,uMsg,wParam,lParam));
		break;
	}

	if (ShowPixelCoords == 1)
		CheckMenuItem(hMenu,ID_SHOWPIXELCOORDS,MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
	else
		CheckMenuItem(hMenu,ID_SHOWPIXELCOORDS,MF_UNCHECKED);

	DrawMenuBar(hWnd);

	return(0L);
}




void PaintImage()

{
	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO			*bm_info;
	int					i,r,c,DISPLAY_ROWS,DISPLAY_COLS;
	unsigned char		*DisplayImage;

	if (OriginalImage == NULL)
		return;		/* no image to draw */

	/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
	DISPLAY_ROWS=ROWS;
	DISPLAY_COLS=COLS;
	if (DISPLAY_ROWS % 4 != 0)
		DISPLAY_ROWS=(DISPLAY_ROWS/4+1)*4;
	if (DISPLAY_COLS % 4 != 0)
		DISPLAY_COLS=(DISPLAY_COLS/4+1)*4;
	DisplayImage=(unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS,1);
	for (r=0; r<ROWS; r++)
		for (c=0; c<COLS; c++)
			DisplayImage[r*DISPLAY_COLS+c]=OriginalImage[r*COLS+c];

	BeginPaint(MainWnd,&Painter);
	hDC=GetDC(MainWnd);
	bm_info_header.biSize=sizeof(BITMAPINFOHEADER); 
	bm_info_header.biWidth=DISPLAY_COLS;
	bm_info_header.biHeight=-DISPLAY_ROWS; 
	bm_info_header.biPlanes=1;
	bm_info_header.biBitCount=8; 
	bm_info_header.biCompression=BI_RGB; 
	bm_info_header.biSizeImage=0; 
	bm_info_header.biXPelsPerMeter=0; 
	bm_info_header.biYPelsPerMeter=0;
	bm_info_header.biClrUsed=256;
	bm_info_header.biClrImportant=256;
	bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
	bm_info->bmiHeader=bm_info_header;
	for (i=0; i<256; i++)
	{
		bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
		bm_info->bmiColors[i].rgbReserved=0;
	} 

	SetDIBitsToDevice(hDC,0,0,DISPLAY_COLS,DISPLAY_ROWS,0,0,
		0, /* first scan line */
		DISPLAY_ROWS, /* number of scan lines */
		DisplayImage,bm_info,DIB_RGB_COLORS);
	ReleaseDC(MainWnd,hDC);
	EndPaint(MainWnd,&Painter);

	free(DisplayImage);
	free(bm_info);
}




void AnimationThread(HWND AnimationWindowHandle)

{
	HDC		hDC;
	char	text[300];

	ThreadRow=ThreadCol=0;
	while (ThreadRunning == 1)
	{
		hDC=GetDC(MainWnd);
		SetPixel(hDC,ThreadCol,ThreadRow,RGB(0,255,0));	/* color the animation pixel green */
		sprintf(text,"%d,%d     ",ThreadRow,ThreadCol);
		TextOut(hDC,300,0,text,strlen(text));		/* draw text on the window */
		ReleaseDC(MainWnd,hDC);
		ThreadRow+=3;
		ThreadCol++;
		Sleep(100);		/* pause 100 ms */
	}
}

void RegionGrowThread(void* aoptions)
{
	int RegionSize=0,i=0,xPos=0,yPos=0;
	user_options *options = NULL;
	int			 *indices =  NULL,index = 0;
	HDC		hDC;
	options = (user_options *)aoptions;

	/* used to quickly erase small grown regions */
	indices=(int *)calloc(ROWS*COLS,sizeof(int));

	RegionGrow(OriginalImage,labels,ROWS,COLS,options->row,options->col,0,options->Count,
		indices,&RegionSize,options->IntensityPredicate,options->DistancePredicate);
	//if (RegionSize < 100)
	//{	/* erase region (relabel pixels back to 0) */
	//	for (i=0; i<RegionSize; i++)
	//		labels[indices[i]]=0;
	//	//TotalRegions--;
	//}
	//else
	{
		hDC=GetDC(MainWnd);
		i = 0;
		while(i < RegionSize && ThreadRun[options->Count])
		{
			if(ePlayMode == DisplayMode || (eStepMode == DisplayMode && TRUE == StepModeState[options->Count]))
			{

				index = indices[i];
				if(index>COLS)
				{
					yPos = index/COLS;
					xPos = index-(yPos*COLS);
				}
				else
				{
					yPos = 0;
					xPos = index;
				}

				switch (options->Colour)
				{
				case eRed:
					SetPixel(hDC,xPos,yPos,RGB(255,0,0));	/* color the cursor position red */
					break;
				case eGreen:
					SetPixel(hDC,xPos,yPos,RGB(0,255,0));	/* color the cursor position green */
					break;
				case eBlue:
					SetPixel(hDC,xPos,yPos,RGB(0,0,255));	/* color the cursor position blue */
					break;
				}
				if(ePlayMode == DisplayMode)
					Sleep(1);
				else
					StepModeState[options->Count] = FALSE;

				i++;
			}
		}
		ReleaseDC(MainWnd,hDC);
	}

	if(indices)
		free(indices);
	if(options)
		free(options);
}

