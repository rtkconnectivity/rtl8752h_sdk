#include "epaper.h"

#include "GUI_Paint.h"
#include "imagedata.h"

#include "os_mem.h"
#include "os_sched.h"

#include <stdlib.h>
#include "rtl876x_gpio.h"
#include "trace.h"
#include "rtl876x_i2c.h"
#include "bmp80.h"

uint8_t WF_PARTIAL_2IN13_V3[159] =
{
	0x0,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x80,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x40,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x14,0x0,0x0,0x0,0x0,0x0,0x0,  
	0x1,0x0,0x0,0x0,0x0,0x0,0x0,
	0x1,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,
	0x22,0x22,0x22,0x22,0x22,0x22,0x0,0x0,0x0,
	0x22,0x17,0x41,0x00,0x32,0x36,
};

uint8_t WS_20_30_2IN13_V3[159] =
{											
	0x80,	0x4A,	0x40,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x40,	0x4A,	0x80,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x80,	0x4A,	0x40,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x40,	0x4A,	0x80,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
	0xF,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
	0xF,	0x0,	0x0,	0xF,	0x0,	0x0,	0x2,					
	0xF,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
	0x1,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,					
	0x22,	0x22,	0x22,	0x22,	0x22,	0x22,	0x0,	0x0,	0x0,			
	0x22,	0x17,	0x41,	0x0,	0x32,	0x36						
};

int epaper_start(void)
{
	//DC - 0, CS - 0, PWR - 1, RST - 1
	GPIO_ResetBits(DC_PIN);
	GPIO_SetBits(PWR_PIN);
	GPIO_SetBits(RST_PIN);
	return 0;
}

static void epaper_exit(void)
{
	GPIO_ResetBits(DC_PIN);
	GPIO_ResetBits(PWR_PIN);
	GPIO_ResetBits(RST_PIN);
}

static void epaper_reset(void)
{
	GPIO_SetBits(RST_PIN);
	os_delay(20);
	GPIO_ResetBits(RST_PIN);
	os_delay(2);
	GPIO_SetBits(RST_PIN);
	os_delay(20);
}

static void epaper_sendcommand(uint8_t reg)
{
	GPIO_ResetBits(DC_PIN);
	uint8_t data_buf[] = {reg};
	SPI_SendBuffer(SPI0, data_buf, sizeof(data_buf));
}

static void epaper_senddata(uint8_t data)
{
	GPIO_SetBits(DC_PIN);
	uint8_t data_buf[] = {data};
	SPI_SendBuffer(SPI0, data_buf, sizeof(data_buf));
}

static void epaper_readbusy(void)
{
	//Debug
	APP_PRINT_INFO0("e-Paper busy\r\n");
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIO_GetPin(BUSY_PIN))==0)
		{
			break;
		}
		os_delay(10);
	}
	os_delay(10);
	//Debug
	APP_PRINT_INFO0("e-Paper busy release\r\n");
}

static void epaper_turnondisplay(void)
{
	epaper_sendcommand(0x22);
	epaper_senddata(0xc7);
	epaper_sendcommand(0x20);
	epaper_readbusy();
}


static void epaper_turnondisplaypartial(void)
{
	epaper_sendcommand(0x22);
	epaper_senddata(0x0f);
	epaper_sendcommand(0x20);
	epaper_readbusy();
}

static void epaper_lut(uint8_t *lut)
{
		unsigned int count;
		epaper_sendcommand(0x32);
		for(count = 0; count < 153; count++) {
			epaper_senddata(lut[count]);
		}
		epaper_readbusy();
}

static void epaper_lutbyhost(uint8_t *lut)
{
	epaper_lut((uint8_t*)lut);
	epaper_sendcommand(0x3f);
	epaper_senddata(*(lut+153));
	epaper_sendcommand(0x03);
	epaper_senddata(*(lut+154));
	epaper_sendcommand(0x04);
	epaper_senddata(*(lut+155));
	epaper_senddata(*(lut+156));
	epaper_senddata(*(lut+157));
	epaper_sendcommand(0x2c);
	epaper_senddata(*(lut+158));
}

static void epaper_setwindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
    epaper_sendcommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    epaper_senddata((Xstart>>3) & 0xFF);
    epaper_senddata((Xend>>3) & 0xFF);
	
    epaper_sendcommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    epaper_senddata(Ystart & 0xFF);
    epaper_senddata((Ystart >> 8) & 0xFF);
    epaper_senddata(Yend & 0xFF);
    epaper_senddata((Yend >> 8) & 0xFF);
}

static void epaper_setcursor(uint16_t Xstart, uint16_t Ystart)
{
    epaper_sendcommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    epaper_senddata(Xstart & 0xFF);

    epaper_sendcommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    epaper_senddata(Ystart & 0xFF);
    epaper_senddata((Ystart >> 8) & 0xFF);
}

void epaper_init(void)
{
	epaper_reset();
	os_delay(100);

	epaper_readbusy();   
	APP_PRINT_INFO0("SWRESET Command");
	epaper_sendcommand(0x12);  //SWRESET
	epaper_readbusy();   

	epaper_sendcommand(0x01); //Driver output control      
	epaper_senddata(0xf9);
	epaper_senddata(0x00);
	epaper_senddata(0x00);

	epaper_sendcommand(0x11); //data entry mode       
	epaper_senddata(0x03);
	
	epaper_setwindows(0, 0, EPAPER_WIDTH-1, EPAPER_HEIGHT-1);
	epaper_setcursor(0, 0);
	
	epaper_sendcommand(0x3C); //BorderWavefrom
	epaper_senddata(0x05);	

	epaper_sendcommand(0x21); //  Display update control
	epaper_senddata(0x00);
	epaper_senddata(0x80);	

	epaper_sendcommand(0x18); //Read built-in temperature sensor
	epaper_senddata(0x80);	

	epaper_readbusy();
	epaper_lutbyhost(WS_20_30_2IN13_V3);
}



void epaper_clear(void)
{
	uint16_t width, height;
	width = (EPAPER_WIDTH % 8 == 0)? (EPAPER_WIDTH/8): (EPAPER_WIDTH/8 + 1);
	height = EPAPER_HEIGHT;
	
	epaper_sendcommand(0x24);
	for(uint16_t j = 0; j < height; j++) {
		for(uint16_t i = 0; i < width; i++) {
			epaper_senddata(0xFF);
		}
	}
	
	epaper_turnondisplay();
}

void epaper_display(uint8_t *Image)
{
	uint16_t width, height;
	width = (EPAPER_WIDTH % 8 == 0)? (EPAPER_WIDTH/8): (EPAPER_WIDTH/8 + 1);
	height = EPAPER_HEIGHT;
	epaper_sendcommand(0x24);
	//for(uint16_t j = 0; j < height; j++) {
	for(uint16_t j = height; j >0; j--)  {
		//APP_PRINT_INFO1("J: %d", j);
		for(uint16_t i = 0; i < width; i++) {
			//APP_PRINT_INFO1("I: %d", i);
			epaper_senddata(Image[i + (j-1) * width]);
		}
	}
	epaper_turnondisplay();
}

void epaper_displaybase(uint8_t *Image)
{
	uint16_t width, height;
	width = (EPAPER_WIDTH % 8 == 0)? (EPAPER_WIDTH/8): (EPAPER_WIDTH/8 + 1);
	height = EPAPER_HEIGHT;
	
	epaper_sendcommand(0x24);
	//for(uint16_t j = 0; j < height; j++) {
	for(uint16_t j = height; j >0; j--)  {
		for(uint16_t i = 0; i < width; i++) {
			epaper_senddata(Image[i + (j-1) * width]);
		}
	}
	epaper_sendcommand(0x26);
	//for(uint16_t j = 0; j < height; j++) {
	for(uint16_t j = height; j >0; j--)  {
		for(uint16_t i = 0; i < width; i++) {
			epaper_senddata(Image[i + (j-1) * width]);
		}
	}
	epaper_turnondisplay();
}


void epaper_partial_display(uint8_t *Image)
{
	uint8_t Width, Height;
  Width = (EPAPER_WIDTH % 8 == 0)? (EPAPER_WIDTH / 8 ): (EPAPER_WIDTH / 8 + 1);
  Height = EPAPER_HEIGHT;
	
	//Reset
  GPIO_WriteBit(RST_PIN, (BitAction)(0));
  os_delay(1);
  GPIO_WriteBit(RST_PIN, (BitAction)(1));

	epaper_lutbyhost(WF_PARTIAL_2IN13_V3);

	epaper_sendcommand(0x37); 
	epaper_senddata(0x00);  
	epaper_senddata(0x00);  
	epaper_senddata(0x00);  
	epaper_senddata(0x00); 
	epaper_senddata(0x00);  
	epaper_senddata(0x40);  ///RAM Ping-Pong enable 
	epaper_senddata(0x00);  
	epaper_senddata(0x00);   
	epaper_senddata(0x00);  
	epaper_senddata(0x00);

	epaper_sendcommand(0x3C); //BorderWavefrom
	epaper_senddata(0x80);	

	epaper_sendcommand(0x22); //Display Update Sequence Option
	epaper_senddata(0xC0);    // Enable clock and  Enable analog
	epaper_sendcommand(0x20);  //Activate Display Update Sequence
	epaper_readbusy();  
	
	epaper_setwindows(0, 0, EPAPER_WIDTH-1, EPAPER_HEIGHT-1);
	epaper_setcursor(0, 0);

	epaper_sendcommand(0x24);   //Write Black and White image to RAM
    for (uint8_t j = 0; j < Height; j++) {
        for (uint8_t i = 0; i < Width; i++) {
			epaper_senddata(Image[i + j * Width]);
		}
	}
	epaper_turnondisplaypartial();
}


void epaper_displaypartial(UBYTE *Image)
{
	UWORD Width, Height;
  Width = (EPAPER_WIDTH % 8 == 0)? (EPAPER_WIDTH / 8 ): (EPAPER_WIDTH / 8 + 1);
  Height = EPAPER_HEIGHT;
	
	//Reset
  GPIO_ResetBits(RST_PIN);
  os_delay(1);
  GPIO_SetBits(RST_PIN);

	epaper_lutbyhost(WF_PARTIAL_2IN13_V3);

	epaper_sendcommand(0x37); 
	epaper_senddata(0x00);  
	epaper_senddata(0x00);  
	epaper_senddata(0x00);  
	epaper_senddata(0x00); 
	epaper_senddata(0x00);  
	epaper_senddata(0x40);  ///RAM Ping-Pong enable 
	epaper_senddata(0x00);  
	epaper_senddata(0x00);   
	epaper_senddata(0x00);  
	epaper_senddata(0x00);

	epaper_sendcommand(0x3C); //BorderWavefrom
	epaper_senddata(0x80);	

	epaper_sendcommand(0x22); //Display Update Sequence Option
	epaper_senddata(0xC0);    // Enable clock and  Enable analog
	epaper_sendcommand(0x20);  //Activate Display Update Sequence
	epaper_readbusy();  
	
	epaper_setwindows(0, 0, EPAPER_WIDTH-1, EPAPER_HEIGHT-1);
	epaper_setcursor(0, 0);

	epaper_sendcommand(0x24);   //Write Black and White image to RAM
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
			epaper_senddata(Image[i + j * Width]);
		}
	}
	epaper_turnondisplaypartial();
}


void epaper_Sleep(void)
{
	epaper_sendcommand(0x10); //enter deep sleep
	epaper_senddata(0x01); 
	os_delay(100);
}

int epaper_redraw_temp(int32_t);

int epaper_clear_image(void)
{
	if(epaper_start()!=0){
		return -1;
	}
	epaper_init();
	epaper_clear();
	
	uint8_t *blackimage;
	APP_PRINT_INFO2("Epaper height: %d, Epaper width: %d", EPAPER_HEIGHT, EPAPER_WIDTH);
	uint16_t imagesize = ((EPAPER_WIDTH % 8 == 0)? (EPAPER_WIDTH/8): (EPAPER_WIDTH/8 + 1)) * EPAPER_WIDTH;
	APP_PRINT_INFO0("OS Memory Allocation");
	blackimage = (uint8_t *)os_mem_alloc(RAM_TYPE_DATA_ON,imagesize);
	if(blackimage == NULL) {
		return -1;
	}
	
	APP_PRINT_INFO0("Allocation successful");
	Paint_NewImage(blackimage, EPAPER_WIDTH, EPAPER_HEIGHT, 90, WHITE);
	Paint_Clear(WHITE);
	os_delay(2000);
	
	APP_PRINT_INFO0("New Image successful");
  //show image for array    
  //Debug("show image for array\r\n");
  Paint_SelectImage(blackimage);
  Paint_Clear(WHITE);
	epaper_display(blackimage);
	epaper_displaybase(blackimage);
	os_delay(1000);
	epaper_Sleep();
	os_mem_free(blackimage);
  blackimage = NULL;
	os_delay(2000);
	epaper_clear();
	return 0;
}
int epaper_test(void)
{
	if(epaper_start()!=0){
		return -1;
	}
	epaper_init();
	epaper_clear();
	
	uint8_t *blackimage;
	APP_PRINT_INFO2("Epaper height: %d, Epaper width: %d", EPAPER_HEIGHT, EPAPER_WIDTH);
	uint16_t imagesize = ((EPAPER_WIDTH % 8 == 0)? (EPAPER_WIDTH/8): (EPAPER_WIDTH/8 + 1)) * EPAPER_WIDTH;
	APP_PRINT_INFO0("OS Memory Allocation");
	blackimage = (uint8_t *)os_mem_alloc(RAM_TYPE_DATA_ON,imagesize);
	if(blackimage == NULL) {
		return -1;
	}
	APP_PRINT_INFO0("Allocation successful");
	Paint_NewImage(blackimage, EPAPER_WIDTH, EPAPER_HEIGHT, 90, WHITE);
	Paint_Clear(WHITE);
	os_delay(2000);
	
	APP_PRINT_INFO0("New Image successful");
#if 1   //show image for array    
  //Debug("show image for array\r\n");
  Paint_SelectImage(blackimage);
  Paint_Clear(WHITE);
  Paint_DrawBitMap(gImage_2in13);
	APP_PRINT_INFO0("DrawBitMap successful");

  epaper_display(blackimage);
  os_delay(3000);
	APP_PRINT_INFO0("epaper display drawbitmap successful");
#endif

#if 1  // Drawing on the image
		
		int8_t write_data[TransferLength] ;

    for (uint8_t i = 0; i < TransferLength; i++)
    {
        write_data[i] = i;
    }
		
		while(!begin_bmp())
		{
				DBG_DIRECT("Failed to connect to BMP80");
				//os_delay(10);
		}
		
		//while(true)	
		float temp = readTemperature();
		DBG_DIRECT("Temperature read: %f", temp);
		os_delay(3000);
	Paint_NewImage(blackimage, EPAPER_WIDTH, EPAPER_HEIGHT, 90, WHITE);  	
    //Debug("Drawing\r\n");
    //1.Select Image
  Paint_SelectImage(blackimage);
  Paint_Clear(WHITE);
	
    // 2.Drawing on the image
  //Paint_DrawPoint(5, 10, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
  //Paint_DrawPoint(5, 25, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
  //Paint_DrawPoint(5, 40, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
  //Paint_DrawPoint(5, 55, BLACK, DOT_PIXEL_4X4, DOT_STYLE_DFT);

  //Paint_DrawLine(20, 10, 70, 60, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  //Paint_DrawLine(70, 10, 20, 60, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  //Paint_DrawRectangle(20, 10, 70, 60, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  //Paint_DrawRectangle(85, 10, 135, 60, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

  //Paint_DrawLine(45, 15, 45, 55, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
  //Paint_DrawLine(25, 35, 70, 35, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
  //Paint_DrawCircle(45, 35, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  //Paint_DrawCircle(110, 35, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

  Paint_DrawString_EN(120, 15, "Temperature: ", &Font16, BLACK, WHITE);
  Paint_DrawNum(140, 40, (int32_t)temp , &Font16, BLACK, WHITE);

    //Paint_DrawString_CN(140, 60, "ÄãºÃabc", &Font12CN, BLACK, WHITE);
    //Paint_DrawString_CN(5, 65, "Î¢Ñ©µç×Ó", &Font24CN, WHITE, BLACK);
		
	Paint_SetRotate(ROTATE_90);
  epaper_displaybase(blackimage);
  os_delay(3000);
#endif

#if 0   //Partial refresh, example shows time
	Paint_NewImage(blackimage, EPAPER_WIDTH, EPAPER_HEIGHT, 90, WHITE);  
  APP_PRINT_INFO0("Partial refresh\r\n");
  Paint_SelectImage(blackimage);
	
  PAINT_TIME sPaint_time;
  sPaint_time.Hour = 12;
  sPaint_time.Min = 34;
  sPaint_time.Sec = 56;
  UBYTE num = 10;
  for (;;) {
      sPaint_time.Sec = sPaint_time.Sec + 1;
      if (sPaint_time.Sec == 60) {
          sPaint_time.Min = sPaint_time.Min + 1;
          sPaint_time.Sec = 0;
          if (sPaint_time.Min == 60) {
              sPaint_time.Hour =  sPaint_time.Hour + 1;
              sPaint_time.Min = 0;
              if (sPaint_time.Hour == 24) {
                  sPaint_time.Hour = 0;
                  sPaint_time.Min = 0;
                  sPaint_time.Sec = 0;
              }
          }
      }
      Paint_ClearWindows(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, WHITE);
      Paint_DrawTime(150, 80, &sPaint_time, &Font20, WHITE, BLACK);

      num = num - 1;
      if(num == 0) {
          break;
      }
		epaper_displaypartial(blackimage);
		epaper_displaybase(blackimage);
    os_delay(1000);//Analog clock 1s
		APP_PRINT_INFO0("One Second");
  }
#endif
#if 0
		Paint_NewImage(blackimage, EPAPER_WIDTH, EPAPER_HEIGHT, 90, WHITE); 
		Paint_SelectImage(blackimage);
		Paint_Clear(WHITE);
		Paint_DrawBitMap(gImage_2in13);
		APP_PRINT_INFO0("DrawBitMap successful");

		epaper_display(blackimage);
		
		os_delay(3000);
		APP_PRINT_INFO0("epaper display drawbitmap successful");
#endif
		os_delay(3000);
    //Debug("Drawing\r\n");
    //1.Select Image
		Paint_NewImage(blackimage, EPAPER_WIDTH, EPAPER_HEIGHT, 90, WHITE);  	
		Paint_SelectImage(blackimage);
		Paint_Clear(WHITE);
		Paint_DrawString_EN(120, 15, "New Temp: ", &Font16, BLACK, WHITE);
		Paint_DrawNum(140, 40, 90, &Font16, BLACK, WHITE);
		Paint_SetRotate(ROTATE_90);
		epaper_display(blackimage);
		APP_PRINT_INFO0("Display");
		
		os_delay(3000);
		
		APP_PRINT_INFO0("Clear...\r\n");
		epaper_init();
    epaper_clear();
	
    APP_PRINT_INFO0("Goto Sleep...\r\n");
    epaper_Sleep();
    os_mem_free(blackimage);
    blackimage = NULL;
    os_delay(2000);//important, at least 2s
    // close 5V
    APP_PRINT_INFO0("close 5V, Module enters 0 power consumption ...\r\n");
    epaper_exit();
    return 0;
	
}

int epaper_redraw_temp(int32_t temp)
{
	if(epaper_start()!=0){
		return -1;
	}
	epaper_init();
	epaper_clear();
	
	uint8_t *blackimage;
	APP_PRINT_INFO2("Epaper height: %d, Epaper width: %d", EPAPER_HEIGHT, EPAPER_WIDTH);
	uint16_t imagesize = ((EPAPER_WIDTH % 8 == 0)? (EPAPER_WIDTH/8): (EPAPER_WIDTH/8 + 1)) * EPAPER_WIDTH;
	APP_PRINT_INFO0("OS Memory Allocation");
	blackimage = (uint8_t *)os_mem_alloc(RAM_TYPE_DATA_ON, imagesize);
	if(blackimage == NULL) {
		return -1;
	}
	APP_PRINT_INFO0("Allocation successful");
	Paint_NewImage(blackimage, EPAPER_WIDTH, EPAPER_HEIGHT, 90, WHITE);
	Paint_Clear(WHITE);
	os_delay(2000);
	
	APP_PRINT_INFO0("New Image successful");
#if 1   //show image for array    
  //Debug("show image for array\r\n");
  Paint_SelectImage(blackimage);
  Paint_Clear(WHITE);
  Paint_DrawBitMap(gImage_2in13);
	APP_PRINT_INFO0("DrawBitMap successful");

  epaper_display(blackimage);
  os_delay(2000);
	APP_PRINT_INFO0("epaper display drawbitmap successful");
#endif

#if 1  // Drawing on the image
	Paint_NewImage(blackimage, EPAPER_WIDTH, EPAPER_HEIGHT, 90, WHITE);  	
    //Debug("Drawing\r\n");
    //1.Select Image
  Paint_SelectImage(blackimage);
  Paint_Clear(WHITE);

  Paint_DrawString_EN(120, 15, "Temperature: ", &Font16, BLACK, WHITE);
  Paint_DrawNum(140, 40, temp, &Font16, BLACK, WHITE);
		
	Paint_SetRotate(ROTATE_90);
  epaper_displaybase(blackimage);
  os_delay(3000);
#endif

#if 0   //Partial refresh, example shows time
	Paint_NewImage(blackimage, EPAPER_WIDTH, EPAPER_HEIGHT, 90, WHITE);  
    //Debug("Partial refresh\r\n");
  Paint_SelectImage(blackimage);
	
  PAINT_TIME sPaint_time;
  sPaint_time.Hour = 12;
  sPaint_time.Min = 34;
  sPaint_time.Sec = 56;
  UBYTE num = 10;
  for (;;) {
      sPaint_time.Sec = sPaint_time.Sec + 1;
      if (sPaint_time.Sec == 60) {
          sPaint_time.Min = sPaint_time.Min + 1;
          sPaint_time.Sec = 0;
          if (sPaint_time.Min == 60) {
              sPaint_time.Hour =  sPaint_time.Hour + 1;
              sPaint_time.Min = 0;
              if (sPaint_time.Hour == 24) {
                  sPaint_time.Hour = 0;
                  sPaint_time.Min = 0;
                  sPaint_time.Sec = 0;
              }
          }
      }
      Paint_ClearWindows(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, WHITE);
      Paint_DrawTime(150, 80, &sPaint_time, &Font20, WHITE, BLACK);

      num = num - 1;
      if(num == 0) {
          break;
      }
		epaper_displaypartial(blackimage);
    os_delay(500);//Analog clock 1s
  }
#endif

		//Paint_NewImage(blackimage, EPAPER_WIDTH, EPAPER_HEIGHT, 90, WHITE);  	
    //Debug("Drawing\r\n");
    //1.Select Image
		//Paint_SelectImage(blackimage);
		//Paint_Clear(WHITE);
		
		APP_PRINT_INFO0("Temperature");
		Paint_DrawString_EN(120, 15, "Temperature: ", &Font16, BLACK, WHITE);
		APP_PRINT_INFO0("90");
		Paint_DrawNum(140, temp, 90, &Font16, BLACK, WHITE);
		APP_PRINT_INFO0("Rotate");
		Paint_SetRotate(ROTATE_90);
		APP_PRINT_INFO0("Display");
		epaper_displaybase(blackimage);
		os_delay(3000);
		
		APP_PRINT_INFO0("Clear...\r\n");
		epaper_init();
    epaper_clear();
	
    APP_PRINT_INFO0("Goto Sleep...\r\n");
    epaper_Sleep();
    os_mem_free(blackimage);
    blackimage = NULL;
    os_delay(2000);//important, at least 2s
    // close 5V
    APP_PRINT_INFO0("close 5V, Module enters 0 power consumption ...\r\n");
    epaper_exit();
    return 0;	
}