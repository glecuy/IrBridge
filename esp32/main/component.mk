#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

#COMPONENT_SRCDIRS := WebUi

COMPONENT_DIRS += $(PROJECT_PATH)/WebUi

COMPONENT_OBJS := main.o InfraredTx.o legrandTx.o samsungTx.o

COMPONENT_OBJS += samsung_html.o legrand_html.o
COMPONENT_OBJS += legrand_css.o samsung_css.o keyClick_js.o
COMPONENT_OBJS += legrand_png.o power_png.o samsung_png.o

# Images
legrand_png.c: $(PROJECT_PATH)/../WebUi/images/legrand.png
	$(PROJECT_PATH)/../pages2c/pages2c  $< $@ 
legrand_png.o: legrand_png.c
	$(CC) -o $@ -c $< $(CFLAGS)

power_png.c: $(PROJECT_PATH)/../WebUi/images/power.png
	$(PROJECT_PATH)/../pages2c/pages2c  $< $@ 
power_png.o: power_png.c
	$(CC) -o $@ -c $< $(CFLAGS)

samsung_png.c: $(PROJECT_PATH)/../WebUi/images/samsung.png
	$(PROJECT_PATH)/../pages2c/pages2c  $< $@ 
samsung_png.o: samsung_png.c
	$(CC) -o $@ -c $< $(CFLAGS)


#HTML pages
samsung_html.c: $(PROJECT_PATH)/../WebUi/samsung.html
	$(PROJECT_PATH)/../pages2c/pages2c  $< $@ 

samsung_html.o: samsung_html.c
	$(CC) -o $@ -c $< $(CFLAGS)

legrand_html.c: $(PROJECT_PATH)/../WebUi/legrand.html
	$(PROJECT_PATH)/../pages2c/pages2c  $< $@ 

legrand_html.o: legrand_html.c
	$(CC) -o $@ -c $< $(CFLAGS)

# css 
samsung_css.c: $(PROJECT_PATH)/../WebUi/samsung.css
	$(PROJECT_PATH)/../pages2c/pages2c  $< $@ 

samsung_css.o: samsung_css.c
	$(CC) -o $@ -c $< $(CFLAGS)

legrand_css.c: $(PROJECT_PATH)/../WebUi/legrand.css
	$(PROJECT_PATH)/../pages2c/pages2c  $< $@ 

legrand_css.o: legrand_css.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
# js 
keyClick_js.c: $(PROJECT_PATH)/../WebUi/keyClick.js
	$(PROJECT_PATH)/../pages2c/pages2c  $< $@ 

keyClick_js.o: keyClick_js.c
	$(CC) -o $@ -c $< $(CFLAGS)

COMPONENT_EXTRA_CLEAN := legrand_png.c
	
