/**
 * MK4duo Firmware for 3D Printer, Laser and CNC
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (C) 2013 Alberto Cotronei @MagoKimbra
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../../../MK4duo.h"

Host_Action host_action;

/** Public Parameters */
HostPromptEnum Host_Action::prompt_reason = PROMPT_NOT_DEFINED;

/** Public Function */
void Host_Action::say_m876_response(PGM_P const msg) {
  SERIAL_MSG("M876 Responding PROMPT_");
  SERIAL_PGM(msg);
  SERIAL_EOL();
}

void Host_Action::response_handler(const uint8_t response) {

  switch (prompt_reason) {
    case PROMPT_FILAMENT_RUNOUT:
      if (response == 0) {
        #if ENABLED(ADVANCED_PAUSE_FEATURE)
          advancedpause.menu_response = ADVANCED_PAUSE_RESPONSE_EXTRUDE_MORE;
        #endif
      }
      else if (response == 1) {
        #if ENABLED(FILAMENT_RUNOUT_SENSOR)
          filamentrunout.setEnabled(false);
          filamentrunout.reset();
        #endif
        #if ENABLED(ADVANCED_PAUSE_FEATURE)
          advancedpause.menu_response = ADVANCED_PAUSE_RESPONSE_RESUME_PRINT;
        #endif
        printer.setWaitForUser(false);
      }
      say_m876_response(PSTR("FILAMENT_RUNOUT"));
      break;
    case PROMPT_FILAMENT_RUNOUT_CONTINUE:
      #if ENABLED(ADVANCED_PAUSE_FEATURE)
        if (response == 0)
          advancedpause.menu_response = ADVANCED_PAUSE_RESPONSE_EXTRUDE_MORE;
        else if (response == 1)
          advancedpause.menu_response = ADVANCED_PAUSE_RESPONSE_RESUME_PRINT;
      #endif
      say_m876_response(PSTR("FILAMENT_RUNOUT_CONTINUE"));
      break;
    case PROMPT_FILAMENT_RUNOUT_REHEAT:
      printer.setWaitForUser(false);
      say_m876_response(PSTR("FILAMENT_RUNOUT_REHEAT"));
      break;
    case PROMPT_PAUSE_RESUME:
      commands.enqueue_and_echo_P(PSTR("M24"));
      say_m876_response(PSTR("LCD_PAUSE_RESUME"));
      break;
    case PROMPT_INFO:
      say_m876_response(PSTR("GCODE_INFO"));
      break;
    default:
      say_m876_response(PSTR("UNKNOWN STATE"));
      break;
  }
  host_action.prompt_reason = PROMPT_NOT_DEFINED;
}

void Host_Action::filrunout(const uint8_t t) {
  print_action(PSTR("out_of_filament T"), false);
  SERIAL_EV(int(t));
}

void Host_Action::prompt_begin(PGM_P const msg, const bool eol/*=true*/) {
  prompt_end(); // ensure any current prompt is closed before we begin a new one
  print_prompt_plus(PSTR("begin"), msg, eol);
}

void Host_Action::prompt_choice(PGM_P const msg) {
  print_prompt_plus(PSTR("choice"), msg);
}

void Host_Action::prompt_button(PGM_P const msg) {
  print_prompt_plus(PSTR("button"), msg);
}

void Host_Action::prompt_do(const HostPromptEnum reason, PGM_P const pstr, PGM_P const pbtn/*=NULL*/) {
  prompt_reason = reason;
  prompt_begin(pstr);
  if (pbtn) prompt_button(pbtn);
  prompt_show();
}

/** Private Function */
void Host_Action::print_action(PGM_P const msg, const bool eol/*=true*/) {
  SERIAL_MSG("// action:");
  SERIAL_PGM(msg);
  if (eol) SERIAL_EOL();
}

void Host_Action::print_prompt(PGM_P const msg, const bool eol/*=true*/) {
  print_action(PSTR("prompt_"), false);
  SERIAL_PGM(msg);
  if (eol) SERIAL_EOL();
}

void Host_Action::print_prompt_plus(PGM_P const ptype, PGM_P const msg, const bool eol/*=true*/) {
  print_prompt(ptype, false);
  SERIAL_CHR(' ');
  SERIAL_PGM(msg);
  if (eol) SERIAL_EOL();
}
