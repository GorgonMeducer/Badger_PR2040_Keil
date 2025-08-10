/****************************************************************************
*  Copyright 2021 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
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
#include "platform/platform.h"

#include <stdio.h>

#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "arm_2d_disp_adapters.h"
#include "arm_2d_scenes.h"
#include "arm_2d_demos.h"


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void SysTick_Handler(void)
{

}

void scene_text_reader_loader(void) 
{

    arm_2d_scene_text_reader_init(&DISP0_ADAPTER);
}

void scene_mono_loading_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_mono_loading_init(&DISP0_ADAPTER);
}

void scene_mono_histogram_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_mono_histogram_init(&DISP0_ADAPTER);
}

void scene_mono_clock_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_mono_clock_init(&DISP0_ADAPTER);
}

void scene_mono_list_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_mono_list_init(&DISP0_ADAPTER);
}

void scene_mono_tracking_list_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_mono_tracking_list_init(&DISP0_ADAPTER);
}

void scene_mono_icon_menu_loader(void) 
{
    arm_2d_scene_player_set_switching_mode( &DISP0_ADAPTER,
                                            ARM_2D_SCENE_SWITCH_MODE_SLIDE_LEFT);
    arm_2d_scene_player_set_switching_period(&DISP0_ADAPTER, 300);

    arm_2d_scene_mono_icon_menu_init(&DISP0_ADAPTER);
}

typedef struct demo_scene_t {
    int32_t nLastInMS;
    void (*fnLoader)(void);
} demo_scene_t;

static demo_scene_t const c_SceneLoaders[] = {

#if 1
    {
        15000,
        scene_mono_loading_loader,
    },
    {
        10000,
        scene_mono_clock_loader,
    },
    {
        10000,
        scene_mono_histogram_loader,
    },
    {
        15000,
        scene_mono_list_loader,
    },
    {
        15000,
        scene_mono_icon_menu_loader,
    },
    {
        30000,
        scene_text_reader_loader,
    },
    

#else
    {
        .fnLoader = 
        scene_text_reader_loader,
        //scene_mono_clock_loader
    },
#endif

};

static
struct {
    int8_t chIndex;
    bool bIsTimeout;
    int32_t nDelay;
    int64_t lTimeStamp;
    
} s_tDemoCTRL = {
    .chIndex = -1,
    .bIsTimeout = true,
};

/* load scene one by one */
void before_scene_switching_handler(void *pTarget,
                                    arm_2d_scene_player_t *ptPlayer,
                                    arm_2d_scene_t *ptScene)
{

    switch (arm_2d_scene_player_get_switching_status(&DISP0_ADAPTER)) {
        case ARM_2D_SCENE_SWITCH_STATUS_MANUAL_CANCEL:
            s_tDemoCTRL.chIndex--;
            break;
        default:
            s_tDemoCTRL.chIndex++;
            break;
    }

    if (s_tDemoCTRL.chIndex >= dimof(c_SceneLoaders)) {
        s_tDemoCTRL.chIndex = 0;
    } else if (s_tDemoCTRL.chIndex < 0) {
        s_tDemoCTRL.chIndex += dimof(c_SceneLoaders);
    }
    
    //epd_sceen_clear();
    
    /* call loader */
    arm_with(const demo_scene_t, &c_SceneLoaders[s_tDemoCTRL.chIndex]) {
        if (_->nLastInMS > 0) {
            s_tDemoCTRL.bIsTimeout = false;
            s_tDemoCTRL.lTimeStamp = 0;
            s_tDemoCTRL.nDelay = _->nLastInMS;
        }
        _->fnLoader();
    }
}


#if defined(__RTE_ACCELERATION_ARM_2D__) || defined(RTE_Acceleration_Arm_2D)
static bool __lcd_sync_handler(void *pTarget)
{
    return !epd_screen_is_busy();
}
#endif

static void system_init(void)
{
    platform_init();

    arm_2d_init();
    disp_adapter0_init();
    
    arm_2d_helper_full_frame_refresh_mode(
                                    &DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t, 
                                    true);

    arm_2d_scene_player_register_before_switching_event_handler(
            &DISP0_ADAPTER,
            before_scene_switching_handler);
    /* register a low level sync-up handler to wait LCD finish rendering the previous frame */
    do {
        arm_2d_helper_pfb_dependency_t tDependency = {
            .evtOnLowLevelSyncUp = {
                .fnHandler = &__lcd_sync_handler,
            },
        };
        arm_2d_helper_pfb_update_dependency(&DISP0_ADAPTER.use_as__arm_2d_helper_pfb_t, 
                                            ARM_2D_PFB_DEPEND_ON_LOW_LEVEL_SYNC_UP,
                                            &tDependency);
    } while(0);

}


int main(void) 
{
    system_init();

    __cycleof__("printf") {
        printf("Hello Badger RP2040!\r\n");
    }

#if defined( __PERF_COUNTER_COREMARK__ ) && __PERF_COUNTER_COREMARK__
    printf("\r\nRun Coremark 1.0...\r\n");
    coremark_main();
#endif

    arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);

    while (true) {

        arm_fsm_rt_t tResult = disp_adapter0_task();
        if (arm_fsm_rt_cpl == tResult) {
            epd_flush();
        } else if (ARM_2D_RT_FRAME_SKIPPED) {
            __NOP();
        }

        if (!s_tDemoCTRL.bIsTimeout) {

            if (arm_2d_helper_is_time_out(s_tDemoCTRL.nDelay, &s_tDemoCTRL.lTimeStamp)) {
                s_tDemoCTRL.bIsTimeout = true;

                arm_2d_scene_player_switch_to_next_scene(&DISP0_ADAPTER);
            }
        }

    }
    //return 0;
}
