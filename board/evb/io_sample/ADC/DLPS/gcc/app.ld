#include "./mem_config.h"
#include "./flash_map.h"

/* Entry Point */
ENTRY(Reset_Handler)

#if (BUILD_BANK == 0)
#define APP_ADDR                    BANK0_APP_ADDR
#else
#define APP_ADDR                    BANK1_APP_ADDR
#endif
#define APP_SIZE                    BANK0_APP_SIZE

MEMORY
{
APP_DATA_ON (rwx)   : ORIGIN = APP_GLOBAL_ADDR, LENGTH = APP_GLOBAL_SIZE
HEAP_DATA_ON (rwx)  : ORIGIN = (APP_GLOBAL_ADDR + APP_GLOBAL_SIZE) , LENGTH = HEAP_DATA_ON_SIZE
CACHE_DATA_ON (rwx)   : ORIGIN = SHARE_CACHE_RAM_ADDR, LENGTH = SHARE_CACHE_RAM_SIZE
APP_FLASH (r)   : ORIGIN = APP_ADDR, LENGTH = APP_SIZE
APP_TRACE (r): ORIGIN = 0x08100000, LENGTH = 0x80000
}

/* Define output sections */
SECTIONS 
{
    IMAGE_HEADER : 
    {
        KEEP(* (.app.flash.header))
        KEEP(* (.app.flash.header.auth))
    } >APP_FLASH

#if (FEATURE_FLASH_SEC == 1)
    FLASH_GAP_REGION :
    {
        . = ALIGN(4);
        * (.app.flash.gap.text)
        ../../../../../../bin/gcc/librtl8752h_sdk.a
        . = ALIGN(0x1000);
    } >APP_FLASH
#endif

    FLASH_START_ADDR : 
    {
        . = ALIGN(4);
        build/startup_rtl876x_gcc.o(RESET)
        . = ALIGN(4);
    __flash_start_load_ad__ = LOADADDR(FLASH_START_ADDR);
    __flash_start_exe_ad__ = ADDR(FLASH_START_ADDR);
    } >APP_FLASH

    FLASH_TEXT :
    {
        *(.text*)
        . = ALIGN(4);
        *(.rodata*)
        . = ALIGN(4);
        *(.app.flash.text)
        *(.app.flash.rodata)
        *(.ARM.exidx*)
    } >APP_FLASH

    RAM_VECTOR_TABLE BLOCK(4) : 
    {
        __ram_vector_table_start__ = .;
        KEEP(*(VECTOR))
        __ram_vector_table_end__ = .;
        . = ALIGN(4);
    } >APP_FLASH
    __ram_vector_load_ad__ = LOADADDR(RAM_VECTOR_TABLE);
    __ram_vector_table_length__ = __ram_vector_table_end__ - __ram_vector_table_start__; 

    RAM_DATA_ON :
    {
        . = ALIGN(4);
        __ram_dataon_ro_start__ = .;
        * (.app.data_ram.text)
        . = ALIGN(4);
        __ram_dataon_ro_end__ = .;
        __ram_dataon_rw_start__ = .;
        * (.data*)
        * (.ram.dataon.data)
        . = ALIGN(4);
        __ram_dataon_rw_end__ = .;
    } >APP_DATA_ON  AT > APP_FLASH

    BSS (NOLOAD):
    {
        . = ALIGN(4);
        __ram_dataon_zi_start__ = .;
        * (.ram.dataon.bss*)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __ram_dataon_zi_end__ = .;
    } >APP_DATA_ON  AT > APP_FLASH

    __ram_ro_load_ad__ = LOADADDR(RAM_DATA_ON);
    __ram_dataon_ro_length__ = __ram_dataon_ro_end__ - __ram_dataon_ro_start__;
    __ram_dataon_rw_length__ = __ram_dataon_rw_end__ - __ram_dataon_rw_start__;
    __ram_dataon_zi_length__ = __ram_dataon_zi_end__ - __ram_dataon_zi_start__;
    __ram_rw_load_ad__ = LOADADDR(RAM_DATA_ON)+__ram_dataon_ro_length__;

    HEAP_DATA_ON  :
    {
        __heap_start__ = .;
        end = __heap_start__;
        _end = end;
        __end = end;
        KEEP(*(.heap))
        __heap_end__ = .;
        __HeapLimit = __heap_end__;
    } >HEAP_DATA_ON  AT > APP_FLASH

    CACHE_DATA_ON :  
    {   
        __cache_dataon_start__ = . ;
        *(.ram.sharecacheram.text)
        __cache_dataon_end__ = .;
    } >CACHE_DATA_ON AT > APP_FLASH

    __cache_dataon_length__ = __cache_dataon_end__ - __cache_dataon_start__;
    __cache_data_load_ad__ = LOADADDR(CACHE_DATA_ON);

#if (FEATURE_FLASH_SEC == 1)
    __flash_sec_base_addr__ = __flash_start_load_ad__;
    __flash_sec_region_size__ = __cache_data_load_ad__ + __cache_dataon_length__ - __flash_start_load_ad__;
#endif

    App.trace 0x08100000: 
    {
        *(.TRACE*)
    } >APP_TRACE

}

