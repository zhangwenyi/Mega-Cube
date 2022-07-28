#include "Animation.h"

#include "Accelerometer.h"
#include "Arrows.h"
#include "Atoms.h"
#include "Fireworks.h"
#include "Helix.h"
#include "Life.h"
#include "Mario.h"
#include "Plasma.h"
#include "Pong.h"
#include "Scroller.h"
#include "Sinus.h"
#include "Spectrum.h"
#include "Starfield.h"
#include "Twinkels.h"
/*------------------------------------------------------------------------------
 * ANIMATION STATIC DEFINITIONS
 *----------------------------------------------------------------------------*/
Noise Animation::noise = Noise();
Timer Animation::animation_timer = Timer();
uint16_t Animation::animation_sequence = 0;
/*------------------------------------------------------------------------------
 * ANIMATION GLOBAL DEFINITIONS
 *----------------------------------------------------------------------------*/
Sinus sinus;
Fireworks fireworks1;
Fireworks fireworks2;
Twinkels twinkels;
Starfield starfield;
Helix helix;
Atoms atoms;
Arrows arrows;
Plasma plasma;
Mario mario;
Life life;
Pong pong;
Scroller scroller;
Accelerometer accelerometer;
Spectrum spectrum;

Animation *Animations[] = {&atoms,      &sinus,    &starfield,    &fireworks1,
                           &fireworks2, &twinkels, &helix,        &arrows,
                           &plasma,     &mario,    &life,         &pong,
                           &spectrum,   &scroller, &accelerometer};

const uint8_t ANIMATIONS = sizeof(Animations) / sizeof(Animation *);
/*----------------------------------------------------------------------------*/
// Start display asap to minimize PL9823 blue startup
void Animation::begin() { Display::begin(); }

// Render an animation frame, but only if the display allows it (non blocking)
void Animation::animate() {
  // Only draw to the display if it's available
  if (Display::available()) {
    // Update the animation timer to determine frame deltatime
    animation_timer.update();
    // Clear the display before drawing any animations
    Display::clear();
    // Draw all active animations from the animation pool
    uint8_t active_animation_count = 0;
    for (uint8_t i = 0; i < ANIMATIONS; i++) {
      Animation &animation = *Animations[i];
      if (animation.state != state_t::INACTIVE) {
        animation.draw(animation_timer.dt());
      }
      // Animation can become inactive after drawing so check again
      if (animation.state != state_t::INACTIVE) {
        active_animation_count++;
        if (config.animation.changed) animation.end();
      }
    }
    // Select the next or specific animation from the sequence list
    if (active_animation_count == 0) {
      Animation::next(config.animation.changed, config.animation.animation);
      config.animation.changed = false;
    }
    // Commit current animation frame to the display
    Display::update();
  }
}

// Override end method if more is needed than changing state
void Animation::end() { state = state_t::ENDING; }

// Get fps, if animate has been called (t > 0)
float Animation::fps() {
  if (animation_timer.dt() > 0) {
    return 1 / animation_timer.dt();
  }
  return 0;
}

void TWINKEL_WHITE(float f) {
  auto &_ = config.animation.twinkels;
  twinkels.init(f * _.timer_duration, true, false);
  twinkels.speed(_.timer_interval, _.fade_in_speed, _.fade_out_speed);
  twinkels.color(Color(255, 150, 30));
  twinkels.clear();
}
void TWINKEL_MULTI(float f) {
  auto &_ = config.animation.twinkels;
  twinkels.init(f * _.timer_duration, false, true);
  twinkels.speed(_.timer_interval, _.fade_in_speed, _.fade_out_speed);
}
void STARFIELD(float f) {
  auto &_ = config.animation.starfield;
  starfield.init(_.fade_in_speed, f * _.timer_duration, _.fade_out_speed);
  starfield.speed(_.phase_speed, _.hue_speed);
}
void SINUS(float f) {
  auto &_ = config.animation.sinus;
  sinus.init(_.fade_in_speed, f * _.timer_duration, _.fade_out_speed, _.radius);
  sinus.speed(_.phase_speed, _.hue_speed);
}
void FIREWORKS(float f) {
  auto &_ = config.animation.fireworks;
  fireworks1.init(f * _.timer_duration);
  fireworks2.init(f * _.timer_duration);
}
void PLASMA(float f) {
  auto &_ = config.animation.plasma;
  plasma.init(_.fade_in_speed, f * _.timer_duration, _.fade_out_speed);
}
void HELIX(float f) {
  auto &_ = config.animation.helix;
  helix.init(f * _.timer_duration, _.timer_interval);
  helix.speed(_.phase_speed, _.hue_speed);
}
void MARIO(float f) {
  auto &_ = config.animation.mario;
  mario.init(f * _.timer_duration, _.time_expansion, _.time_contraction,
             _.timer_interval, _.angular_speed, _.radius);
}
void ARROWS(float f) {
  auto &_ = config.animation.arrows;
  arrows.init(f * _.timer_duration, _.time_expansion, _.time_contraction,
              _.angular_speed, _.radius, _.hue_speed);
}
void ATOMS(float f) {
  auto &_ = config.animation.atoms;
  atoms.init(f * _.timer_duration, _.time_expansion, _.time_contraction,
             _.angular_speed, _.radius, _.hue_speed);
}
void PONG(float f) {
  auto &_ = config.animation.pong;
  pong.init(f * _.timer_duration, _.time_fading, _.hue_speed);
}
void LIFE(float f) {
  auto &_ = config.animation.life;
  life.init(f * _.timer_duration, _.time_fading);
}
void SPECTRUM(float f) {
  auto &_ = config.animation.spectrum;
  spectrum.init(f * _.timer_duration, _.hue_speed);
}
void SCROLLER(float f) {
  auto &_ = config.animation.scroller;
  // " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\xff");
  scroller.init(f * _.timer_duration, _.rotation_speed, "#MALT WHISKEY$\xff");
}
void ACCELEROMETER(float f) {
  auto &_ = config.animation.accelerometer;
  accelerometer.init(f * _.timer_duration);
}

// Animation sequencer (jumptable)
void Animation::next(boolean changed, uint8_t index) {
  static void (*jump_table[])(float f) =  //
      {&SCROLLER,  &SPECTRUM, &PONG,          &LIFE,         &FIREWORKS,
       &STARFIELD, &HELIX,    &SINUS,         &ATOMS,        &ARROWS,
       &MARIO,     &PLASMA,   &TWINKEL_WHITE, &TWINKEL_MULTI};
  if (changed && (index < sizeof(jump_table) / sizeof(void *))) {
    jump_table[index](0);
  } else {
    if (animation_sequence >= sizeof(jump_table) / sizeof(void *)) {
      animation_sequence = 0;
    }
    jump_table[animation_sequence++](1);
  }
}