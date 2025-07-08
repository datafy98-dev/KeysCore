idf_component_register(SRCS "main.c" 
                           "../lib/st7735/lcd_st7735.c"
                           "../lib/buttons/buttons.c"
                    INCLUDE_DIRS "." 
                                "../lib/st7735"
                                "../lib/buttons")
