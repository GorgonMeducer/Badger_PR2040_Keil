/****************************************************************************
*  Copyright 2025 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/
/*============================ INCLUDES ======================================*/
#include "./platform.h"

#include "hardware/spi.h"

#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "arm_2d_disp_adapters.h"

/*============================ MACROS ========================================*/
#ifndef EPD_SCREEN_HEIGHT
#   define EPD_SCREEN_HEIGHT 296
#endif

#ifndef EPD_SCREEN_WIDTH
#   define EPD_SCREEN_WIDTH  128
#endif

#ifndef EPD_SPI_FREQ
#   define EPD_SPI_FREQ 12000000ul
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define SEND_LUT(__CMD, __LUT)                                                  \
    epd_send_cmd(__CMD);                                                        \
    for(uint32_t wCount=0; wCount < dimof(__LUT); wCount++) {                   \
        epd_send_data(__LUT[wCount]);                                           \
    }

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

/**
 * full screen update LUT
**/
const unsigned char epd_lut_vcom_dc[] = {
    0x00, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x60, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
};
const unsigned char epd_lut_ww[] = {
    0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
    0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char epd_lut_bw[] = {
    0x40, 0x17, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x0F, 0x0F, 0x00, 0x00, 0x03,
    0x40, 0x0A, 0x01, 0x00, 0x00, 0x01,
    0xA0, 0x0E, 0x0E, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char epd_lut_wb[] = {
    0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char epd_lut_bb[] = {
    0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
    0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
    0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


/**
 * partial screen update LUT
**/
const unsigned char epd_lut_vcom1[] = {
    0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    ,0x00, 0x00,
};
const unsigned char epd_lut_ww1[] = {
    0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char epd_lut_bw1[] = {
    0x80, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char epd_lut_wb1[] = {
    0x40, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char epd_lut_bb1[] = {
    0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

enum {

    EPD_CS_PIN    = 17,
    EPD_CLK_PIN   = 18,
    EPD_MOSI_PIN  = 19,
    EPD_DC_PIN    = 20,
    EPD_RST_PIN   = 21,
    EPD_BUSY_PIN  = 26,

};

#define SPI_PORT    spi0


/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void epd_spi_write_byte(uint8_t Value)
{
    spi_write_blocking((spi_inst_t *)SPI_PORT, &Value, 1);
}

static void epd_screen_reset(void)
{
    gpio_put(EPD_RST_PIN, 1);
    perfc_delay_ms(200);
    gpio_put(EPD_RST_PIN, 0);
    perfc_delay_ms(2);
    gpio_put(EPD_RST_PIN, 1);
    perfc_delay_ms(200);
}

static void epd_send_cmd(uint8_t Reg)
{
    gpio_put(EPD_DC_PIN, 0);
    gpio_put(EPD_CS_PIN, 0);
    epd_spi_write_byte(Reg);
    gpio_put(EPD_CS_PIN, 1);
}

static void epd_send_data(uint8_t Data)
{
    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_CS_PIN, 0);
    epd_spi_write_byte(Data);
    gpio_put(EPD_CS_PIN, 1);
}

__STATIC_INLINE 
void epd_read_busy(void)
{
    do {
        epd_send_cmd(0x71);
    } while(!(gpio_get(EPD_BUSY_PIN) & 0x01));
}

void epd_screen_init(void)
{
    gpio_set_function(EPD_RST_PIN, GPIO_FUNC_SIO);
    gpio_init(EPD_RST_PIN);
    gpio_set_dir(EPD_RST_PIN, GPIO_OUT);
    gpio_put(EPD_RST_PIN, 1);

    gpio_set_function(EPD_DC_PIN, GPIO_FUNC_SIO);
    gpio_init(EPD_DC_PIN);
    gpio_set_dir(EPD_DC_PIN, GPIO_OUT);
    gpio_put(EPD_DC_PIN, 1);

    gpio_set_function(EPD_CS_PIN, GPIO_FUNC_SIO);
    gpio_init(EPD_CS_PIN);
    gpio_set_dir(EPD_CS_PIN, GPIO_OUT);
    gpio_put(EPD_CS_PIN, 1);

    gpio_set_function(EPD_BUSY_PIN, GPIO_FUNC_SIO);
    gpio_init(EPD_BUSY_PIN);
    gpio_set_dir(EPD_BUSY_PIN, GPIO_IN);

    spi_init((spi_inst_t *)SPI_PORT, EPD_SPI_FREQ);
    gpio_set_function(EPD_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(EPD_MOSI_PIN, GPIO_FUNC_SPI);

    epd_screen_reset();

    epd_send_cmd(0x01);         //! Power Settings
    epd_send_data(0x03);
    epd_send_data(0x00);
    epd_send_data(0x2b);
    epd_send_data(0x2b);
    epd_send_data(0x03);

    epd_send_cmd(0x06);         //! boost soft start
    epd_send_data(0x17);        //A
    epd_send_data(0x17);        //B
    epd_send_data(0x17);        //C

    epd_send_cmd(0x04);
    epd_read_busy();

    epd_send_cmd(0x00);         //panel setting
    epd_send_data(0xbf);        //128x296
    epd_send_data(0x0e);        //VCOM to 0V fast

    epd_send_cmd(0x30);         //PLL setting
    epd_send_data(0x3a);        // 3a 100HZ   29 150Hz 39 200HZ 31 171HZ

    epd_send_cmd(0x61);
    epd_send_data(EPD_SCREEN_WIDTH);                                            //!< width
    epd_send_data((EPD_SCREEN_HEIGHT >> 8) & 0xff);                             //!< height high byte
    epd_send_data(EPD_SCREEN_HEIGHT & 0xff);                                    //!< height low byte

    epd_send_cmd(0x82);         //vcom_DC setting
    epd_send_data(0x28);
}

static void epd_set_partial_refresh_mode(void)
{
    epd_send_cmd(0x82);
    epd_send_data(0x00);
    epd_send_cmd(0x50);
    epd_send_data(0xb7);

    
    SEND_LUT(0x20, epd_lut_vcom1);
    SEND_LUT(0x21, epd_lut_ww1);
    SEND_LUT(0x22, epd_lut_bw1);
    SEND_LUT(0x23, epd_lut_wb1);
    SEND_LUT(0x24, epd_lut_bb1);
}

static void epd_set_full_refresh_mode(void)
{
    epd_send_cmd(0x50);
    epd_send_data(0xb7);

    SEND_LUT(0x20, epd_lut_vcom_dc);
    SEND_LUT(0x21, epd_lut_ww);
    SEND_LUT(0x22, epd_lut_bw);
    SEND_LUT(0x23, epd_lut_wb);
    SEND_LUT(0x24, epd_lut_bb);
}

void epd_sceen_clear(void)
{
    int16_t iWidth = (EPD_SCREEN_WIDTH + 7) >> 3;
    int16_t iHeight =  EPD_SCREEN_HEIGHT;

    epd_send_cmd(0x10);
    for (int16_t j = 0; j < iHeight; j++) {
        for (int16_t i = 0; i < iWidth; i++) {
            epd_send_data(0x00);
        }
    }

    epd_send_cmd(0x13);
    for (int16_t j = 0; j < iHeight; j++) {
        for (int16_t i = 0; i < iWidth; i++) {
            epd_send_data(0xFF);
        }
    }

    epd_set_full_refresh_mode();
    
    while(!epd_flush()) __NOP();
}

static
void epd_screen_set_window(int16_t iX, int16_t iY, int16_t iWidth, int16_t iHeight)
{
    epd_set_partial_refresh_mode();
    epd_send_cmd(0x91);            //This command makes the display enter partial mode
    epd_send_cmd(0x90);            //resolution setting
    

    epd_send_data(iX);                 //x-start
    epd_send_data(iX + iWidth - 1);    //x-end

    epd_send_data(iY >> 8);
    epd_send_data(iY & 0xFF);          //y-start
    
    iY += iHeight - 1;
    epd_send_data(iY);
    epd_send_data(iY & 0xFF);          //y-end
    epd_send_data(0x28);
}

void EPD_DrawBitmap(int16_t iX, int16_t iY, int16_t iWidth, int16_t iHeight, const uint8_t *pchBuffer)
{
    assert((iX & 0x7) == 0);
    assert((iWidth & 0x7) == 0);


    iX += iWidth - 1;

    int16_t iRotatedX = iY;
    int16_t iRotatedY = EPD_SCREEN_HEIGHT - iX - 1;
    int16_t iRotatedWidth = iHeight;
    int16_t iRotatedHeight = iWidth;

    epd_screen_set_window(iRotatedX, iRotatedY, iRotatedWidth, iRotatedHeight);
    
    epd_send_cmd(0x13);

    for (int16_t i = 0; i < iRotatedHeight; i++) {

        for (int16_t j = 0; j < iRotatedWidth; j+= 8) {
            uint8_t chData = 0;

            const uint8_t *pchBlock = &pchBuffer[iWidth - i - 1 + j * iWidth];
            
            chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
            pchBlock += iWidth;
            chData <<= 1;
            
            chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
            pchBlock += iWidth;
            chData <<= 1;
            
            chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
            pchBlock += iWidth;
            chData <<= 1;
            
            chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
            pchBlock += iWidth;
            chData <<= 1;
            
            chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
            pchBlock += iWidth;
            chData <<= 1;
            
            chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
            pchBlock += iWidth;
            chData <<= 1;
            
            chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
            pchBlock += iWidth;
            chData <<= 1;
            
            chData |= *pchBlock >= 0x80 ? 0x01 : 0x00;
            pchBlock += iWidth;

            epd_send_data(chData);
        
        }
    }
    
    epd_send_cmd(0x11);
    epd_send_cmd(0x92);

}

void Disp0_DrawBitmap(  int16_t x, 
                        int16_t y, 
                        int16_t width, 
                        int16_t height, 
                        const uint8_t *bitmap)
{
    EPD_DrawBitmap(x, y, width, height, bitmap);
}

bool epd_flush(void)
{
    static enum {
        START = 0,
        WAIT_BUSY,
    } s_chState = START;
    
    switch (s_chState) {
        case START:
            s_chState++;
            epd_send_cmd(0x12);
            //fall-through;
        case WAIT_BUSY:
            epd_send_cmd(0x71);
            if (!(gpio_get(EPD_BUSY_PIN) & 0x01)) {
                break;
            }
            s_chState = START;
            return true;

        default:
            s_chState = START;
            break;
    }

    return false;
}