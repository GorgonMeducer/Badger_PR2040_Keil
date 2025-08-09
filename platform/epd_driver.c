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
    do {                                                                        \
        epd_send_cmd(__CMD);                                                    \
        epd_send_data((uint8_t *)(__LUT), sizeof(__LUT));                       \
    } while(0)


/*============================ TYPES =========================================*/

enum {
    PANEL_SETTING                             = 0x00,
    POWER_SETTING                             = 0x01,
    POWER_OFF                                 = 0x02,
    POWER_OFF_SEQUENCE_SETTING                = 0x03,
    POWER_ON                                  = 0x04,
    POWER_ON_MEASURE                          = 0x05,
    BOOSTER_SOFT_START                        = 0x06,
    DEEP_SLEEP                                = 0x07,
    DATA_START_TRANSMISSION_1                 = 0x10,
    DATA_STOP                                 = 0x11,
    DISPLAY_REFRESH                           = 0x12,
    DATA_START_TRANSMISSION_2                 = 0x13,
    LUT_VCOM                                  = 0x20,
    LUT_WW                                    = 0x21,
    LUT_BW                                    = 0x22,
    LUT_WB                                    = 0x23,
    LUT_BB                                    = 0x24,
    PLL_CONTROL                               = 0x30,
    TEMPERATURE_SENSOR_COMMAND                = 0x40,
    TEMPERATURE_SENSOR_CALIBRATION            = 0x41,
    TEMPERATURE_SENSOR_WRITE                  = 0x42,
    TEMPERATURE_SENSOR_READ                   = 0x43,
    VCOM_AND_DATA_INTERVAL_SETTING            = 0x50,
    LOW_POWER_DETECTION                       = 0x51,
    TCON_SETTING                              = 0x60,
    TCON_RESOLUTION                           = 0x61,
    SOURCE_AND_GATE_START_SETTING             = 0x62,
    GET_STATUS                                = 0x71,
    AUTO_MEASURE_VCOM                         = 0x80,
    VCOM_VALUE                                = 0x81,
    VCM_DC_SETTING_REGISTER                   = 0x82,
    PARTIAL_WINDOW                            = 0x90,
    PARTIAL_IN                                = 0x91,
    PARTIAL_OUT                               = 0x92,
    PROGRAM_MODE                              = 0xA0,
    ACTIVE_PROGRAMMING                        = 0xA1,
    READ_OTP                                  = 0xA2,
    POWER_SAVING                              = 0xE3,
};

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
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,
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

void epd_spi_write(uint8_t *pchBuff, size_t tSize)
{
    spi_write_blocking((spi_inst_t *)SPI_PORT, pchBuff, tSize);
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

static void epd_send_byte(uint8_t Data)
{
    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_CS_PIN, 0);
    epd_spi_write_byte(Data);
    gpio_put(EPD_CS_PIN, 1);
}

static void epd_send_data(uint8_t *pchBuff, size_t tSize)
{
    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_CS_PIN, 0);
    epd_spi_write(pchBuff, tSize);
    gpio_put(EPD_CS_PIN, 1);
}

__STATIC_INLINE 
void epd_read_busy(void)
{
    do {
        epd_send_cmd(GET_STATUS);
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

    epd_send_cmd(POWER_SETTING);
    epd_send_byte(0x03);
    epd_send_byte(0x00);
    epd_send_byte(0x2b);
    epd_send_byte(0x2b);
    epd_send_byte(0x03);

    epd_send_cmd(BOOSTER_SOFT_START);
    epd_send_byte(0x17);        //A
    epd_send_byte(0x17);        //B
    epd_send_byte(0x17);        //C

    epd_send_cmd(POWER_ON);
    epd_read_busy();

    epd_send_cmd(PANEL_SETTING);
    epd_send_byte(0xbf);        //128x296
    epd_send_byte(0x0e);        //VCOM to 0V fast

    epd_send_cmd(PLL_CONTROL);
    epd_send_byte(0x3a);        // 3a 100HZ   29 150Hz 39 200HZ 31 171HZ

    epd_send_cmd(TCON_RESOLUTION);
    epd_send_byte(EPD_SCREEN_WIDTH);                                            //!< width
    epd_send_byte((EPD_SCREEN_HEIGHT >> 8) & 0xff);                             //!< height high byte
    epd_send_byte(EPD_SCREEN_HEIGHT & 0xff);                                    //!< height low byte

    epd_send_cmd(VCM_DC_SETTING_REGISTER);
    epd_send_byte(0x28);
}

static void epd_set_partial_refresh_mode(void)
{
    epd_send_cmd(VCM_DC_SETTING_REGISTER);
    epd_send_byte(0x00);
    epd_send_cmd(VCOM_AND_DATA_INTERVAL_SETTING);
    epd_send_byte(0xb7);

    
    SEND_LUT(LUT_VCOM,  epd_lut_vcom1);
    SEND_LUT(LUT_WW,    epd_lut_ww1);
    SEND_LUT(LUT_BW,    epd_lut_bw1);
    SEND_LUT(LUT_WB,    epd_lut_wb1);
    SEND_LUT(LUT_BB,    epd_lut_bb1);
}

static void epd_set_full_refresh_mode(void)
{
    epd_send_cmd(VCOM_AND_DATA_INTERVAL_SETTING);
    epd_send_byte(0xb7);

    SEND_LUT(LUT_VCOM,  epd_lut_vcom_dc);
    SEND_LUT(LUT_WW,    epd_lut_ww);
    SEND_LUT(LUT_BW,    epd_lut_bw);
    SEND_LUT(LUT_WB,    epd_lut_wb);
    SEND_LUT(LUT_BB,    epd_lut_bb);
}

void epd_sceen_clear(void)
{
    int16_t iWidth = (EPD_SCREEN_WIDTH + 7) >> 3;
    int16_t iHeight =  EPD_SCREEN_HEIGHT;

    epd_send_cmd(DATA_START_TRANSMISSION_1);
    for (int16_t j = 0; j < iHeight; j++) {
        for (int16_t i = 0; i < iWidth; i++) {
            epd_send_byte(0x00);
        }
    }

    epd_send_cmd(DATA_START_TRANSMISSION_2);
    for (int16_t j = 0; j < iHeight; j++) {
        for (int16_t i = 0; i < iWidth; i++) {
            epd_send_byte(0xFF);
        }
    }

    epd_set_full_refresh_mode();
    
    while(!epd_flush()) __NOP();
}

static
void epd_screen_set_window(int16_t iX, int16_t iY, int16_t iWidth, int16_t iHeight)
{
    epd_set_partial_refresh_mode();
    epd_send_cmd(PARTIAL_IN);                   //This command makes the display enter partial mode
    epd_send_cmd(PARTIAL_WINDOW);               //resolution setting
    
    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_CS_PIN, 0);

    epd_spi_write_byte(iX);                 //x-start
    epd_spi_write_byte(iX + iWidth - 1);    //x-end

    epd_spi_write_byte(iY >> 8);
    epd_spi_write_byte(iY & 0xFF);          //y-start
    
    iY += iHeight - 1;
    epd_spi_write_byte(iY);
    epd_spi_write_byte(iY & 0xFF);          //y-end
    epd_spi_write_byte(0x28);
    
    gpio_put(EPD_CS_PIN, 1);
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
    
    epd_send_cmd(DATA_START_TRANSMISSION_2);

    gpio_put(EPD_DC_PIN, 1);
    gpio_put(EPD_CS_PIN, 0);
    
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

            epd_spi_write_byte(chData);
        
        }
    }
    
    gpio_put(EPD_CS_PIN, 1);
    
    epd_send_cmd(DATA_STOP);
    epd_send_cmd(PARTIAL_OUT);

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
            epd_send_cmd(DISPLAY_REFRESH);
            //fall-through;
        case WAIT_BUSY:
            epd_send_cmd(GET_STATUS);
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