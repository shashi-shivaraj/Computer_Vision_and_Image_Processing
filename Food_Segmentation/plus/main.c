#include "header.h"
#include "resource.h"
#include "globals.h"

static void configure_Algo();



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPTSTR lpCmdLine, int nCmdShow)

{
	MSG			msg;
	HWND		hWnd;
	WNDCLASS	wc;
	int			i;
	HDC					hDC;


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



	hWnd=CreateWindow("PLUS","Segmenting Foods",
		WS_TILEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT,0,400,400,NULL,NULL,hInstance,NULL);
	if (!hWnd)
		return(FALSE);

	ShowScrollBar(hWnd,SB_BOTH,TRUE);
	ShowWindow(hWnd,nCmdShow|SW_MAXIMIZE);
	UpdateWindow(hWnd);
	MainWnd=hWnd;

	/*Get screen resolution*/
	hDC=GetDC(MainWnd);
	screen_width = GetDeviceCaps(hDC,HORZRES);
	screen_height = GetDeviceCaps(hDC,VERTRES);

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

	Gfp = fopen("debug.txt","w+");

	/*create a global mutex object*/
	ghMutex = (void*)CreateMutex(NULL,FALSE,0);
	if(0 == ghMutex)
	{
		fprintf(Gfp,"Create mutex failed\n");
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

//BOOL CALLBACK PredicateDialogProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	BOOL bSuccess;
//	switch(Message)
//	{
//	case WM_COMMAND:
//		switch(LOWORD(wParam))
//		{
//		case IDC_BUTTON_OK:
//			IntensityPredicate = GetDlgItemInt(hwnd, IDC_NUM_INT, &bSuccess, FALSE);
//			DistancePredicate  = GetDlgItemInt(hwnd, IDC_NUM_DIST, &bSuccess, FALSE);
//			DestroyWindow(hwnd);
//			break;
//		case IDC_BUTTON_CANCEL:/*no need to update the predicate values*/
//			DestroyWindow(hwnd);
//			break;
//		}
//		break;
//	default:
//		return TRUE;
//	}
//	return TRUE;
//}

LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg,
						  WPARAM wParam, LPARAM lParam)

{
	HMENU				hMenu;
	HWND				hPredicateDlg;
	MSG					msg;
	OPENFILENAME		ofn;
	FILE				*fpt;
	unsigned char		*temp;
	HDC					hDC;
	contour_position *head = NULL,*curr_cont = NULL,*next_cont = NULL,*prev_cont = NULL;
	char				header[320],text[320];
	int					i=0,j=0,k=0,BYTES=0,xPos = 0,yPos = 0,count=0;
	int					prev_xPos = 0,prev_yPos = 0;
	int					image_type=0,index=0,ret =0;


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

			/*deallocate all the heap memory */
			for(i=0;i<100;i++)
			{
				if(list_head[i])
				{
					destroy_list(list_head[i]);
					list_head[i] = NULL;
				}


			}



			ShowPixelCoords = 0;/*default disabled*/
			PaintImage();
			CheckMenuItem(hMenu,ID_SHOWPIXELCOORDS,MF_UNCHECKED);
			break;
		case ID_SHOWPIXELCOORDS:
			ShowPixelCoords=(ShowPixelCoords+1)%2;
			break;
		case ID_FILE_LOAD:
			if (OriginalImage != NULL)
			{
				free(OriginalImage);
				OriginalImage=NULL;
			}
			if(FileImage)
			{
				free(FileImage);
				FileImage=NULL;
			}
			if(sobel_image)
			{
				free(sobel_image);
				sobel_image = NULL;
			}

			/*deallocate all the heap memory */
			for(i=0;i<100;i++)
			{
				if(list_head[i])
					destroy_list(list_head[i]);
			}


			memset(&(ofn),0,sizeof(ofn));
			ofn.lStructSize=sizeof(ofn);
			ofn.lpstrFile=filename;
			filename[0]=0;
			ofn.nMaxFile=MAX_FILENAME_CHARS;
			ofn.Flags=OFN_EXPLORER | OFN_HIDEREADONLY;
			ofn.lpstrFilter = "PNM files\0*.pnm\0PPM files\0*.ppm\0All files\0*.*\0\0";
			if (!( GetOpenFileName(&ofn))  ||  filename[0] == '\0')
				break;		/* user cancelled load */
			if ((fpt=fopen(filename,"rb")) == NULL)
			{
				MessageBox(NULL,"Unable to open file",filename,MB_OK | MB_APPLMODAL);
				break;
			}
			fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
			if ((strcmp(header,"P5") != 0 && strcmp(header,"P6") != 0)||  BYTES != 255 )
			{
				MessageBox(NULL,"Not a PPM (P5 greyscale)/PNM (P6) image",filename,MB_OK | MB_APPLMODAL);
				fclose(fpt);
				break;
			}

			configure_Algo();

			/*Store the image type*/
			if(!strcmp(header,"P5"))
				image_type = eP5;
			else 
				image_type = eP6;

			/* display image labels; calloc initializes all labels to 0 */
			labels=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));


			header[0]=fgetc(fpt);	/* whitespace character after header */


			if(eP6 == image_type)
			{
				FileImage = (unsigned char *)calloc(ROWS*COLS*3,1); /*RGB representation*/
				fread(FileImage,1,ROWS*COLS*3,fpt); /*1 pixel has rgb bytes*/

				/*Reduce the image resolution to fit to screen*/
				while(screen_width < COLS || screen_height < ROWS)
				{
					temp = (unsigned char *)calloc(3*(ROWS/2)*(COLS/2),1);
					/*reduce the resolution by factor of 2*/
					downsample_rgb_image(FileImage,temp,ROWS,COLS);
					COLS = COLS/2;
					ROWS = ROWS/2;

					free(FileImage);
					FileImage = temp;
				}

				OriginalImage = (unsigned char *)calloc(ROWS*COLS,1);/*Grayscale representation*/
				convert_rgb_to_grey(FileImage,OriginalImage,ROWS,COLS);
			}
			else
			{
				OriginalImage=(unsigned char *)calloc(ROWS*COLS,1);
				fread(OriginalImage,1,ROWS*COLS,fpt);
			}

			/*Reduce the image resolution to fit to screen*/
			while(screen_width < COLS || screen_height < ROWS)
			{
				temp = (unsigned char *)calloc(ROWS/2*COLS/2,1);
				/*reduce the resolution by factor of 2*/
				downsample_grey_image(OriginalImage,temp,ROWS,COLS);
				COLS = COLS/2;
				ROWS = ROWS/2;

				free(OriginalImage);
				OriginalImage = temp;
			}

			sobel_image = (unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));/*Allocate memory to store input image data*/
			if(!sobel_image)			/*Error handling*/
			{
				printf("calloc failed\n");/*Memory allocation failed*/
				return -1;              /*return error code*/
			}

			memcpy((void*)sobel_image,(void*)OriginalImage,ROWS*COLS);

			/*To convolve the input image with the sobel filters*/
			Sobel_Convolution(sobel_image,ROWS,COLS);

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

			/*free the allocated memory*/
			if (OriginalImage != NULL)
			{
				free(OriginalImage);
				OriginalImage=NULL;
			}

			if(FileImage)
			{
				free(FileImage);
				FileImage=NULL;
			}

			if(sobel_image)
			{
				free(sobel_image);
				sobel_image = NULL;
			}

			if(labels)
			{
				free(labels);
				labels = NULL;
			}

			/*deallocate all the heap memory */
			for(i=0;i<100;i++)
			{
				if(list_head[i])
					destroy_list(list_head[i]);
			}

			/*Destroy the Mutex*/
			if(!CloseHandle((HANDLE)ghMutex))
			{
				fprintf(Gfp,"Destroy mutex failed\n");
			}

			if(Gfp)
			{
				fclose(Gfp);
				Gfp = NULL;
			}

			DestroyWindow(hWnd);
			break;
		}
		break;
	case WM_CLOSE:
		{

			/*exit all the child threads*/
			for(i=0;i<255;i++)
			{
				ThreadRun[i]=0;/*total threads = total regions possible = 255*/
				StepModeState[i]=0;
			}

			/*free the allocated memory*/
			if (OriginalImage != NULL)
			{
				free(OriginalImage);
				OriginalImage=NULL;
			}

			if(FileImage)
			{
				free(FileImage);
				FileImage=NULL;
			}

			if(sobel_image)
			{
				free(sobel_image);
				sobel_image = NULL;
			}

			if(labels)
			{
				free(labels);
				labels = NULL;
			}

			/*deallocate all the heap memory */
			for(i=0;i<100;i++)
			{
				if(list_head[i])
					destroy_list(list_head[i]);
			}

			/*Destroy the Mutex*/
			if(!CloseHandle((HANDLE)ghMutex))
			{
				fprintf(Gfp,"Destroy mutex failed\n");
			}

			if(Gfp)
			{
				fclose(Gfp);
				Gfp = NULL;
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
	case WM_RBUTTONUP:
		{
			xPos=LOWORD(lParam);
			yPos=HIWORD(lParam);
			if (xPos >= 0  &&  xPos < COLS  &&  yPos >= 0  &&  yPos < ROWS)
			{

				sprintf(text,"%d,%d=>%d     ",xPos,yPos,OriginalImage[yPos*COLS+xPos]);
				hDC=GetDC(MainWnd);
				TextOut(hDC,0,0,text,strlen(text));		/* draw text on the window */

				new_data = (contour_position*)malloc(sizeof(contour_position));
				if(!new_data)
				{
					printf("memory allocation failed");
					return NULL;
				}
				memset(new_data,0,sizeof(contour_position));

				new_data->Count = 0;
				new_data->y_pos = yPos;
				new_data->x_pos = xPos;
				ret = convert_height_width2index(&new_data->index,ROWS,COLS,
					xPos,yPos);
				if(ret != 0 || 0 > new_data->index )
				{
					fprintf(Gfp,"convert_height_width2index returned error");
				}

				thread_count ++;

				if(!list_head[thread_count])
				{
					list_head[thread_count] = new_data;
					list_head[thread_count]->prev = NULL;
					list_head[thread_count]->Thread_index = thread_count;
				}

				_beginthread(RightClickThread,0,(void*)list_head[thread_count]);
			}
		}
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
			int red = 0,green = 0,blue =0;

			sprintf(text,"%d,%d=>%d     ",xPos,yPos,OriginalImage[yPos*COLS+xPos]);
			hDC=GetDC(MainWnd);
			TextOut(hDC,0,0,text,strlen(text));		/* draw text on the window */

			/*Check if its an present countour point*/
			for(k=thread_count;k>0;k--)
			{
				head = (contour_position *)list_head[k];
				curr_cont = next_cont = head;
				while(next_cont)
				{
					curr_cont = next_cont;
					next_cont = curr_cont->next;

					/*check if the clicked point is a contour point*/
					for(i=-2;i<=2;i++)
					{
						for(j=-2;j<=2;j++)
						{
							if(curr_cont->x_pos+i == xPos && 
								curr_cont->y_pos+j== yPos)
							{
								AlterContour = 1;
								AlterContourThread = k;
								alter_contour_node =  curr_cont;
								break;
							}
						}

						if(AlterContour)
							break;
					}
					if(AlterContour)
						break;
				}

			}
			if(!AlterContour)
			{
				InitialContourDraw = 1;
				contour_count = 0;
				prev_data = NULL;
				new_data  = NULL;
				thread_count ++;
			}
		}

		fprintf(Gfp,"WM_LBUTTONDOWN detected\n");

		break;
	case WM_LBUTTONUP:
		{
			if(InitialContourDraw == 1)
			{
				InitialContourDraw = 0;
				if(list_head[thread_count])
					_beginthread(LeftClickThread,0,(void*)list_head[thread_count]);
			}
			else if(AlterContour)
			{
				AlterContour = 0; 
				alter_contour_node->pos_state = eFixed;
				if(list_head[alter_contour_node->Thread_index])
					_beginthread(AlterActiveContourThread,0,(void*)list_head[alter_contour_node->Thread_index]);
				alter_contour_node = NULL;
				AlterContourThread = 0;
			}

			contour_count = 0;
			prev_data = NULL;
			new_data  = NULL;

			fprintf(Gfp,"WM_LBUTTONUP detected\n");

		}
		break;
	case WM_MOUSEMOVE:
		if (ShowPixelCoords == 1 || InitialContourDraw == 1 || AlterContour == 1)
		{
			xPos=LOWORD(lParam);
			yPos=HIWORD(lParam);
			if (xPos >= 0  &&  xPos < COLS  &&  yPos >= 0  &&  yPos < ROWS)
			{
				{
					sprintf(text,"%d,%d=>%d     ",xPos,yPos,OriginalImage[yPos*COLS+xPos]);
					hDC=GetDC(MainWnd);
					TextOut(hDC,0,0,text,strlen(text));		/* draw text on the window */
				}

				if(InitialContourDraw == 1)
				{
					/*Store each point of the contour drawn by the user*/
					new_data = (contour_position*)malloc(sizeof(contour_position));
					if(!new_data)
					{
						printf("memory allocation failed");
						return NULL;
					}
					memset(new_data,0,sizeof(contour_position));

					new_data->Count = contour_count;
					contour_count ++;
					new_data->y_pos = yPos;
					new_data->x_pos = xPos;
					ret = convert_height_width2index(&new_data->index,ROWS,COLS,
						xPos,yPos);
					if(ret != 0 || 0 > new_data->index )
					{
						fprintf(Gfp,"convert_height_width2index returned error");
					}

					if(!list_head[thread_count])
					{
						list_head[thread_count] = new_data;
						list_head[thread_count]->prev = NULL;
						list_head[thread_count]->Thread_index = thread_count;
					}

					if(prev_data)
					{
						prev_data->next = new_data;
						new_data->prev = prev_data;
					}

					new_data->next = NULL;
					prev_data = new_data;

					/*Display the contour on the screen*/
					for(i=-2;i<=2;i++)
					{
						for(j=-2;j<=2;j++)
						{
							SetPixel(hDC,xPos+i,yPos+j,RGB(0,0,255));	/* color the cursor position blue */
						}
					}
				}
				else if(1 == AlterContour)
				{
					/*erase the previous contour point*/
					if(alter_contour_node)
					{
						for(i=-2;i<=2;i++)
						{
							for(j=-2;j<=2;j++)
							{
								ret = convert_height_width2index(&index,ROWS,COLS,alter_contour_node->x_pos+i,alter_contour_node->y_pos+j);
								if(ret != 0 || 0 > index)
									continue;
								labels[index] = 0;
							}
						}

						PaintImage();
						alter_contour_node->x_pos = xPos;
						alter_contour_node->y_pos = yPos;
						ret = convert_height_width2index(&alter_contour_node->index,ROWS,COLS,xPos,yPos);
						if(ret != 0 || 0 > alter_contour_node->index)
						{
							fprintf(Gfp,"convert_height_width2index returned error");
						}

						/*paint the new contour point moved*/
						for(i=-2;i<=2;i++)
						{
							for(j=-2;j<=2;j++)
							{
								ret = convert_height_width2index(&index,ROWS,COLS,alter_contour_node->x_pos+i,alter_contour_node->y_pos+j);
								if(ret == 0 && index > 0)
								{
									labels[index] = 255;
								}

							}
						}

						PaintImage();
					}
				}
			}
			ReleaseDC(MainWnd,hDC);
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
	unsigned char		*DisplayImage = NULL;
	int					i,r,c,DISPLAY_ROWS,DISPLAY_COLS;
	int					screen_width = 0,screen_height = 0;
	int					xPos = 0,yPos = 0,ret =0;


	if (OriginalImage == NULL)
		return;		/* no image to draw */

	/*Acquire the Mutex lock or wait*/
	MutexLock(ghMutex);


	/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
	DISPLAY_ROWS=ROWS;
	DISPLAY_COLS=COLS;
	if (DISPLAY_ROWS % 4 != 0)
		DISPLAY_ROWS=(DISPLAY_ROWS/4+1)*4;
	if (DISPLAY_COLS % 4 != 0)
		DISPLAY_COLS=(DISPLAY_COLS/4+1)*4;
	if(!DisplayImage)
	{
		DisplayImage=(unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS,1);
		for (r=0; r<ROWS; r++)
			for (c=0; c<COLS; c++)
				DisplayImage[r*DISPLAY_COLS+c]=OriginalImage[r*COLS+c];
	}


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

	for(i=0;i<ROWS*COLS && labels;i++)
	{
		if(255 == labels[i])
		{
			ret = convert_index2height_width(i,ROWS,COLS,&xPos,&yPos);
			if(ret != 0 || 0 > xPos || 0>yPos )
				continue;

			SetPixel(hDC,xPos,yPos,RGB(0,0,255));	/* color the cursor position blue */
		}
	}

	if(DisplayImage)
		free(DisplayImage);
	DisplayImage = NULL;

	ReleaseDC(MainWnd,hDC);
	EndPaint(MainWnd,&Painter);

	free(bm_info);

	MutexUnLock(ghMutex);
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
	int			 *reg_indices =  NULL,index = 0;
	HDC		hDC;
	options = (user_options *)aoptions;

	/* used to quickly erase small grown regions */
	reg_indices=(int *)calloc(ROWS*COLS,sizeof(int));

	RegionGrow(OriginalImage,labels,ROWS,COLS,options->row,options->col,0,options->Count,
		reg_indices,&RegionSize,options->IntensityPredicate,options->DistancePredicate);
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

				index = reg_indices[i];
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

	if(reg_indices)
		free(reg_indices);
	if(options)
		free(options);
}

void RightClickThread(void* aoptions) /*passes address of the stucture with pointer to initial counter*/
{
	contour_position *head = NULL,
		*curr_cont = NULL,
		*next_cont = NULL,
		*prev_cont = NULL;
	FILE			 *fp		= NULL;
	contour_position	*new_data = NULL,*prev_data = NULL;
	HDC		hDC;
	char file [50];
	int count = 0,x_pos=0,y_pos=0,index = 0,contour_count = 0,ret =0;
	int prev_xpos = 0,prev_ypos =0,i=0,j=0;
	int center_xpos = 0,center_ypos = 0,thread_count = 0;
	float theta = 0;
	head = (contour_position *)aoptions;

	/*sprintf(file,"RightClickThread%d.txt",head->Thread_index);
	fp = fopen(file,"w+");*/

	center_xpos = head->x_pos;
	center_ypos = head->y_pos;
	thread_count = head->Thread_index;


	free(head);
	head = NULL;
	hDC=GetDC(MainWnd);
	/*fprintf(fp,"initial contour is:\n");*/
	/*initail contour as a circle of radius 10 pixels around clicked point*/
	for(theta = 0;theta <= 360;theta = theta + 5)
	{
		x_pos = 10*cos(CONV_2_RADIAN(theta))+center_xpos;
		y_pos = 10*sin(CONV_2_RADIAN(theta))+center_ypos;

		/*to prevent duplicate contour points*/
		if(prev_xpos == x_pos && prev_ypos == y_pos)
		{
			continue;
		}
		else
		{	
			prev_xpos == x_pos;
			prev_ypos == y_pos;
		}

		/*Store each point of the circle contour*/
		new_data = (contour_position*)malloc(sizeof(contour_position));
		if(!new_data)
		{
			printf("memory allocation failed");
			return NULL;
		}
		memset(new_data,0,sizeof(contour_position));

		new_data->Count = contour_count;
		contour_count ++;
		new_data->y_pos = y_pos;
		new_data->x_pos = x_pos;
		new_data->Thread_index = thread_count;
		ret = convert_height_width2index(&new_data->index,ROWS,COLS,x_pos,y_pos);
		if(ret != 0 || 0 > new_data->index )
			continue;


		if(!head)
		{
			head = new_data;
			head->prev = NULL;
			list_head[new_data->Thread_index] = head;
		}

		if(prev_data)
		{
			prev_data->next = new_data;
			new_data->prev = prev_data;
		}

		new_data->next = NULL;
		prev_data = new_data;

		/*Display the circle*/
		if(labels)
			labels[new_data->index] = 255;

		//fprintf(fp,"count %d xpos %d ypos %d:\n",new_data->Count,new_data->x_pos,new_data->y_pos);
	}

	/*Display the contour*/
	PaintImage();

	Sleep(500);
	/*Downsample the contour list to every third point*/
	curr_cont = next_cont = head;
	//fprintf(fp,"\nDownsampled contour is:\n");
	while(next_cont)
	{
		curr_cont = next_cont;
		next_cont = curr_cont->next;

		if(0 == (curr_cont->Count %3))
		{
			/*fprintf(fp,"prev count %d new count %d xpos %d ypos %d:\n"
			,curr_cont->Count,count,curr_cont->x_pos,curr_cont->y_pos);*/
			curr_cont->Count = count;
			count ++;

			if(prev_cont)
			{
				prev_cont->next = curr_cont;
				curr_cont->prev = prev_cont;
			}

			curr_cont->next = NULL;
			prev_cont = curr_cont;	

			/*Display the contour on the screen*/
			for(i=-1;i<=1;i++)
			{
				for(j=-1;j<=1;j++)
				{
					ret = convert_height_width2index(&index,ROWS,COLS,curr_cont->x_pos+i,curr_cont->y_pos+j);
					if(ret != 0 || 0 > index )
						continue;
					labels[index] = 255;
				}
			}

		}
		else
		{
			curr_cont->next = NULL;
			curr_cont->prev = NULL;

			/*erase the points from screen*/
			/*Display the circle*/
			if(labels)
				labels[curr_cont->index] = 0;

			if(curr_cont)
				free(curr_cont);
			curr_cont = NULL;
		}	
	}

	PaintImage();

	for(i=0;i<Algo_Opt.right_contour_iteration;i++)
	{
		update_contour_display(head,ROWS,COLS,labels,eErase);
		BalloonActiveContourAlgo(OriginalImage,sobel_image,FileImage,head,ROWS,COLS,center_xpos,center_ypos,&Algo_Opt);
		update_contour_display(head,ROWS,COLS,labels,eDisplay);

		Sleep(100);

		hDC=GetDC(MainWnd);
		PaintImage();	
		ReleaseDC(MainWnd,hDC);
	}

	/*deallocate the memory from application thread*/
	//destroy_list(list_head);

	if(fp)
		fclose(fp);

}

void LeftClickThread(void* aoptions) /*passes address of the stucture with pointer to initial counter*/
{
	contour_position *head = NULL,
		*curr_cont = NULL,
		*next_cont = NULL,
		*prev_cont = NULL;
	int xPos=0,yPos=0,index = 0,i=0,j=0,ret;
	int thread_count = 0;


	HDC		hDC;
	char filename [50];
	int count = 0;
	int cont_row =0,cont_col =0;
	FILE *fp = NULL;
	head = (contour_position *)aoptions;
	thread_count = head->Thread_index;

	/*sprintf(filename,"LeftClickThread%d.txt",head->Thread_index);
	fp = fopen(filename,"w+");*/


	//curr_cont = next_cont = head;
	////fprintf(fp,"initial contour is:\n");
	//while(next_cont)
	//{
	//	curr_cont = next_cont;
	//	next_cont = curr_cont->next;

	//	//fprintf(fp,"count %d xpos %d ypos %d:\n",curr_cont->Count,curr_cont->x_pos,curr_cont->y_pos);
	//}

	/*Downsample the contour list to every fifth point*/
	curr_cont = next_cont = head;
	//fprintf(fp,"\nDownsampled contour is:\n");
	while(next_cont)
	{
		curr_cont = next_cont;
		next_cont = curr_cont->next;

		if(0 == (curr_cont->Count %10))
		{
			/*fprintf(fp,"prev count %d new count %d xpos %d ypos %d:\n"
			,curr_cont->Count,count,curr_cont->x_pos,curr_cont->y_pos);*/
			curr_cont->Thread_index = thread_count;
			curr_cont->Count = count;
			count ++;

			if(prev_cont)
			{
				prev_cont->next = curr_cont;
				curr_cont->prev = prev_cont;
			}

			curr_cont->next = NULL;
			prev_cont = curr_cont;

			xPos = curr_cont->x_pos;
			yPos = curr_cont->y_pos;

			/*Display the contour on the screen*/
			for(i=-2;i<=2;i++)
			{
				for(j=-2;j<=2;j++)
				{
					ret = convert_height_width2index(&index,ROWS,COLS,curr_cont->x_pos+i,curr_cont->y_pos+j);
					if(ret != 0 || 0 > index )
						continue;

					labels[index] = 255;
				}
			}
		}
		else
		{

			curr_cont->next = NULL;
			curr_cont->prev = NULL;
			if(curr_cont)
				free(curr_cont);
			curr_cont = NULL;
		}	
	}

	Sleep(500);


	/*paste hack code here if required*/

	hDC=GetDC(MainWnd);
	PaintImage();	
	ReleaseDC(MainWnd,hDC);

	for(i=0;i<Algo_Opt.left_contour_iteration;i++)
	{
		if(i == Algo_Opt.left_contour_iteration/2+10)
			Algo_Opt.left_EE2_W = 1.2;
		update_contour_display(head,ROWS,COLS,labels,eErase);
		RubberBandActiveContourAlgo(OriginalImage,sobel_image,FileImage,head,ROWS,COLS,&Algo_Opt);
		update_contour_display(head,ROWS,COLS,labels,eDisplay);

		Sleep(100);

		hDC=GetDC(MainWnd);
		PaintImage();	
		ReleaseDC(MainWnd,hDC);
	}

	/*deallocate the memory from application thread*/
	//destroy_list(head);

	if(fp)
		fclose(fp);
}

void AlterActiveContourThread(void* aoptions) /*passes address of the stucture with pointer to initial counter*/
{
	contour_position *head = NULL,
		*curr_cont = NULL,
		*next_cont = NULL,
		*prev_cont = NULL;
	FILE			 *fp		= NULL;
	int xPos=0,yPos=0,index = 0,i=0,j=0;
	int thread_count = 0;


	HDC		hDC;
	char filename [50];
	int count = 0;
	head = (contour_position *)aoptions;
	thread_count = head->Thread_index;

	update_contour_display(head,ROWS,COLS,labels,eDisplay);

	hDC=GetDC(MainWnd);
	PaintImage();	
	ReleaseDC(MainWnd,hDC);

	for(i=0;i<Algo_Opt.alter_contour_iteration;i++)
	{
		if(i == Algo_Opt.alter_contour_iteration/2)
		{
			Algo_Opt.alter_EE2_W = 1;/*reduce the weigtage after initial few iterations*/
		}
		update_contour_display(head,ROWS,COLS,labels,eErase);
		AlterActiveContourAlgo(OriginalImage,sobel_image,FileImage,head,ROWS,COLS,&Algo_Opt);
		update_contour_display(head,ROWS,COLS,labels,eDisplay);

		Sleep(100);

		hDC=GetDC(MainWnd);
		PaintImage();	
		ReleaseDC(MainWnd,hDC);
	}

	/*reset the postion falgs back to movable*/
	reset_contour_movestate(head);

	/*deallocate the memory from application thread*/
	//destroy_list(head);

	if(fp)
		fclose(fp);
	return;
}


static void configure_Algo()
{

	/*Based on the original image resolution Algo is configured for best results*/
	if(ROWS == 360 && COLS == 480)/*Mac&Kale*/
	{
		 Algo_Opt.left_contour_iteration = 40;
		 Algo_Opt.left_click_window = 3;
		 Algo_Opt.left_IE1_W = 1;
		 Algo_Opt.left_IE2_W = 1;
		 Algo_Opt.left_EE1_W = 1;
		 Algo_Opt.left_EE2_W = 1;

		 Algo_Opt.right_contour_iteration = 35;
		 Algo_Opt.right_click_window = 5;
		 Algo_Opt.right_IE1_W = 1;
		 Algo_Opt.right_IE2_W = 1;
		 Algo_Opt.right_EE1_W = 1;
		 Algo_Opt.right_EE2_W = 1;
		 Algo_Opt.right_EE3_W = 0;

		 Algo_Opt.alter_contour_iteration = 30;
		 Algo_Opt.alter_click_window = 3;
		 Algo_Opt.alter_IE1_W = 0;
		 Algo_Opt.alter_IE2_W = 1.5;
		 Algo_Opt.alter_EE1_W = 1;
		 Algo_Opt.alter_EE2_W = 1;

	}
	else if(ROWS == 600 && COLS == 720)/*fish lemon rice green*/
	{
		 Algo_Opt.left_contour_iteration = 40;
		 Algo_Opt.left_click_window = 3;
		 Algo_Opt.left_IE1_W = 1;
		 Algo_Opt.left_IE2_W = 1;
		 Algo_Opt.left_EE1_W = 1;
		 Algo_Opt.left_EE2_W = 1;

		 Algo_Opt.right_contour_iteration = 65;
		 Algo_Opt.right_click_window = 5;
		 Algo_Opt.right_IE1_W = 1;
		 Algo_Opt.right_IE2_W = 1;
		 Algo_Opt.right_EE1_W = 1;
		 Algo_Opt.right_EE2_W = 1;
		 Algo_Opt.right_EE3_W = 0;

		 Algo_Opt.alter_contour_iteration = 30;
		 Algo_Opt.alter_click_window = 3;
		 Algo_Opt.alter_IE1_W = 0;
		 Algo_Opt.alter_IE2_W = 1.5;
		 Algo_Opt.alter_EE1_W = 1;
		 Algo_Opt.alter_EE2_W = 1;

	}
	else if(ROWS == 565 && COLS == 849)/*Bacon Egg Toast*/
	{
		 Algo_Opt.left_contour_iteration = 30;
		 Algo_Opt.left_click_window = 5;
		 Algo_Opt.left_IE1_W = 1;
		 Algo_Opt.left_IE2_W = 1;
		 Algo_Opt.left_EE1_W = 1;
		 Algo_Opt.left_EE2_W = 0;

		 Algo_Opt.right_contour_iteration = 50;
		 Algo_Opt.right_click_window = 7;
		 Algo_Opt.right_IE1_W = 1;
		 Algo_Opt.right_IE2_W = 1;
		 Algo_Opt.right_EE1_W = 1;
		 Algo_Opt.right_EE2_W = 1;
		 Algo_Opt.right_EE3_W = 0;

		 Algo_Opt.alter_contour_iteration = 30;
		 Algo_Opt.alter_click_window = 3;
		 Algo_Opt.alter_IE1_W = 0;
		 Algo_Opt.alter_IE2_W = 1.5;
		 Algo_Opt.alter_EE1_W = 1;
		 Algo_Opt.alter_EE2_W = 1;

	}
	else if(ROWS == 1000 && COLS == 1500)/*HP and biscuits*/
	{
		 Algo_Opt.left_contour_iteration = 30;
		 Algo_Opt.left_click_window = 3;
		 Algo_Opt.left_IE1_W = 1;
		 Algo_Opt.left_IE2_W = 1;
		 Algo_Opt.left_EE1_W = 1;
		 Algo_Opt.left_EE2_W = 1;

		 Algo_Opt.right_contour_iteration = 50;
		 Algo_Opt.right_click_window = 9;
		 Algo_Opt.right_IE1_W = 1;
		 Algo_Opt.right_IE2_W = 1;
		 Algo_Opt.right_EE1_W = 1;
		 Algo_Opt.right_EE2_W = 1;
		 Algo_Opt.right_EE3_W = 0;

		 Algo_Opt.alter_contour_iteration = 30;
		 Algo_Opt.alter_click_window = 3;
		 Algo_Opt.alter_IE1_W = 0;
		 Algo_Opt.alter_IE2_W = 1.5;
		 Algo_Opt.alter_EE1_W = 1;
		 Algo_Opt.alter_EE2_W = 1;
	}
	else if(ROWS == 1140 && COLS == 1600)/*Eggs Pancake Milk*/
	{	
		 Algo_Opt.left_contour_iteration = 30;
		 Algo_Opt.left_click_window = 5;
		 Algo_Opt.left_IE1_W = 1;
		 Algo_Opt.left_IE2_W = 1;
		 Algo_Opt.left_EE1_W = 1;
		 Algo_Opt.left_EE2_W = 1;

		 Algo_Opt.right_contour_iteration = 65;
		 Algo_Opt.right_click_window = 5;
		 Algo_Opt.right_IE1_W = 1;
		 Algo_Opt.right_IE2_W = 1;
		 Algo_Opt.right_EE1_W = 1;
		 Algo_Opt.right_EE2_W = 1;
		 Algo_Opt.right_EE3_W = 0;

		 Algo_Opt.alter_contour_iteration = 30;
		 Algo_Opt.alter_click_window = 3;
		 Algo_Opt.alter_IE1_W = 0;
		 Algo_Opt.alter_IE2_W = 1.5;
		 Algo_Opt.alter_EE1_W = 1;
		 Algo_Opt.alter_EE2_W = 1;

	}
	else
	{
		 Algo_Opt.left_contour_iteration = 30;
		 Algo_Opt.left_click_window = 3;
		 Algo_Opt.left_IE1_W = 1;
		 Algo_Opt.left_IE2_W = 1;
		 Algo_Opt.left_EE1_W = 1;
		 Algo_Opt.left_EE2_W = 1;

		 Algo_Opt.right_contour_iteration = 50;
		 Algo_Opt.right_click_window = 5;
		 Algo_Opt.right_IE1_W = 1;
		 Algo_Opt.right_IE2_W = 1;
		 Algo_Opt.right_EE1_W = 1;
		 Algo_Opt.right_EE2_W = 1;
		 Algo_Opt.right_EE3_W = 0;

		 Algo_Opt.alter_contour_iteration = 30;
		 Algo_Opt.alter_click_window = 3;
		 Algo_Opt.alter_IE1_W = 0;
		 Algo_Opt.alter_IE2_W = 1.5;
		 Algo_Opt.alter_EE1_W = 1;
		 Algo_Opt.alter_EE2_W = 1;
	}
}