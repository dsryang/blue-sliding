#include <pebble.h>

static Window *main_window;
static TextLayer *time_layer, *date_layer;
static Layer *light_layer, *dark_layer;
static GFont time_font;
static PropertyAnimation *property_animation_light, *property_animation_dark;
static Animation *animation_light, *animation_dark, *layer_slide;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(time_layer, buffer);
}

static void get_date() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *date = localtime(&temp);
  
  static char full_date[16];
  int day_of_week = date->tm_wday;
  int month = date->tm_mon;
  char day[] = "31";
  
  int str_end = 0;
  
  // Get current date
  strftime(day, sizeof("31"), "%d", date);
  
  switch (day_of_week) {
    case 0:
      strncat(full_date, "SUN, ", 5);
      str_end += 5;
      break;
    case 1:
      strncat(full_date, "MON, ", 5);
      str_end += 5;
      break;
    case 2:
      strncat(full_date, "TUES, ", 6);
      str_end += 6;
      break;
    case 3:
      strncat(full_date, "WED, ", 5);
      str_end += 5;
      break;
    case 4:
      strncat(full_date, "THURS, ", 7);
      str_end += 7;
      break;
    case 5:
      strncat(full_date, "FRI, ", 5);
      str_end += 5;
      break;
    case 6:
      strncat(full_date, "SAT, ", 5);
      str_end += 5;
      break;
  }
  
  switch (month) {
    case 0:
      strncat(full_date, "JAN ", 4);
      str_end += 4;
      break;
    case 1:
      strncat(full_date, "FEB ", 4);
      str_end += 4;
      break;
    case 2:
      strncat(full_date, "MAR ", 4);
      str_end += 4;
      break;
    case 3:
      strncat(full_date, "APR ", 4);
      str_end += 4;
      break;
    case 4:
      strncat(full_date, "MAY ", 4);
      str_end += 4;
      break;
    case 5:
      strncat(full_date, "JUNE ", 5);
      str_end += 5;
      break;
    case 6:
      strncat(full_date, "JULY ", 5);
      str_end += 5;
      break;
    case 7:
      strncat(full_date, "AUG ", 4);
      str_end += 4;
      break;
    case 8:
      strncat(full_date, "SEPT ", 5);
      str_end += 5;
      break;
    case 9:
      strncat(full_date, "OCT ", 4);
      str_end += 4;
      break;
    case 10:
      strncat(full_date, "NOV ", 4);
      str_end += 4;
      break;
    case 11:
      strncat(full_date, "DEC ", 4);
      str_end += 4;
      break;
  }
  
  if (day[0] == '0') {
    full_date[str_end] = day[1];
    str_end += 1;
  }
  else {
    strncat(full_date, day, 2);
    str_end += 2;
  }
  
  full_date[str_end] = '\0';

  // Display this date on the TextLayer
  text_layer_set_text(date_layer, full_date);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void draw_light_layer(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // Draw a light blue filled rectangle with sharp corners
  graphics_context_set_fill_color(ctx, GColorPictonBlue);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}

static void draw_dark_layer(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // Draw a dark blue filled rectangle with sharp corners
  graphics_context_set_fill_color(ctx, GColorCobaltBlue);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}

static void animations() {
  // Slide from left animation for light background layer
  GRect light_start = GRect(-144, 0, 0, 168);
  GRect light_end = GRect(0, 0, 144, 168);
  property_animation_light = property_animation_create_layer_frame(light_layer, &light_start, &light_end);
  animation_light = property_animation_get_animation(property_animation_light);
  animation_set_duration(animation_light, 800);
  
  // Slide from right animation for dark background layer
  GRect dark_start = GRect(144, 43, 144, 82);
  GRect dark_end = GRect(0, 43, 144, 82);
  property_animation_dark = property_animation_create_layer_frame(dark_layer, &dark_start, &dark_end);
  animation_dark = property_animation_get_animation(property_animation_dark);
  animation_set_duration(animation_dark, 800);
  
  // Create spawn animation for both layers to slide at the same time
  layer_slide = animation_spawn_create(animation_light, animation_dark, NULL);
  animation_set_play_count(layer_slide, 1);
  animation_schedule(layer_slide);
}

static void main_window_load(Window *window) {
  // Get the root layer
  Layer *window_layer = window_get_root_layer(window);
  
  // Create light layer
  light_layer = layer_create(GRectZero);
  layer_set_update_proc(light_layer, draw_light_layer);
  layer_add_child(window_layer, light_layer);
  
  // Create dark layer
  dark_layer = layer_create(GRectZero);
  layer_set_update_proc(dark_layer, draw_dark_layer);
  layer_add_child(window_layer, dark_layer);
  
  // Create time TextLayer
  time_layer = text_layer_create(GRect(0, 42, 144, 70));
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_color(time_layer, GColorWhite);

  // Apply to time TextLayer
  time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_BOLD_46));
  text_layer_set_font(time_layer, time_font);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
  
  // Create date TextLayer
  date_layer = text_layer_create(GRect(0, 88, 144, 80));
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_text_color(date_layer, GColorWhite);

  // Apply to date TextLayer
  text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(date_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  
  // Destroy colored layers
  layer_destroy(light_layer);
  layer_destroy(dark_layer);
  
  // Destroy animations
  property_animation_destroy(property_animation_dark);
  property_animation_destroy(property_animation_light);
  animation_destroy(animation_light);
  animation_destroy(animation_dark);
  animation_destroy(layer_slide);
}

static void init() {
  // Create main Window element and assign to pointer
  main_window = window_create();
  window_set_background_color(main_window, GColorBlack);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(main_window, true);
  
  // Play animations
  animations();
  
  // Make sure the time is displayed from the start
  update_time();
  
  // Make sure the date is displayed from the start
  get_date();
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}