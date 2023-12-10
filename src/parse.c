#include "parse.h"
#include "border.h"

static bool str_starts_with(char* string, char* prefix) {
  if (!string || !prefix) return false;
  if (strlen(string) < strlen(prefix)) return false;
  if (strncmp(prefix, string, strlen(prefix)) == 0) return true;
  return false;
}

static uint32_t parse_color(struct color_style* style, char* token) {
  uint32_t update_mask = 0;
  if (sscanf(token, "=0x%x", &style->color) == 1) {
    style->stype = COLOR_STYLE_SOLID;
  } else if (sscanf(token,
             "=gradient(top_left=0x%x,bottom_right=0x%x)",
             &style->gradient.color1,
             &style->gradient.color2) == 2) {
      style->stype = COLOR_STYLE_GRADIENT;
      style->gradient.direction = TL_TO_BR;
      update_mask |= BORDER_UPDATE_MASK_ACTIVE;
  } else if (sscanf(token,
             "=gradient(top_right=0x%x,bottom_left=0x%x)",
             &style->gradient.color1,
             &style->gradient.color2) == 2) {
      style->stype = COLOR_STYLE_GRADIENT;
      style->gradient.direction = TR_TO_BL;
      update_mask |= BORDER_UPDATE_MASK_ACTIVE;
  } else {
    printf("[?] Borders: Invalid color argument color%s\n", token);
  }
  return update_mask;
}

uint32_t parse_settings(struct settings* settings, int count, char** arguments) {
  static char active_color[] = "active_color";
  static char inactive_color[] = "inactive_color";

  uint32_t update_mask = 0;
  for (int i = 0; i < count; i++) {
    if (str_starts_with(arguments[i], active_color)) {
      update_mask |= parse_color(&settings->active_window,
                                 arguments[i] + strlen(active_color));
    } else  if (str_starts_with(arguments[i], inactive_color)) {
      update_mask |= parse_color(&settings->inactive_window,
                                 arguments[i] + strlen(inactive_color));
    }
    else if (sscanf(arguments[i], "width=%f", &settings->border_width) == 1) {
      update_mask |= BORDER_UPDATE_MASK_ALL;
    }
    else if (sscanf(arguments[i], "style=%c", &settings->border_style) == 1) {
      update_mask |= BORDER_UPDATE_MASK_ALL;
    } else if (strcmp(arguments[i], "hidpi=on") == 0) {
      update_mask |= BORDER_UPDATE_MASK_RECREATE_ALL;
      settings->hidpi = true;
    } else if (strcmp(arguments[i], "hidpi=off") == 0) {
      update_mask |= BORDER_UPDATE_MASK_RECREATE_ALL;
      settings->hidpi = false;
    } else {
      printf("[?] Borders: Invalid argument '%s'\n", arguments[i]);
    }
  }
  return update_mask;
}